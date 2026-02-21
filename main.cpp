#include <vector>
#include <limits>
#include <concepts>
#include <iostream>
#include <algorithm>
#include <functional>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// Force lib to be memory aligned
#ifdef STB_IMAGE_IMPLEMENTATION
    #ifdef ALIGNED_ALLOCATION
        #include <cstdlib>
        #include <cstring>

        // Memory-aligned realloc implementation
        inline void* aligned_realloc(void* ptr, size_t new_size, size_t alignment) {
            if (new_size == 0) { std::free(ptr); return nullptr; }
            // Reserve new aligned block
            void* new_ptr = std::aligned_alloc(alignment, (new_size + alignment - 1) & ~(alignment - 1));
            if (!new_ptr) return nullptr;
            if (ptr) {
                // We can safely assume that reallocations are always to bigger sizes
                std::memcpy(new_ptr, ptr, new_size);
                std::free(ptr);
            } return new_ptr;
        }

        // Configure STB library to use alignead allocation
        #define STBI_MALLOC(sz)           std::aligned_alloc(64, (sz + 63) & ~63)
        #define STBI_REALLOC(p, newsz)    aligned_realloc(p, newsz, 64)
        #define STBI_FREE(p)              std::free(p)
    #endif
#endif

// Disable asserts to optimize further
#ifdef NO_THROWS
    #define STBI_ASSERT(x)
#endif

// Disable certain implementations
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

#include "stb_image.h"
#include "stb_image_write.h"

// Preprocessor flags definition logic
#ifdef HARD_ERRORS
    #define NOEXCEPT noexcept
    #define NOEXCEPT_THROWS noexcept
    #define NOEXCEPT_CHECKBOUNDS noexcept
#else
    #define NOEXCEPT
    #ifdef NO_THROWS // Disable all throws in the code
        #define NOEXCEPT_THROWS noexcept
    #else
        #define NOEXCEPT_THROWS
    #endif
    #ifdef CHECK_BOUNDS // Enable checking bounds on access
        #define NOEXCEPT_CHECKBOUNDS
    #else
        #define NOEXCEPT_CHECKBOUNDS noexcept
    #endif
#endif

// Images data type
#ifndef IMG_DTYPE
    #define IMG_DTYPE unsigned char
#endif

// Convolution data type
#ifndef CNV_DTYPE
    #define CNV_DTYPE float
#endif

#ifndef RATIO
    #define RATIO 0.01
#endif
#ifndef MIN_CHANNELS
    #define MIN_CHANNELS 3
#endif
#ifndef INPUT_DIRECTORY
    #define INPUT_DIRECTORY "./input/"
#endif
#ifndef OUTPUT_DIRECTORY
    #define OUTPUT_DIRECTORY "./output/"
#endif

// Macro for measuring the time that some instructions take to execute
#define BENCHMARK_CHRONO(name, ...) \
    do { \
        auto _bench_start = std::chrono::high_resolution_clock::now(); \
        __VA_ARGS__; \
        asm volatile("" ::: "memory"); \
        auto _bench_end = std::chrono::high_resolution_clock::now(); \
        std::chrono::duration<double, std::micro> _bench_dur = (_bench_end - _bench_start); \
        std::cout << "[CHRONO] " << name << " -> " << _bench_dur.count() << " us\n"; \
    } while(0)

// Macro for measuring the CPU cycles some instructions take to execute
#define BENCHMARK_CYCLES(name, ...) \
    do { \
        unsigned int _bench_aux; \
        uint64_t _bench_start = __rdtscp(&_bench_aux); \
        __VA_ARGS__; \
        asm volatile("" ::: "memory"); \
        uint64_t _bench_end = __rdtscp(&_bench_aux); \
        std::cout << "[CYCLES] " << name << " -> " << (_bench_end - _bench_start) << " cycles\n"; \
    } while(0)

/**
* @brief Base templated class representing a 3D tensor (Width x Height x Channels).
* * Manages raw memory manually using a flat array. Supports move semantics
* but disables copy semantics to prevent accidental deep copies of large data.
* * @tparam T The data type stored in the tensor (e.g., unsigned char, double).
*/
template <typename T>
class Tensor {
    public:
        /**
        * @brief Construct a new Tensor object.
        * Allocates memory but leaves it uninitialized.
        */
        Tensor(const unsigned int width, const unsigned int height, const unsigned int channels) noexcept
            : width(width), height(height), channels(channels), length(width * height * channels), data(nullptr) {}

