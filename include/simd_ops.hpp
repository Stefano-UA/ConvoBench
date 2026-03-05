#pragma once
#include "config.hpp"
#include <immintrin.h>
#include <type_traits>

// Structs used as tags for dispatching to the correct SIMD implementation.
struct SSE_SIMD {};
struct AVX_SIMD {};
struct AVX2_SIMD {};
struct AVX512_SIMD {};

// Define the optimal Extended Instruction Set (EIS) available on the current CPU.
// This allows the code to compile with the best possible SIMD implementation.
#if defined(__AVX512__)
    using OptimalEIS = AVX512_SIMD;
#elif defined(__AVX2__)
    using OptimalEIS = AVX2_SIMD;
#elif defined(__AVX__)
    // AVX lacks most 256-bit integer instructions. For integer types, SSE is optimal.
    using OptimalEIS = std::conditional_t<std::is_same_v<IMG_DTYPE, unsigned char>, SSE_SIMD, AVX_SIMD>;
#elif defined(__SSE4_1__)
    using OptimalEIS = SSE_SIMD;
#else
    // If no SIMD instruction sets are detected, fall back to a version
    // that the compiler can attempt to auto-vectorize.
    #undef VECTORIZED_CONVO
    #define VECTORIZABLE_CONVO
#endif

/**
 * @brief A helper struct to hold two __m128d registers.
 * Used for SSE operations on doubles, as __m128d can only hold 2 doubles,
 * but we want to process up to 4 values (e.g., for an RGBA pixel) at a time.
 */
struct DoubleReg128 { __m128d lo, hi; };

/**
 * @brief A generic template for abstracting SIMD operations.
 * @tparam I The data type of the Image (e.g., float, unsigned char).
 * @tparam K The data type of the Kernel (e.g., float, double).
 * @tparam EIS The Extended Instruction Set tag (e.g., SSE_SIMD, AVX_SIMD).
 *
 * This struct is specialized for different combinations of data types and
 * instruction sets to provide a uniform interface for SIMD-accelerated
 * convolution.
 */
template <typename I, typename K, typename EIS> struct SimdOps;

// ============================================================================
// SSE IMPLEMENTATIONS
// ============================================================================

/** @brief SSE specialization for float Images and float Kernels. */
template <>
struct SimdOps<float, float, SSE_SIMD> {
    using registry = __m128;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x08;
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 1.0f;

    static inline registry get_empty() { return _mm_setzero_ps(); }
    static inline registry load(const float* ptr) { return _mm_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) { return _mm_setr_ps(c0, c1, c2, pad); }
    static inline registry set_zero(registry v) { return _mm_blend_ps(v, _mm_setzero_ps(), pad_mask); }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm_add_ps(_mm_mul_ps(a, b), acc); }
    static inline registry add(registry a, registry b) { return _mm_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm_max_ps(v, _mm_set1_ps(cmin));
        v = _mm_min_ps(v, _mm_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, float* dst) { _mm_storeu_ps(dst, v); }
    static inline void extract(registry v, float* dst) { _mm_storeu_ps(dst, v); }
};

/** @brief SSE specialization for unsigned char Images and float Kernels. */
template <>
struct SimdOps<unsigned char, float, SSE_SIMD> {
    using registry = __m128;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x08;
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 255.0f;

