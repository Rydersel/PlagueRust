// lea_simd.cpp - written and placed in the public domain by Jeffrey Walton
//
//    This source file uses intrinsics and built-ins to gain access to
//    SSSE3, ARM NEON and ARMv8a, and Power8 Altivec instructions. A separate
//    source file is needed because additional CXXFLAGS are required to enable
//    the appropriate instructions sets in some build configurations.

#include "pch.h"
#include "config.h"

#include "lea.h"
#include "misc.h"

// Uncomment for benchmarking C++ against SSE or NEON.
// Do so in both simon.cpp and simon-simd.cpp.
// #undef CRYPTOPP_SSSE3_AVAILABLE
// #undef CRYPTOPP_ARM_NEON_AVAILABLE

#if (CRYPTOPP_SSSE3_AVAILABLE)
# include "adv_simd.h"
# include <pmmintrin.h>
# include <tmmintrin.h>
#endif

#if defined(__XOP__)
# include <ammintrin.h>
#endif

#if defined(__AVX512F__)
# define CRYPTOPP_AVX512_ROTATE 1
# include <immintrin.h>
#endif

// C1189: error: This header is specific to ARM targets
#if (CRYPTOPP_ARM_NEON_AVAILABLE)
# include "adv_simd.h"
# ifndef _M_ARM64
#  include <arm_neon.h>
# endif
#endif

#if (CRYPTOPP_ARM_ACLE_AVAILABLE)
# include <stdint.h>
# include <arm_acle.h>
#endif

// Do not port this to POWER architecture. Naively we hoped
// for a 2x to 3x speedup. The result was a 5x slow down.
// The table below shows MiB/s and cpb.
//
// C++:
// <TD>LEA-128(128)/CTR (128-bit key)<TD>C++<TD>207<TD>15.64
// <TD>LEA-128(192)/CTR (192-bit key)<TD>C++<TD>186<TD>17.48
// <TD>LEA-128(256)/CTR (256-bit key)<TD>C++<TD>124<TD>26.2
//
// Power8:
// <TD>LEA-128(128)/CTR (128-bit key)<TD>Power8<TD>37<TD>88.7
// <TD>LEA-128(192)/CTR (192-bit key)<TD>Power8<TD>40<TD>82.1
// <TD>LEA-128(256)/CTR (256-bit key)<TD>Power8<TD>28<TD>116.0

#undef CRYPTOPP_POWER8_AVAILABLE
#if defined(CRYPTOPP_POWER8_AVAILABLE)
# include "adv_simd.h"
# include "ppc_simd.h"
#endif

// Squash MS LNK4221 and libtool warnings
extern const char LEA_SIMD_FNAME[] = __FILE__;

ANONYMOUS_NAMESPACE_BEGIN

using CryptoPP::word32;

// *************************** ARM NEON ***************************//

#if (CRYPTOPP_ARM_NEON_AVAILABLE)

inline uint32x4_t Xor(const uint32x4_t& a, const uint32x4_t& b)
{
    return veorq_u32(a, b);
}

inline uint32x4_t Add(const uint32x4_t& a, const uint32x4_t& b)
{
    return vaddq_u32(a, b);
}

inline uint32x4_t Sub(const uint32x4_t& a, const uint32x4_t& b)
{
    return vsubq_u32(a, b);
}

template <unsigned int R>
inline uint32x4_t RotateLeft(const uint32x4_t& val)
{
    const uint32x4_t a(vshlq_n_u32(val, R));
    const uint32x4_t b(vshrq_n_u32(val, 32 - R));
    return vorrq_u32(a, b);
}

template <unsigned int R>
inline uint32x4_t RotateRight(const uint32x4_t& val)
{
    const uint32x4_t a(vshlq_n_u32(val, 32 - R));
    const uint32x4_t b(vshrq_n_u32(val, R));
    return vorrq_u32(a, b);
}

#if defined(__aarch32__) || defined(__aarch64__)
template <>
inline uint32x4_t RotateLeft<8>(const uint32x4_t& val)
{
#if (CRYPTOPP_BIG_ENDIAN)
    const uint8_t maskb[16] = { 14,13,12,15, 10,9,8,11, 6,5,4,7, 2,1,0,3 };
    const uint8x16_t mask = vld1q_u8(maskb);
#else
    const uint8_t maskb[16] = { 3,0,1,2, 7,4,5,6, 11,8,9,10, 15,12,13,14 };
    const uint8x16_t mask = vld1q_u8(maskb);
#endif

    return vreinterpretq_u32_u8(
        vqtbl1q_u8(vreinterpretq_u8_u32(val), mask));
}

template <>
inline uint32x4_t RotateRight<8>(const uint32x4_t& val)
{
#if (CRYPTOPP_BIG_ENDIAN)
    const uint8_t maskb[16] = { 12,15,14,13, 8,11,10,9, 4,7,6,5, 0,3,2,1 };
    const uint8x16_t mask = vld1q_u8(maskb);
#else
    const uint8_t maskb[16] = { 1,2,3,0, 5,6,7,4, 9,10,11,8, 13,14,14,12 };
    const uint8x16_t mask = vld1q_u8(maskb);
#endif

    return vreinterpretq_u32_u8(
        vqtbl1q_u8(vreinterpretq_u8_u32(val), mask));
}
#endif

uint32x4_t UnpackLow32(uint32x4_t a, uint32x4_t b)
{
    uint32x2_t a1 = vget_low_u32(a);
    uint32x2_t b1 = vget_low_u32(b);
    uint32x2x2_t result = vzip_u32(a1, b1);
    return vcombine_u32(result.val[0], result.val[1]);
}

uint32x4_t UnpackHigh32(uint32x4_t a, uint32x4_t b)
{
    uint32x2_t a1 = vget_high_u32(a);
    uint32x2_t b1 = vget_high_u32(b);
    uint32x2x2_t result = vzip_u32(a1, b1);
    return vcombine_u32(result.val[0], result.val[1]);
}

uint32x4_t UnpackLow64(uint32x4_t a, uint32x4_t b)
{
    uint64x1_t a1 = vget_low_u64((uint64x2_t)a);
    uint64x1_t b1 = vget_low_u64((uint64x2_t)b);
    return (uint32x4_t)vcombine_u64(a1, b1);
}

uint32x4_t UnpackHigh64(uint32x4_t a, uint32x4_t b)
{
    uint64x1_t a1 = vget_high_u64((uint64x2_t)a);
    uint64x1_t b1 = vget_high_u64((uint64x2_t)b);
    return (uint32x4_t)vcombine_u64(a1, b1);
}

template <unsigned int IDX>
inline uint32x4_t LoadKey(const word32 rkey[])
{
    return vdupq_n_u32(rkey[IDX]);
}

template <unsigned int IDX>
inline uint32x4_t UnpackNEON(const uint32x4_t& a, const uint32x4_t& b, const uint32x4_t& c, const uint32x4_t& d)
{
    // Should not be instantiated
    CRYPTOPP_ASSERT(0);;
    return vmovq_n_u32(0);
}