        /**
        * @brief Destructor. Frees allocated memory if data is not nullptr.
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

        // Disable copying.
        // Deep copying tensors is expensive and usually unintended in this context.
        Tensor(const Tensor<T>&) = delete;
        Tensor<T>& operator=(const Tensor<T>&) = delete;

        /**
        * @brief Move constructor.
        * Transfers ownership of the memory pointer from 'o' to this object.
        * The source object 'o' is left in a valid but empty state (data = nullptr).
        */
        Tensor(Tensor<T>&& o) noexcept
            : width(o.width), height(o.height), channels(o.channels), length(o.length), data(o.data) {
            // Steal the pointer
            o.data = nullptr;
        }

        /**
        * @brief Mutable linear index access.
        * @throws std::out_of_range If ix is outside the buffer length.
        */
        inline T& operator[](const unsigned int ix) NOEXCEPT_CHECKBOUNDS {
            // Throw error when out of bounds
            #ifdef CHECK_BOUNDS
                if (ix >= this->length) { throw std::out_of_range("Coordinate out of image bounds"); }
            #endif
            return this->data[ix];
        }

        /**
        * @brief Read-only linear index access.
        * @throws std::out_of_range If ix is outside the buffer length.
        */
        inline const T& operator[](const unsigned int ix) const NOEXCEPT_CHECKBOUNDS {
            // Throw error when out of bounds
            #ifdef CHECK_BOUNDS
                if (ix >= this->length) { throw std::out_of_range("Coordinate out of image bounds"); }
            #endif
            return this->data[ix];
        }

        /**
        * @brief Mutable 3D coordinate access (x, y, channel).
        * @throws std::out_of_range If coordinates exceed dimensions.
        */
        inline T& operator[](const unsigned int x, const unsigned y, const unsigned c) NOEXCEPT_CHECKBOUNDS {
            // Throw error when out of bounds
            #ifdef CHECK_BOUNDS
                if ((x >= this->width) || (y >= this->height)) { throw std::out_of_range("Coordinate out of image bounds"); }
            #endif
            // Get double at (x, y) for channel c
            return this->data[((x + (y * this->width)) * this->channels) + c];
        }

        /**
        * @brief Read-only 3D coordinate access (x, y, channel).
        * @throws std::out_of_range If coordinates exceed dimensions.
        */
        inline const T& operator[](const unsigned int x, const unsigned int y, const unsigned int c) const NOEXCEPT_CHECKBOUNDS {
            // Throw error when out of bounds
            #ifdef CHECK_BOUNDS
                if ((x >= this->width) || (y >= this->height)) { throw std::out_of_range("Coordinate out of image bounds"); }
            #endif
            // Get char at (x, y) for channel c
            return this->data[((x + (y * this->width)) * this->channels) + c];
        }

        /**
        * @brief Output stream operator to serialize the tensor to text.
        * Format: Width Height Channels [Newline] Data...
        */
        friend std::ostream& operator<<(std::ostream& os, const Tensor<T>& tensor) NOEXCEPT {
            os << tensor.width << " " << tensor.height << " " << tensor.channels << "\n";
            for (unsigned int i = 0; i < tensor.length; ++i) {
                os << tensor.data[i] << " ";
                // Output aesthetic newline
                if (((i + 1) % tensor.width) == 0) { os << "\n"; }
            } return os;
        }

        /**
        * @brief Input stream operator to deserialize the tensor from text.
        * Reallocates memory based on the read dimensions.
        */
        friend std::istream& operator>>(std::istream& is, Tensor<T>& tensor) NOEXCEPT {
            unsigned int w, h, c;
            if (!(is >> w >> h >> c)) { return is; }
            // Store previous length
            unsigned int plen = tensor.length;
            // Load properties
            tensor.width = w; tensor.height = h; tensor.channels = c;
            tensor.length = tensor.width * tensor.height * tensor.channels;
            // Reallocate if size changed
            if (plen != tensor.length) {
                // Delete data if any, and reallocate
                if (tensor.data) { delete[] tensor.data; }
                tensor.data = new T[tensor.length]();
            }
            // Read data from input stream
            for (unsigned int i = 0; i < tensor.length; ++i) {
                is >> tensor.data[i];
            } return is;
        }

        inline unsigned int w() const noexcept { return this->width; }
        inline unsigned int h() const noexcept { return this->height; }
        inline unsigned int c() const noexcept { return this->channels; }
        inline unsigned int len() const noexcept { return this->length; }

    protected:

        unsigned int width, height, channels;
        unsigned int length;
        T* data = nullptr;

