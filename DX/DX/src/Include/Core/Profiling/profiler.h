#pragma once
/**********************************************************************
    class: Profiler (profiler.h)

    author: S. Hau
    date: October 28, 2017

    @TODO:
      - Add gpu profiling
**********************************************************************/

#include "Common/i_subsystem.hpp"

namespace Core { namespace Profiling {

    //**********************************************************************
    class Profiler : public ISubSystem
    {
    public:
        Profiler() = default;
        ~Profiler() = default;

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void OnUpdate(Time::Seconds delta) override;
        void OnTick(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        U32                 getFPS()            const { return m_fps; }
        Time::Milliseconds  getUpdateDelta()    const { return m_updateDelta; }
        Time::Seconds       getDelta()          const { return m_tickDelta; }

        //----------------------------------------------------------------------
        // Start a new profiling section.
        // @Params:
        //  "name": The name of the section, by which the measurements can be found.
        //----------------------------------------------------------------------
        void profileCodeSectionBegin( const char* name );

        //----------------------------------------------------------------------
        // End a profiling section.
        // @Params:
        //  "name": The name of the section, by which the measurements can be found.
        //----------------------------------------------------------------------
        void profileCodeSectionEnd( const char* name );

        //----------------------------------------------------------------------
        // End a profiling section.
        // @Return:
        //   The time the given code section took.
        //----------------------------------------------------------------------
        F64 getCodeSectionTime( const char* name );

        //----------------------------------------------------------------------
        // Log the whole profiling stuff to the console.
        //----------------------------------------------------------------------
        void log();

        //----------------------------------------------------------------------
        // Log gpu profiling stuff (Drawcalls, num vertices etc) to the console.
        //----------------------------------------------------------------------
        void logGPU();

    private:
        U32                 m_fps = 0;
        Time::Milliseconds  m_updateDelta = 0.0f;
        Time::Seconds       m_tickDelta = 0.0f;

        // Maps [Name] <-> [Time]
        HashMap<StringID, U64> m_entries;

        NULL_COPY_AND_ASSIGN(Profiler)
    };


} } // End namespaces