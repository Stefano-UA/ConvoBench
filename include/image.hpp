#pragma once
#include "config.hpp"
#include "tensor.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <algorithm>
#include <concepts>
#include <string>

/**
 * @brief An image class that extends Tensor to add image-specific functionality.
 * @tparam I The data type of the image pixels (e.g., unsigned char, float).
 *
 * This class provides constructors to create blank images or load them from
 * files using the stb_image library. It correctly handles memory management
 * for data loaded by stb_image versus memory allocated by the base Tensor class.
 * It also provides a safe, padded access operator for convolution operations.
 */
template <typename I>
class Image : public Tensor<I> {
    public:
        /**
         * @brief Constructs a new, blank Image object.
         * @param width The width of the image.
         * @param height The height of the image.
         * @param channels The number of channels.
         */
        Image(const unsigned int width, const unsigned int height, const unsigned int channels) noexcept
            : Tensor<I>(width, height, channels), is_stbi(false) { this->allocate(this->length);
            // This memory is managed by the base Tensor's destructor.
        }

        /**
         * @brief Constructs a new Image by loading from a file path.
         * @param path The filesystem path to the image file.
         * @throws std::runtime_error If loading the image fails.
         * @note This constructor is only enabled for `unsigned char` and `float` types.
         */
        Image(const std::string& path) NOEXCEPT_THROWS requires std::same_as<I, float> || std::same_as<I, unsigned char>
            : Tensor<I>(0, 0, 0), is_stbi(true) {
            int w, h, c;
            // Use stbi_loadf for float images and stbi_load for unsigned char images.
            if constexpr (std::same_as<I, float>) {
                this->data = stbi_loadf(path.c_str(), &w, &h, &c, MIN_CHANNELS);
            } else {
                this->data = stbi_load(path.c_str(), &w, &h, &c, MIN_CHANNELS);
            }

            // Throw an error if the image failed to load.
            #ifndef NO_THROWS
                if (this->data == NULL) { throw std::runtime_error("Failed to load image: " + path); }
            #endif

            // Store properties from the loaded image.
            this->width = static_cast<unsigned int>(w);
            this->height = static_cast<unsigned int>(h);
            this->channels = static_cast<unsigned int>(std::max(c, MIN_CHANNELS));
            this->length = (this->width * this->height * this->channels);
        }

        /**
         * @brief Destructor for the Image class.
         * Frees memory using `stbi_image_free` if it was loaded from a file.
         * Otherwise, the base Tensor destructor will handle memory allocated with `new[]`.
         */
        ~Image() noexcept {
            if (this->is_stbi) {
                stbi_image_free(this->data);
                this->data = nullptr;
            }
        }

        /**
         * @brief Move constructor for Image.
         * @param o The source Image to move from.
         * Transfers ownership of the tensor data and the `is_stbi` flag.
         */
        Image(Image<I>&& o) noexcept : Tensor<I>(std::move(o)), is_stbi(o.is_stbi) {}

        // Inherit the base class's access operators.
        using Tensor<I>::operator[];

        /**
         * @brief Provides safe, read-only access for operations like convolution.
         * @param x The width coordinate.
         * @param y The height coordinate.
         * @param c The channel coordinate.
         * @return A const reference to the element at the coordinates, or to a
         *         static padding value (0) if the access is out of bounds.
         * @note This behavior is enabled by the `PADDED_ACCESS` macro.
         */
        inline const I& operator[](const unsigned int x, const unsigned int y, const unsigned int c) const noexcept {
            #ifdef PADDED_ACCESS
                if ((x >= this->width) || (y >= this->height)) { return Image<I>::padding; }
            #endif
            return Tensor<I>::operator[](x, y, c);
        }

        /**
         * @brief Writes the image data to a file.
         * @param path The filesystem path where the image will be saved.
         * @note Writes HDR files for `float` images and PNG files for `unsigned char` images.
         *       It also ensures the alpha channel is fully opaque if present.
         */
        void write(const std::string& path) NOEXCEPT requires std::same_as<I, float> || std::same_as<I, unsigned char> {
            if constexpr (std::same_as<I, float>) {
                if (this->channels == 4) {
                    // Set alpha channel to 1.0 for HDR
                    for (unsigned int s = 3; s < this->length; s+=4) { (*this)[s] = static_cast<I>(1); }
                }
                stbi_write_hdr(path.c_str(), this->width, this->height, this->channels, this->data);
            } else {
                if (this->channels == 4) {
                    // Set alpha channel to 255 for PNG
                    for (unsigned int s = 3; s < this->length; s+=4) { (*this)[s] = static_cast<I>(255); }
                }
                stbi_write_png(path.c_str(), this->width, this->height, this->channels, this->data, 0);
            }
        }

    private:
        /** @brief A static padding value (0) returned for out-of-bounds access. */
        static constexpr I padding = 0;
        /** @brief Flag to track if memory is owned by STB Image (and needs `stbi_image_free`). */
        const bool is_stbi;
};