        /**
        * @brief Helper to allocate memory for the internal data buffer.
        * Initializes memory to zero.
        */
        inline void allocate(const unsigned int size) NOEXCEPT {
            #ifdef ALIGNED_ALLOCATION
                if (this->data) { std::free(this->data); }
                // Aligment to 64 bytes for improving speed
                const size_t alignment = 64;
                const size_t total_bytes = size * sizeof(T);
                // Padding to make size multiple of aligment
                const size_t padded_size = (total_bytes + alignment - 1) & ~(alignment - 1);
                // Aligment memory allocation
                this->data = static_cast<T*>(std::aligned_alloc(alignment, padded_size));
                // Initialize to zero
                if (this->data) {
                    std::fill_n(this->data, size, static_cast<T>(0));
                }
            #else
                if (this->data) { delete[] this->data; }
                // Allocate memory for the data initialized to zero
                this->data = new T[size]();
            #endif
        }
};

/**
* @brief Represents an image container. Inherits from Tensor<unsigned char>.
* * Handles loading/saving images via stb_image and specific out-of-bounds
* access logic (returning padding instead of throwing).
*/
template <typename I>
class Image : public Tensor<I> {
    public:

        /**
        * @brief Constructor for creating an empty blank image.
        */
        Image(const unsigned int width, const unsigned int height, const unsigned int channels) noexcept : Tensor<I>(width, height, channels), is_stbi(false) {
            // Allocate memory for the data initialized to zero
            Tensor<I>::allocate(this->length);
        }

        /**
        * @brief Constructor that loads an image from a file path.
        * @param path The filesystem path to the image.
        * @throws std::runtime_error If loading fails.
        */
        Image(const std::string& path) NOEXCEPT_THROWS requires std::same_as<I, float> || std::same_as<I, unsigned char> : Tensor<I>(0, 0, 0), is_stbi(true) {
            int w, h, c;
            // Two ways of doing things depending on decimals or not
            if constexpr (std::same_as<I, float>) {
                // Load image using external library
                this->data = stbi_loadf(path.c_str(), &w, &h, &c, MIN_CHANNELS);
            } else {
                // Load image using external library
                this->data = stbi_load(path.c_str(), &w, &h, &c, MIN_CHANNELS);
            }
            // Throw error if failed to load image
            #ifndef NO_THROWS
                if (this->data == NULL) { throw std::runtime_error("Failed to load image: " + path); }
            #endif
            // Store properties
            this->width = static_cast<unsigned int>(w); this->height = static_cast<unsigned int>(h); this->channels = static_cast<unsigned int>(c);
            // Compute length
            this->length = (this->width * this->height * this->channels);
        }

        /**
        * @brief Destructor.
        * Distinguishes between memory allocated by 'new' (Base class)
        * and memory allocated by 'stbi_load' (malloc), freeing appropriately.
        */
        ~Image() noexcept {
            if (this->is_stbi) {
                stbi_image_free(this->data);
                this->data = nullptr;
            }
        }

        /**
        * @brief Move constructor specific to Image.
        * Copies the 'is_stbi' flag in addition to moving the Tensor data.
        */
        Image(Image<I>&& o) noexcept : Tensor<I>(std::move(o)), is_stbi(o.is_stbi) {}

        // Inherit operator
        using Tensor<I>::operator[];

        /**
        * @brief Safe read-only access for convolution.
        * * Unlike the base Tensor class, this returns a static 'padding' value (0)
        * if coordinates are out of bounds, allowing the convolution kernel to
        * slide over the edges of the image without crashing.
        */
        inline const I& operator[](const unsigned int x, const unsigned int y, const unsigned int c) const noexcept {
            // Return 0 Padding when out of bounds
            #ifdef PADDED_ACCESS
                if ((x >= this->width) || (y >= this->height)) { return Image<I>::padding; }
            #endif
            // Get char at (x, y) for channel c
            return Tensor<I>::operator[](x, y, c);
        }

        /**
        * @brief Writes the current image data to a file (PNG format).
        */
        void write(const std::string& path) const NOEXCEPT requires std::same_as<I, float> || std::same_as<I, unsigned char> {
            if constexpr (std::same_as<I, float>) {
                stbi_write_hdr(path.c_str(), this->width, this->height, this->channels, this->data);
            } else {
                stbi_write_png(path.c_str(), this->width, this->height, this->channels, this->data, 0);
            }
        }

    private:

        static constexpr I padding = 0;
        const bool is_stbi; // Flag to track if memory is owned by stbi (malloc) or new
};

/**
* @brief Represents a convolution kernel (filter). Inherits from Tensor<double>.
*/
template <typename K>
class Kernel : public Tensor<K> {
    public:

        /**
        * @brief Constructor for creating an empty blank kernel.
        */
        Kernel(const unsigned int width, const unsigned int height, const unsigned int channels) noexcept : Tensor<K>(width, height, channels) {
            // Allocate memory for the data initialized to zero
            Tensor<K>::allocate(this->length);
        }

