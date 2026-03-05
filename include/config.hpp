#pragma once
#include <chrono>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
    #include <x86intrin.h>
#endif

/**
 * @file config.hpp
 * @brief Global configuration file for the application.
 *
 * This file contains preprocessor directives to control various compile-time
 * settings, including error handling, default data types, and benchmark toggles.
 */

// ============================================================================
// Error Handling Configuration
// ============================================================================

/**
 * @def HARD_ERRORS
 * @brief If defined, all functions become `noexcept`, and the program will
 * terminate on errors like out-of-bounds access instead of throwing exceptions.
 * This is for performance-critical builds where exception overhead is undesirable.
 */
#ifdef HARD_ERRORS
    #define NOEXCEPT noexcept
    #define NOEXCEPT_THROWS noexcept
    #define NOEXCEPT_CHECKBOUNDS noexcept
#else
    #define NOEXCEPT
    /**
     * @def NO_THROWS
     * @brief If defined (and HARD_ERRORS is not), disables exceptions for
     * operations that can fail (like file loading), making them `noexcept`.
     */
    #ifdef NO_THROWS
        #define NOEXCEPT_THROWS noexcept
    #else
        #define NOEXCEPT_THROWS
    #endif
    /**
     * @def CHECK_BOUNDS
     * @brief If defined (and HARD_ERRORS is not), enables bounds checking for
     * Tensor access, throwing `std::out_of_range` on violations.
     */
    #ifdef CHECK_BOUNDS
        #define NOEXCEPT_CHECKBOUNDS
    #else
        #define NOEXCEPT_CHECKBOUNDS noexcept
    #endif
#endif

// ============================================================================
// Data Type and Parameter Configuration
// ============================================================================

/** @def IMG_DTYPE The default data type for image pixels. */
#ifndef IMG_DTYPE
    #define IMG_DTYPE unsigned char
#endif

/** @def CNV_DTYPE The default data type for convolution kernel elements. */
#ifndef CNV_DTYPE
    #define CNV_DTYPE float
#endif

/** @def RATIO Default ratio for calculating dynamic kernel sizes relative to image dimensions. */
#ifndef RATIO
    #define RATIO 0.01
#endif

/** @def MIN_CHANNELS The minimum number of channels to load for an image (e.g., 3 for RGB). */
#ifndef MIN_CHANNELS
    #define MIN_CHANNELS 3
#endif

// ============================================================================
// Directory Configuration
// ============================================================================

/** @def INPUT_DIRECTORY Default path for input images. */
#ifndef INPUT_DIRECTORY
    #define INPUT_DIRECTORY "./input/"
#endif
/** @def OUTPUT_DIRECTORY Default path for saving output images. */
#ifndef OUTPUT_DIRECTORY
    #define OUTPUT_DIRECTORY "./output/"
#endif

// ============================================================================
// Benchmarking Configuration
// ============================================================================

/**
 * @def DO_BENCHMARK
 * @brief If defined, enables benchmarking macros throughout the code.
 */
#ifdef DO_BENCHMARK
    // Helper macros to ensure __COUNTER__ is expanded correctly.
    #define _MACRO_CONCAT(a, b) a ## b
    #define MACRO_CONCAT(a, b) _MACRO_CONCAT(a, b)

    /**
     * @def BENCHMARK_CHRONO(out_var, ...)
     * @brief A macro to measure the execution time of a code block in microseconds.
     * @param out_var The variable (double) to which the duration will be added.
     * @param ... The code block to be benchmarked.
     */
    #define _BENCHMARK_CHRONO(out_var, id, ...) \
        auto MACRO_CONCAT(_b_start_, id) = std::chrono::high_resolution_clock::now(); \
        __VA_ARGS__; \
        asm volatile("" ::: "memory"); \
        auto MACRO_CONCAT(_b_end_, id) = std::chrono::high_resolution_clock::now(); \
        std::chrono::duration<double, std::micro> MACRO_CONCAT(_b_dur_, id) = MACRO_CONCAT(_b_end_, id) - MACRO_CONCAT(_b_start_, id); \
        (out_var) += MACRO_CONCAT(_b_dur_, id).count();

    #define BENCHMARK_CHRONO(out_var, ...) _BENCHMARK_CHRONO(out_var, __COUNTER__, __VA_ARGS__)

    /**
     * @def BENCHMARK_CYCLES(out_var, ...)
     * @brief A macro to measure the CPU cycles of a code block using `rdtscp`.
     * @param out_var The variable (uint64_t) to which the cycle count will be added.
     * @param ... The code block to be benchmarked.
     * @note This is only available on x86/x64 architectures.
     */
    #if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
        #define _BENCHMARK_CYCLES(out_var, id, ...) \
            unsigned int MACRO_CONCAT(_b_aux_, id); \
            uint64_t MACRO_CONCAT(_b_start_, id) = __rdtscp(&MACRO_CONCAT(_b_aux_, id)); \
            __VA_ARGS__; \
            asm volatile("" ::: "memory"); \
            uint64_t MACRO_CONCAT(_b_end_, id) = __rdtscp(&MACRO_CONCAT(_b_aux_, id)); \
            (out_var) += (MACRO_CONCAT(_b_end_, id) - MACRO_CONCAT(_b_start_, id));
    #else
        #define _BENCHMARK_CYCLES(out_var, id, ...) \
            __VA_ARGS__;
    #endif
    #define BENCHMARK_CYCLES(out_var, ...) _BENCHMARK_CYCLES(out_var, __COUNTER__, __VA_ARGS__)
#else
    // If DO_BENCHMARK is not defined, the macros expand to just the code block,
    // effectively removing the benchmarking overhead.
    #define BENCHMARK_CHRONO(out_var, ...) \
        __VA_ARGS__;

    #define BENCHMARK_CYCLES(out_var, ...) \
        __VA_ARGS__;
#endif