template <>
inline uint32x4_t UnpackNEON<0>(const uint32x4_t& a, const uint32x4_t& b, const uint32x4_t& c, const uint32x4_t& d)
{
    const uint32x4_t r1 = UnpackLow32(a, b);
    const uint32x4_t r2 = UnpackLow32(c, d);
    return UnpackLow64(r1, r2);
}

template <>
inline uint32x4_t UnpackNEON<1>(const uint32x4_t& a, const uint32x4_t& b, const uint32x4_t& c, const uint32x4_t& d)
{
    const uint32x4_t r1 = UnpackLow32(a, b);
    const uint32x4_t r2 = UnpackLow32(c, d);
    return UnpackHigh64(r1, r2);
}

template <>
inline uint32x4_t UnpackNEON<2>(const uint32x4_t& a, const uint32x4_t& b, const uint32x4_t& c, const uint32x4_t& d)
{
    const uint32x4_t r1 = UnpackHigh32(a, b);
    const uint32x4_t r2 = UnpackHigh32(c, d);
    return UnpackLow64(r1, r2);
}

template <>
inline uint32x4_t UnpackNEON<3>(const uint32x4_t& a, const uint32x4_t& b, const uint32x4_t& c, const uint32x4_t& d)
{
    const uint32x4_t r1 = UnpackHigh32(a, b);
    const uint32x4_t r2 = UnpackHigh32(c, d);
    return UnpackHigh64(r1, r2);
}

template <unsigned int IDX>
inline uint32x4_t UnpackNEON(const uint32x4_t& v)
{
    // Should not be instantiated
    CRYPTOPP_ASSERT(0);;
    return vmovq_n_u32(0);
}

template <>
inline uint32x4_t UnpackNEON<0>(const uint32x4_t& v)
{
    // Splat to all lanes
    return vdupq_n_u32(vgetq_lane_u32(v, 0));
}

template <>
inline uint32x4_t UnpackNEON<1>(const uint32x4_t& v)
{
    // Splat to all lanes
    return vdupq_n_u32(vgetq_lane_u32(v, 1));
}

template <>
inline uint32x4_t UnpackNEON<2>(const uint32x4_t& v)
{
    // Splat to all lanes
    return vdupq_n_u32(vgetq_lane_u32(v, 2));
}

template <>
inline uint32x4_t UnpackNEON<3>(const uint32x4_t& v)
{
    // Splat to all lanes
    return vdupq_n_u32(vgetq_lane_u32(v, 3));
}

template <unsigned int IDX>
inline uint32x4_t RepackNEON(const uint32x4_t& a, const uint32x4_t& b, const uint32x4_t& c, const uint32x4_t& d)
{
    return UnpackNEON<IDX>(a, b, c, d);
}

template <unsigned int IDX>
inline uint32x4_t RepackNEON(const uint32x4_t& v)
{
    return UnpackNEON<IDX>(v);
}

#endif  // CRYPTOPP_ARM_NEON_AVAILABLE

// *************************** IA-32 ***************************//

#if (CRYPTOPP_SSSE3_AVAILABLE)

inline __m128i Xor(const __m128i& a, const __m128i& b)
{
    return _mm_xor_si128(a, b);
}

inline __m128i Add(const __m128i& a, const __m128i& b)
{
    return _mm_add_epi32(a, b);
}

inline __m128i Sub(const __m128i& a, const __m128i& b)
{
    return _mm_sub_epi32(a, b);
}

template <unsigned int R>
inline __m128i RotateLeft(const __m128i& val)
{
#if defined(__XOP__)
    return _mm_roti_epi32(val, R);
#else
    return _mm_or_si128(
        _mm_slli_epi32(val, R), _mm_srli_epi32(val, 32-R));
#endif
}

template <unsigned int R>
inline __m128i RotateRight(const __m128i& val)
{
#if defined(__XOP__)
    return _mm_roti_epi32(val, 32-R);
#else
    return _mm_or_si128(
        _mm_slli_epi32(val, 32-R), _mm_srli_epi32(val, R));
#endif
}

// Faster than two Shifts and an Or.
template <>
inline __m128i RotateLeft<8>(const __m128i& val)
{
#if defined(__XOP__)
    return _mm_roti_epi32(val, 8);
#else
    const __m128i mask = _mm_set_epi8(14,13,12,15, 10,9,8,11, 6,5,4,7, 2,1,0,3);
    return _mm_shuffle_epi8(val, mask);
#endif
}

// Faster than two Shifts and an Or.
template <>
inline __m128i RotateRight<8>(const __m128i& val)
{
#if defined(__XOP__)
    return _mm_roti_epi32(val, 32-8);
#else
    const __m128i mask = _mm_set_epi8(12,15,14,13, 8,11,10,9, 4,7,6,5, 0,3,2,1);
    return _mm_shuffle_epi8(val, mask);
#endif
}

template <unsigned int IDX>
inline __m128i LoadKey(const word32 rkey[])
{
    float rk; std::memcpy(&rk, rkey+IDX, sizeof(rk));
    return _mm_castps_si128(_mm_load_ps1(&rk));
}

template <unsigned int IDX>
inline __m128i UnpackXMM(const __m128i& a, const __m128i& b, const __m128i& c, const __m128i& d)
{
    // Should not be instantiated
    CRYPTOPP_UNUSED(a); CRYPTOPP_UNUSED(b);
    CRYPTOPP_UNUSED(c); CRYPTOPP_UNUSED(d);
    CRYPTOPP_ASSERT(0);
    return _mm_setzero_si128();
}

template <>
inline __m128i UnpackXMM<0>(const __m128i& a, const __m128i& b, const __m128i& c, const __m128i& d)
{
    // LEA is little-endian oriented, so there is no need for a separate shuffle.
    const __m128i r1 = _mm_unpacklo_epi32(a, b);
    const __m128i r2 = _mm_unpacklo_epi32(c, d);
    return _mm_unpacklo_epi64(r1, r2);
}

template <>
inline __m128i UnpackXMM<1>(const __m128i& a, const __m128i& b, const __m128i& c, const __m128i& d)
{
    // LEA is little-endian oriented, so there is no need for a separate shuffle.
    const __m128i r1 = _mm_unpacklo_epi32(a, b);
    const __m128i r2 = _mm_unpacklo_epi32(c, d);
    return _mm_unpackhi_epi64(r1, r2);
}

template <>
inline __m128i UnpackXMM<2>(const __m128i& a, const __m128i& b, const __m128i& c, const __m128i& d)
{
    // LEA is little-endian oriented, so there is no need for a separate shuffle.
    const __m128i r1 = _mm_unpackhi_epi32(a, b);
    const __m128i r2 = _mm_unpackhi_epi32(c, d);
    return _mm_unpacklo_epi64(r1, r2);
}

template <>
inline __m128i UnpackXMM<3>(const __m128i& a, const __m128i& b, const __m128i& c, const __m128i& d)
{
    // LEA is little-endian oriented, so there is no need for a separate shuffle.
    const __m128i r1 = _mm_unpackhi_epi32(a, b);
    const __m128i r2 = _mm_unpackhi_epi32(c, d);
    return _mm_unpackhi_epi64(r1, r2);
}

template <unsigned int IDX>
inline __m128i UnpackXMM(const __m128i& v)
{
    // Should not be instantiated
    CRYPTOPP_UNUSED(v); CRYPTOPP_ASSERT(0);
    return _mm_setzero_si128();
}