        /**
        * @brief Move constructor specific to Kernel.
        */
        Kernel(Kernel<K>&& o) noexcept : Tensor<K>(std::move(o)) {}

        // Inherit operators
        using Tensor<K>::operator[];

        /**
        * @brief Sets the kernel values using a 2D initializer list.
        * The 2D pattern is applied identically to all channels in the kernel.
        * * Usage: kernel.set({{0,-1,0}, {-1,4,-1}, {0,-1,0}});
        * @throws std::invalid_argument If matrix dimension doesn't matches kernel's.
        */
        void set(const std::initializer_list<std::initializer_list<K>>& matrix) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                // Check if dimensions match
                if (matrix.size() != this->height || matrix.begin()->size() != this->width) {
                    throw std::invalid_argument("Matrix size does not match kernel dimensions");
                }
            #endif
            K* ptr = this->data;
            const unsigned int c = this->channels;
            for (const auto& row : matrix) {
                for (const K v : row) {
                    std::fill_n(ptr, c, v);
                    ptr+=c;
                }
            }
        }

        inline unsigned int hw() const noexcept { return (this->width/2); }
        inline unsigned int hh() const noexcept { return (this->height/2); }

        /**
        * @brief Identity Kernel (1x1).
        * Does not modify the image.
        */
        [[nodiscard]] static Kernel<K> identity(const unsigned int channels = 3) NOEXCEPT_THROWS {
            Kernel<K> k(1, 1, channels);
            for (unsigned int s = 0; s < k.length; ++s) {
                k[s] = static_cast<K>(1);
            } return k;
        }

        /**
        * @brief Box Blur Kernel (NxN).
        * A simple average of the surrounding pixels.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> box_blur(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            // Fill with 1. The convolution will divide by the total sum.
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = static_cast<K>(1); }
            return k;
        }

        /**
        * @brief Gaussian Blur (NxN).
        * Generates a Gaussian kernel based on the desired kernel size.
        * Automatically calculates the appropriate Sigma to fit the bell curve in the box.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> gaussian(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            // Compute sigma assuming the kernel radius covers roughly 3 sigma.
            const K sigma = std::max(static_cast<K>(1), size / static_cast<K>(6));
            const K two_sigma_sq = static_cast<K>(1) / (static_cast<K>(2) * sigma * sigma);
            const unsigned int radius = size / 2;
            // Precompute Gaussian values
            std::vector<K> gaussian_vs(size);
            for (unsigned int i = 0; i < size; ++i) {
                const int x = (i - radius);
                gaussian_vs[i] = static_cast<K>(std::exp(-(x * x) * two_sigma_sq));
            }
            // Get raw pointer
            K* ptr = k.data;
            for (unsigned int y = 0; y < size; ++y) {
                // Cache the Y component of the gaussian
                const K v_y = gaussian_vs[y];
                for (unsigned int x = 0; x < size; ++x) {
                    // Combine X and Y components (exp(a+b) = exp(a)*exp(b))
                    const K v = (v_y * gaussian_vs[x]);
                    std::fill_n(ptr, channels, v);
                    ptr += channels;
                }
            } return k;
        }

        /**
        * @brief Sharpen (3x3).
        * Increases contrast between adjacent pixels.
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
        * @brief Sharpen (5x5).
        * A more aggressive sharpening filter that accounts for a wider area.
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
        * @brief Laplacian (3x3).
        * Standard omnidirectional edge detection.
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
        * @brief Laplacian of Gaussian (LoG) (5x5).
        * Combines Gaussian smoothing with Laplacian edge detection.
        * Less sensitive to noise than the standard 3x3 Laplacian.
        */
        [[nodiscard]] static Kernel<K> laplacian5(const unsigned int channels = 3) NOEXCEPT_THROWS requires std::floating_point<K> || std::signed_integral<K> {
            Kernel<K> k(5, 5, channels);
            k.set({
                { static_cast<K>(0),  static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0),  static_cast<K>(0)},
                { static_cast<K>(0), static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(-1),  static_cast<K>(0)},
                {static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(16), static_cast<K>(-2), static_cast<K>(-1)},
                { static_cast<K>(0), static_cast<K>(-1), static_cast<K>(-2), static_cast<K>(-1),  static_cast<K>(0)},
                { static_cast<K>(0),  static_cast<K>(0), static_cast<K>(-1),  static_cast<K>(0),  static_cast<K>(0)}
            });
            return k;
        }

        /**
        * @brief Sobel X (3x3).
        * Detects vertical lines, meaning changes in horizontal intensity.
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
        * @brief Sobel Y (3x3).
        * Detects horizontal lines, meaning changes in vertical intensity.
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
        * @brief Emboss (3x3).
        * Simulates a 3D relief effect by highlighting edges in a specific direction.
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
        * @brief Motion Blur X (Nx1).
        * Simulates motion blur due to the image moving horizontally.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> motion_blur_x(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, 1, channels);
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = static_cast<K>(1); }
            return k;
        }

        /**
        * @brief Motion Blur Y (1xN).
        * Simulates motion blur due to the image moving vertically.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> motion_blur_y(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(1, size, channels);
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = static_cast<K>(1); }
            return k;
        }

        /**
        * @brief Glitch X (Nx1).
        * Simulates a chromatic aberration horizontal effect (RGB). Ignores all channels after the third.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> glitch_x(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, 1, channels);
            k[0, 0, 0] = static_cast<K>(1); k[(k.width/2), 0, 1] = static_cast<K>(1); k[(k.width-1), 0, 2] = static_cast<K>(1);
            for (unsigned int c = 3; c < channels; ++c) {
                k[(k.width/2), 0, c] = static_cast<K>(1);
            } return k;
        }

        /**
        * @brief Glitch Y (1xN).
        * Simulates a chromatic aberration vertical effect (RGB). Ignores all channels after the third.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> glitch_y(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(1, size, channels);
            k[0, 0, 0] = static_cast<K>(1); k[0, (k.height/2), 1] = static_cast<K>(1); k[0, (k.height-1), 2] = static_cast<K>(1);
            for (unsigned int c = 3; c < channels; ++c) {
                k[0, (k.height/2), c] = static_cast<K>(1);
            } return k;
        }

        /**
        * @brief Ghost (NxN).
        * Simulates an unfocus effect, like if oneself was drunk.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> ghost(const unsigned int size, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            k[0] = static_cast<K>(1); k[k.length - 1] = static_cast<K>(1);
            return k;
        }

        /**
        * @brief Glow B (NxN).
        * Simulates a blur effect on all but one channel.
        * @param size Kernel size (must be odd).
        * @param channel Channel to not apply blur effect to.
        * @throws std::invalid_argument If size isn't odd.
        */
        [[nodiscard]] static Kernel<K> glow(const unsigned int size, const unsigned int channel, const unsigned int channels = 3) NOEXCEPT_THROWS {
            #ifndef NO_THROWS
                if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            #endif
            Kernel<K> k(size, size, channels);
            for (unsigned int c = 0; c < k.channels; ++c) {
                if (c == channel) { continue; }
                for (unsigned int s = 0; s < k.length; s+=k.channels) {
                    k[s + c] = static_cast<K>(1);
                }
            } return k;
        }

        /**
        * @brief Calculates a dynamic kernel size relative to an image dimensions.
        * Uses the smaller dimension to determine the base size based on a defined ratio constant.
        * This ensures consistent visual impact of filters across different resolutions.
        * @param image The reference image to calculate the scale against.
        * @param ratio The fraction of the image dimension to use.
        * @return unsigned int An odd integer representing the kernel size (min 3).
        */
        template<typename I>
        static unsigned int get_size_by_ratio(const Image<I>& image, const double ratio = RATIO) NOEXCEPT_THROWS {
            // Compute size with ratio
            unsigned int size = static_cast<unsigned int>(std::min(image.w(), image.h()) * ratio);
            // Size must always be odd
            if (size % 2 == 0) { ++size; }
            // Ensure a minimum value
            return std::max(3U, size);
        }
};

