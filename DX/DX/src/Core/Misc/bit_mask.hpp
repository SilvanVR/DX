#pragma once

/**********************************************************************
    class: BitMask (layer_mask.hpp)

    author: S. Hau
    date: October 11, 2017

    See below for a class description.
**********************************************************************/

namespace Core {

    //*********************************************************************
    // Represents an 32-Bit value used as a bit-mask. Allows setting
    // single bits and checking if two layer-masks overlaps etc.
    //*********************************************************************
    class BitMask
    {
        U32 m_bitMask;

    public:
        explicit BitMask(U32 initialMask = 0) 
            : m_bitMask( initialMask ) {}

        //----------------------------------------------------------------------
        bool isBitSet(BitMask other) const { return (m_bitMask & other.m_bitMask) != 0; }
        bool isBitSet(U32 other) const { return (m_bitMask & other) != 0; }
        bool isAnyBitSet() const { return m_bitMask != 0; }

        //----------------------------------------------------------------------
        // Replaces the current mask with the new given mask.
        //----------------------------------------------------------------------
        void setMask(U32 newMask){ m_bitMask = newMask; }

        //----------------------------------------------------------------------
        // Set a bit by the given numerical value! Valid Range: [0 - 31]
        // Example: setBit(3) -> Set's third bit (Numerical value: 8)
        //----------------------------------------------------------------------
        void setBit(U32 bit) { m_bitMask |= (1 << bit); }

        //----------------------------------------------------------------------
        // Unset a bit by the given numerical value. Valid Range: [0 - 31]
        // Example: unsetBit(3) -> Unset's third bit (Numerical value: 8)
        //----------------------------------------------------------------------
        void unsetBit(U32 bit) { m_bitMask &= (~ (1 << bit)); }

        //----------------------------------------------------------------------
        // Set all bits in this layer mask from the given "bits".
        // Same as logical OR (result = mask1 | mask2).
        //----------------------------------------------------------------------
        void setBits(U32 bits) { m_bitMask |= bits; }

        //----------------------------------------------------------------------
        // Unsets all bits in this layer mask from the given "bits".
        // Same as logical AND with the inverse (result = mask1 & (~mask2)).
        //----------------------------------------------------------------------
        void unsetBits(U32 bits) { m_bitMask &= (~bits); }

        //----------------------------------------------------------------------
        // Unset any bit. (BitMask will be 0 after that call)
        //----------------------------------------------------------------------
        void unsetAnyBit() { m_bitMask = 0; }

        //----------------------------------------------------------------------
        BitMask operator & (BitMask other) const { return BitMask( m_bitMask & other.m_bitMask ); }
        BitMask operator | (BitMask other) const { return BitMask( m_bitMask | other.m_bitMask ); }
        BitMask operator ^ (BitMask other) const { return BitMask( m_bitMask ^ other.m_bitMask ); }
        BitMask operator ~ () const { return BitMask( ~m_bitMask ); }
    };


}