    static inline registry get_empty() { return _mm_setzero_ps(); }
    static inline registry load(const unsigned char* ptr) {
        __m128i chars = _mm_cvtsi32_si128(*reinterpret_cast<const int*>(ptr));
        return _mm_cvtepi32_ps(_mm_cvtepu8_epi32(chars));
    }
    static inline registry load(const float* ptr) { return _mm_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) { return _mm_setr_ps(c0, c1, c2, pad); }
    static inline registry set_zero(registry v) { return _mm_blend_ps(v, _mm_setzero_ps(), pad_mask); }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm_add_ps(_mm_mul_ps(a, b), acc); }
    static inline registry add(registry a, registry b) { return _mm_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm_max_ps(v, _mm_set1_ps(cmin));
        v = _mm_min_ps(v, _mm_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, float* dst) { _mm_storeu_ps(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        __m128i ints = _mm_cvtps_epi32(v);
        __m128i pack16 = _mm_packus_epi32(ints, ints);
        *(reinterpret_cast<int*>(dst)) = _mm_cvtsi128_si32(_mm_packus_epi16(pack16, pack16));
    }
};

/** @brief SSE specialization for float Images and double Kernels. */
template <>
struct SimdOps<float, double, SSE_SIMD> {
    using registry = DoubleReg128;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x02;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 1.0;

    static inline registry get_empty() { return { _mm_setzero_pd(), _mm_setzero_pd() }; }
    static inline registry load(const float* ptr) {
        __m128 f = _mm_loadu_ps(ptr);
        return { _mm_cvtps_pd(f), _mm_cvtps_pd(_mm_movehl_ps(f, f)) };
    }
    static inline registry load(const double* ptr) { return { _mm_loadu_pd(ptr), _mm_loadu_pd(ptr + 2) }; }
    static inline registry load(double c0, double c1, double c2, double pad) { return { _mm_setr_pd(c0, c1), _mm_setr_pd(c2, pad) }; }
    static inline registry set_zero(registry v) { return { v.lo, _mm_blend_pd(v.hi, _mm_setzero_pd(), pad_mask) }; }
    static inline registry multiply_add(registry a, registry b, registry acc) {
        return { _mm_add_pd(_mm_mul_pd(a.lo, b.lo), acc.lo), _mm_add_pd(_mm_mul_pd(a.hi, b.hi), acc.hi) };
    }
    static inline registry add(registry a, registry b) { return { _mm_add_pd(a.lo, b.lo), _mm_add_pd(a.hi, b.hi) }; }
    static inline registry multiply(registry a, registry b) { return { _mm_mul_pd(a.lo, b.lo), _mm_mul_pd(a.hi, b.hi) }; }
    static inline registry divide(registry a, registry b) { return { _mm_div_pd(a.lo, b.lo), _mm_div_pd(a.hi, b.hi) }; }
    static inline void clamp(registry& v) {
        v.lo = _mm_max_pd(v.lo, _mm_set1_pd(cmin)); v.lo = _mm_min_pd(v.lo, _mm_set1_pd(cmax));
        v.hi = _mm_max_pd(v.hi, _mm_set1_pd(cmin)); v.hi = _mm_min_pd(v.hi, _mm_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, double* dst) { _mm_storeu_pd(dst, v.lo); _mm_storeu_pd(dst + 2, v.hi); }
    static inline void extract(registry v, float* dst) {
        _mm_storeu_ps(dst, _mm_movelh_ps(_mm_cvtpd_ps(v.lo), _mm_cvtpd_ps(v.hi)));
    }
};

/** @brief SSE specialization for unsigned char Images and double Kernels. */
template <>
struct SimdOps<unsigned char, double, SSE_SIMD> {
    using registry = DoubleReg128;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x02;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 255.0;

    static inline registry get_empty() { return { _mm_setzero_pd(), _mm_setzero_pd() }; }
    static inline registry load(const unsigned char* ptr) {
        __m128i ints = _mm_cvtepu8_epi32(_mm_cvtsi32_si128(*reinterpret_cast<const int*>(ptr)));
        return { _mm_cvtepi32_pd(ints), _mm_cvtepi32_pd(_mm_srli_si128(ints, 8)) };
    }
    static inline registry load(const double* ptr) { return { _mm_loadu_pd(ptr), _mm_loadu_pd(ptr + 2) }; }
    static inline registry load(double c0, double c1, double c2, double pad) { return { _mm_setr_pd(c0, c1), _mm_setr_pd(c2, pad) }; }
    static inline registry set_zero(registry v) { return { v.lo, _mm_blend_pd(v.hi, _mm_setzero_pd(), pad_mask) }; }
    static inline registry multiply_add(registry a, registry b, registry acc) {
        return { _mm_add_pd(_mm_mul_pd(a.lo, b.lo), acc.lo), _mm_add_pd(_mm_mul_pd(a.hi, b.hi), acc.hi) };
    }
    static inline registry add(registry a, registry b) { return { _mm_add_pd(a.lo, b.lo), _mm_add_pd(a.hi, b.hi) }; }
    static inline registry multiply(registry a, registry b) { return { _mm_mul_pd(a.lo, b.lo), _mm_mul_pd(a.hi, b.hi) }; }
    static inline registry divide(registry a, registry b) { return { _mm_div_pd(a.lo, b.lo), _mm_div_pd(a.hi, b.hi) }; }
    static inline void clamp(registry& v) {
        v.lo = _mm_max_pd(v.lo, _mm_set1_pd(cmin)); v.lo = _mm_min_pd(v.lo, _mm_set1_pd(cmax));
        v.hi = _mm_max_pd(v.hi, _mm_set1_pd(cmin)); v.hi = _mm_min_pd(v.hi, _mm_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, double* dst) { _mm_storeu_pd(dst, v.lo); _mm_storeu_pd(dst + 2, v.hi); }
    static inline void extract(registry v, unsigned char* dst) {
        __m128i ints = _mm_unpacklo_epi64(_mm_cvtpd_epi32(v.lo), _mm_cvtpd_epi32(v.hi));
        __m128i pack16 = _mm_packus_epi32(ints, ints);
        *(reinterpret_cast<int*>(dst)) = _mm_cvtsi128_si32(_mm_packus_epi16(pack16, pack16));
    }
};

// ============================================================================
// AVX & AVX1 IMPLEMENTATIONS
// ============================================================================

/** @brief AVX specialization for float Images and float Kernels. */
template <>
struct SimdOps<float, float, AVX_SIMD> {
    using registry = __m256;
    static constexpr unsigned int pixel_step = 2; // Processes 2 RGB pixels (6 floats)
    static constexpr unsigned int size = 8;
    static constexpr int pad_mask = 0xC0;         // Blends out the upper 2 floats
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 1.0f;

    static inline registry get_empty() { return _mm256_setzero_ps(); }
    static inline registry load(const float* ptr) { return _mm256_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) {
        return _mm256_castps128_ps256(_mm_setr_ps(c0, c1, c2, pad));
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_ps(v, _mm256_setzero_ps(), pad_mask);
    }
    // FMA is not available in AVX1. Separated into multiply and add.
    static inline registry multiply_add(registry a, registry b, registry acc) {
        return _mm256_add_ps(_mm256_mul_ps(a, b), acc);
    }
    static inline registry add(registry a, registry b) { return _mm256_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_ps(v, _mm256_set1_ps(cmin));
        v = _mm256_min_ps(v, _mm256_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m128 lo = _mm256_castps256_ps128(v);
        __m128 hi = _mm256_extractf128_ps(v, 1);
        __m128 hi_shifted = _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(hi), 4));
        __m128 r1_shifted = _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(lo), 12));
        __m128 new_hi = _mm_or_ps(r1_shifted, hi_shifted);
        __m128 new_lo = _mm_blend_ps(lo, _mm_setzero_ps(), 0x08);
        return _mm256_insertf128_ps(_mm256_castps128_ps256(new_lo), new_hi, 1);
    }
    static inline registry collapse(registry v) {
        __m128 upper = _mm256_extractf128_ps(v, 1);
        __m128 lower = _mm256_castps256_ps128(v);
        return _mm256_castps128_ps256(_mm_add_ps(upper, lower));
    }
        static inline void store(registry v, float* dst) { _mm256_storeu_ps(dst, v); }
    static inline void extract(registry v, float* dst) {
        _mm256_store_ps(dst, v);
    }
};