/**
* @brief Applies a convolution kernel to an image.
* @param image The source image.
* @param kernel The convolution kernel.
* @return Image A new image containing the convolution result.
*/
#define VECTORIZABLE_CONVO 2

template <typename I, typename K>
requires (std::integral<I> || std::floating_point<I>) && (std::integral<K> || std::floating_point<K>)
Image<I> convolute(const Image<I>& image, const Kernel<K>& kernel) NOEXCEPT {
    #ifdef VECTORIZED_CONVO
    // Need to implement this
    #elif defined(VECTORIZABLE_CONVO)
        // Initialize convolution image
        Image<I> conv(image.w(), image.h(), image.c());
        // Specialized algorithm for 3 channels or fallback to slower one for N channels
        switch (image.c()) {
            case 3: {
                // Scalar promotion for sums
                K sum0 = static_cast<K>(0), sum1 = static_cast<K>(0), sum2 = static_cast<K>(0);
                // Loop through kernel length
                #pragma GCC ivdep
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr < &kernel[kernel.len()-1]; k_ptr += 3) {
                    sum0 += k_ptr[0]; sum1 += k_ptr[1]; sum2 += k_ptr[2];
                }
                // Two ways of doing things depending on decimals or not
                if constexpr (std::floating_point<K>) {
                    // Check if zero with some leeway
                    sum0 = (std::abs(sum0) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum0);
                    sum1 = (std::abs(sum1) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum1);
                    sum2 = (std::abs(sum2) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum2);
                } else {
                    // Check if zero with some leeway
                    sum0 = (sum0 == static_cast<K>(0)) ? static_cast<K>(1) : sum0;
                    sum1 = (sum1 == static_cast<K>(0)) ? static_cast<K>(1) : sum1;
                    sum2 = (sum2 == static_cast<K>(0)) ? static_cast<K>(1) : sum2;
                }
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * 3;
                // Get image stride accounting for kernel
                const unsigned int stride = (image.w() - kernel.w()) * 3;
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Loop until we reach end of image
                for (; i_ptr < &image[kernel.hw(), image.h() - kernel.hh(), 0]; i_ptr += ((kernel.w() - 1) * 3), c_ptr += ((kernel.w() - 1) * 3)) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + 3;
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr += 3, c_ptr += 3) {
                        // Scalar promotion for buffers
                        K buf0 = static_cast<K>(0), buf1 = static_cast<K>(0), buf2 = static_cast<K>(0);
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
                                buf0 += si_ptr[0] * k_ptr[0];
                                buf1 += si_ptr[1] * k_ptr[1];
                                buf2 += si_ptr[2] * k_ptr[2];
                                k_ptr += 3; si_ptr += 3;
                            } si_ptr += stride;
                        }
                        // Get clamp limits depending on the type
                        K cmin, cmax;
                        if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = std::numeric_limits<I>::min(), cmax = std::numeric_limits<I>::max(); }
                        // Two ways of doing things depending on decimals or not
                        if constexpr (std::floating_point<K>) {
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
                K sum0 = static_cast<K>(0), sum1 = static_cast<K>(0), sum2 = static_cast<K>(0), sum3 = static_cast<K>(0);
                // Loop through kernel length
                #pragma GCC ivdep
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr < &kernel[kernel.len()-1]; k_ptr += 4) {
                    sum0 += k_ptr[0]; sum1 += k_ptr[1]; sum2 += k_ptr[2]; sum3 += k_ptr[3];
                }
                // Two ways of doing things depending on decimals or not
                if constexpr (std::floating_point<K>) {
                    // Check if zero with some leeway
                    sum0 = (std::abs(sum0) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum0);
                    sum1 = (std::abs(sum1) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum1);
                    sum2 = (std::abs(sum2) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum2);
                    sum3 = (std::abs(sum3) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sum3);
                } else {
                    // Check if zero with some leeway
                    sum0 = (sum0 == static_cast<K>(0)) ? static_cast<K>(1) : sum0;
                    sum1 = (sum1 == static_cast<K>(0)) ? static_cast<K>(1) : sum1;
                    sum2 = (sum2 == static_cast<K>(0)) ? static_cast<K>(1) : sum2;
                    sum3 = (sum3 == static_cast<K>(0)) ? static_cast<K>(1) : sum3;
                }
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * 4;
                // Get image stride accounting for kernel
                const unsigned int stride = (image.w() - kernel.w()) * 4;
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Loop until we reach end of image
                for (; i_ptr < &image[kernel.hw(), image.h() - kernel.hh(), 0]; i_ptr += ((kernel.w() - 1) * 4), c_ptr += ((kernel.w() - 1) * 4)) {
                    // Get row ending pointer
                    const I* r_ptr = i_ptr + stride + 4;
                    // Loop until we reach end of image row
                    for (; i_ptr < r_ptr; i_ptr += 4, c_ptr += 4) {
                        // Scalar promotion for buffers
                        K buf0 = static_cast<K>(0), buf1 = static_cast<K>(0), buf2 = static_cast<K>(0), buf3 = static_cast<K>(0);
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
                                buf0 += si_ptr[0] * k_ptr[0];
                                buf1 += si_ptr[1] * k_ptr[1];
                                buf2 += si_ptr[2] * k_ptr[2];
                                buf3 += si_ptr[3] * k_ptr[3];
                                k_ptr += 4; si_ptr += 4;
                            } si_ptr += stride;
                        }
                        // Get clamp limits depending on the type
                        K cmin, cmax;
                        if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = std::numeric_limits<I>::min(), cmax = std::numeric_limits<I>::max(); }
                        // Two ways of doing things depending on decimals or not
                        if constexpr (std::floating_point<K>) {
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
                std::vector<K> sums(kernel.c(), static_cast<K>(0));
                // Loop through kernel length
                for (const K* __restrict k_ptr = &kernel[0]; k_ptr < &kernel[kernel.len()-1]; k_ptr+=kernel.c()) {
                    // Precompute kernel sums
                    #pragma GCC ivdep
                    for (unsigned int c = 0; c < kernel.c(); ++c) {
                        sums[c] += k_ptr[c];
                    }
                } // Check if zero with some leeway
                #pragma GCC ivdep
                for (unsigned int c = 0; c < kernel.c(); ++c) {
                    // Two ways of doing things depending on decimals or not
                    if constexpr (std::floating_point<K>) {
                        // Check if zero with some leeway
                        sums[c] = (std::abs(sums[c]) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sums[c]);
                    } else {
                        // Check if zero with some leeway
                        sums[c] = (sums[c] == static_cast<K>(0)) ? static_cast<K>(1) : sums[c];
                    }
                }
                // Operation buffer
                std::vector<K> buffers(image.c(), static_cast<K>(0));
                // Get half kernel length delta in image
                const unsigned int padding = (kernel.hw() + (kernel.hh() * image.w())) * image.c();
                // Get image stride accounting for kernel
                const unsigned int stride = ((image.w() - kernel.w()) * image.c());
                // Get image and convo starting pointers
                I* __restrict c_ptr = &conv[kernel.hw(), kernel.hh(), 0];
                const I* __restrict i_ptr = &image[kernel.hw(), kernel.hh(), 0];
                // Loop until we reach end of image
                for (; i_ptr < &image[kernel.hw(), image.h() - kernel.hh(), 0]; i_ptr+=((kernel.w() - 1)*image.c()), c_ptr+=((kernel.w() - 1)*conv.c())) {
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
                                    buffers[c] += si_ptr[c] * k_ptr[c];
                                } k_ptr+=kernel.c(); si_ptr+=kernel.c();
                            } si_ptr += stride;
                        }
                        // Loop through channels
                        #pragma GCC ivdep
                        for (unsigned int c = 0; c < image.c(); ++c) {
                            // Two ways of doing things depending on decimals or not
                            if constexpr (std::floating_point<K>) {
                                // Normalize, sums[c] is never 0
                                buffers[c] *= sums[c];
                            } else {
                                // Normalize, sums[c] is never 0
                                buffers[c] /= sums[c];
                            }
                        }
                        // Get clamp limits depending on the type
                        K cmin, cmax;
                        if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = std::numeric_limits<I>::min(), cmax = std::numeric_limits<I>::max(); }
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
    #else
        // Initialize convolution image
        Image<I> conv(image.w(), image.h(), image.c());
        // Initialize sum vector
        std::vector<K> sums(kernel.c(), static_cast<K>(0));
        // Precompute kernel sums
        for (unsigned int c = 0; c < kernel.c(); ++c) {
            // Loop through kernel width
            for (unsigned int i = 0; i < kernel.w(); ++i) {
                // Loop through kernel height
                for (unsigned int j = 0; j < kernel.h(); ++j) {
                    sums[c] += kernel[i, j, c];
                }
            }
            // Two ways of doing things depending on decimals or not
            if constexpr (std::floating_point<K>) {
                // Check if zero with some leeway
                sums[c] = (std::abs(sums[c]) <= static_cast<K>(1e-5)) ? static_cast<K>(1) : (static_cast<K>(1) / sums[c]);
            } else {
                // Check if zero with some leeway
                sums[c] = (sums[c] == static_cast<K>(0)) ? static_cast<K>(1) : sums[c];
            }
        }
        // Loop through image height
        for (unsigned int y = kernel.hh(); y < (image.h() - kernel.hh()); ++y) {
            // Loop through image width
            for (unsigned int x = kernel.hw(); x < (image.w() - kernel.hw()); ++x) {
                // Loop through channels
                for (unsigned int c = 0; c < image.c(); ++c) {
                    // Operation buffer
                    K buffer = 0;
                    // Loop through kernel height
                    for (unsigned int j = 0; j < kernel.h(); ++j) {
                        // Loop through kernel width
                        for (unsigned int i = 0; i < kernel.w(); ++i) {
                            // Accumulate operations
                            buffer += image[x + (i - kernel.hw()), y + (j - kernel.hh()), c] * kernel[i, j, c];
                        }
                        // Get clamp limits depending on the type
                        K cmin, cmax;
                        if constexpr (std::floating_point<I>) { cmin = 0.0, cmax = 1.0; } else { cmin = std::numeric_limits<I>::min(), cmax = std::numeric_limits<I>::max(); }
                        // Two ways of doing things depending on decimals or not
                        if constexpr (std::floating_point<K>) {
                            // Fused Normalization and Clamping per channel
                            conv[x, y, c] = static_cast<I>(std::clamp(buffer * sums[c], cmin, cmax));
                        } else {
                            // Fused Normalization and Clamping per channel
                            conv[x, y, c] = static_cast<I>(std::clamp(buffer / sums[c], cmin, cmax));
                        }
                    }
                }
            }
        } return conv;
    #endif
}
/**
* @brief Scans a directory for supported image files.
* * @param folder The directory path to scan.
* @return std::vector<std::string> List of full paths to found images.
*/
std::vector<std::string> get_image_paths(const std::string& folder) noexcept {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        // Skip non-file entries
        if (!entry.is_regular_file()) { continue; };
        // Get extension
        const std::string ext = entry.path().extension().string();
        // Check if image extension
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
            files.push_back(entry.path().string());
        }
    } return files;
}