template <>
inline __m128i UnpackXMM<0>(const __m128i& v)
{
    // Splat to all lanes
    return _mm_shuffle_epi8(v, _mm_set_epi8(3,2,1,0, 3,2,1,0, 3,2,1,0, 3,2,1,0));
}

template <>
inline __m128i UnpackXMM<1>(const __m128i& v)
{
    // Splat to all lanes
    return _mm_shuffle_epi8(v, _mm_set_epi8(7,6,5,4, 7,6,5,4, 7,6,5,4, 7,6,5,4));
}

template <>
inline __m128i UnpackXMM<2>(const __m128i& v)
{
    // Splat to all lanes
    return _mm_shuffle_epi8(v, _mm_set_epi8(11,10,9,8, 11,10,9,8, 11,10,9,8, 11,10,9,8));
}

template <>
inline __m128i UnpackXMM<3>(const __m128i& v)
{
    // Splat to all lanes
    return _mm_shuffle_epi8(v, _mm_set_epi8(15,14,13,12, 15,14,13,12, 15,14,13,12, 15,14,13,12));
}

template <unsigned int IDX>
inline __m128i RepackXMM(const __m128i& a, const __m128i& b, const __m128i& c, const __m128i& d)
{
    return UnpackXMM<IDX>(a, b, c, d);
}

template <unsigned int IDX>
inline __m128i RepackXMM(const __m128i& v)
{
    return UnpackXMM<IDX>(v);
}

#endif  // CRYPTOPP_SSSE3_AVAILABLE

// *************************** Power8 ***************************//

#if (CRYPTOPP_POWER8_AVAILABLE)

using CryptoPP::uint8x16_p;
using CryptoPP::uint32x4_p;
using CryptoPP::uint64x2_p;

inline uint32x4_p Xor(const uint32x4_p& a, const uint32x4_p& b)
{
    return VecXor(a, b);
}

inline uint32x4_p Add(const uint32x4_p& a, const uint32x4_p& b)
{
    return VecAdd(a, b);
}

inline uint32x4_p Sub(const uint32x4_p& a, const uint32x4_p& b)
{
    return VecSub(a, b);
}

template <unsigned int R>
inline uint32x4_p RotateLeft(const uint32x4_p& val)
{
    const uint32x4_p m = {R, R, R, R};
    return vec_rl(val, m);
}

template <unsigned int R>
inline uint32x4_p RotateRight(const uint32x4_p& val)
{
    const uint32x4_p m = {32-R, 32-R, 32-R, 32-R};
    return vec_rl(val, m);
}

template <unsigned int IDX>
inline uint32x4_p LoadKey(const word32 rkey[])
{
    return vec_splats(rkey[IDX]);
}

template <unsigned int IDX>
inline uint32x4_p UnpackSIMD(const uint32x4_p& a, const uint32x4_p& b, const uint32x4_p& c, const uint32x4_p& d)
{
    // Should not be instantiated
    CRYPTOPP_UNUSED(a); CRYPTOPP_UNUSED(b);
    CRYPTOPP_UNUSED(c); CRYPTOPP_UNUSED(d);
    CRYPTOPP_ASSERT(0);
    return VecXor(a, a);
}

template <>
inline uint32x4_p UnpackSIMD<0>(const uint32x4_p& a, const uint32x4_p& b, const uint32x4_p& c, const uint32x4_p& d)
{
    const uint64x2_p r1 = (uint64x2_p)vec_mergel(a, b);
    const uint64x2_p r2 = (uint64x2_p)vec_mergel(c, d);
    return (uint32x4_p)vec_mergel(r1, r2);
}

template <>
inline uint32x4_p UnpackSIMD<1>(const uint32x4_p& a, const uint32x4_p& b, const uint32x4_p& c, const uint32x4_p& d)
{
    const uint64x2_p r1 = (uint64x2_p)vec_mergel(a, b);
    const uint64x2_p r2 = (uint64x2_p)vec_mergel(c, d);
    return (uint32x4_p)vec_mergeh(r1, r2);
}

template <>
inline uint32x4_p UnpackSIMD<2>(const uint32x4_p& a, const uint32x4_p& b, const uint32x4_p& c, const uint32x4_p& d)
{
    const uint64x2_p r1 = (uint64x2_p)vec_mergeh(a, b);
    const uint64x2_p r2 = (uint64x2_p)vec_mergeh(c, d);
    return (uint32x4_p)vec_mergel(r1, r2);
}

template <>
inline uint32x4_p UnpackSIMD<3>(const uint32x4_p& a, const uint32x4_p& b, const uint32x4_p& c, const uint32x4_p& d)
{
    const uint64x2_p r1 = (uint64x2_p)vec_mergeh(a, b);
    const uint64x2_p r2 = (uint64x2_p)vec_mergeh(c, d);
    return (uint32x4_p)vec_mergeh(r1, r2);
}

template <unsigned int IDX>
inline uint32x4_p UnpackSIMD(const uint32x4_p& v)
{
    // Should not be instantiated
    CRYPTOPP_ASSERT(0);
    return VecXor(v, v);
}

template <>
inline uint32x4_p UnpackSIMD<0>(const uint32x4_p& v)
{
    // Splat to all lanes
    const uint8x16_p m = {3,2,1,0, 3,2,1,0, 3,2,1,0, 3,2,1,0};
    return (uint32x4_p)VecPermute(v, v, m);
}

template <>
inline uint32x4_p UnpackSIMD<1>(const uint32x4_p& v)
{
    // Splat to all lanes
    const uint8x16_p m = {7,6,5,4, 7,6,5,4, 7,6,5,4, 7,6,5,4};
    return (uint32x4_p)VecPermute(v, v, m);
}

template <>
inline uint32x4_p UnpackSIMD<2>(const uint32x4_p& v)
{
    // Splat to all lanes
    const uint8x16_p m = {11,10,9,8, 11,10,9,8, 11,10,9,8, 11,10,9,8};
    return (uint32x4_p)VecPermute(v, v, m);
}

template <>
inline uint32x4_p UnpackSIMD<3>(const uint32x4_p& v)
{
    // Splat to all lanes
    const uint8x16_p m = {15,14,13,12, 15,14,13,12, 15,14,13,12, 15,14,13,12};
    return (uint32x4_p)VecPermute(v, v, m);
}

template <unsigned int IDX>
inline uint32x4_p RepackSIMD(const uint32x4_p& a, const uint32x4_p& b, const uint32x4_p& c, const uint32x4_p& d)
{
    return UnpackSIMD<IDX>(a, b, c, d);
}

template <unsigned int IDX>
inline uint32x4_p RepackSIMD(const uint32x4_p& v)
{
    return UnpackSIMD<IDX>(v);
}

#endif  // CRYPTOPP_POWER8_AVAILABLE

// *************************** LEA Encryption ***************************//

#if (CRYPTOPP_ARM_NEON_AVAILABLE || CRYPTOPP_SSSE3_AVAILABLE)

