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


        //----------------------------------------------------------------------
        // ISubSystem Interface
        //----------------------------------------------------------------------
        void init() override;
        void update(F32 delta) override;
        void shutdown() override;


    private:
        U32 m_fps = 0;


        //----------------------------------------------------------------------
        Profiler(const Profiler& other)               = delete;
        Profiler& operator = (const Profiler& other)  = delete;
        Profiler(Profiler&& other)                    = delete;
        Profiler& operator = (Profiler&& other)       = delete;
    };


} } // End namespaces