#include <arduino.h>

//#include <avr/pgmspace.h>
//#include <avr/interrupt.h>
#include "inc\defines.h"
#include "inc\Program.h"
#include "inc\Timer.h"
#include "inc\Cart.h"
#include "inc\LedMatrix.h"
#include "inc\ultrasonic.h"
#include "inc\Servo.h"
#include <IRremote.h>


#if USE_FAN_FUNCTION  /****use fan*******/
int flame_L = A1; //define the analog port of the left flame sensor to A1
int flame_R = A2; //define the analog port of the right flame sensor to A2
int flame_valL, flame_valR;

//the pin of 130 motor
int INA = 12;
int INB = 13;

#else /****use the ultrasonic sensor*******/
#define light_L_Pin A1   // define the pin of the left photoresistor
#define light_R_Pin A2   // define the pin of the right photoresistor

#endif

/***********/
#define USE_FAN_FUNCTION   0


// wire up the line tracking sensor
#define L_pin  11   //  left
#define M_pin  7    //  middle
#define R_pin  8    //  right

void Avoid();
void Tracking();
void Confinement();
void Follow();
void Light_following();

IRrecv irrecv(3);  //
// Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
//IRrecv irrecv(3);



void
CProgram::Init()
{
    CTimer::CreateInstance();
    CCart::CreateInstance();
    CLedMatrix::CreateInstance();
    CUltrasonic::CreateInstance();
    CServo::CreateInstance();

    //
    // start lazy init in background
    //
    CTimer::Get().SetCounter(LargeTimer, 2000);

    CTimer* pTimer = &CTimer::Get();

    Serial.begin(115200);
    //irrecv.enableIRIn();   // Initialize the library of the IR remote
    irrecv.begin(3, ENABLE_LED_FEEDBACK);

    CLedMatrix::Get().Init();
    CCart::Get().Init();
    CUltrasonic::Get().Init();
    CServo::Get().Init();

    pinMode(L_pin, INPUT); // define the pins of sensors to INPUT
    pinMode(M_pin, INPUT);
    pinMode(R_pin, INPUT);


#if USE_FAN_FUNCTION/****use the fan*******/
    pinMode(INA, OUTPUT);//set INA to OUTPUT
    pinMode(INB, OUTPUT);//set INB to OUTPUT

    //define inputs of the flame sensor
    pinMode(flame_L, INPUT);
    pinMode(flame_R, INPUT);
#else/****use the ultrasonic sensor*******/
    pinMode(light_L_Pin, INPUT);
    pinMode(light_R_Pin, INPUT);

    CServo::Get().setAngle(90); //set the angle of the servo to 90°
#endif

    while (!CTimer::Get().IsTimerFinish(LargeTimer))
    {
        _NOP();
    }

    CLedMatrix::Get().displayBanner(&HelloBanner);  //show start
    //CLedMatrix::Get().printString("Hello!!", -6);  //show start
    CServo::Get().waitForSet();

    //CLedMatrix::Get().display(start01);  //show start
    //CLedMatrix::Get().displayChar(54);
}

bool
CProgram::Run()
{

    for (;;)
    {
        char           bluetoothValue = 0;       // used to save the Bluetooth value
        decode_results results;
        long           ir_rec;  //used to save the IR value 


        if (Serial.available()) //if there is data in the serial buffer
        {
            bluetoothValue = Serial.read();
            Serial.println(bluetoothValue);


            if (irrecv.decode_old(&results))   //Receive infrared remote control value
            {

                //ir_rec = irrecv.decodedIRData.decodedRawData;
                ir_rec = results.value;
                //Serial.println(ir_rec, HEX);

                switch (ir_rec)
                {
                case 0xFF629D:    // go front
                    CCart::Get().Front();
                    break;

                case 0xFFA857:    // go back
                    CCart::Get().Back();
                    break;

                case 0xFF22DD:    // rotate to left
                    CCart::Get().Left();
                    break;

                case 0xFFC23D:    // rotate to right
                    CCart::Get().Right();
                    break;

                case 0xFF02FD:   // stop
                    CCart::Get().Stop();
                    break;

                default:;
                }

                irrecv.resume();
            }

        }

        if (bluetoothValue == 'S')
            m_stopFlag = true;

        if (this->Process(bluetoothValue) != true)
        {
            return false;
        }

        CCart::Get().Update();
        CLedMatrix::Get().Update();
        CServo::Get().Update();

    }

    return true;
}

