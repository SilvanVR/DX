#include "clock.h"
/**********************************************************************
    class: Clock (clock.cpp)
    
    author: S. Hau
    date: October 27, 2017

    @Considerations:
       - Sophisticated ID generator
       - Better data structure for callback-functions
         + sorted linked-list and check only next elem? (Save index where you were at last frame)
**********************************************************************/

#include "locator.h"

namespace Core { namespace Time {


    //----------------------------------------------------------------------
    static U64 NextID()
    {
        static U64 callbackIDs = 0;
        callbackIDs++;
        return callbackIDs;
    }

    //----------------------------------------------------------------------
    Clock::Clock( Milliseconds duration )
        : m_curTime( 0, duration )
    {
    }

    //----------------------------------------------------------------------
    void Clock::tick(Seconds delta)
    {
        m_lastTime = m_curTime.value();
        m_curTime += (delta * m_tickModifier);

        _CheckCallbacks();
    }

    //----------------------------------------------------------------------
    CallbackID Clock::attachCallback( const std::function<void()>& func, Milliseconds ms, ECallFrequency freq )
    {
        AttachedCallback newFunc;
        newFunc.callback   = func;
        newFunc.id         = NextID();
        newFunc.freq       = freq;
        newFunc.time       = ( ms % m_curTime.getUpperBound() );

        m_attachedCallbacks.push_back( newFunc );

        return newFunc.id;
    }

    //----------------------------------------------------------------------
    void Clock::clearCallback(CallbackID id)
    {
        if (id == INVALID_CALLBACK_ID)
            return;

        for (auto it = m_attachedCallbacks.begin(); it != m_attachedCallbacks.end(); it++)
        {
            if (it->id == id)
            {
                m_attachedCallbacks.erase( it );
                return;
            }
        }

        WARN( "Clock::clearCallback(): Given CallbackID #" + TS( id ) + " does not exist." );
    }

    //----------------------------------------------------------------------
    void Clock::_CheckCallbacks()
    {
        for (auto it = m_attachedCallbacks.begin(); it != m_attachedCallbacks.end(); )
        {
            Milliseconds min = m_tickModifier < 0 ? m_curTime.value()   : m_lastTime;
            Milliseconds max = m_tickModifier < 0 ? m_lastTime          : m_curTime.value();

            if ( isBetweenCircular( min, it->time, max ) )
            {
                it->callback();
                switch ( it->freq )
                {
                    case ECallFrequency::ONCE:
                    {
                        it = m_attachedCallbacks.erase( it );
                        continue;
                    }
                    case ECallFrequency::REPEAT:
                        // do nothing
                        break;
                }
            }
            it++;
        }
    }


    //----------------------------------------------------------------------
    //void Clock::_AddFunction(const AttachedCallback& func)
    //{
    //    bool foundSlot = false;
    //    for (auto it = m_attachedFunctions.begin(); it != m_attachedFunctions.end(); it++)
    //    {
    //        if (func.time > m_curTime && func.time < it->time)
    //        {
    //            m_attachedFunctions.insert( it, func );
    //            foundSlot = true;
    //            break;
    //        }
    //    }
    //    if (!foundSlot)
    //        m_attachedFunctions.push_back( func );
    //}

} } // end namespaces