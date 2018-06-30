#pragma once
/**********************************************************************
    class: RenderSystem (render_system.h)

    author: S. Hau
    date: June 30, 2018
**********************************************************************/

namespace Core {

    //**********************************************************************
    class RenderSystem
    {
    public:
        static RenderSystem& Instance()
        {
            static RenderSystem rs;
            return rs;
        }

        void execute(F32 lerp);

    private:
        RenderSystem() = default;
        NULL_COPY_AND_ASSIGN(RenderSystem)
    };

}