template <class W>
inline void LEA_Encryption(W temp[4], const word32 *subkeys, unsigned int rounds)
{
    temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<4>(subkeys)), Xor(temp[3], LoadKey<5>(subkeys))));
    temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<2>(subkeys)), Xor(temp[2], LoadKey<3>(subkeys))));
    temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<0>(subkeys)), Xor(temp[1], LoadKey<1>(subkeys))));
    temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<10>(subkeys)), Xor(temp[0], LoadKey<11>(subkeys))));
    temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<8>(subkeys)), Xor(temp[3], LoadKey<9>(subkeys))));
    temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<6>(subkeys)), Xor(temp[2], LoadKey<7>(subkeys))));
    temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<16>(subkeys)), Xor(temp[1], LoadKey<17>(subkeys))));
    temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<14>(subkeys)), Xor(temp[0], LoadKey<15>(subkeys))));
    temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<12>(subkeys)), Xor(temp[3], LoadKey<13>(subkeys))));
    temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<22>(subkeys)), Xor(temp[2], LoadKey<23>(subkeys))));
    temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<20>(subkeys)), Xor(temp[1], LoadKey<21>(subkeys))));
    temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<18>(subkeys)), Xor(temp[0], LoadKey<19>(subkeys))));

    temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<28>(subkeys)), Xor(temp[3], LoadKey<29>(subkeys))));
    temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<26>(subkeys)), Xor(temp[2], LoadKey<27>(subkeys))));
    temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<24>(subkeys)), Xor(temp[1], LoadKey<25>(subkeys))));
    temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<34>(subkeys)), Xor(temp[0], LoadKey<35>(subkeys))));
    temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<32>(subkeys)), Xor(temp[3], LoadKey<33>(subkeys))));
    temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<30>(subkeys)), Xor(temp[2], LoadKey<31>(subkeys))));
    temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<40>(subkeys)), Xor(temp[1], LoadKey<41>(subkeys))));
    temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<38>(subkeys)), Xor(temp[0], LoadKey<39>(subkeys))));
    temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<36>(subkeys)), Xor(temp[3], LoadKey<37>(subkeys))));
    temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<46>(subkeys)), Xor(temp[2], LoadKey<47>(subkeys))));
    temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<44>(subkeys)), Xor(temp[1], LoadKey<45>(subkeys))));
    temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<42>(subkeys)), Xor(temp[0], LoadKey<43>(subkeys))));

    temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<52>(subkeys)), Xor(temp[3], LoadKey<53>(subkeys))));
    temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<50>(subkeys)), Xor(temp[2], LoadKey<51>(subkeys))));
    temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<48>(subkeys)), Xor(temp[1], LoadKey<49>(subkeys))));
    temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<58>(subkeys)), Xor(temp[0], LoadKey<59>(subkeys))));
    temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<56>(subkeys)), Xor(temp[3], LoadKey<57>(subkeys))));
    temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<54>(subkeys)), Xor(temp[2], LoadKey<55>(subkeys))));
    temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<64>(subkeys)), Xor(temp[1], LoadKey<65>(subkeys))));
    temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<62>(subkeys)), Xor(temp[0], LoadKey<63>(subkeys))));
    temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<60>(subkeys)), Xor(temp[3], LoadKey<61>(subkeys))));
    temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<70>(subkeys)), Xor(temp[2], LoadKey<71>(subkeys))));
    temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<68>(subkeys)), Xor(temp[1], LoadKey<69>(subkeys))));
    temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<66>(subkeys)), Xor(temp[0], LoadKey<67>(subkeys))));

    temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<76>(subkeys)), Xor(temp[3], LoadKey<77>(subkeys))));
    temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<74>(subkeys)), Xor(temp[2], LoadKey<75>(subkeys))));
    temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<72>(subkeys)), Xor(temp[1], LoadKey<73>(subkeys))));
    temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<82>(subkeys)), Xor(temp[0], LoadKey<83>(subkeys))));
    temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<80>(subkeys)), Xor(temp[3], LoadKey<81>(subkeys))));
    temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<78>(subkeys)), Xor(temp[2], LoadKey<79>(subkeys))));
    temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<88>(subkeys)), Xor(temp[1], LoadKey<89>(subkeys))));
    temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<86>(subkeys)), Xor(temp[0], LoadKey<87>(subkeys))));
    temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<84>(subkeys)), Xor(temp[3], LoadKey<85>(subkeys))));
    temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<94>(subkeys)), Xor(temp[2], LoadKey<95>(subkeys))));
    temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<92>(subkeys)), Xor(temp[1], LoadKey<93>(subkeys))));
    temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<90>(subkeys)), Xor(temp[0], LoadKey<91>(subkeys))));

    temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<100>(subkeys)), Xor(temp[3], LoadKey<101>(subkeys))));
    temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<98>(subkeys)), Xor(temp[2], LoadKey<99>(subkeys))));
    temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<96>(subkeys)), Xor(temp[1], LoadKey<97>(subkeys))));
    temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<106>(subkeys)), Xor(temp[0], LoadKey<107>(subkeys))));
    temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<104>(subkeys)), Xor(temp[3], LoadKey<105>(subkeys))));
    temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<102>(subkeys)), Xor(temp[2], LoadKey<103>(subkeys))));
    temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<112>(subkeys)), Xor(temp[1], LoadKey<113>(subkeys))));
    temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<110>(subkeys)), Xor(temp[0], LoadKey<111>(subkeys))));
    temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<108>(subkeys)), Xor(temp[3], LoadKey<109>(subkeys))));
    temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<118>(subkeys)), Xor(temp[2], LoadKey<119>(subkeys))));
    temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<116>(subkeys)), Xor(temp[1], LoadKey<117>(subkeys))));
    temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<114>(subkeys)), Xor(temp[0], LoadKey<115>(subkeys))));

    temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<124>(subkeys)), Xor(temp[3], LoadKey<125>(subkeys))));
    temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<122>(subkeys)), Xor(temp[2], LoadKey<123>(subkeys))));
    temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<120>(subkeys)), Xor(temp[1], LoadKey<121>(subkeys))));
    temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<130>(subkeys)), Xor(temp[0], LoadKey<131>(subkeys))));
    temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<128>(subkeys)), Xor(temp[3], LoadKey<129>(subkeys))));
    temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<126>(subkeys)), Xor(temp[2], LoadKey<127>(subkeys))));
    temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<136>(subkeys)), Xor(temp[1], LoadKey<137>(subkeys))));
    temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<134>(subkeys)), Xor(temp[0], LoadKey<135>(subkeys))));
    temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<132>(subkeys)), Xor(temp[3], LoadKey<133>(subkeys))));
    temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<142>(subkeys)), Xor(temp[2], LoadKey<143>(subkeys))));
    temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<140>(subkeys)), Xor(temp[1], LoadKey<141>(subkeys))));
    temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<138>(subkeys)), Xor(temp[0], LoadKey<139>(subkeys))));

    if(rounds > 24)
    {
        temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<148>(subkeys)), Xor(temp[3], LoadKey<149>(subkeys))));
        temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<146>(subkeys)), Xor(temp[2], LoadKey<147>(subkeys))));
        temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<144>(subkeys)), Xor(temp[1], LoadKey<145>(subkeys))));
        temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<154>(subkeys)), Xor(temp[0], LoadKey<155>(subkeys))));
        temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<152>(subkeys)), Xor(temp[3], LoadKey<153>(subkeys))));
        temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<150>(subkeys)), Xor(temp[2], LoadKey<151>(subkeys))));
        temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<160>(subkeys)), Xor(temp[1], LoadKey<161>(subkeys))));
        temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<158>(subkeys)), Xor(temp[0], LoadKey<159>(subkeys))));
        temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<156>(subkeys)), Xor(temp[3], LoadKey<157>(subkeys))));
        temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<166>(subkeys)), Xor(temp[2], LoadKey<167>(subkeys))));
        temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<164>(subkeys)), Xor(temp[1], LoadKey<165>(subkeys))));
        temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<162>(subkeys)), Xor(temp[0], LoadKey<163>(subkeys))));
    }

    if(rounds > 28)
    {
        temp[3] = RotateRight<3>(Add(Xor(temp[2], LoadKey<172>(subkeys)), Xor(temp[3], LoadKey<173>(subkeys))));
        temp[2] = RotateRight<5>(Add(Xor(temp[1], LoadKey<170>(subkeys)), Xor(temp[2], LoadKey<171>(subkeys))));
        temp[1] = RotateLeft<9>(Add(Xor(temp[0], LoadKey<168>(subkeys)), Xor(temp[1], LoadKey<169>(subkeys))));
        temp[0] = RotateRight<3>(Add(Xor(temp[3], LoadKey<178>(subkeys)), Xor(temp[0], LoadKey<179>(subkeys))));
        temp[3] = RotateRight<5>(Add(Xor(temp[2], LoadKey<176>(subkeys)), Xor(temp[3], LoadKey<177>(subkeys))));
        temp[2] = RotateLeft<9>(Add(Xor(temp[1], LoadKey<174>(subkeys)), Xor(temp[2], LoadKey<175>(subkeys))));
        temp[1] = RotateRight<3>(Add(Xor(temp[0], LoadKey<184>(subkeys)), Xor(temp[1], LoadKey<185>(subkeys))));
        temp[0] = RotateRight<5>(Add(Xor(temp[3], LoadKey<182>(subkeys)), Xor(temp[0], LoadKey<183>(subkeys))));
        temp[3] = RotateLeft<9>(Add(Xor(temp[2], LoadKey<180>(subkeys)), Xor(temp[3], LoadKey<181>(subkeys))));
        temp[2] = RotateRight<3>(Add(Xor(temp[1], LoadKey<190>(subkeys)), Xor(temp[2], LoadKey<191>(subkeys))));
        temp[1] = RotateRight<5>(Add(Xor(temp[0], LoadKey<188>(subkeys)), Xor(temp[1], LoadKey<189>(subkeys))));
        temp[0] = RotateLeft<9>(Add(Xor(temp[3], LoadKey<186>(subkeys)), Xor(temp[0], LoadKey<187>(subkeys))));
    }
}