/** @brief AVX specialization for unsigned char Images and float Kernels. */
template <>
struct SimdOps<unsigned char, float, AVX_SIMD> {
    using registry = __m256;
    static constexpr unsigned int pixel_step = 2;
    static constexpr unsigned int size = 8;
    static constexpr int pad_mask = 0xC0;
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 255.0f;

    static inline registry get_empty() { return _mm256_setzero_ps(); }
    static inline registry load(const unsigned char* ptr) {
        // Splitting the 256i load into two 128i operations for AVX1 compatibility
        __m128i chars = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(ptr));

        __m128i ints_lo = _mm_cvtepu8_epi32(chars);
        __m128i chars_hi = _mm_srli_si128(chars, 4);
        __m128i ints_hi = _mm_cvtepu8_epi32(chars_hi);

        __m128 floats_lo = _mm_cvtepi32_ps(ints_lo);
        __m128 floats_hi = _mm_cvtepi32_ps(ints_hi);

        __m256 result = _mm256_castps128_ps256(floats_lo);
        return _mm256_insertf128_ps(result, floats_hi, 1);
    }
    static inline registry load(const float* ptr) { return _mm256_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) {
        return _mm256_castps128_ps256(_mm_setr_ps(c0, c1, c2, pad));
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_ps(v, _mm256_setzero_ps(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) {
        return _mm256_add_ps(_mm256_mul_ps(a, b), acc);
    }
    static inline registry add(registry a, registry b) { return _mm256_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_ps(v, _mm256_set1_ps(cmin));
        v = _mm256_min_ps(v, _mm256_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m128 lo = _mm256_castps256_ps128(v);
        __m128 hi = _mm256_extractf128_ps(v, 1);
        __m128 hi_shifted = _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(hi), 4));
        __m128 r1_shifted = _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(lo), 12));
        __m128 new_hi = _mm_or_ps(r1_shifted, hi_shifted);
        __m128 new_lo = _mm_blend_ps(lo, _mm_setzero_ps(), 0x08);
        return _mm256_insertf128_ps(_mm256_castps128_ps256(new_lo), new_hi, 1);
    }
    static inline registry collapse(registry v) {
        __m128 upper = _mm256_extractf128_ps(v, 1);
        __m128 lower = _mm256_castps256_ps128(v);
        return _mm256_castps128_ps256(_mm_add_ps(upper, lower));
    }
        static inline void store(registry v, float* dst) { _mm256_storeu_ps(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        // Splitting 256 into two 128 chunks for conversion and packing
        __m128 floats_lo = _mm256_castps256_ps128(v);
        __m128 floats_hi = _mm256_extractf128_ps(v, 1);

        __m128i ints_lo = _mm_cvtps_epi32(floats_lo);
        __m128i ints_hi = _mm_cvtps_epi32(floats_hi);

        __m128i pack16 = _mm_packus_epi32(ints_lo, ints_hi);
        __m128i pack8 = _mm_packus_epi16(pack16, pack16);

        // Store 8 bytes (2 pixels of RGBA).
        _mm_storel_epi64(reinterpret_cast<__m128i*>(dst), pack8);
    }
};

