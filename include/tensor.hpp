#pragma once
#include "config.hpp"
#include <iostream>
#include <algorithm>
#include <cstdlib>

/**
 * @brief A templated base class for a 3D tensor (Width x Height x Channels).
 * @tparam T The data type of the tensor elements (e.g., float, unsigned char).
 *
 * This class manages a flat memory block for 3D data. It is designed to be a
 * base for more specialized classes like images or kernels. Copy semantics are
 * disabled to prevent accidental deep copies of large data buffers; move
 * semantics are supported for efficient transfers of ownership.
 */
template <typename T>
class Tensor {
    public:
        /**
         * @brief Constructs a new Tensor object.
         * @param width The width of the tensor.
         * @param height The height of the tensor.
         * @param channels The number of channels.
         * @note This constructor does not allocate memory. Derived classes are
         * responsible for memory allocation via the `allocate` method.
         */
        Tensor(const unsigned int width, const unsigned int height, const unsigned int channels) noexcept
            : width(width), height(height), channels(channels), length(width * height * channels), data(nullptr) {}

        /**
         * @brief Virtual destructor to ensure proper cleanup of derived classes.
         * Frees the allocated memory if the data pointer is not null.
         */
        virtual ~Tensor() noexcept {
            if (this->data != nullptr) {
                #ifdef ALIGNED_ALLOCATION
                    std::free(this->data);
                #else
                    delete[] this->data;
                #endif
                this->data = nullptr;
            }
        }

        // Disable copy constructor and copy assignment to prevent expensive deep copies.
        Tensor(const Tensor<T>&) = delete;
        Tensor<T>& operator=(const Tensor<T>&) = delete;

        /**
         * @brief Move constructor.
         * @param o The source Tensor to move from.
         * Transfers ownership of the memory from the source object to this one,
         * leaving the source in a valid but empty state.
         */
        Tensor(Tensor<T>&& o) noexcept
            : width(o.width), height(o.height), channels(o.channels), length(o.length), data(o.data) {
            // Leave the source object in a safe, empty state.
            o.data = nullptr;
        }

        /**
         * @brief Provides mutable access to the tensor data using a linear index.
         * @param ix The linear index.
         * @return A reference to the element at the specified index.
         * @throws std::out_of_range If CHECK_BOUNDS is defined and the index is out of bounds.
         */
        inline T& operator[](const unsigned int ix) NOEXCEPT_CHECKBOUNDS {
            #ifdef CHECK_BOUNDS
                if (ix >= this->length) { throw std::out_of_range("Tensor access out of bounds"); }
            #endif
            return this->data[ix];
        }

        /**
         * @brief Provides read-only access to the tensor data using a linear index.
         * @param ix The linear index.
         * @return A const reference to the element at the specified index.
         * @throws std::out_of_range If CHECK_BOUNDS is defined and the index is out of bounds.
         */
        inline const T& operator[](const unsigned int ix) const NOEXCEPT_CHECKBOUNDS {
            #ifdef CHECK_BOUNDS
                if (ix >= this->length) { throw std::out_of_range("Tensor access out of bounds"); }
            #endif
            return this->data[ix];
        }

        /**
         * @brief Provides mutable access to the tensor data using 3D coordinates.
         * @param x The width coordinate.
         * @param y The height coordinate.
         * @param c The channel coordinate.
         * @return A reference to the element at the specified coordinates.
         * @throws std::out_of_range If CHECK_BOUNDS is defined and coordinates are out of bounds.
         */
        inline T& operator[](const unsigned int x, const unsigned y, const unsigned c) NOEXCEPT_CHECKBOUNDS {
            #ifdef CHECK_BOUNDS
                if ((x >= this->width) || (y >= this->height)) { throw std::out_of_range("Tensor access out of bounds"); }
            #endif
            return this->data[((x + (y * this->width)) * this->channels) + c];
        }

        /**
         * @brief Provides read-only access to the tensor data using 3D coordinates.
         * @param x The width coordinate.
         * @param y The height coordinate.
         * @param c The channel coordinate.
         * @return A const reference to the element at the specified coordinates.
         * @throws std::out_of_range If CHECK_BOUNDS is defined and coordinates are out of bounds.
         */
        inline const T& operator[](const unsigned int x, const unsigned int y, const unsigned int c) const NOEXCEPT_CHECKBOUNDS {
            #ifdef CHECK_BOUNDS
                if ((x >= this->width) || (y >= this->height)) { throw std::out_of_range("Tensor access out of bounds"); }
            #endif
            return this->data[((x + (y * this->width)) * this->channels) + c];
        }

        /**
         * @brief Serializes the tensor to an output stream.
         * @param os The output stream.
         * @param tensor The tensor to serialize.
         * @return The output stream.
         * The format is: Width Height Channels [Newline] Data...
         */
        friend std::ostream& operator<<(std::ostream& os, const Tensor<T>& tensor) NOEXCEPT {
            os << tensor.width << " " << tensor.height << " " << tensor.channels << "\n";
            for (unsigned int i = 0; i < tensor.length; ++i) {
                os << tensor.data[i] << " ";
                if (((i + 1) % tensor.width) == 0) { os << "\n"; }
            } return os;
        }

        /**
         * @brief Deserializes the tensor from an input stream.
         * @param is The input stream.
         * @param tensor The tensor to deserialize into.
         * @return The input stream.
         * Reallocates memory if the dimensions read from the stream differ from the tensor's.
         */
        friend std::istream& operator>>(std::istream& is, Tensor<T>& tensor) NOEXCEPT {
            unsigned int w, h, c;
            if (!(is >> w >> h >> c)) { return is; }

            unsigned int old_length = tensor.length;
            tensor.width = w;
            tensor.height = h;
            tensor.channels = c;
            tensor.length = w * h * c;

            if (old_length != tensor.length) {
                tensor.allocate(tensor.length);
            }

            for (unsigned int i = 0; i < tensor.length; ++i) {
                is >> tensor.data[i];
            } return is;
        }

        /** @brief Gets the width of the tensor. */
        inline unsigned int w() const noexcept { return this->width; }
        /** @brief Gets the height of the tensor. */
        inline unsigned int h() const noexcept { return this->height; }
        /** @brief Gets the number of channels in the tensor. */
        inline unsigned int c() const noexcept { return this->channels; }
        /** @brief Gets the total number of elements in the tensor. */
        inline unsigned int len() const noexcept { return this->length; }

    protected:
        unsigned int width, height, channels;
        unsigned int length;
        T* data = nullptr;

        /**
         * @brief Allocates or reallocates memory for the tensor.
         * @param size The number of elements to allocate.
         * The memory is initialized to zero. If ALIGNED_ALLOCATION is defined,
         * it uses aligned memory allocation for better SIMD performance.
         */
        inline void allocate(const unsigned int size) NOEXCEPT {
            #ifdef ALIGNED_ALLOCATION
                if (this->data) { std::free(this->data); }
                const size_t alignment = 64;
                const size_t total_bytes = size * sizeof(T);
                const size_t padded_size = (total_bytes + alignment - 1) & ~(alignment - 1);
                this->data = static_cast<T*>(std::aligned_alloc(alignment, padded_size));
                if (this->data) {
                    std::fill_n(this->data, size, static_cast<T>(0));
                }
            #else
                if (this->data) { delete[] this->data; }
                this->data = new T[size]();
            #endif
        }
};