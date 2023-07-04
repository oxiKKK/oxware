//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef COMMONMACROS_H
#define COMMONMACROS_H

//=============================================================================
//
// commonmacros.h
//
//	This should contain ONLY general purpose macros that are 
//	appropriate for use in engine/launcher/all tools
//
//=============================================================================

#define V_min(a,b)						(((a) < (b)) ? (a) : (b))
#define V_max(a,b)						(((a) > (b)) ? (a) : (b))
#define V_clamp(val, min, max)			(((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

// Makes a 4-byte "packed ID" int out of 4 characters
#define MAKEID(d,c,b,a)					(((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | ((int)(d)))

// Compares a string with a 4-byte packed ID constant
#define STRING_MATCHES_ID(p, id)		((*((int *)(p)) == (id)) ? true : false)
#define ID_TO_STRING(id, p)				((p)[3] = (((id)>>24) & 0xFF), (p)[2] = (((id)>>16) & 0xFF), (p)[1] = (((id)>>8) & 0xFF), (p)[0] = (((id)>>0) & 0xFF))

#define ALIGN_VALUE(val, alignment) ((val + alignment - 1) & ~(alignment - 1)) //  need macro for constant expression

#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif
#define Q_ARRAYSIZE(p)					(sizeof(p)/sizeof(p[0]))

// Bit manipulators
#define FBitSet(iBitVector, bit)		(!!((iBitVector) & (bit)))
#define FSetBit(iBitVector, bit)		((iBitVector) |= (bit))
#define FClrBit(iBitVector, bit)		((iBitVector) &= ~(bit))

template <typename T>
inline bool IsPowerOfTwo(T value)
{
	return (value & (value - (T)1)) == (T)0;
}

#endif // COMMONMACROS_H