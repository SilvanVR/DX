#pragma once

/**********************************************************************
    class: LayerMask (layer_mask.hpp)

    author: S. Hau
    date: October 11, 2017

    Represents an 32-Bit value used as a bit-mask. Allows setting
    single bits and checking if two layer-masks overlaps etc.
**********************************************************************/


namespace Core {


    class LayerMask
    {
        U32 m_layerMask;

    public:
        explicit LayerMask(U32 initialMask = 0) 
            : m_layerMask(initialMask) {}

        //----------------------------------------------------------------------
        bool isBitSet(LayerMask other) const { return (m_layerMask & other.m_layerMask) != 0; }
        bool isBitSet(U32 other) const { return (m_layerMask & other) != 0; }
        bool isAnyBitSet() const { return m_layerMask != 0; }

        //----------------------------------------------------------------------
        // Replaces the current mask with the new given mask.
        //----------------------------------------------------------------------
        void setMask(U32 newMask){ m_layerMask = newMask; }

        //----------------------------------------------------------------------
        // Set a bit by the given numerical value! Valid Range: [0 - 31]
        // Example: setBit(3) -> Set's third bit (Numerical value: 8)
        //----------------------------------------------------------------------
        void setBit(U32 bit) { m_layerMask |= (1 << bit); }

        //----------------------------------------------------------------------
        // Unset a bit by the given numerical value. Valid Range: [0 - 31]
        // Example: unsetBit(3) -> Unset's third bit (Numerical value: 8)
        //----------------------------------------------------------------------
        void unsetBit(U32 bit) { m_layerMask &= (~ (1 << bit)); }

        //----------------------------------------------------------------------
        // Set all bits in this layer mask from the given "bits".
        // Same as logical OR (result = mask1 | mask2).
        //----------------------------------------------------------------------
        void setBits(U32 bits) { m_layerMask |= bits; }

        //----------------------------------------------------------------------
        // Unsets all bits in this layer mask from the given "bits".
        // Same as logical AND with the inverse (result = mask1 & (~mask2)).
        //----------------------------------------------------------------------
        void unsetBits(U32 bits) { m_layerMask &= (~bits); }

        //----------------------------------------------------------------------
        LayerMask operator & (LayerMask other) const { return LayerMask(m_layerMask & other.m_layerMask); }
        LayerMask operator | (LayerMask other) const { return LayerMask(m_layerMask | other.m_layerMask); }
        LayerMask operator ^ (LayerMask other) const { return LayerMask(m_layerMask ^ other.m_layerMask); }
        LayerMask operator ~ () const { return LayerMask(~m_layerMask); }
    };


}