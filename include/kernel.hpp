#pragma once
#include "config.hpp"
#include "tensor.hpp"
#include "image.hpp"
#include <initializer_list>
#include <concepts>
#include <vector>
#include <cmath>

/**
 * @brief Represents a convolution kernel (filter), inheriting from Tensor.
 * @tparam K The data type of the kernel elements (e.g., float, int).
 *
 * This class provides a convenient way to create and manage convolution kernels.
 * It includes a `set` method for manual initialization and a wide range of
 * static factory methods to generate common kernels like Gaussian blur, Sobel,
 * Laplacian, and more.
 */
template <typename K>
class Kernel : public Tensor<K> {
    public:
        /**
         * @brief Constructs a new, blank Kernel object.
         * @param width The width of the kernel.
         * @param height The height of the kernel.
         * @param channels The number of channels.
         */
        Kernel(const unsigned int width, const unsigned int height, const unsigned int channels) noexcept
            : Tensor<K>(width, height, channels) { this->allocate(this->length);
            // Memory is allocated by the base Tensor constructor.
        }

        /**
         * @brief Move constructor for Kernel.
         * @param o The source Kernel to move from.
         */
        Kernel(Kernel<K>&& o) noexcept : Tensor<K>(std::move(o)) {}

        // Inherit the base class's access operators.
        using Tensor<K>::operator[];

