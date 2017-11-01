#pragma once
/**********************************************************************
    class: Profiler (profiler.h)

    author: S. Hau
    date: October 28, 2017

    @TODO:
      - Add gpu profiling
**********************************************************************/

#include "Core/i_subsystem.hpp"

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
        void update(Time::Seconds delta) override;
        void shutdown() override;

        //----------------------------------------------------------------------
        U32 getFPS() const { return m_fps; }

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


    private:
        U32                     m_fps = 0;

        // Maps [Name] <-> [Time]
        std::map<StringID, U64> m_entries;


        //----------------------------------------------------------------------
        Profiler(const Profiler& other)               = delete;
        Profiler& operator = (const Profiler& other)  = delete;
        Profiler(Profiler&& other)                    = delete;
        Profiler& operator = (Profiler&& other)       = delete;
    };


} } // End namespaces