bool
CProgram::Process(char &command)
{
    switch (command)
    {


    case 'F': // the command to go front
        CCart::Get().Front();
        break;

    case 'B': // the command to go back
        CCart::Get().Back();
        break;

    case 'L': // the command to turn left
        CCart::Get().Left();
        break;

    case 'R': // the command to turn right
        CCart::Get().Right();
        break;

    case 'S':  // stop
        CCart::Get().Stop();
        break;

    case 'e':  // enter the line tracking mode
    {
        CTrackingProgram trackingProgram(this);
        trackingProgram.Init();
        trackingProgram.Run();

        //Tracking();
    }
    break;

    case 'f':   // enter the confinement mode
    {
        CConfinementProgram confinementProgram(this);
        confinementProgram.Init();
        confinementProgram.Run();
        //Confinement();

    }
    break;

#if USE_FAN_FUNCTION/****use fan*******/
    case 'j': Fire(); break;  //enable extinguishing fire mode

    case 'c': fan_begin(); break;  //enable the fan

    case 'd': fan_stop();  break;  //turn off the fan

#else/****use the ultrasonic sensor*******/
    case 'j':
        CServo::Get().setAngle(0);
        CServo::Get().waitForSet();
        break;

    case 'c':
        CServo::Get().setAngle(93);
        CServo::Get().waitForSet();
        break;

    case 'd':
        CServo::Get().setAngle(180);
        CServo::Get().waitForSet();
        break;

    case 'a': // led on
        digitalWrite(9, HIGH);
        break;

    case 'b': // led off
        digitalWrite(9, LOW);
        break;

    case 'g': // enter obstacle avoidance mode
    {
        CAvoidProgram avoidProgram(this);
        avoidProgram.Init();
        avoidProgram.Run();
        //Avoid();
    }
    break;

    case 'h': // enter light following mode
    {
        CFollowProgram followProgram(this);
        followProgram.Init();
        followProgram.Run();
        //Follow();
    }
    break;

    case 'i':   // enter light following mode
    {
        CLightFollowing lightFollowing(this);
        lightFollowing.Init();
        lightFollowing.Run();

        //Light_following();
    }
    break;
#endif

    case 'u':  //start by receiving u, end by receiving characters # and convert into the integer
    {
        String speeds_l;

        speeds_l = Serial.readStringUntil('#');
        CCart::Get().setLeftSpeed(String(speeds_l).toInt());
    }
    break;

    case 'v':  //start by receiving u, end by receiving characters # and convert into the integer
    {
        String speeds_r; //receive PWM characters and convert them into PWM value
        speeds_r = Serial.readStringUntil('#');
        CCart::Get().setRightSpeed(String(speeds_r).toInt());
    }
    break;

    case 'k': CLedMatrix::Get().display(Smile);    break;  // show "smile" face
    case 'l': CLedMatrix::Get().display(Disgust);  break;  // show "disgust" face
    case 'm': CLedMatrix::Get().display(Happy);    break;  // show "happy" face
    case 'n': CLedMatrix::Get().display(Squint);   break;  // show "Sad" face
    case 'o': CLedMatrix::Get().display(Despise);  break;  // show "despise" face
    case 'p': CLedMatrix::Get().display(Heart);    break;  // show the hearbeat image
    case 'z': CLedMatrix::Get().display();         break;  // clear images


#if (USE_FAN_FUNCTION != 1)/****the function to not use the fan*******/
            //The following three signals are mainly used for cyclic printing
    case 'x':
    {
        float distance; // Store the distance values detected by ultrasonic for following

        distance = CUltrasonic::Get().distance();
        Serial.println(distance);
    }
    break;

    case 'w':
    {
        int left_light;

        left_light = analogRead(light_L_Pin);
        Serial.println(left_light);
    }
    break;

    case 'y':
    {
        int right_light;

        right_light = analogRead(light_R_Pin);
        Serial.println(right_light);
    }
    break;

#endif


    default:;

    }

    return true;
}


#if (USE_FAN_FUNCTION != 1)/****use the ultrasonic sensor*******/