/** @brief AVX specialization for float Images and double Kernels. */
template <>
struct SimdOps<float, double, AVX_SIMD> {
    using registry = __m256d;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x08;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 1.0;

    static inline registry get_empty() { return _mm256_setzero_pd(); }
    static inline registry load(const float* ptr) {
        __m128 f = _mm_loadu_ps(ptr);
        return _mm256_cvtps_pd(f);
    }
    static inline registry load(const double* ptr) { return _mm256_loadu_pd(ptr); }
    static inline registry load(double c0, double c1, double c2, double pad) {
        return _mm256_setr_pd(c0, c1, c2, pad);
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_pd(v, _mm256_setzero_pd(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) {
        return _mm256_add_pd(_mm256_mul_pd(a, b), acc);
    }
    static inline registry add(registry a, registry b) { return _mm256_add_pd(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_pd(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_pd(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_pd(v, _mm256_set1_pd(cmin));
        v = _mm256_min_pd(v, _mm256_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, double* dst) { _mm256_storeu_pd(dst, v); }
    static inline void extract(registry v, float* dst) {
        __m128 f = _mm256_cvtpd_ps(v);
        _mm_storeu_ps(dst, f);
    }
};

/** @brief AVX specialization for unsigned char Images and double Kernels. */
template <>
struct SimdOps<unsigned char, double, AVX_SIMD> {
    using registry = __m256d;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x08;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 255.0;

    static inline registry get_empty() { return _mm256_setzero_pd(); }
    static inline registry load(const unsigned char* ptr) {
        __m128i chars = _mm_cvtsi32_si128(*reinterpret_cast<const int*>(ptr));
        __m128i ints = _mm_cvtepu8_epi32(chars);
        return _mm256_cvtepi32_pd(ints);
    }
    static inline registry load(const double* ptr) { return _mm256_loadu_pd(ptr); }
    static inline registry load(double c0, double c1, double c2, double pad) {
        return _mm256_setr_pd(c0, c1, c2, pad);
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_pd(v, _mm256_setzero_pd(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) {
        return _mm256_add_pd(_mm256_mul_pd(a, b), acc);
    }
    static inline registry add(registry a, registry b) { return _mm256_add_pd(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_pd(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_pd(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_pd(v, _mm256_set1_pd(cmin));
        v = _mm256_min_pd(v, _mm256_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, double* dst) { _mm256_storeu_pd(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        __m128i ints = _mm256_cvtpd_epi32(v);
        __m128i pack16 = _mm_packus_epi32(ints, ints);
        __m128i pack8 = _mm_packus_epi16(pack16, pack16);
        *(reinterpret_cast<int*>(dst)) = _mm_cvtsi128_si32(pack8);
    }
};

// ============================================================================
// AVX2 IMPLEMENTATIONS
// ============================================================================

/** @brief AVX2 specialization for float Images and float Kernels. */
template <>
struct SimdOps<float, float, AVX2_SIMD> {
    using registry = __m256;
    static constexpr unsigned int pixel_step = 2; // Processes 2 RGB pixels (6 floats)
    static constexpr unsigned int size = 8;
    static constexpr int pad_mask = 0xC0;         // Blends out the upper 2 floats
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 1.0f;

    static inline registry get_empty() { return _mm256_setzero_ps(); }
    static inline registry load(const float* ptr) { return _mm256_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) {
        return _mm256_castps128_ps256(_mm_setr_ps(c0, c1, c2, pad));
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_ps(v, _mm256_setzero_ps(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm256_fmadd_ps(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm256_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_ps(v, _mm256_set1_ps(cmin));
        v = _mm256_min_ps(v, _mm256_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m128 lo = _mm256_castps256_ps128(v);
        __m128 hi = _mm256_extractf128_ps(v, 1);
        __m128 hi_shifted = _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(hi), 4));
        __m128 r1_shifted = _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(lo), 12));
        __m128 new_hi = _mm_or_ps(r1_shifted, hi_shifted);
        __m128 new_lo = _mm_blend_ps(lo, _mm_setzero_ps(), 0x08);
        return _mm256_insertf128_ps(_mm256_castps128_ps256(new_lo), new_hi, 1);
    }
    static inline registry collapse(registry v) {
        __m128 upper = _mm256_extractf128_ps(v, 1);
        __m128 lower = _mm256_castps256_ps128(v);
        return _mm256_castps128_ps256(_mm_add_ps(upper, lower));
    }
        static inline void store(registry v, float* dst) { _mm256_storeu_ps(dst, v); }
    static inline void extract(registry v, float* dst) {
        _mm256_store_ps(dst, v);
    }
};

/** @brief AVX2 specialization for unsigned char Images and float Kernels. */
template <>
struct SimdOps<unsigned char, float, AVX2_SIMD> {
    using registry = __m256;
    static constexpr unsigned int pixel_step = 2;
    static constexpr unsigned int size = 8;
    static constexpr int pad_mask = 0xC0;
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 255.0f;

    static inline registry get_empty() { return _mm256_setzero_ps(); }
    static inline registry load(const unsigned char* ptr) {
        __m128i chars = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(ptr));
        __m256i ints = _mm256_cvtepu8_epi32(chars);
        return _mm256_cvtepi32_ps(ints);
    }
    static inline registry load(const float* ptr) { return _mm256_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) {
        return _mm256_castps128_ps256(_mm_setr_ps(c0, c1, c2, pad));
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_ps(v, _mm256_setzero_ps(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm256_fmadd_ps(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm256_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_ps(v, _mm256_set1_ps(cmin));
        v = _mm256_min_ps(v, _mm256_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m128 lo = _mm256_castps256_ps128(v);
        __m128 hi = _mm256_extractf128_ps(v, 1);
        __m128 hi_shifted = _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(hi), 4));
        __m128 r1_shifted = _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(lo), 12));
        __m128 new_hi = _mm_or_ps(r1_shifted, hi_shifted);
        __m128 new_lo = _mm_blend_ps(lo, _mm_setzero_ps(), 0x08);
        return _mm256_insertf128_ps(_mm256_castps128_ps256(new_lo), new_hi, 1);
    }
    static inline registry collapse(registry v) {
        __m128 upper = _mm256_extractf128_ps(v, 1);
        __m128 lower = _mm256_castps256_ps128(v);
        return _mm256_castps128_ps256(_mm_add_ps(upper, lower));
    }
        static inline void store(registry v, float* dst) { _mm256_storeu_ps(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        __m256i ints = _mm256_cvtps_epi32(v);
        __m256i pack16 = _mm256_packus_epi32(ints, ints);
        __m256i pack8 = _mm256_packus_epi16(pack16, pack16);
        _mm_storel_epi64(reinterpret_cast<__m128i*>(dst), _mm256_castsi256_si128(pack8));
    }
};

/** @brief AVX2 specialization for float Images and double Kernels. */
template <>
struct SimdOps<float, double, AVX2_SIMD> {
    using registry = __m256d;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x08;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 1.0;

    static inline registry get_empty() { return _mm256_setzero_pd(); }
    static inline registry load(const float* ptr) {
        __m128 f = _mm_loadu_ps(ptr);
        return _mm256_cvtps_pd(f);
    }
    static inline registry load(const double* ptr) { return _mm256_loadu_pd(ptr); }
    static inline registry load(double c0, double c1, double c2, double pad) {
        return _mm256_setr_pd(c0, c1, c2, pad);
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_pd(v, _mm256_setzero_pd(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm256_fmadd_pd(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm256_add_pd(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_pd(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_pd(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_pd(v, _mm256_set1_pd(cmin));
        v = _mm256_min_pd(v, _mm256_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, double* dst) { _mm256_storeu_pd(dst, v); }
    static inline void extract(registry v, float* dst) {
        __m128 f = _mm256_cvtpd_ps(v);
        _mm_storeu_ps(dst, f);
    }
};

/** @brief AVX2 specialization for unsigned char Images and double Kernels. */
template <>
struct SimdOps<unsigned char, double, AVX2_SIMD> {
    using registry = __m256d;
    static constexpr unsigned int pixel_step = 1;
    static constexpr unsigned int size = 4;
    static constexpr int pad_mask = 0x08;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 255.0;

    static inline registry get_empty() { return _mm256_setzero_pd(); }
    static inline registry load(const unsigned char* ptr) {
        __m128i chars = _mm_cvtsi32_si128(*reinterpret_cast<const int*>(ptr));
        __m128i ints = _mm_cvtepu8_epi32(chars);
        return _mm256_cvtepi32_pd(ints);
    }
    static inline registry load(const double* ptr) { return _mm256_loadu_pd(ptr); }
    static inline registry load(double c0, double c1, double c2, double pad) {
        return _mm256_setr_pd(c0, c1, c2, pad);
    }
    static inline registry set_zero(registry v) {
        return _mm256_blend_pd(v, _mm256_setzero_pd(), pad_mask);
    }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm256_fmadd_pd(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm256_add_pd(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm256_mul_pd(a, b); }
    static inline registry divide(registry a, registry b) { return _mm256_div_pd(a, b); }
    static inline void clamp(registry& v) {
        v = _mm256_max_pd(v, _mm256_set1_pd(cmin));
        v = _mm256_min_pd(v, _mm256_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) { return v; }
    static inline registry collapse(registry v) { return v; }
        static inline void store(registry v, double* dst) { _mm256_storeu_pd(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        __m128i ints = _mm256_cvtpd_epi32(v);
        __m128i pack16 = _mm_packus_epi32(ints, ints);
        __m128i pack8 = _mm_packus_epi16(pack16, pack16);
        // Since pixel_step is 1, it should store 4 bytes max (RGBA).
        *(reinterpret_cast<int*>(dst)) = _mm_cvtsi128_si32(pack8);
    }
};

// ============================================================================
// AVX512 IMPLEMENTATIONS
// ============================================================================

/** @brief AVX512 specialization for float Images and float Kernels. */
template <>
struct SimdOps<float, float, AVX512_SIMD> {
    using registry = __m512;
    static constexpr unsigned int pixel_step = 4;
    static constexpr unsigned int size = 16;
    static constexpr __mmask16 pad_mask = 0xFFF; // Keep first 12 values (4 RGB pixels)
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 1.0f;

    static inline registry get_empty() { return _mm512_setzero_ps(); }
    static inline registry load(const float* ptr) { return _mm512_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) { return _mm512_castps128_ps512(_mm_setr_ps(c0, c1, c2, pad)); }
    static inline registry set_zero(registry v) { return _mm512_mask_blend_ps(pad_mask, _mm512_setzero_ps(), v); }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm512_fmadd_ps(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm512_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm512_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm512_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm512_max_ps(v, _mm512_set1_ps(cmin));
        v = _mm512_min_ps(v, _mm512_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m512i indices = _mm512_setr_epi32(0, 1, 2, 15, 3, 4, 5, 15, 6, 7, 8, 15, 9, 10, 11, 15);
        return _mm512_permutexvar_ps(indices, v);
    }
    static inline registry collapse(registry v) {
        __m256 sum2 = _mm256_add_ps(_mm512_extractf32x8_ps(v, 1), _mm512_castps512_ps256(v));
        return _mm512_castps128_ps512(_mm_add_ps(_mm256_extractf128_ps(sum2, 1), _mm256_castps256_ps128(sum2)));
    }
        static inline void store(registry v, float* dst) { _mm512_storeu_ps(dst, v); }
    static inline void extract(registry v, float* dst) { _mm512_store_ps(dst, v); }
};

/** @brief AVX512 specialization for unsigned char Images and float Kernels. */
template <>
struct SimdOps<unsigned char, float, AVX512_SIMD> {
    using registry = __m512;
    static constexpr unsigned int pixel_step = 4;
    static constexpr unsigned int size = 16;
    static constexpr __mmask16 pad_mask = 0xFFF;
    static constexpr float cmin = 0.0f;
    static constexpr float cmax = 255.0f;

    static inline registry get_empty() { return _mm512_setzero_ps(); }
    static inline registry load(const unsigned char* ptr) {
        return _mm512_cvtepi32_ps(_mm512_cvtepu8_epi32(_mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr))));
    }
    static inline registry load(const float* ptr) { return _mm512_loadu_ps(ptr); }
    static inline registry load(float c0, float c1, float c2, float pad) { return _mm512_castps128_ps512(_mm_setr_ps(c0, c1, c2, pad)); }
    static inline registry set_zero(registry v) { return _mm512_mask_blend_ps(pad_mask, _mm512_setzero_ps(), v); }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm512_fmadd_ps(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm512_add_ps(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm512_mul_ps(a, b); }
    static inline registry divide(registry a, registry b) { return _mm512_div_ps(a, b); }
    static inline void clamp(registry& v) {
        v = _mm512_max_ps(v, _mm512_set1_ps(cmin));
        v = _mm512_min_ps(v, _mm512_set1_ps(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m512i indices = _mm512_setr_epi32(0, 1, 2, 15, 3, 4, 5, 15, 6, 7, 8, 15, 9, 10, 11, 15);
        return _mm512_permutexvar_ps(indices, v);
    }
    static inline registry collapse(registry v) {
        __m256 sum2 = _mm256_add_ps(_mm512_extractf32x8_ps(v, 1), _mm512_castps512_ps256(v));
        return _mm512_castps128_ps512(_mm_add_ps(_mm256_extractf128_ps(sum2, 1), _mm256_castps256_ps128(sum2)));
    }
        static inline void store(registry v, float* dst) { _mm512_storeu_ps(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        __m512i ints = _mm512_cvtps_epi32(v);
        __m128i packed = _mm512_cvtepi32_epi8(ints);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), packed);
    }
};

/** @brief AVX512 specialization for float Images and double Kernels. */
template <>
struct SimdOps<float, double, AVX512_SIMD> {
    using registry = __m512d;
    static constexpr unsigned int pixel_step = 2;
    static constexpr unsigned int size = 8;
    static constexpr __mmask8 pad_mask = 0x3F; // Keep first 6 values
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 1.0;

    static inline registry get_empty() { return _mm512_setzero_pd(); }
    static inline registry load(const float* ptr) { return _mm512_cvtps_pd(_mm256_loadu_ps(ptr)); }
    static inline registry load(const double* ptr) { return _mm512_loadu_pd(ptr); }
    static inline registry load(double c0, double c1, double c2, double pad) { return _mm512_castpd256_pd512(_mm256_setr_pd(c0, c1, c2, pad)); }
    static inline registry set_zero(registry v) { return _mm512_mask_blend_pd(pad_mask, _mm512_setzero_pd(), v); }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm512_fmadd_pd(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm512_add_pd(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm512_mul_pd(a, b); }
    static inline registry divide(registry a, registry b) { return _mm512_div_pd(a, b); }
    static inline void clamp(registry& v) {
        v = _mm512_max_pd(v, _mm512_set1_pd(cmin));
        v = _mm512_min_pd(v, _mm512_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m512i indices = _mm512_setr_epi64(0, 1, 2, 7, 3, 4, 5, 7);
        return _mm512_permutexvar_pd(indices, v);
    }
    static inline registry collapse(registry v) {
        return _mm512_castpd256_pd512(_mm256_add_pd(_mm512_extractf64x4_pd(v, 1), _mm512_castpd512_pd256(v)));
    }
        static inline void store(registry v, double* dst) { _mm512_storeu_pd(dst, v); }
    static inline void extract(registry v, float* dst) { _mm256_storeu_ps(dst, _mm512_cvtpd_ps(v)); }
};

/** @brief AVX512 specialization for unsigned char Images and double Kernels. */
template <>
struct SimdOps<unsigned char, double, AVX512_SIMD> {
    using registry = __m512d;
    static constexpr unsigned int pixel_step = 2;
    static constexpr unsigned int size = 8;
    static constexpr __mmask8 pad_mask = 0x3F;
    static constexpr double cmin = 0.0;
    static constexpr double cmax = 255.0;

    static inline registry get_empty() { return _mm512_setzero_pd(); }
    static inline registry load(const unsigned char* ptr) {
        return _mm512_cvtepi32_pd(_mm256_cvtepu8_epi32(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(ptr))));
    }
    static inline registry load(const double* ptr) { return _mm512_loadu_pd(ptr); }
    static inline registry load(double c0, double c1, double c2, double pad) { return _mm512_castpd256_pd512(_mm256_setr_pd(c0, c1, c2, pad)); }
    static inline registry set_zero(registry v) { return _mm512_mask_blend_pd(pad_mask, _mm512_setzero_pd(), v); }
    static inline registry multiply_add(registry a, registry b, registry acc) { return _mm512_fmadd_pd(a, b, acc); }
    static inline registry add(registry a, registry b) { return _mm512_add_pd(a, b); }
    static inline registry multiply(registry a, registry b) { return _mm512_mul_pd(a, b); }
    static inline registry divide(registry a, registry b) { return _mm512_div_pd(a, b); }
    static inline void clamp(registry& v) {
        v = _mm512_max_pd(v, _mm512_set1_pd(cmin));
        v = _mm512_min_pd(v, _mm512_set1_pd(cmax));
    }
    static inline registry permute_3_channels(registry v) {
        __m512i indices = _mm512_setr_epi64(0, 1, 2, 7, 3, 4, 5, 7);
        return _mm512_permutexvar_pd(indices, v);
    }
    static inline registry collapse(registry v) {
        return _mm512_castpd256_pd512(_mm256_add_pd(_mm512_extractf64x4_pd(v, 1), _mm512_castpd512_pd256(v)));
    }
        static inline void store(registry v, double* dst) { _mm512_storeu_pd(dst, v); }
    static inline void extract(registry v, unsigned char* dst) {
        __m256i ints = _mm512_cvtpd_epi32(v);
        __m128i packed = _mm256_cvtepi32_epi8(ints);
        _mm_storel_epi64(reinterpret_cast<__m128i*>(dst), packed);
    }
};