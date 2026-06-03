#pragma once

/** @file
 * @brief SIMD feature detection and include for VIPA.
 */

#if !defined(VIPA_USE_AVX2) && defined(__AVX2__)
#define VIPA_USE_AVX2 1
#endif

#if !defined(VIPA_USE_AVX2)
#define VIPA_USE_AVX2 0
#endif

#if VIPA_USE_AVX2
#include <immintrin.h>
#endif
