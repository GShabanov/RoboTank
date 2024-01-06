#pragma once

#ifndef __SINGLETON__H__
#define __SINGLETON__H__


template<class Impl>
class Singleton
{
public:
    static inline Impl &Get()
    {
        return (Impl &)m_body;
    }

    static inline Impl &CreateInstance()
    {
        if (!m_inited) {

            new Impl();

            m_inited = true;
        }

        return (Impl&)m_body;
    }

protected:
    Singleton() { }

private:
    void* operator new( unsigned int /*size*/ )
    {
        return m_body;
    }

    inline void operator delete( void* /*p*/ )
    {
    }

    static char m_body[];
    static bool m_inited;

    Singleton( const Singleton& );
    Singleton& operator=( const Singleton& ) = delete;
};

template<class Impl>
char Singleton<Impl>::m_body[ sizeof( Impl ) ];

template<class Impl >
bool Singleton<Impl>::m_inited = false;

#endif // __SINGLETON__H__