/**
* @brief Main entry point.
* 1. Defines a list of available kernels.
* 2. Iterates through each kernel type.
* 3. Creates a specific subdirectory for that kernel.
* 4. Applies the kernel to all images found in input.
*/
int main() noexcept {
    // Delete directory if exists
    std::filesystem::remove_all(OUTPUT_DIRECTORY);
    // Ensure base output directory exists
    std::filesystem::create_directory(OUTPUT_DIRECTORY);
    // Get all image paths
    std::vector<std::string> files = get_image_paths(INPUT_DIRECTORY);
    // Check if we found any image
    if (files.empty()) {
        std::cerr << "No images found in " << INPUT_DIRECTORY << "\n";
        return 1;
    }
    // Define list of kernels to process.
    // We use a lambda to generate the kernel object on demand.
    const std::vector<std::pair<std::string, std::function<Kernel<CNV_DTYPE>(unsigned int, unsigned int)>>> kernel_list = {
        {"identity",       [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::identity(c); }},
        {"box_blur",       [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::box_blur(s, c); }},
        {"gaussian",       [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::gaussian(s, c); }},
        {"sharpen_3",      [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::sharpen(c); }},
        {"sharpen_5",      [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::sharpen5(c); }},
        {"laplacian_3",    [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::laplacian(c); }},
        {"laplacian_5",    [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::laplacian5(c); }},
        {"sobel_x",        [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::sobel_x(c); }},
        {"sobel_y",        [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::sobel_y(c); }},
        {"emboss",         [](unsigned int, unsigned int c){ return Kernel<CNV_DTYPE>::emboss(c); }},
        {"motion_blur_x",  [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::motion_blur_x(s, c); }},
        {"motion_blur_y",  [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::motion_blur_y(s, c); }},
        {"glitch_x",       [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::glitch_x(s, c); }},
        {"glitch_y",       [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::glitch_y(s, c); }},
        {"ghost",          [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::ghost(s, c); }},
        {"glow_r",         [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::glow(s, 0, c); }},
        {"glow_g",         [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::glow(s, 1, c); }},
        {"glow_b",         [](unsigned int s, unsigned int c){ return Kernel<CNV_DTYPE>::glow(s, 2, c); }}
    };

    #ifndef SILENT
        // Output start message
        std::cout << "Starting batch processing...\n";
    #endif

    // Loop through each kernel definition
    for (const auto& kv : kernel_list) {
        // Create all kernel subfolders
        std::filesystem::create_directory(OUTPUT_DIRECTORY + kv.first + "/");
    }

    // Process all images with kernel
    for (const std::string& file : files) {
        #ifndef NO_THROWS
            try {
        #endif
            // Load Image
            const Image<IMG_DTYPE> image(file);

            // Loop through each kernel definition
            for (const auto& kv : kernel_list) {
                // Extract pair
                std::string k_name = kv.first;
                const auto& k_factory = kv.second;

                // Get output folder path
                std::string out_dir = OUTPUT_DIRECTORY + k_name + "/";

                // Create the kernel object
                const Kernel<CNV_DTYPE> kernel = k_factory(Kernel<CNV_DTYPE>::get_size_by_ratio(image), image.c());

                // Apply Convolution
                Image<IMG_DTYPE> conv = convolute(image, kernel);

                // Generate Output Path
                const std::string filename = std::filesystem::path(file).filename().replace_extension(".png").string();
                const std::string out_path = out_dir + filename;

                // Write to disk
                //conv.write(out_path);

                #ifndef SILENT
                    // Print processing info
                    printf(" [%s](%dx%dx%d) -> Processed '%s' (%dx%dx%d)\n", k_name.c_str(), kernel.w(), kernel.h(), kernel.c(), filename.c_str(), image.w(), image.h(), image.c());
                #endif
            }
        #ifndef NO_THROWS
            } catch (const std::exception& e) {
                std::cerr << " [ERROR] Failed to process '" << file << "': " << e.what() << "\n";
            }
        #endif
    }
    #ifndef SILENT
        std::cout << " Done.\n";
    #endif

    #ifndef SILENT
        std::cout << "All operations completed successfully.\n";
    #endif
    return 0;
}