void
CAvoidProgram::Init()
{
    m_angleDelta = -10;
    m_angleInc = 4;

    m_minDistance = 2000;
    m_frontDistanceSum = 0;
    m_currentDistance = 0;
    m_samples = 0;

    m_runForwardCredit = 0;

    CTimer::Get().SetCounter(ScanTimer, -1000);
}

bool
CAvoidProgram::Process(char &command)
{

    // 
    // Store the distance values detected by ultrasonic for obstacle avoidance
    //
    // int a1;
    // int a2;
    bool sumReady = false;

    if (command == 'S')
    {
        this->m_stopFlag = true;
        CServo::Get().setAngle(90);
        return false;
    }

    if (CTimer::Get().IsTimerFinish(ScanTimer))
    {

        m_angleDelta += m_angleInc;

        if (m_angleDelta >= 30) {

            m_angleInc = -4;
            sumReady = true;
        }

        if (m_angleDelta <= -10) {

            m_angleInc = 4;
            sumReady = true;
        }

        m_currentDistance = CUltrasonic::Get().distance();

        if (m_currentDistance < m_minDistance)
        {
            m_minDistance = m_currentDistance;
        }

        m_frontDistanceSum += m_currentDistance;
        m_samples++;

        CServo::Get().setAngle(90 + m_angleDelta);          // servo turns left

        if (m_runForwardCredit > 0)
        {
            m_runForwardCredit--;

            if (m_runForwardCredit <= 0)
            {
                CCart::Get().Stop();    // stop
            }
        }

    }

    if (sumReady == true)
    {
        int  frontDistance;

        frontDistance = m_frontDistanceSum / m_samples;

        m_frontDistanceSum = 0;
        m_samples = 0;


        if (frontDistance > 20 && m_minDistance > 20) // if the front distance ≥20cm，robot goes front
        {
            //
            // give a move credit according to distance
            //
            m_runForwardCredit = _map(frontDistance, 20, 200, 5, 10);
            //m_runForwardCredit = 7;
            CCart::Get().Front();  // go front

            

        }
        else   // When the distance in front is less than 20cm
        {
            m_runForwardCredit = 0;
            m_minDistance = 2000;
            m_samples = 0;
            CCart::Get().Stop();    // stop

            CFindWayProgram  findWay(this);
            findWay.Init();
            findWay.Run();

            if (findWay.stopFlag())
                return false;

        }


    }

    if ((m_runForwardCredit != 0) && (m_currentDistance < 20 || m_minDistance < 20))
    {
        m_runForwardCredit = 0;
        m_minDistance = 2000;
        m_samples = 0;
        CCart::Get().Stop();    // stop


        CFindWayProgram  findWay(this);
        findWay.Init();
        findWay.Run();

        if (findWay.stopFlag())
            return false;

    }


    //frontDistance = CUltrasonic::Get()->distance();  // the front distance is set to a


    /*if (a < 20)
    {
        delay(500);              // delay in 500ms
        CServo::Get()->setAngle(180);          // servo turns left
        delay(500);              // delay in 500ms
        a1 = CUltrasonic::Get()->distance();    // the left distance is set to a1
        delay(100);              // read value

        CServo::Get()->setAngle(0);            // servo turns right
        delay(500);              // delay in 500ms
        a2 = CUltrasonic::Get()->distance();    // the right distance is set to a2
        delay(100);              // read value

        CServo::Get()->setAngle(90);           // back to 90°
        delay(500);

        if (a1 > a2) { // When the distance on the left is greater than the distance on the right
            CCart::Get()->Left();   // the robot turns left
            delay(700);  // turn left 700ms
        }
        else
        {
            CCart::Get()->Right();   // turn right
            delay(700);
        }
    }
    else
    {
    }*/

    return true;
}


void
CFindWayProgram::Init()
{
    m_angle = 0;
    m_currentDistance = 0;
    m_moveCredit = 0;

    m_Subprogram = &CFindWayProgram::ScanRight;
}

void
CFindWayProgram::getMinMax(int* arr, int size, int& Min, int& Max)
{
    Max = Min = arr[0];

    for (int i = 0; i < size; i++)
    {
        if (arr[i] > Max) {

            Max = arr[i];
        }

        if (arr[i] < Min) {

            Min = arr[i];
        }
    }

}

void
CFindWayProgram::getMid(int* arr, int size, int& Mid)
{
    float m = 0;

    for (int i = 0; i < size; i++)
    {
        m += (float)arr[i];
    }

    m = m / size;

    Mid = (int)m;

}

