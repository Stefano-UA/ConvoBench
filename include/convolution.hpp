#pragma once
#include "config.hpp"
#include "image.hpp"
#include "kernel.hpp"
#include "simd_ops.hpp"
#include <algorithm>
#include <stdexcept>
#include <concepts>
#include <type_traits>
#include <vector>
#include <cmath>
#include <limits>

/**
 * @brief Performs a 2D convolution on an image using a specified kernel.
 * @tparam I The data type of the image pixels (e.g., unsigned char, float).
 * @tparam K The data type of the kernel elements (e.g., float, int).
 * @param image The constant reference to the source image.
 * @param kernel The constant reference to the convolution kernel.
 * @return A new Image object containing the result of the convolution.
 *
 * This function contains multiple implementations of the convolution algorithm,
 * selected at compile time based on preprocessor flags:
 * - `VECTORIZED_CONVO`: A highly optimized version using SIMD intrinsics (SSE, AVX, etc.)
 *   for floating-point kernels. It has specializations for 3 and 4 channel images.
 * - `VECTORIZABLE_CONVO`: A version written to be easily auto-vectorized by the compiler.
 * - Default: A standard, unoptimized C++ implementation for maximum compatibility.
 */
template <typename I, typename K>
requires (std::integral<I> || std::floating_point<I>) && (std::integral<K> || std::floating_point<K>)
Image<I> convolute(const Image<I>& image, const Kernel<K>& kernel) NOEXCEPT {
    using T = std::common_type_t<I, K>;
    #ifndef NO_THROWS
        if (kernel.w() > image.w() || kernel.h() > image.h()) {
            throw std::invalid_argument("Kernel dimensions cannot exceed image dimensions.");
        }
    #endif

    // --- Optimized SIMD Implementation ---
    // This path is taken for floating point kernels when hardware support is available.
    #ifdef VECTORIZED_CONVO
    if constexpr (!std::floating_point<K>) {

        // Initialize convolution image
        Image<I> conv(image.w(), image.h(), image.c());
        // Initialize sum vector
        std::vector<T> sums(kernel.c(), static_cast<T>(0));
        // Precompute kernel sums for normalization
        for (unsigned int c = 0; c < kernel.c(); ++c) {
            for (unsigned int i = 0; i < kernel.w(); ++i) {
                for (unsigned int j = 0; j < kernel.h(); ++j) {
                    sums[c] += static_cast<T>(kernel[i, j, c]);
                }
            }
            // Avoid division by zero and pre-calculate reciprocal for multiplication
            if constexpr (std::floating_point<T>) {
                sums[c] = (std::abs(sums[c]) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sums[c]);
            } else {
                sums[c] = (sums[c] == static_cast<T>(0)) ? static_cast<T>(1) : sums[c];
            }
        }
        // Iterate over each pixel of the image, avoiding the borders
        for (unsigned int y = kernel.hh(); y < (image.h() - kernel.hh()); ++y) {
            for (unsigned int x = kernel.hw(); x < (image.w() - kernel.hw()); ++x) {
                for (unsigned int c = 0; c < image.c(); ++c) {
                    T buffer = 0; // Accumulator for the convolution result
                    // Apply kernel to the neighborhood of the current pixel
                    for (unsigned int j = 0; j < kernel.h(); ++j) {
                        for (unsigned int i = 0; i < kernel.w(); ++i) {
                            buffer += static_cast<T>(image[x + (i - kernel.hw()), y + (j - kernel.hh()), c]) * static_cast<T>(kernel[i, j, c]);
                        }
                    }
                    // Get clamp limits depending on the type
                    T cmin, cmax;
                    if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = static_cast<T>(std::numeric_limits<I>::min()), cmax = static_cast<T>(std::numeric_limits<I>::max()); }
                    // Normalize, clamp and store the final pixel value
                    if constexpr (std::floating_point<T>) {
                        conv[x, y, c] = static_cast<I>(std::clamp(buffer * sums[c], cmin, cmax));
                    } else {
                        conv[x, y, c] = static_cast<I>(std::clamp(buffer / sums[c], cmin, cmax));
                    }
                }
            }
        } return conv;

    } else {
        Image<I> conv(image.w(), image.h(), image.c());
        // Select the appropriate SIMD operation set based on instruction set support
        using Ops = SimdOps<I, K, OptimalEIS>;
        // Specialized algorithm for 3 & 4 channels or fallback to a slower one for N channels
        switch (image.c()) {
            case 3: {
                // Scalar promotion for sums
                T sum0 = static_cast<T>(0), sum1 = static_cast<T>(0), sum2 = static_cast<T>(0);
                // Loop through kernel length
                #pragma GCC ivdep
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr <= &kernel[kernel.len()-1]; k_ptr += 3) {
                    sum0 += static_cast<T>(k_ptr[0]); sum1 += static_cast<T>(k_ptr[1]); sum2 += static_cast<T>(k_ptr[2]);
                }
                // Two ways of doing things depending on decimals or not
                if constexpr (std::floating_point<T>) {
                    // Check if zero with some leeway
                    sum0 = (std::abs(sum0) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum0);
                    sum1 = (std::abs(sum1) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum1);
                    sum2 = (std::abs(sum2) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum2);
                } else {
                    // Check if zero with some leeway
                    sum0 = (sum0 == static_cast<T>(0)) ? static_cast<T>(1) : sum0;
                    sum1 = (sum1 == static_cast<T>(0)) ? static_cast<T>(1) : sum1;
                    sum2 = (sum2 == static_cast<T>(0)) ? static_cast<T>(1) : sum2;
                }
                // Load sums into vectorial registry
                typename Ops::registry v_sum = Ops::load(static_cast<K>(sum0), static_cast<K>(sum1), static_cast<K>(sum2), static_cast<K>(1));
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * 3;
                // Get image stride accounting for kernel
                const unsigned int stride = (image.w() - kernel.w()) * 3;
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Calculate end pointer to avoid past-the-end reference leak
                const I* i_ptr_end = i_ptr + (image.h() - 2 * kernel.hh()) * image.w() * 3;
                // Loop until we reach end of image
                for (; i_ptr < i_ptr_end; i_ptr += ((kernel.w() - 1) * 3), c_ptr += ((kernel.w() - 1) * 3)) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + 3;
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr += 3, c_ptr += 3) {
                        // Scalar promotion for buffers
                        T buf0 = static_cast<T>(0), buf1 = static_cast<T>(0), buf2 = static_cast<T>(0);
                        // Get kernel start pointer
                        const K* __restrict k_ptr = &kernel[0];
                        // Get image start pointer for kernel
                        const I* __restrict si_ptr = (i_ptr - padding);
                        // Get empty register
                        typename Ops::registry v_buf = Ops::get_empty();
                        // Loop through kernel height
                        for (unsigned int ky = 0; ky < kernel.h(); ++ky) {
                            // Loop through kernel width
                            unsigned int kx = 0;
                            for (; (kx + Ops::pixel_step) <= kernel.w(); kx+=Ops::pixel_step) {
                                // Load into registry
                                typename Ops::registry v_img = Ops::load(si_ptr);
                                typename Ops::registry v_ker = Ops::load(k_ptr);
                                // Set padding to zero so its operation doest delay
                                v_img = Ops::set_zero(v_img);
                                v_ker = Ops::set_zero(v_ker);
                                // Multiply and add (v_buf += v_img * v_ker)
                                v_buf = Ops::multiply_add(v_img, v_ker, v_buf);
                                // Increase pointers
                                k_ptr += (3 * Ops::pixel_step); si_ptr += (3 * Ops::pixel_step);
                            }
                            // Tail handling
                            for (; kx < kernel.w(); ++kx) {
                                buf0 += static_cast<T>(si_ptr[0]) * static_cast<T>(k_ptr[0]);
                                buf1 += static_cast<T>(si_ptr[1]) * static_cast<T>(k_ptr[1]);
                                buf2 += static_cast<T>(si_ptr[2]) * static_cast<T>(k_ptr[2]);
                                k_ptr += 3; si_ptr += 3;
                            } si_ptr += stride;
                        }
                        // Permute channels before collapsing
                        v_buf = Ops::permute_3_channels(v_buf);
                        // Collapse 2 half sums
                        v_buf = Ops::collapse(v_buf);
                        // Load sums into vectorial registry
                        typename Ops::registry v_tail = Ops::load(static_cast<K>(buf0), static_cast<K>(buf1), static_cast<K>(buf2), static_cast<K>(0));
                        // Sum tail to buffer
                        v_buf = Ops::add(v_buf, v_tail);
                        // Two ways of doing things depending on decimals or not
                        if constexpr (std::floating_point<K>) { v_buf = Ops::multiply(v_buf, v_sum); } else { v_buf = Ops::divide(v_buf, v_sum); }
                        Ops::clamp(v_buf);
                        // Get vector to extract from registry
                        alignas(32) I extracted[Ops::size];
                        // Extract values from vectorial registry
                        Ops::extract(v_buf, extracted);
                        // Set computed values
                        c_ptr[0] = extracted[0]; c_ptr[1] = extracted[1]; c_ptr[2] = extracted[2];
                    }
                } break;
            }
            case 4: {
                // Scalar promotion for sums
                T sum0 = static_cast<T>(0), sum1 = static_cast<T>(0), sum2 = static_cast<T>(0), sum3 = static_cast<T>(0);
                // Loop through kernel length
                #pragma GCC ivdep
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr <= &kernel[kernel.len()-1]; k_ptr += 4) {
                    sum0 += static_cast<T>(k_ptr[0]); sum1 += static_cast<T>(k_ptr[1]); sum2 += static_cast<T>(k_ptr[2]); sum3 += static_cast<T>(k_ptr[3]);
                }
                // Two ways of doing things depending on decimals or not
                if constexpr (std::floating_point<T>) {
                    // Check if zero with some leeway
                    sum0 = (std::abs(sum0) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum0);
                    sum1 = (std::abs(sum1) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum1);
                    sum2 = (std::abs(sum2) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum2);
                    sum3 = (std::abs(sum3) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum3);
                } else {
                    // Check if zero with some leeway
                    sum0 = (sum0 == static_cast<T>(0)) ? static_cast<T>(1) : sum0;
                    sum1 = (sum1 == static_cast<T>(0)) ? static_cast<T>(1) : sum1;
                    sum2 = (sum2 == static_cast<T>(0)) ? static_cast<T>(1) : sum2;
                    sum3 = (sum3 == static_cast<T>(0)) ? static_cast<T>(1) : sum3;
                }
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * 4;
                // Get image stride accounting for kernel
                const unsigned int stride = (image.w() - kernel.w()) * 4;
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Calculate end pointer to avoid past-the-end reference leak
                const I* i_ptr_end = i_ptr + (image.h() - 2 * kernel.hh()) * image.w() * 4;
                // Loop until we reach end of image
                for (; i_ptr < i_ptr_end; i_ptr += ((kernel.w() - 1) * 4), c_ptr += ((kernel.w() - 1) * 4)) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + 4;
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr += 4, c_ptr += 4) {
                        // Scalar promotion for buffers
                        T buf0 = static_cast<T>(0), buf1 = static_cast<T>(0), buf2 = static_cast<T>(0), buf3 = static_cast<T>(0);
                        // Get kernel start pointer
                        const K* __restrict k_ptr = &kernel[0];
                        // Get image start pointer for kernel
                        const I* __restrict si_ptr = (i_ptr - padding);
                        // Get empty register
                        typename Ops::registry v_buf = Ops::get_empty();
                        // Loop through kernel height
                        for (unsigned int ky = 0; ky < kernel.h(); ++ky) {
                            // Loop through kernel width
                            unsigned int kx = 0;
                            for (; (kx + Ops::pixel_step) <= kernel.w(); kx+=Ops::pixel_step) {
                                // Load into registry
                                typename Ops::registry v_img = Ops::load(si_ptr);
                                typename Ops::registry v_ker = Ops::load(k_ptr);
                                // Multiply and add (v_buf += v_img * v_ker)
                                v_buf = Ops::multiply_add(v_img, v_ker, v_buf);
                                // Increase pointers
                                k_ptr += (4 * Ops::pixel_step); si_ptr += (4 * Ops::pixel_step);
                            }
                            // Tail handling
                            for (; kx < kernel.w(); ++kx) {
                                buf0 += static_cast<T>(si_ptr[0]) * static_cast<T>(k_ptr[0]);
                                buf1 += static_cast<T>(si_ptr[1]) * static_cast<T>(k_ptr[1]);
                                buf2 += static_cast<T>(si_ptr[2]) * static_cast<T>(k_ptr[2]);
                                buf3 += static_cast<T>(si_ptr[3]) * static_cast<T>(k_ptr[3]);
                                k_ptr += 4; si_ptr += 4;
                            } si_ptr += stride;
                        }
                        // Store vectorized accumulators to array
                        alignas(64) K raw_buf[Ops::size];
                        Ops::store(v_buf, raw_buf);

                        // Accumulate SIMD lanes and tail scalars
                        T total0 = static_cast<T>(raw_buf[0]) + buf0;
                        T total1 = static_cast<T>(raw_buf[1]) + buf1;
                        T total2 = static_cast<T>(raw_buf[2]) + buf2;
                        T total3 = static_cast<T>(raw_buf[3]) + buf3;

                        for (unsigned int p = 1; p < Ops::pixel_step; ++p) {
                            total0 += static_cast<T>(raw_buf[p * 4 + 0]);
                            total1 += static_cast<T>(raw_buf[p * 4 + 1]);
                            total2 += static_cast<T>(raw_buf[p * 4 + 2]);
                            total3 += static_cast<T>(raw_buf[p * 4 + 3]);
                        }

                        // Normalize
                        if constexpr (std::floating_point<T>) {
                            total0 *= sum0; total1 *= sum1; total2 *= sum2; total3 *= sum3;
                        } else {
                            total0 /= sum0; total1 /= sum1; total2 /= sum2; total3 /= sum3;
                        }

                        // Clamp and cast to image bounds
                        c_ptr[0] = static_cast<I>(std::clamp(total0, static_cast<T>(Ops::cmin), static_cast<T>(Ops::cmax)));
                        c_ptr[1] = static_cast<I>(std::clamp(total1, static_cast<T>(Ops::cmin), static_cast<T>(Ops::cmax)));
                        c_ptr[2] = static_cast<I>(std::clamp(total2, static_cast<T>(Ops::cmin), static_cast<T>(Ops::cmax)));
                        c_ptr[3] = static_cast<I>(std::clamp(total3, static_cast<T>(Ops::cmin), static_cast<T>(Ops::cmax)));
                    }
                } break;
            }
            default: {
                // Initialize sum vector
                std::vector<T> sums(kernel.c(), static_cast<T>(0));
                // Loop through kernel length
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr <= &kernel[kernel.len()-1]; k_ptr+=kernel.c()) {
                    // Precompute kernel sums
                    #pragma GCC ivdep
                    for (unsigned int c = 0; c < kernel.c(); ++c) {
                        sums[c] += static_cast<T>(k_ptr[c]);
                    }
                } // Check if zero with some leeway
                #pragma GCC ivdep
                for (unsigned int c = 0; c < kernel.c(); ++c) {
                    // Two ways of doing things depending on decimals or not
                    if constexpr (std::floating_point<T>) {
                        // Check if zero with some leeway
                        sums[c] = (std::abs(sums[c]) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sums[c]);
                    } else {
                        // Check if zero with some leeway
                        sums[c] = (sums[c] == static_cast<T>(0)) ? static_cast<T>(1) : sums[c];
                    }
                }
                // Operation buffer
                std::vector<T> buffers(image.c(), static_cast<T>(0));
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * image.c();
                // Get image stride accounting for kernel
                const unsigned int stride = ((image.w() - kernel.w()) * image.c());
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Calculate end pointer to avoid past-the-end reference leak
                const I* i_ptr_end = i_ptr + (image.h() - 2 * kernel.hh()) * image.w() * image.c();
                // Loop until we reach end of image
                for (; i_ptr < i_ptr_end; i_ptr+=((kernel.w() - 1)*image.c()), c_ptr+=((kernel.w() - 1)*conv.c())) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + image.c();
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr+=image.c(), c_ptr+=conv.c()) {
                        // Reset accumulators
                        std::fill_n(buffers.data(), image.c(), 0.0);
                        // Get kernel start pointer
                        const K* __restrict k_ptr = &kernel[0];
                        // Get image start pointer for kernel
                        const I* __restrict si_ptr = (i_ptr - padding);
                        // Loop through kernel height
                        for (unsigned int ky = 0; ky < kernel.h(); ++ky) {
                            // Loop through kernel width
                            for (unsigned int kx = 0; kx < kernel.w(); ++kx) {
                                // Loop through channels
                                #pragma GCC ivdep
                                for (unsigned int c = 0; c < image.c(); ++c) {
                                    // Accumulate operations
                                    buffers[c] += static_cast<T>(si_ptr[c]) * static_cast<T>(k_ptr[c]);
                                } k_ptr+=kernel.c(); si_ptr+=kernel.c();
                            } si_ptr += stride;
                        }
                        // Loop through channels
                        #pragma GCC ivdep
                        for (unsigned int c = 0; c < image.c(); ++c) {
                            // Two ways of doing things depending on decimals or not
                            if constexpr (std::floating_point<T>) {
                                // Normalize, sums[c] is never 0
                                buffers[c] *= sums[c];
                            } else {
                                // Normalize, sums[c] is never 0
                                buffers[c] /= sums[c];
                            }
                        }
                        // Loop through channels
                        #pragma GCC ivdep
                        for (unsigned int c = 0; c < image.c(); ++c) {
                            // Clamp buffer to ensure its in range and cast to unsigned char
                            c_ptr[c] = static_cast<I>(std::clamp(buffers[c], static_cast<T>(Ops::cmin), static_cast<T>(Ops::cmax)));
                        }
                    }
                } break;
            }
        } return conv;
    }
    // --- Auto-Vectorizable Implementation ---
    // This path is taken when SIMD intrinsics are disabled but the compiler can still
    // attempt to auto-vectorize the code.
    #elif defined(VECTORIZABLE_CONVO)
        // Initialize convolution image
        Image<I> conv(image.w(), image.h(), image.c());
        // Get clamp limits depending on the type
        T cmin, cmax;
        if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = static_cast<T>(std::numeric_limits<I>::min()), cmax = static_cast<T>(std::numeric_limits<I>::max()); }
        // Specialized algorithm for 3 & 4 channels or fallback to slower one for N channels
        switch (image.c()) {
            case 3: {
                // Scalar promotion for sums
                T sum0 = static_cast<T>(0), sum1 = static_cast<T>(0), sum2 = static_cast<T>(0);
                // Loop through kernel length
                #pragma GCC ivdep
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr <= &kernel[kernel.len()-1]; k_ptr += 3) {
                    sum0 += static_cast<T>(k_ptr[0]); sum1 += static_cast<T>(k_ptr[1]); sum2 += static_cast<T>(k_ptr[2]);
                }
                // Two ways of doing things depending on decimals or not
                if constexpr (std::floating_point<T>) {
                    // Check if zero with some leeway
                    sum0 = (std::abs(sum0) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum0);
                    sum1 = (std::abs(sum1) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum1);
                    sum2 = (std::abs(sum2) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum2);
                } else {
                    // Check if zero with some leeway
                    sum0 = (sum0 == static_cast<T>(0)) ? static_cast<T>(1) : sum0;
                    sum1 = (sum1 == static_cast<T>(0)) ? static_cast<T>(1) : sum1;
                    sum2 = (sum2 == static_cast<T>(0)) ? static_cast<T>(1) : sum2;
                }
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * 3;
                // Get image stride accounting for kernel
                const unsigned int stride = (image.w() - kernel.w()) * 3;
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Calculate end pointer to avoid past-the-end reference leak
                const I* i_ptr_end = i_ptr + (image.h() - 2 * kernel.hh()) * image.w() * 3;
                // Loop until we reach end of image
                for (; i_ptr < i_ptr_end; i_ptr += ((kernel.w() - 1) * 3), c_ptr += ((kernel.w() - 1) * 3)) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + 3;
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr += 3, c_ptr += 3) {
                        // Scalar promotion for buffers
                        T buf0 = static_cast<T>(0), buf1 = static_cast<T>(0), buf2 = static_cast<T>(0);
                        // Get kernel start pointer
                        const K* __restrict k_ptr = &kernel[0];
                        // Get image start pointer for kernel
                        const I* __restrict si_ptr = (i_ptr - padding);
                        // Loop through kernel height
                        for (unsigned int ky = 0; ky < kernel.h(); ++ky) {
                            // Loop through kernel width
                            #pragma GCC ivdep
                            for (unsigned int kx = 0; kx < kernel.w(); ++kx) {
                                // Unrolled channel accumulation
                                buf0 += static_cast<T>(si_ptr[0]) * static_cast<T>(k_ptr[0]);
                                buf1 += static_cast<T>(si_ptr[1]) * static_cast<T>(k_ptr[1]);
                                buf2 += static_cast<T>(si_ptr[2]) * static_cast<T>(k_ptr[2]);
                                k_ptr += 3; si_ptr += 3;
                            } si_ptr += stride;
                        }
                        // Two ways of doing things depending on decimals or not
                        if constexpr (std::floating_point<T>) {
                            // Fused Normalization and Clamping per channel
                            c_ptr[0] = static_cast<I>(std::clamp(buf0 * sum0, cmin, cmax));
                            c_ptr[1] = static_cast<I>(std::clamp(buf1 * sum1, cmin, cmax));
                            c_ptr[2] = static_cast<I>(std::clamp(buf2 * sum2, cmin, cmax));
                        } else {
                            // Fused Normalization and Clamping per channel
                            c_ptr[0] = static_cast<I>(std::clamp(buf0 / sum0, cmin, cmax));
                            c_ptr[1] = static_cast<I>(std::clamp(buf1 / sum1, cmin, cmax));
                            c_ptr[2] = static_cast<I>(std::clamp(buf2 / sum2, cmin, cmax));
                        }
                    }
                } break;
            }
            case 4: {
                // Scalar promotion for sums
                T sum0 = static_cast<T>(0), sum1 = static_cast<T>(0), sum2 = static_cast<T>(0), sum3 = static_cast<T>(0);
                // Loop through kernel length
                #pragma GCC ivdep
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr <= &kernel[kernel.len()-1]; k_ptr += 4) {
                    sum0 += static_cast<T>(k_ptr[0]); sum1 += static_cast<T>(k_ptr[1]); sum2 += static_cast<T>(k_ptr[2]); sum3 += static_cast<T>(k_ptr[3]);
                }
                // Two ways of doing things depending on decimals or not
                if constexpr (std::floating_point<T>) {
                    // Check if zero with some leeway
                    sum0 = (std::abs(sum0) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum0);
                    sum1 = (std::abs(sum1) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum1);
                    sum2 = (std::abs(sum2) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum2);
                    sum3 = (std::abs(sum3) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sum3);
                } else {
                    // Check if zero with some leeway
                    sum0 = (sum0 == static_cast<T>(0)) ? static_cast<T>(1) : sum0;
                    sum1 = (sum1 == static_cast<T>(0)) ? static_cast<T>(1) : sum1;
                    sum2 = (sum2 == static_cast<T>(0)) ? static_cast<T>(1) : sum2;
                    sum3 = (sum3 == static_cast<T>(0)) ? static_cast<T>(1) : sum3;
                }
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * 4;
                // Get image stride accounting for kernel
                const unsigned int stride = (image.w() - kernel.w()) * 4;
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Calculate end pointer to avoid past-the-end reference leak
                const I* i_ptr_end = i_ptr + (image.h() - 2 * kernel.hh()) * image.w() * 4;
                // Loop until we reach end of image
                for (; i_ptr < i_ptr_end; i_ptr += ((kernel.w() - 1) * 4), c_ptr += ((kernel.w() - 1) * 4)) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + 4;
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr += 4, c_ptr += 4) {
                        // Scalar promotion for buffers
                        T buf0 = static_cast<T>(0), buf1 = static_cast<T>(0), buf2 = static_cast<T>(0), buf3 = static_cast<T>(0);
                        // Get kernel start pointer
                        const K* __restrict k_ptr = &kernel[0];
                        // Get image start pointer for kernel
                        const I* __restrict si_ptr = (i_ptr - padding);
                        // Loop through kernel height
                        for (unsigned int ky = 0; ky < kernel.h(); ++ky) {
                            // Loop through kernel width
                            #pragma GCC ivdep
                            for (unsigned int kx = 0; kx < kernel.w(); ++kx) {
                                // Unrolled channel accumulation
                                buf0 += static_cast<T>(si_ptr[0]) * static_cast<T>(k_ptr[0]);
                                buf1 += static_cast<T>(si_ptr[1]) * static_cast<T>(k_ptr[1]);
                                buf2 += static_cast<T>(si_ptr[2]) * static_cast<T>(k_ptr[2]);
                                buf3 += static_cast<T>(si_ptr[3]) * static_cast<T>(k_ptr[3]);
                                k_ptr += 4; si_ptr += 4;
                            } si_ptr += stride;
                        }
                        // Two ways of doing things depending on decimals or not
                        if constexpr (std::floating_point<T>) {
                            // Fused Normalization and Clamping per channel
                            c_ptr[0] = static_cast<I>(std::clamp(buf0 * sum0, cmin, cmax));
                            c_ptr[1] = static_cast<I>(std::clamp(buf1 * sum1, cmin, cmax));
                            c_ptr[2] = static_cast<I>(std::clamp(buf2 * sum2, cmin, cmax));
                            c_ptr[3] = static_cast<I>(std::clamp(buf3 * sum3, cmin, cmax));
                        } else {
                            // Fused Normalization and Clamping per channel
                            c_ptr[0] = static_cast<I>(std::clamp(buf0 / sum0, cmin, cmax));
                            c_ptr[1] = static_cast<I>(std::clamp(buf1 / sum1, cmin, cmax));
                            c_ptr[2] = static_cast<I>(std::clamp(buf2 / sum2, cmin, cmax));
                            c_ptr[3] = static_cast<I>(std::clamp(buf3 / sum3, cmin, cmax));
                        }
                    }
                } break;
            }
            default: {
                // Initialize sum vector
                std::vector<T> sums(kernel.c(), static_cast<T>(0));
                // Loop through kernel length
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr <= &kernel[kernel.len()-1]; k_ptr+=kernel.c()) {
                    // Precompute kernel sums
                    #pragma GCC ivdep
                    for (unsigned int c = 0; c < kernel.c(); ++c) {
                        sums[c] += static_cast<T>(k_ptr[c]);
                    }
                } // Check if zero with some leeway
                #pragma GCC ivdep
                for (unsigned int c = 0; c < kernel.c(); ++c) {
                    // Two ways of doing things depending on decimals or not
                    if constexpr (std::floating_point<T>) {
                        // Check if zero with some leeway
                        sums[c] = (std::abs(sums[c]) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sums[c]);
                    } else {
                        // Check if zero with some leeway
                        sums[c] = (sums[c] == static_cast<T>(0)) ? static_cast<T>(1) : sums[c];
                    }
                }
                // Operation buffer
                std::vector<T> buffers(image.c(), static_cast<T>(0));
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * image.c();
                // Get image stride accounting for kernel
                const unsigned int stride = ((image.w() - kernel.w()) * image.c());
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Calculate end pointer to avoid past-the-end reference leak
                const I* i_ptr_end = i_ptr + (image.h() - 2 * kernel.hh()) * image.w() * image.c();
                // Loop until we reach end of image
                for (; i_ptr < i_ptr_end; i_ptr+=((kernel.w() - 1)*image.c()), c_ptr+=((kernel.w() - 1)*conv.c())) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + image.c();
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr+=image.c(), c_ptr+=conv.c()) {
                        // Reset accumulators
                        std::fill_n(buffers.data(), image.c(), 0.0);
                        // Get kernel start pointer
                        const K* __restrict k_ptr = &kernel[0];
                        // Get image start pointer for kernel
                        const I* __restrict si_ptr = (i_ptr - padding);
                        // Loop through kernel height
                        for (unsigned int ky = 0; ky < kernel.h(); ++ky) {
                            // Loop through kernel width
                            for (unsigned int kx = 0; kx < kernel.w(); ++kx) {
                                // Loop through channels
                                #pragma GCC ivdep
                                for (unsigned int c = 0; c < image.c(); ++c) {
                                    // Accumulate operations
                                    buffers[c] += static_cast<T>(si_ptr[c]) * static_cast<T>(k_ptr[c]);
                                } k_ptr+=kernel.c(); si_ptr+=kernel.c();
                            } si_ptr += stride;
                        }
                        // Loop through channels
                        #pragma GCC ivdep
                        for (unsigned int c = 0; c < image.c(); ++c) {
                            // Two ways of doing things depending on decimals or not
                            if constexpr (std::floating_point<T>) {
                                // Normalize, sums[c] is never 0
                                buffers[c] *= sums[c];
                            } else {
                                // Normalize, sums[c] is never 0
                                buffers[c] /= sums[c];
                            }
                        }
                        // Loop through channels
                        #pragma GCC ivdep
                        for (unsigned int c = 0; c < image.c(); ++c) {
                            // Clamp buffer to ensure its in range and cast to unsigned char
                            c_ptr[c] = static_cast<I>(std::clamp(buffers[c], cmin, cmax));
                        }
                    }
                } break;
            }
        } return conv;
    // --- Fallback Naive Implementation ---
    // This is the most basic implementation, used when no other optimizations are enabled.
    #else
        // Initialize convolution image
        Image<I> conv(image.w(), image.h(), image.c());
        // Initialize sum vector
        std::vector<T> sums(kernel.c(), static_cast<T>(0));
        // Precompute kernel sums for normalization
        for (unsigned int c = 0; c < kernel.c(); ++c) {
            for (unsigned int i = 0; i < kernel.w(); ++i) {
                for (unsigned int j = 0; j < kernel.h(); ++j) {
                    sums[c] += static_cast<T>(kernel[i, j, c]);
                }
            }
            // Avoid division by zero and pre-calculate reciprocal for multiplication
            if constexpr (std::floating_point<T>) {
                sums[c] = (std::abs(sums[c]) <= static_cast<T>(1e-5)) ? static_cast<T>(1) : (static_cast<T>(1) / sums[c]);
            } else {
                sums[c] = (sums[c] == static_cast<T>(0)) ? static_cast<T>(1) : sums[c];
            }
        }
        // Iterate over each pixel of the image, avoiding the borders
        for (unsigned int y = kernel.hh(); y < (image.h() - kernel.hh()); ++y) {
            for (unsigned int x = kernel.hw(); x < (image.w() - kernel.hw()); ++x) {
                for (unsigned int c = 0; c < image.c(); ++c) {
                    T buffer = 0; // Accumulator for the convolution result
                    // Apply kernel to the neighborhood of the current pixel
                    for (unsigned int j = 0; j < kernel.h(); ++j) {
                        for (unsigned int i = 0; i < kernel.w(); ++i) {
                            buffer += static_cast<T>(image[x + (i - kernel.hw()), y + (j - kernel.hh()), c]) * static_cast<T>(kernel[i, j, c]);
                        }
                    }
                    // Get clamp limits depending on the type
                    T cmin, cmax;
                    if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = static_cast<T>(std::numeric_limits<I>::min()), cmax = static_cast<T>(std::numeric_limits<I>::max()); }
                    // Normalize, clamp and store the final pixel value
                    if constexpr (std::floating_point<T>) {
                        conv[x, y, c] = static_cast<I>(std::clamp(buffer * sums[c], cmin, cmax));
                    } else {
                        conv[x, y, c] = static_cast<I>(std::clamp(buffer / sums[c], cmin, cmax));
                    }
                }
            }
        } return conv;
    #endif
}