// *************************** LEA Decryption ***************************//

template <class W>
inline void LEA_Decryption(W temp[4], const word32 *subkeys, unsigned int rounds)
{
    if(rounds > 28)
    {
        temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<186>(subkeys))), LoadKey<187>(subkeys));
        temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<188>(subkeys))), LoadKey<189>(subkeys));
        temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<190>(subkeys))), LoadKey<191>(subkeys));
        temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<180>(subkeys))), LoadKey<181>(subkeys));
        temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<182>(subkeys))), LoadKey<183>(subkeys));
        temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<184>(subkeys))), LoadKey<185>(subkeys));
        temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<174>(subkeys))), LoadKey<175>(subkeys));
        temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<176>(subkeys))), LoadKey<177>(subkeys));
        temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<178>(subkeys))), LoadKey<179>(subkeys));
        temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<168>(subkeys))), LoadKey<169>(subkeys));
        temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<170>(subkeys))), LoadKey<171>(subkeys));
        temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<172>(subkeys))), LoadKey<173>(subkeys));
    }

    if(rounds > 24)
    {
        temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<162>(subkeys))), LoadKey<163>(subkeys));
        temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<164>(subkeys))), LoadKey<165>(subkeys));
        temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<166>(subkeys))), LoadKey<167>(subkeys));
        temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<156>(subkeys))), LoadKey<157>(subkeys));
        temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<158>(subkeys))), LoadKey<159>(subkeys));
        temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<160>(subkeys))), LoadKey<161>(subkeys));
        temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<150>(subkeys))), LoadKey<151>(subkeys));
        temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<152>(subkeys))), LoadKey<153>(subkeys));
        temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<154>(subkeys))), LoadKey<155>(subkeys));
        temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<144>(subkeys))), LoadKey<145>(subkeys));
        temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<146>(subkeys))), LoadKey<147>(subkeys));
        temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<148>(subkeys))), LoadKey<149>(subkeys));
    }

    temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<138>(subkeys))), LoadKey<139>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<140>(subkeys))), LoadKey<141>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<142>(subkeys))), LoadKey<143>(subkeys));
    temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<132>(subkeys))), LoadKey<133>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<134>(subkeys))), LoadKey<135>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<136>(subkeys))), LoadKey<137>(subkeys));
    temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<126>(subkeys))), LoadKey<127>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<128>(subkeys))), LoadKey<129>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<130>(subkeys))), LoadKey<131>(subkeys));
    temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<120>(subkeys))), LoadKey<121>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<122>(subkeys))), LoadKey<123>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<124>(subkeys))), LoadKey<125>(subkeys));

    temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<114>(subkeys))), LoadKey<115>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<116>(subkeys))), LoadKey<117>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<118>(subkeys))), LoadKey<119>(subkeys));
    temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<108>(subkeys))), LoadKey<109>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<110>(subkeys))), LoadKey<111>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<112>(subkeys))), LoadKey<113>(subkeys));
    temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<102>(subkeys))), LoadKey<103>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<104>(subkeys))), LoadKey<105>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<106>(subkeys))), LoadKey<107>(subkeys));
    temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<96>(subkeys))), LoadKey<97>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<98>(subkeys))), LoadKey<99>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<100>(subkeys))), LoadKey<101>(subkeys));

    temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<90>(subkeys))), LoadKey<91>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<92>(subkeys))), LoadKey<93>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<94>(subkeys))), LoadKey<95>(subkeys));
    temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<84>(subkeys))), LoadKey<85>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<86>(subkeys))), LoadKey<87>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<88>(subkeys))), LoadKey<89>(subkeys));
    temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<78>(subkeys))), LoadKey<79>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<80>(subkeys))), LoadKey<81>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<82>(subkeys))), LoadKey<83>(subkeys));
    temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<72>(subkeys))), LoadKey<73>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<74>(subkeys))), LoadKey<75>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<76>(subkeys))), LoadKey<77>(subkeys));

    temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<66>(subkeys))), LoadKey<67>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<68>(subkeys))), LoadKey<69>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<70>(subkeys))), LoadKey<71>(subkeys));
    temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<60>(subkeys))), LoadKey<61>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<62>(subkeys))), LoadKey<63>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<64>(subkeys))), LoadKey<65>(subkeys));
    temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<54>(subkeys))), LoadKey<55>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<56>(subkeys))), LoadKey<57>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<58>(subkeys))), LoadKey<59>(subkeys));
    temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<48>(subkeys))), LoadKey<49>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<50>(subkeys))), LoadKey<51>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<52>(subkeys))), LoadKey<53>(subkeys));

    temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<42>(subkeys))), LoadKey<43>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<44>(subkeys))), LoadKey<45>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<46>(subkeys))), LoadKey<47>(subkeys));
    temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<36>(subkeys))), LoadKey<37>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<38>(subkeys))), LoadKey<39>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<40>(subkeys))), LoadKey<41>(subkeys));
    temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<30>(subkeys))), LoadKey<31>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<5>(temp[3]), Xor(temp[2], LoadKey<32>(subkeys))), LoadKey<33>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<3>(temp[0]), Xor(temp[3], LoadKey<34>(subkeys))), LoadKey<35>(subkeys));
    temp[1] = Xor(Sub(RotateRight<9>(temp[1]), Xor(temp[0], LoadKey<24>(subkeys))), LoadKey<25>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<5>(temp[2]), Xor(temp[1], LoadKey<26>(subkeys))), LoadKey<27>(subkeys));
    temp[3] = Xor(Sub(RotateLeft<3>(temp[3]), Xor(temp[2], LoadKey<28>(subkeys))), LoadKey<29>(subkeys));

    temp[0] = Xor(Sub(RotateRight<9>(temp[0]), Xor(temp[3], LoadKey<18>(subkeys))), LoadKey<19>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<5>(temp[1]), Xor(temp[0], LoadKey<20>(subkeys))), LoadKey<21>(subkeys));
    temp[2] = Xor(Sub(RotateLeft<3>(temp[2]), Xor(temp[1], LoadKey<22>(subkeys))), LoadKey<23>(subkeys));
    temp[3] = Xor(Sub(RotateRight<9>(temp[3]), Xor(temp[2], LoadKey<12>(subkeys))), LoadKey<13>(subkeys));
    temp[0] = Xor(Sub(RotateLeft<5>(temp[0]), Xor(temp[3], LoadKey<14>(subkeys))), LoadKey<15>(subkeys));
    temp[1] = Xor(Sub(RotateLeft<3>(temp[1]), Xor(temp[0], LoadKey<16>(subkeys))), LoadKey<17>(subkeys));
    temp[2] = Xor(Sub(RotateRight<9>(temp[2]), Xor(temp[1], LoadKey<6>(subkeys))), LoadKey<7>(subkeys));
    temp[3] = Xor(Sub(Ä≥D# m Ä.<&Å@f¿¡\|~ @@ ÑÄ∫@‡¿Ú ò| 4!VAâ@Q% k%Ä,ê¿-Ä@%Ä·Äm @@ H}`#F¿f &}&Ä#} ¿ &√Ü‡a!@&|} Ä#}·1 &#†& n&&%t|¿M††‡)B‡}%#} m†##}¿}m&| k‡5Ä&|%F&%¿L&&  , `j|@#Ä'%Ä	¿ &†# mm A2#¿im|| |`	|&Q@$|%`m¿@&C†$¿ò|| %†& # &m&m%¢| #F|%†#†sB} && &A|%&Ä^}AÅ@&%ÄÜm# %}&| }%% #&mÄ5|Ç |}&||†°Z a*Är B|@Bh#@&¿a ) +FÄIÄo%|¿r }#"%Ä%|m¿+ §#¿+} ‡+¿*¢" °¿R% }  Äâ†¶%##}}@/}&2†ê|&¿äÄ||$†$#!"%@¶ FD|&@	|%`Üá î¡è`à#@% 2 ¡O   H}}@#(}m  ?`b& † |&} #⁄ ‡"m`%@  k`{Ü `d¿Cm%†m m°} &Ä5&F%}`E & `  `Ω ß@( W &¿î}P}m ñ†ÿ%B#†÷ mÅt…`%@e }¿õÄ &&Ü·‡â&mÄﬂ@¿ñ%»#†v#°H‡d F#‡##}#`¬@&X%Ä&Ä¢}`<%Ü @d‡X#%@  ‡% &¢‡9m /¿M ì @1| Ç|ÄO && #t#†≥`˜ÄY‡¨&%F}#Äb ¢Ä†#}`O|‡5-!]‡†}a£@I@M†F@#|< Ä+`= &|}†F†Ä∞ 0 !@}  å&‡#∞l%@B	ê9&‡f&#@F##Äu @%  #@Ä#Äm }&p-Ç }m&%(‡#B¿ —[S | &%%&m|}!p |& |° QUêP†  ∞¡–$}& p`bÄ8–PV  mq`B}`Tz∞/%™#:†0†7&P-Ù  ëPP4Ä|#PÉ =∞ | &–
}Ä@>&»}%m—/ ‡2pie; mÄHê#Bêïm0á¿###–4ê>`f`;¿%#|Q  †EPì  ∞%&#—a∞ç¿D|¿0#|PB&@1} #d%#@&∞R }  m}&#Å–|F&m0
°‡L }—6Äd†}m@ò∞ç–ó% QÒA& F}ê|GqpC∞ &}@%@  |Üm›PO#∞N _¿'m–2U }Ä%##I`)}F¿}| aëm† #m– ÅSAI–la–&mPêæ‡†30SPrëT  >W¿âÒ=–q , ‡O} ?‡)  ß Ç¿c&ê}–?U@W±`#P2&`H}∞î&‡ &Ä|} ∞yu0}–[#Yê0Ê `7P  –Ä3‡3Ål†B}& ,D@mÄm&™êá18&–ä&`Z#°ï <&FR|Äl %Òdm0¯&%ëdPT`på†Y	0& êr} }r†`  & Ü†‡\m@1u–Z‡ñ&†ú@V êù–ß™|}^%@7™@M8}W#m}mÄH|™ Ä4†,}@% ¿úL%ë m†∂m ##%}7PH =pÉ}∞"m& Aê# %)¿%}Ä ‡ºm ‰p%0u‡çÄ¨ %%&& &#ƒ  êú|F%–]@73‡Å=&}∞w y#%ï†m%0;0c¿y‡zrP= ∞‡qò&%Äê %¿h&|mâ|‡ m# #0√}L# 4a	|∞% &F}m*|ÄF }0∆#§%Ph %‡ÉmÄf√†è– |∆‡M¬aF|}  Ä4‡`%†b–…êtmP 0é @ m& m}m∂m†LP<mÄ≥@ûpÑ–   p#B √õ†Wk «0-&06Ë%& Q}†!0É0/˝¿!—_¿R`$pB†çì¿É0.&êo#` # C|&}¿ }ÄFz≠—#, Ç%Ò/ ær#ê⁄ %Äû†Xœm #> TÄm@iM#—` }0 A†VÆ¿!W`ya) ∞mm†Wêâmp“Bm0v}–”3TÄC‡‡J%@%mm¬\Ä& ê|}FT} ` ñU#! J  %m†∆&Ë 0ò‡UêŸ 4ñ  5‡ `#& /≠0} ∫ *|∞O–ë0%‡\pt  Ümê–}∞Ní%‡ôm%êm∞em rmF%a0	}#P…@i|Á`"0nP ÏÚ≠†x¯%0)Á∞Y–ê°&± }0Pq w˝"ƒ¡‰pHÌ¡∏–‹ëœWê_∞ﬁ`X ∫# &ê p}F# 2’PMÒºÄ≤ ?–:∞6—È∞êÚ¥Ä_ op¿¿Ò°E–4#Å˘mÂ…A\#m‡”†à`?gêx@πä|0Õ∞‘mU ‰  Z`'êªmﬁ}¿/∞öêP†   ·H8 m“ –:1*ÌΩ  #& |  m  # m%&}#| m mm    %|m Pm  &}#% | %# }   %&& &%  %&  F#}m8# }}  &m  &  m%}&&  > ##} | # #   !#	 -| ∏}& %| | m|||# }&m m ’F|#} |# | w&| } | m}#%mm| ( 
 @}  Ä  &% X#%}#%Äâ m m|%% |& |%#  #& }###%@F|Ä  ##  m} &Ä|#Äê$% ;| Ä¡ #%|%& mm##}#Ä#}  %" Ä}mÅÅ Ä	ÄD & m |¡ ¢%%%Ä ‘ÄS}}F##H#  m D%}  &Ä@@*}#|TAB@| B}&%@m&&@y}#}    @7É$%  ||¿w mm   Ü }FÄ}Äï' í Ä5 |Å$%|&@ã&#m }}%%(|  è¿: $@ ¿4|&¡| ,#| Bm L¿Å} }m ^ +}% }}Äw#m F}#    @a}mÅ@G}}m &ÄO