bool
CFindWayProgram::ScanRight()
{
    if (CServo::Get().Ready())
    {
        int distance = (int)CUltrasonic::Get().distance();

        m_distancesRight[m_currentDistance++] = distance;

        if (m_angle < m_maxAngle)
        {
            m_angle += m_div;

            CServo::Get().setAngle(m_angle);
        }
        else
        {
            CServo::Get().setAngle(90);
            m_currentDistance = 0;
            m_angle = 180 - m_maxAngle;

            m_Subprogram = &CFindWayProgram::ScanLeft;
        }
    }

    return true;
}

bool
CFindWayProgram::ScanLeft()
{
    if (CServo::Get().Ready())
    {
        int distance = (int)CUltrasonic::Get().distance();

        m_distancesLeft[m_currentDistance++] = distance;

        if (m_angle < 180)
        {
            m_angle += m_div;

            CServo::Get().setAngle(m_angle);
        }
        else
        {
            m_currentDistance = 0;
            m_angle = 90;
            CServo::Get().setAngle(90);

            m_Subprogram = &CFindWayProgram::MakeDecisionMove;
        }
    }

    return true;
}

bool
CFindWayProgram::MakeDecisionMove()
{
    int rightMid = 0;
    int leftMid = 0;

    //
    // decision
    //
#if 0
    Serial.print("distances: ");

    for (int i = 0; i < ARRAYSIZE(m_distancesRight); i++)
    {
        Serial.print(m_distancesRight[i]);
        Serial.print(" ");
    }

    Serial.println();
    for (int i = 0; i < ARRAYSIZE(m_distancesLeft); i++)
    {
        Serial.print(m_distancesLeft[i]);
        Serial.print(" ");
    }

    Serial.println();
#endif // if 0

    getMid(m_distancesRight, ARRAYSIZE(m_distancesRight), rightMid);
    getMid(m_distancesLeft, ARRAYSIZE(m_distancesLeft), leftMid);

    m_moveCredit = 4;

    if (rightMid > leftMid)
    {
        if (rightMid <= 20)
        {
            m_moveCredit = 6;
            CCart::Get().Back();
        }
        else
        {
            CCart::Get().Right();
        }
    }
    else
    {
        if (leftMid <= 20)
        {
            m_moveCredit = 6;
            CCart::Get().Back();
        }
        else
        {
            CCart::Get().Left();
        }
    }


    m_Subprogram = &CFindWayProgram::FinishMove;
    return true;
}

bool
CFindWayProgram::FinishMove()
{
    if (m_moveCredit > 0)
    {
        m_moveCredit--;

    }
    else
    {
        CCart::Get().Stop();    // stop
        m_Subprogram = NULL;
    }

    return true;
}

bool
CFindWayProgram::Process(char &command)
{
    if (command == 'S')
    {
        CServo::Get().setAngle(90);
        this->m_stopFlag = true;
        return false;
    }

    if (CTimer::Get().IsTimerFinish(ScanTimer))
    {
        if (m_Subprogram != NULL)
        {
            if ((this->*m_Subprogram)() == false)
            {
                CServo::Get().setAngle(90);
                this->m_stopFlag = true;
                return false;
            }
            
        }
        else
        {
            CServo::Get().setAngle(90);
            return false;
        }
    }


    return true;

}


bool
CFollowProgram::Process(char &command)
{
    float distance; // Store the distance values detected by ultrasonic for following

    if (command == 'S')
    {
        this->m_stopFlag = true;
        return false;
    }


    distance = CUltrasonic::Get().distance();  //set the distance value to distance

    if (distance >= 20 && distance <= 60)     // go front
    {
        CCart::Get().Front();
    }
    else if (distance > 10 && distance < 20)  // stop
    {
        CCart::Get().Stop();
    }
    else if (distance <= 10)  // go back
    {
        CCart::Get().Back();
    }
    else  // stop
    {
        CCart::Get().Stop();
    }


    return true;
}