        /**
         * @brief Sets kernel values from a 2D initializer list.
         * @param matrix The 2D initializer list (e.g., `{{0,-1,0}, {-1,4,-1}, {0,-1,0}}`).
         * The 2D pattern is applied identically across all channels of the kernel.
         * @throws std::invalid_argument If the matrix dimensions do not match the kernel's.
         */
        void set(const std::initializer_list<std::initializer_list<K>>& matrix) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (matrix.size() != this->height || matrix.begin()->size() != this->width) {
                    throw std::invalid_argument("Initializer list dimensions do not match kernel dimensions");
                }
            #endif
            K* ptr = this->data;
            const unsigned int c = this->channels;
            for (const auto& row : matrix) {
                for (const K v : row) {
                    std::fill_n(ptr, c, v);
                    ptr += c;
                }
            }
        }

        /** @brief Gets the half-width of the kernel (radius). */
        inline unsigned int hw() const noexcept { return (this->width / 2); }
        /** @brief Gets the half-height of the kernel (radius). */
        inline unsigned int hh() const noexcept { return (this->height / 2); }

        /**
         * @brief Creates a 1x1 identity kernel.
         * @param channels The number of channels for the kernel.
         * @return A new Kernel object that, when convoluted, does not modify the image.
         */
        [[nodiscard]] static Kernel<K> identity(const unsigned int channels = 3) NOEXCEPT_THROWS {
            Kernel<K> k(1, 1, channels);
            for (unsigned int s = 0; s < k.length; ++s) {
                k[s] = static_cast<K>(1);
            } return k;
        }

        /**
         * @brief Creates a box blur kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel object for a simple averaging blur.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> box_blur(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            // Fill with 1. Normalization is handled by the convolution function.
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = static_cast<K>(1); } return k;
        }

        /**
         * @brief Creates a Gaussian blur kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel object for a Gaussian blur.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> gaussian(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            if constexpr (std::floating_point<K>) {
                Kernel<K> k(size, size, channels);
                // Sigma is chosen so the curve fits nicely within the kernel dimensions.
                const K sigma = std::max(static_cast<K>(1), size / static_cast<K>(6));
                const K two_sigma_sq_inv = static_cast<K>(1) / (static_cast<K>(2) * sigma * sigma);
                const unsigned int radius = size / 2;
                // Precompute Gaussian values for one dimension to build the 2D kernel.
                std::vector<K> gaussian_vs(size);
                for (unsigned int i = 0; i < size; ++i) {
                    const int x = (i - radius);
                    gaussian_vs[i] = static_cast<K>(std::exp(-(x * x) * two_sigma_sq_inv));
                }
                // Fill the 2D kernel by combining the 1D values.
                K* ptr = k.data;
                for (unsigned int y = 0; y < size; ++y) {
                    const K v_y = gaussian_vs[y];
                    for (unsigned int x = 0; x < size; ++x) {
                        const K v = (v_y * gaussian_vs[x]); // exp(a+b) = exp(a)*exp(b)
                        std::fill_n(ptr, channels, v);
                        ptr += channels;
                    }
                } return k;
            } else {
                // For integer types, compute with float and scale the results.
                Kernel<K> k(size, size, channels);
                Kernel<float> temp = Kernel<float>::gaussian(size, channels);
                const float min_val = temp[0]; // Min value is at the corners.
                // Scale, round, and cast to the integer type.
                for (unsigned int i = 0; i < k.len(); ++i) {
                    k[i] = static_cast<K>(std::round(temp[i] / min_val));
                } return k;
            }
        }

        /**
         * @brief Creates a 3x3 sharpening kernel.
         * @param channels The number of channels.
         * @return A new Kernel object that increases contrast between adjacent pixels.
         */
        [[nodiscard]] static Kernel<K> sharpen(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(3, 3, channels);
            k.set({
                { static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0)},
                {static_cast<K>(-1),  static_cast<K>(5), static_cast<K>(-1)},
                { static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0)}
            }); return k;
        }

        /**
         * @brief Creates a 5x5 sharpening kernel for a more aggressive effect.
         * @param channels The number of channels.
         * @return A new Kernel object that provides a stronger sharpening effect.
         */
        [[nodiscard]] static Kernel<K> sharpen5(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(5, 5, channels);
            k.set({
                {static_cast<K>(-1), static_cast<K>(-1), static_cast<K>(-1), static_cast<K>(-1), static_cast<K>(-1)},
                {static_cast<K>(-1),  static_cast<K>(2),  static_cast<K>(2),  static_cast<K>(2), static_cast<K>(-1)},
                {static_cast<K>(-1),  static_cast<K>(2),  static_cast<K>(8),  static_cast<K>(2), static_cast<K>(-1)},
                {static_cast<K>(-1),  static_cast<K>(2),  static_cast<K>(2),  static_cast<K>(2), static_cast<K>(-1)},
                {static_cast<K>(-1), static_cast<K>(-1), static_cast<K>(-1), static_cast<K>(-1), static_cast<K>(-1)}
            }); return k;
        }

        /**
         * @brief Creates a 3x3 Laplacian kernel for edge detection.
         * @param channels The number of channels.
         * @return A new Kernel object for standard omnidirectional edge detection.
         */
        [[nodiscard]] static Kernel<K> laplacian(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(3, 3, channels);
            k.set({
                { static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0)},
                {static_cast<K>(-1),  static_cast<K>(4), static_cast<K>(-1)},
                { static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0)}
            }); return k;
        }

        /**
         * @brief Creates a 5x5 Laplacian of Gaussian (LoG) kernel.
         * @param channels The number of channels.
         * @return A new Kernel object that combines a Gaussian blur with a Laplacian
         *         filter, making it less sensitive to noise.
         */
        [[nodiscard]] static Kernel<K> laplacian5(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(5, 5, channels);
            k.set({
                { static_cast<K>(0),  static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0),  static_cast<K>(0)},
                { static_cast<K>(0), static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(-1),  static_cast<K>(0)},
                {static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(16), static_cast<K>(-2), static_cast<K>(-1)},
                { static_cast<K>(0), static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(-1),  static_cast<K>(0)},
                { static_cast<K>(0),  static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0),  static_cast<K>(0)}
            }); return k;
        }

        /**
         * @brief Creates a 3x3 Sobel kernel for horizontal edge detection.
         * @param channels The number of channels.
         * @return A new Kernel object that detects vertical lines.
         */
        [[nodiscard]] static Kernel<K> sobel_x(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(3, 3, channels);
            k.set({
                {static_cast<K>(-1), static_cast<K>(0), static_cast<K>(1)},
                {static_cast<K>(-2), static_cast<K>(0), static_cast<K>(2)},
                {static_cast<K>(-1), static_cast<K>(0), static_cast<K>(1)}
            }); return k;
        }

        /**
         * @brief Creates a 3x3 Sobel kernel for vertical edge detection.
         * @param channels The number of channels.
         * @return A new Kernel object that detects horizontal lines.
         */
        [[nodiscard]] static Kernel<K> sobel_y(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(3, 3, channels);
            k.set({
                {static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(-1)},
                { static_cast<K>(0),  static_cast<K>(0),  static_cast<K>(0)},
                { static_cast<K>(1),  static_cast<K>(2),  static_cast<K>(1)}
            }); return k;
        }

        /**
         * @brief Creates a 3x3 emboss kernel.
         * @param channels The number of channels.
         * @return A new Kernel object that simulates a 3D relief effect.
         */
        [[nodiscard]] static Kernel<K> emboss(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(3, 3, channels);
            k.set({
                {static_cast<K>(-2), static_cast<K>(-1),  static_cast<K>(0)},
                {static_cast<K>(-1),  static_cast<K>(1),  static_cast<K>(1)},
                { static_cast<K>(0),  static_cast<K>(1),  static_cast<K>(2)}
            }); return k;
        }

        /**
         * @brief Creates a horizontal motion blur kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel object that simulates horizontal motion.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> motion_blur_x(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, 1, channels);
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = static_cast<K>(1); } return k;
        }

        /**
         * @brief Creates a vertical motion blur kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel object that simulates vertical motion.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> motion_blur_y(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(1, size, channels);
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = static_cast<K>(1); } return k;
        }

        /**
         * @brief Creates a horizontal "glitch" kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel that produces a chromatic aberration effect.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> glitch_x(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, 1, channels);
            k[0, 0, 0] = static_cast<K>(1);
            k[(k.width / 2), 0, 1] = static_cast<K>(1);
            k[(k.width - 1), 0, 2] = static_cast<K>(1);
            for (unsigned int c = 3; c < channels; ++c) {
                k[(k.width / 2), 0, c] = static_cast<K>(1);
            } return k;
        }

        /**
         * @brief Creates a vertical "glitch" kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel that produces a vertical chromatic aberration effect.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> glitch_y(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(1, size, channels);
            k[0, 0, 0] = static_cast<K>(1);
            k[0, (k.height / 2), 1] = static_cast<K>(1);
            k[0, (k.height - 1), 2] = static_cast<K>(1);
            for (unsigned int c = 3; c < channels; ++c) {
                k[0, (k.height / 2), c] = static_cast<K>(1);
            } return k;
        }

        /**
         * @brief Creates a "ghosting" kernel.
         * @param size The size of the kernel (must be an odd number).
         * @param channels The number of channels.
         * @return A new Kernel object that produces a light smearing/unfocused effect.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> ghost(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            k[0] = static_cast<K>(1);
            k[k.length - 1] = static_cast<K>(1);
            return k;
        }

        /**
         * @brief Creates a "glow" kernel that blurs all channels except one.
         * @param size The size of the kernel (must be an odd number).
         * @param channel The single channel to *not* apply the blur effect to.
         * @param channels The number of channels.
         * @return A new Kernel object that produces a glow effect on specific channels.
         * @throws std::invalid_argument If `size` is not an odd number.
         */
        [[nodiscard]] static Kernel<K> glow(const unsigned int size, const unsigned int channel, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            for (unsigned int c = 0; c < k.channels; ++c) {
                if (c == channel) { continue; }
                for (unsigned int s = 0; s < k.length; s += k.channels) {
                    k[s + c] = static_cast<K>(1);
                }
            } return k;
        }

        /**
         * @brief Calculates a dynamic kernel size based on image dimensions.
         * @tparam I The data type of the image.
         * @param image The reference image to scale the kernel against.
         * @param ratio The fraction of the smaller image dimension to use for kernel size.
         * @return An odd integer for the kernel size (minimum of 3).
         */
        template<typename I>
        static unsigned int get_size_by_ratio(const Image<I>& image, const double ratio = RATIO) NOEXCEPT_THROWS {
            unsigned int size = static_cast<unsigned int>(std::min(image.w(), image.h()) * ratio);
            // Ensure size is always odd.
            if (size % 2 == 0) { ++size; }
            // Ensure a minimum size.
            return std::max(3U, size);
        }
};