% %¿n|¿¡|&#|† %m¿p# Hò|&Ä6Å¢@Fm£ÄøÄ¡Ã} ¿~}Å¨ ≥m|¡W} F##m¿P|   |# êm ÆÄß  m&   |#&  m# †|mÇ¿¥&&& ›ç Q¿8¿∑  &ÄB%¿πm&}ÄÄ#&&} ¿—¿}#%¡P@@® F`# %Å`ÄùmÅ c%}¡s % &#m&#ma@|  }}H&@+# Ä#`;  @ #|@C&†á m|#% †Åp}% #  B`j##m  #F#†Ö % `  &   |m& &&#m}•¿J†Ç}#† &Ä3’·¡Äa¿$‡Äl† &måÄ~·b #|‡+‡"‡|Ä& á}|Å†9} # `@ |# mm} &&m%Ä‡+}|¿ #& Ç`8 â!¿ |}! |m %|||%|%  Ä&#"†)  `ã†T% a ^ | & Ä@    #F%@¨%  &% Kì@Ç -  å†ß4 m&| @ %ÄNm†c· `e%&Ä}}A†S%|   †Q·
%   |    G}|F  %¬p L ‡`@0 m ‡∞} @±¿
&}‡ú|BÄ@#&‡  }L †b}†|ô°‡AÄ0‡ J†‡_|}`FB†F#Ä°@||  gÄR 0†   `_@%|†Y|cÄl†f}@S z` E‡Ä  ÎÄ”‡j@∏@ì¿ì‡` } |mú`Ä≠Bs%ëÑ%#‡î }‡÷º&†	c|Ä√@C¡L  |Ä ™%†ü R  E@ }#}|Ù  ¿∑¿8‡v··X`Å ¢ òma∏ ‡≤ ‘# #F& 6¿í}†z}D`%% @Ìm  !m%¿8  #  }   &#2@} Ÿ`∏ à » y`Ãm& Ä`E#†‰B@	Ä·|# #m `Ç&`Í†ã#mF† |  ‡Ì| §T|‡m†&1#∂ 0j– p `} ±$}m‡e}å&¡K¿#0Y m&#‘mM 8Ä6ÄWï–& Ñ#|Cπ}A8±D¢F÷&vP0É%†
–;ts }¡0s% #%mÉ†D–r m|‡¶#k‡\|@I}êoä%#% ˙}ÛRm@K—rP V y¡‡m#¿`cdF#0C 7@#eÄç%`GpM Pà#}∞@}p ∞|&|@U{p6`}ä|pjÒ=}Ä&%&¿H|%Äá}  I† 0x}m–í#%#}‡G&#F®%  aF%–# ÄJ}}`fa) ê`ê& @Çm|~ÄG`'|m %êl#‡p }|Âá}—Q}%ÄP c∞È m ‡4%‡Y†(0]1PSFPÄï| ¸mq`∞i{`í07ê Äõ}†:ê@à,p2#∞d–s&†]–&}}`ìÅW`m}Ä`ñq0†m| ¢Ä0‡~#|m –mFapep\—V Ph†$÷ ax# pm õ %X&‡X@8PN&ëù&&&¿%|00#}Ò‡#ñ ±tÄ!s|ÄP|0|U PçF l∞å&=p@ØÄ7–`0êmr∞8} ` ≠ß ö Ä%°_}†{(&êmpT}–  ¡ ‡≠ M`†#||&‡ % âÄ^ÚCGpÖ@Q8|ê	FS wÅ  ;∞o|Oëú –‡G}%ê&X#Ä;q~pt"}∞k#P~#÷}†ã∞}∞E 0kPc® Hmê†î»}PQ|†x†2%@è}#P§‡# }& |`8}X  ¿Ép &Ä#P  êû  fSZÄ&&|n@v à&|}p%∞A@G% | ù™&Pt –ûQc ∞´X##´‡,#·G 8% `º¿Gè0¢D∞±–'F&# %∞å|êR P<mJëE#V 0ö}∫ @4%Ø–â 4 m%ÂÄw}ê5†àpµê'| ††£}&m  ªq?p5‡KA@{} }ª  }|%#F∞} & †28& 6ë0† I 0îL&mp—–m†R#hmmQ≠`ó@}d %o pÑPƒ∆#ê&Q¨& †'±†5–∆|Ä+|¿	Ät|Ép,∞∆}m%–!&¿;¬>F%&"ÄN&#%∞ù}mnÄî¿ @q∞\…·X|#	  0≤  PÒ\ `vp9^ ∞Ií^Ä/ #t|`o¿]02#'¿∫≠·9~D#&p|&F°	 —!n |–á êm†
–> @:Å8 †@Ä:%Fp/Pƒ}%#‡o%˜0•ê8Ä«0H+§0#†$†7}#X& :PQD ‚P¢∞&F#–Y†¶ º4% `¿∫ê0mdùm‡á‡D|† #AµÄîô¿«%0'#%‡ﬂ(}|¿&#∞≤ ∞#1N∞&CÁ‡ﬁØ¿Z&#‡êm†0| p&F_êkù"C¿PD082
 6 G¿™#"mÄ%†ñmç@@≈Ås a*E∞Mp@| €A`O||&Ä}ø¿w∞≥—’ ñ]Ä≥m!5ëS â}†ÄB¿#p @π¡Ä   –¿8ü q`ø‡¶êÑ±¿vD‡A #0†}	Z0‰# UêÜm@2–ø#a}i–p PE op…ê}k–ı ßΩ | Fm| |&# } %&    #} }m%%&  % #  |## ##}  }:% |}&#} |&#} |  m# &m m m &   &&}  &&m} %}|&#@F# 6#D}m Ä%  &  m <&| %m| %} % |||  ||#& |%|#}|m & ##%  #A J &m   %}# }| #F   %#  m} &|## }&& m&%%ê |&  ||Äã |  |#mÄÄì #Äm  | Ç°Ä|#| Ä†}  }Ä #Ä	ÇÄ}&&Ä"` m ¢ N F| ¿m}|}   |& >}    ÄÄ P# ## %    # }}}|Ä%#&}ÄdB&Äí&| L  %&&m m  ´#Äk%  Fm}# #} m@% && &% %} _# %&} &|m@y|}&@ã| |ÄL#( #m @]#m }}#| } && ||	@Ä#(|Ä"Äå} ÄlF&}m& %& | #  z}&|!@&¿X&} Ä|&#& } @rmÄ%| ¿u  #}}Äm ;Ä&&r@|# $Ä |}¿éHm#Ä£ ÄíFí@x L ¿Aµ||&|` ÄP@c& %}#}%¿#¿¿œ}&}¿â@Y} \ Ä…@Z%#& Q¡m}mP%¿ }Ä% mm|Ä(%@∂ÄqF#%  #&% ‰# ”#    %#% }B%Ä_m% !'}}% Z&@H&&|@ï& Ñ¿&&A@&¿À%  ‡@Á&#m#aÄv m@†‡g°fFA%a‡c& &|B†}}@FÑ#&  m}†EÄ| |%Ä/ %% #  m%}}m‡|ã@ Q&†# `Z †,}  %| | År#F7aq`vÄ  V 2m‡Ç†É@A# m%  *}}Ñ& °{ }} í#@w +#%`#|%†}|B ó &  ‡b‡@ ô#}#»m@è}`. s % m  ^Fê %¿C#%†X@m}   Ä>‡§·a9@ó}°E¿è‡¶%|(` ‡, x&} % &f ‡h@9% ‡gn#Ä†]‡#ÄLÄW@|F|¢ô# Äã‡c  N| ö ú¿é#Å0e  ‡è †å@	}v‡e!^`õ¿†L#Åå‡Mm¿| |a| mm u}N `I g 6 !E|pm F@ã ø° T#}¿|Äì|aj Fa}E¢Å@P  ¿w Äs  %¿QÄk| @- *ÄG|mJ†k ÄV%aC@4¿[· | L|FvmÅi|ƒ ¶#AR¿`v&m%%|m>|  '‡ã è@% mÅ∞1`ì&`£Äª##!$a]}&m# ƒ}#Í@ÿaâ#Ä$‡ÜÄ®à&mF ∆}}@U@  ¿-#é%†A‡( |Ä
