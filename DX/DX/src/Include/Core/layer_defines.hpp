#pragma once
/**********************************************************************
    class: None (layer_defines.hpp)

    author: S. Hau
    date: April 28, 2018
**********************************************************************/

#define LAYER_ALL               (~0)
#define LAYER_NONE              (0)
#define LAYER_1                 (1 << 0)
#define LAYER_2                 (1 << 1)
#define LAYER_IGNORE_RAYCASTS   (1 << 8)

#define LAYER_DEFAULT           LAYER_1