bool
CLightFollowing::Process(char &command)
{
    int left_light;
    int right_light;

    if (command == 'S')
    {
        this->m_stopFlag = true;
        return false;
    }

    left_light = analogRead(light_L_Pin);
    right_light = analogRead(light_R_Pin);

    if (left_light > 650 && right_light > 650) //go forward
    {
        CCart::Get().Front();
    }
    else if (left_light > 650 && right_light <= 650)  //turn left
    {
        CCart::Get().Left();
    }
    else if (left_light <= 650 && right_light > 650) //turn right
    {
        CCart::Get().Right();
    }
    else  //or else, stop
    {
        CCart::Get().Stop();
    }

    return true;
}

bool
CTrackingProgram::Process(char &command)
{
    int L_val, M_val, R_val;

    if (command == 'S')
    {
        this->m_stopFlag = true;
        return false;
    }

    L_val = digitalRead(L_pin); // Read the value of the left sensor
    M_val = digitalRead(M_pin); // Read the value of the intermediate sensor
    R_val = digitalRead(R_pin); // Read the value of the sensor on the right

    if (M_val == 1) { //the middle one detects black lines

        if (L_val == 1 && R_val == 0)   //If a black line is detected on the left, but not on the right, turn left
        {
            CCart::Get().Left();
        }
        else if (L_val == 0 && R_val == 1)     //If a black line is detected on the right, not on the left, turn right
        {
            CCart::Get().Right();
        }
        else    //go front
        {
            CCart::Get().Front();
        }
    }
    else  // the middle sensor doesn’t detect black lines
    {
        if (L_val == 1 && R_val == 0)  // If a black line is detected on the left, but not on the right, turn left
        {
            CCart::Get().Left();
        }
        else if (L_val == 0 && R_val == 1)   // If a black line is detected on the right, not on the left, turn right
        {
            CCart::Get().Right();
        }
        else   // otherwise stop
        {

            CCart::Get().Stop();

        }
    }

    return true;

}

bool
CConfinementProgram::Process(char &command)
{
    int L_val, M_val, R_val;

    if (command == 'S')
    {
        this->m_stopFlag = true;
        return false;
    }


    L_val = digitalRead(L_pin); //Read the value of the left sensor
    M_val = digitalRead(M_pin); //Read the value of the intermediate sensor
    R_val = digitalRead(R_pin); //Read the value of the sensor on the right

    if (L_val == 0 && M_val == 0 && R_val == 0) { // Go front when no black lines are detected
        CCart::Get().Front();
    }
    else
    { //

        CCart::Get().Back();
        delay(700);
        CCart::Get().Left();
        delay(800);
    }

    return false;
}

/*******************following***************/
void Follow()
{
    bool exitloop = 0;  // flag invariable, used to enter and exit a mode

    while (exitloop == 0)
    {
        float distance;//Store the distance values detected by ultrasonic for following


        distance = CUltrasonic::Get().distance();  //set the distance value to distance

        if (distance >= 20 && distance <= 60)     // go front
        {
            CCart::Get().Front();
        }
        else if (distance > 10 && distance < 20)  // stop
        {
            CCart::Get().Stop();
        }
        else if (distance <= 10)  //go back
        {
            CCart::Get().Back();
        }
        else  //stop
        {
            CCart::Get().Stop();
        }

        if (Serial.available())
        {
            char bluetooth_val = Serial.read();
            Serial.println(bluetooth_val);

            if (bluetooth_val == 'S')
            {
                exitloop = true;  // exit the loop
                CCart::Get().Stop();
            }
        }

        CCart::Get().Update();
    }

}

/****************light following******************/
void Light_following()
{
    bool flag = 0;  // flag invariable, used to enter and exit a mode
    int left_light;
    int right_light;

    while (flag == 0)
    {

        left_light = analogRead(light_L_Pin);
        right_light = analogRead(light_R_Pin);

        if (left_light > 650 && right_light > 650) //go forward
        {
            CCart::Get().Front();
        }
        else if (left_light > 650 && right_light <= 650)  //turn left
        {
            CCart::Get().Left();
        }
        else if (left_light <= 650 && right_light > 650) //turn right
        {
            CCart::Get().Right();
        }
        else  //or else, stop
        {
            CCart::Get().Stop();
        }

        if (Serial.available())
        {
            char bluetooth_val;       // used to save the Bluetooth value

            bluetooth_val = Serial.read();
            Serial.println(bluetooth_val);

            if (bluetooth_val == 'S') {
                flag = 1;
                CCart::Get().Stop();
            }
        }

        CCart::Get().Update();
    }
}