`õ†|%%#¿d|Ähm&ÜÄ∞†6&| t’Äô †'|b»`¡¿9Q ≈| `√ÄZ%@óÄ• ∆&!@0m| Õ#}M¿7 †€@Ñ É ® #†m@Ç¿G» #|¿m n¿m†|&#`›`»@%||m@ƒ%… x | H‡∑ÄG" ¿6&`π%# %mm†'#†
¿*E@F` %% @3¿5c·∞d| p"&ë#|4|êN&`:∞8&‡&m% †e‡i–á∞@A‡b|# –y|@f! k|mƒ P} ¿>`p8}&±t≤hsF 
}–|PK &% '†â @N ÄA}J}‡L∞G& \  <&}   m&Zq#¿!r ± +4–a| x|  %} # ‹0  B}`H¿ mmF`&%# %—@O& ¢©@%;–çb#™$	7##Œ#¿ â†c}&ê:∞bêmaz  3q3ê|t†Å#mZF—Ö p ‡||¿I}∞ P&4 }–¿¿1¨ê!†Ö∞E&˜90Ä##`+†VÄ}0& ¿f‡R ˆ@`¿) )!Ä#%}} +Ã||†X¿?|Wqí0-m!Äm*`ê>%`Ö }≥0b ä  y†Mm0=@t# ûP}–O ò} (&∞ÄPi&-∞°q[+`k AP0&  ‡V Pr&}|}F±–é}&–¿Z| /¶0§ê|!z`H}m É}|Qx}5p¢∞m%pö^Ë&m†@PúPÄLœPAö0–}0éPC$  0ç     |}}%¿àF‡É}¿ta%|u·¿g‡Ü‡¶D#òm P| [ ma4|% é†â`û@p=„‡@∞m∞
`i∞Ø›0Ü¿( ßÄ/–a3†pU| êFPuFÚP &@2pépê 8®#2ó–~p∞á±î–zQ~#% *‡
#ê"0Çm ‡   }`ú0_–A  #|$%±&¿Æåmó ;}0|Ñ#  %‡)§FÒM m†R,0Æêw `M  &&| Ik‡†HêøP‡üm1É#êQ&%+êP– –X–¿&B%	@h&GPáQì¿®0&ëw}mPíF †d∞=Äª% #Ç#–#&&1eG f0…† –(|[0X‡∞–R qíÈPR±¨|0àê∞Ä°u`x& ‡ª¿g}áP G&& R"&–mÄF}m¿–%%f}†fA} q/¿#/P¥¿5∞)‡Y%–≈Ä-¶ &mÏ%∞ê&∞Ω‡ñê‘± Ω `!Ω}&†#%%∞∞–√%–Cp@%Fí˝–k†s∞é`-p«Ä„¿ 0#¿êß06ç†|@s&& Ä∫ëP"&#¿}– m %<m†ƒÄ…ì“4 ∫  ‡äpWàÄ#A@∑ FmpmXmm¿P #‡”‚ ¿L#à|pkS p<ˆ êç2&¿m#mmm@ÄáÁê!’± m`!Ä™!3@IuF5 Ê&!‡Õ†& ¿>@∏g	&&# æ o`
–›@$ø‹&¿>0µ  ¿ØÅÎ‡]†-‡.}êM @ö±AD%}`"&`Q–	¡´F†'|€‡@Ö%·(`é&∞=pÃÓ}†B A¿õ%m 5â†óm06m#\c‡;P&} Q09ﬂ—ß†È–rp>êa†ù‡√è†è ñ¿	êê|†1##|‡˘ F∞è†Ú&A∑∞Á êWm†E‡8 fP % P#†*a¿œ#–Äÿ‡% m–@¿f¿m#çp›}äΩ m#&#   |m  h  †} m»FÄ &%%%}X |} ##&&  |#& |m#} &% |Ä|&%} à &&mm&ƒ#}#|% .|  %& '  &}mm F | ||} %,}}& | mmd#@&#  |∑ #&}}%!| }º# »|Ä
} } |%###}#mÅi# }F& mm&|mÄ #m     &% &mÄmm|  & m Äd} • &#& |&}&#% }|  } }Å(#Ä}#m|B  GÄBm!∞&F& e} |#Ä&%@m&  }X|& %Äå& •  % #||  ¿   &|0 %m ∂ ’| } m