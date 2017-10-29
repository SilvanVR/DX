#pragma once
/**********************************************************************
    class: Profiler (profiler.h)

    author: S. Hau
    date: October 28, 2017

    @TODO:
      - Add mechanism for measuring code sections
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
        U32 getFPS() const { return m_fps; }

        //----------------------------------------------------------------------
        void profileCodeSectionBegin( const char* name );
        void profileCodeSectionEnd( const char* name );

        F64 getCodeSectionTime( const char* name );


        void log();

        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void update(F32 delta) override;
        void shutdown() override;


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