#else/****use the fan*******/
/***************enable the fan*****************/
void fan_begin() {
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
}

/***************stop fanning*****************/
void fan_stop() {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
}

/***************extinguish fire****************/
void Fire() {
    flag = 0;
    while (flag == 0) {
        //Read the analog value of the flame sensor
        flame_valL = analogRead(flame_L);
        flame_valR = analogRead(flame_R);
        if (flame_valL <= 700 || flame_valR <= 700) {
            Car_Stop();
            fan_begin();
        }
        else {
            fan_stop();
            L_val = digitalRead(L_pin); //Read the value of the left sensor
            M_val = digitalRead(M_pin); //Read the value of the left sensor
            R_val = digitalRead(R_pin); //Read the value of the right sensor

            if (M_val == 1) { //the middle one detects black lines
                if (L_val == 1 && R_val == 0) { //If a black line is detected on the left, but not on the right, turn left
                    Car_left();
                }
                else if (L_val == 0 && R_val == 1) { //If a black line is detected on the right, not on the left, turn right
                    Car_right();
                }
                else { //go front
                    Car_front();
                }
            }
            else { //the middle one detects black lines
                if (L_val == 1 && R_val == 0) { //If a black line is detected on the left, but not on the right, turn left
                    Car_left();
                }
                else if (L_val == 0 && R_val == 1) { //If a black line is detected on the right, not on the left, turn right
                    Car_right();
                }
                else { //otherwise stop
                    Car_Stop();
                }
            }
        }
        if (Serial.available())
        {
            ble_val = Serial.read();
            if (ble_val == 'S') {
                flag = 1;
                Car_Stop();
            }
        }
    }
}

#endif

/***************line tracking*****************/
void Tracking()
{
    int L_val, M_val, R_val;

    bool flag = 0;  // flag invariable, used to enter and exit a mode

    while (flag == 0) {

        L_val = digitalRead(L_pin); // Read the value of the left sensor
        M_val = digitalRead(M_pin); // Read the value of the intermediate sensor
        R_val = digitalRead(R_pin); // Read the value of the sensor on the right

        if (M_val == 1) { //the middle one detects black lines

            if (L_val == 1 && R_val == 0)   //If a black line is detected on the left, but not on the right, turn left
            {
                CCart::Get().Left();
            }
            else if (L_val == 0 && R_val == 1)     //If a black line is detected on the right, not on the left, turn right
            {
                CCart::Get().Right();
            }
            else    //go front
            {
                CCart::Get().Front();
            }
        }
        else  // the middle sensor doesn’t detect black lines
        {
            if (L_val == 1 && R_val == 0)  // If a black line is detected on the left, but not on the right, turn left
            {
                CCart::Get().Left();
            }
            else if (L_val == 0 && R_val == 1)   // If a black line is detected on the right, not on the left, turn right
            {
                CCart::Get().Right();
            }
            else   // otherwise stop
            {

                CCart::Get().Stop();

            }
        }

        if (Serial.available())
        {
            char bluetooth_val = Serial.read();
            Serial.println(bluetooth_val);

            if (bluetooth_val == 'S') {
                flag = 1;

                CCart::Get().Stop();
            }
        }

        CCart::Get().Update();
    }
}

/***************Confinement*****************/
void Confinement()
{
    bool stopFlag = false;  // flag invariable, used to enter and exit a mode
    int L_val, M_val, R_val;

    while (stopFlag == 0) {

        L_val = digitalRead(L_pin); //Read the value of the left sensor
        M_val = digitalRead(M_pin); //Read the value of the intermediate sensor
        R_val = digitalRead(R_pin); //Read the value of the sensor on the right

        if (L_val == 0 && M_val == 0 && R_val == 0) { // Go front when no black lines are detected
            CCart::Get().Front();
        }
        else
        { //

            CCart::Get().Back();
            delay(700);
            CCart::Get().Left();
            delay(800);
        }

        if (Serial.available())
        {
            char bluetooth_val;       // used to save the Bluetooth value

            bluetooth_val = Serial.read();
            Serial.println(bluetooth_val);

            if (bluetooth_val == 'S')
            {

                stopFlag = true;
                CCart::Get().Stop();
            }
        }

        CCart::Get().Update();
    }

}

long _map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//**************************************************************
