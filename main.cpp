#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define RATIO 0.01
#define MIN_CHANNELS 3
#define INPUT_DIRECTORY "./input/"
#define OUTPUT_DIRECTORY "./output/"

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
        Tensor(const unsigned int width, const unsigned int height, const unsigned int channels)
            : width(width), height(height), channels(channels), length(width * height * channels), data(nullptr) {}

        /**
        * @brief Destructor. Frees allocated memory if data is not nullptr.
        */
        virtual ~Tensor() {
            if (this->data != nullptr) {
                delete[] this->data;
                this->data = nullptr;
            }
        }

        // Disable copying.
        // Deep copying tensors is expensive and usually unintended in this context.
        Tensor(const Tensor&) = delete;
        Tensor& operator=(const Tensor&) = delete;

        /**
        * @brief Move constructor.
        * Transfers ownership of the memory pointer from 'o' to this object.
        * The source object 'o' is left in a valid but empty state (data = nullptr).
        */
        Tensor(Tensor&& o) noexcept
            : width(o.width), height(o.height), channels(o.channels), length(o.length), data(o.data) {
            // Steal the pointer
            o.data = nullptr;
        }

        /**
        * @brief Mutable linear index access.
        * @throws std::out_of_range If ix is outside the buffer length.
        */
        inline T& operator[](const unsigned int ix) {
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
        inline const T& operator[](const unsigned int ix) const {
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
        inline T& operator[](const unsigned int x, const unsigned y, const unsigned c) {
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
        inline const T& operator[](const unsigned int x, const unsigned int y, const unsigned int c) const {
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
        friend std::ostream& operator<<(std::ostream& os, const Tensor<T>& tensor) {
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
        friend std::istream& operator>>(std::istream& is, Tensor<T>& tensor) {
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

        inline const unsigned int w() const noexcept { return this->width; }
        inline const unsigned int h() const noexcept { return this->height; }
        inline const unsigned int c() const noexcept { return this->channels; }
        inline const unsigned int len() const noexcept { return this->length; }

    protected:

        unsigned int width, height, channels;
        unsigned int length;
        T* data = nullptr;

        /**
        * @brief Helper to allocate memory for the internal data buffer.
        * Initializes memory to zero.
        */
        inline void allocate(unsigned int size) {
            if (this->data) { delete[] this->data; }
            // Allocate memory for the data initialized to zero
            this->data = new T[size]();
        }
};

/**
* @brief Represents an image container. Inherits from Tensor<unsigned char>.
* * Handles loading/saving images via stb_image and specific out-of-bounds
* access logic (returning padding instead of throwing).
*/
class Image : public Tensor<unsigned char> {
    public:

        /**
        * @brief Constructor for creating an empty blank image.
        */
        Image(const unsigned int width, const unsigned int height, const unsigned int channels) : Tensor(width, height, channels), is_stbi(false) {
            // Allocate memory for the data initialized to zero
            Tensor::allocate(this->length);
        }

        /**
        * @brief Constructor that loads an image from a file path.
        * @param path The filesystem path to the image.
        * @throws std::runtime_error If loading fails.
        */
        Image(const std::string& path) : Tensor(0, 0, 0), is_stbi(true) {
            int w, h, c;
            // Load image using external library
            unsigned char* tdata = stbi_load(path.c_str(), &w, &h, &c, MIN_CHANNELS);
            // Throw error if failed to load image
            if (tdata == NULL) { throw std::runtime_error("Failed to load image: " + path); }
            // Set properties
            this->data = tdata;
            this->width = w; this->height = h; this->channels = c;
            // Compute length
            this->length = (this->width * this->height * this->channels);
        }

        /**
        * @brief Destructor.
        * Distinguishes between memory allocated by 'new' (Base class)
        * and memory allocated by 'stbi_load' (malloc), freeing appropriately.
        */
        ~Image() {
            if (this->is_stbi) {
                stbi_image_free(this->data);
                this->data = nullptr;
            }
        }

        /**
        * @brief Move constructor specific to Image.
        * Copies the 'is_stbi' flag in addition to moving the Tensor data.
        */
        Image(Image&& o) noexcept : Tensor(std::move(o)), is_stbi(o.is_stbi) {}

        // Inherit operator
        using Tensor<unsigned char>::operator[];

        /**
        * @brief Safe read-only access for convolution.
        * * Unlike the base Tensor class, this returns a static 'padding' value (0)
        * if coordinates are out of bounds, allowing the convolution kernel to
        * slide over the edges of the image without crashing.
        */
        inline const unsigned char& operator[](const unsigned int x, const unsigned int y, const unsigned int c) const noexcept {
            // Return 0 Padding when out of bounds
            #ifdef PADDED_ACCESS
                if ((x >= this->width) || (y >= this->height)) { return Image::padding; }
            #endif
            // Get char at (x, y) for channel c
            return Tensor::operator[](x, y, c);
        }

        /**
        * @brief Writes the current image data to a file (PNG format).
        */
        void write(const std::string& path) const {
            stbi_write_png(path.c_str(), this->width, this->height, this->channels, this->data, 0);
        }

    private:

        static constexpr unsigned char padding = 0;
        const bool is_stbi; // Flag to track if memory is owned by stbi (malloc) or new
};

/**
* @brief Represents a convolution kernel (filter). Inherits from Tensor<double>.
*/
class Kernel : public Tensor<double> {
    public:

        /**
        * @brief Constructor for creating an empty blank kernel.
        */
        Kernel(const unsigned int width, const unsigned int height, const unsigned int channels) : Tensor(width, height, channels) {
            // Allocate memory for the data initialized to zero
            Tensor::allocate(this->length);
        }

        /**
        * @brief Move constructor specific to Kernel.
        */
        Kernel(Kernel&& o) noexcept : Tensor(std::move(o)) {}

        // Inherit operators
        using Tensor<double>::operator[];

        /**
        * @brief Sets the kernel values using a 2D initializer list.
        * The 2D pattern is applied identically to all channels in the kernel.
        * * Usage: kernel.set({{0,-1,0}, {-1,4,-1}, {0,-1,0}});
        * @throws std::invalid_argument If matrix dimension doesn't matches kernel's.
        */
        void set(const std::initializer_list<std::initializer_list<double>>& matrix) {
            // Check if dimensions match
            if (matrix.size() != this->height || matrix.begin()->size() != this->width) {
                throw std::invalid_argument("Matrix size does not match kernel dimensions");
            }
            unsigned int y = 0;
            for (const auto& row : matrix) {
                unsigned int x = 0;
                for (const double val : row) {
                    for (unsigned int c = 0; c < this->channels; ++c) {
                        (*this)[x, y, c] = val;
                    } x++;
                } y++;
            }
        }

        inline const unsigned int hw() const noexcept { return (this->width/2); }
        inline const unsigned int hh() const noexcept { return (this->height/2); }

        /**
        * @brief Identity Kernel (3x3).
        * Does not modify the image.
        */
        static Kernel identity(unsigned int channels = 3) {
            Kernel k(3, 3, channels);
            k.set({
                {0, 0, 0},
                {0, 1, 0},
                {0, 0, 0}
            }); return k;
        }

        /**
        * @brief Box Blur Kernel (NxN).
        * A simple average of the surrounding pixels.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel box_blur(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(size, size, channels);
            // Fill with 1.0. The convolution will divide by the total sum.
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = 1.0; }
            return k;
        }

        /**
        * @brief Gaussian Blur (NxN).
        * Generates a Gaussian kernel based on the desired kernel size.
        * Automatically calculates the appropriate Sigma to fit the bell curve in the box.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel gaussian(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(size, size, channels);
            // Compute sigma assuming the kernel radius covers roughly 3 sigma.
            double sigma = std::max(1.0, size / 6.0);
            double two_sigma_sq = 2.0 * sigma * sigma;
            // Compute center offset (radius)
            int radius = size / 2;
            // Loop through values to write
            for (int x = (-radius); x <= radius; ++x) {
                for (int y = (-radius); y <= radius; ++y) {
                    // Gaussian formula without normalization
                    double val = std::exp(-(((x * x) + (y * y)) / two_sigma_sq));
                    // Apply to all channels
                    for (unsigned int c = 0; c < channels; ++c) {
                        k[x + radius, y + radius, c] = val;
                    }
                }
            } return k;
        }

        /**
        * @brief Sharpen (3x3).
        * Increases contrast between adjacent pixels.
        */
        static Kernel sharpen(unsigned int channels = 3) {
            Kernel k(3, 3, channels);
            k.set({
                { 0, -1,  0},
                {-1,  5, -1},
                { 0, -1,  0}
            }); return k;
        }

        /**
        * @brief Sharpen (5x5).
        * A more aggressive sharpening filter that accounts for a wider area.
        */
        static Kernel sharpen5(unsigned int channels = 3) {
            Kernel k(5, 5, channels);
            k.set({
                {-1, -1, -1, -1, -1},
                {-1,  2,  2,  2, -1},
                {-1,  2,  8,  2, -1},
                {-1,  2,  2,  2, -1},
                {-1, -1, -1, -1, -1}
            }); return k;
        }

        /**
        * @brief Laplacian (3x3).
        * Standard omnidirectional edge detection.
        */
        static Kernel laplacian(unsigned int channels = 3) {
            Kernel k(3, 3, channels);
            k.set({
                { 0, -1,  0},
                {-1,  4, -1},
                { 0, -1,  0}
            }); return k;
        }

        /**
        * @brief Laplacian of Gaussian (LoG) (5x5).
        * Combines Gaussian smoothing with Laplacian edge detection.
        * Less sensitive to noise than the standard 3x3 Laplacian.
        */
        static Kernel laplacian5(unsigned int channels = 3) {
            Kernel k(5, 5, channels);
            k.set({
                {0,  0, -1,  0, 0},
                {0, -1, -2, -1, 0},
                {-1,-2, 16, -2,-1},
                {0, -1, -2, -1, 0},
                {0,  0, -1,  0, 0}
            });
            return k;
        }

        /**
        * @brief Sobel X (3x3).
        * Detects vertical lines, meaning changes in horizontal intensity.
        */
        static Kernel sobel_x(unsigned int channels = 3) {
            Kernel k(3, 3, channels);
            k.set({
                {-1, 0, 1},
                {-2, 0, 2},
                {-1, 0, 1}
            }); return k;
        }

        /**
        * @brief Sobel Y (3x3).
        * Detects horizontal lines, meaning changes in vertical intensity.
        */
        static Kernel sobel_y(unsigned int channels = 3) {
            Kernel k(3, 3, channels);
            k.set({
                {-1, -2, -1},
                { 0,  0,  0},
                { 1,  2,  1}
            }); return k;
        }

        /**
        * @brief Emboss (3x3).
        * Simulates a 3D relief effect by highlighting edges in a specific direction.
        */
        static Kernel emboss(unsigned int channels = 3) {
            Kernel k(3, 3, channels);
            k.set({
                {-2, -1,  0},
                {-1,  1,  1},
                { 0,  1,  2}
            }); return k;
        }

        /**
        * @brief Motion Blur X (Nx1).
        * Simulates motion blur due to the image moving horizontally.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel motion_blur_x(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(size, 1, channels);
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = 1.0; }
            return k;
        }

        /**
        * @brief Motion Blur Y (1xN).
        * Simulates motion blur due to the image moving vertically.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel motion_blur_y(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(1, size, channels);
            for(unsigned int i=0; i < k.len(); ++i) { k.data[i] = 1.0; }
            return k;
        }

        /**
        * @brief Glitch X (Nx1).
        * Simulates a chromatic aberration horizontal effect (RGB). Ignores all channels after the third.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel glitch_x(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(size, 1, channels);
            k[0, 0, 0] = 1.0; k[(k.width/2), 0, 1] = 1.0; k[(k.width-1), 0, 2] = 1.0;
            for (unsigned int c = 3; c < channels; ++c) {
                k[(k.width/2), 0, c] = 1.0;
            } return k;
        }

        /**
        * @brief Glitch Y (1xN).
        * Simulates a chromatic aberration vertical effect (RGB). Ignores all channels after the third.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel glitch_y(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(1, size, channels);
            k[0, 0, 0] = 1.0; k[0, (k.height/2), 1] = 1.0; k[0, (k.height-1), 2] = 1.0;
            for (unsigned int c = 3; c < channels; ++c) {
                k[0, (k.height/2), c] = 1.0;
            } return k;
        }

        /**
        * @brief Ghost (NxN).
        * Simulates an unfocus effect, like if oneself was drunk.
        * @param size Kernel size (must be odd).
        * @throws std::invalid_argument If size isn't odd.
        */
        static Kernel ghost(unsigned int size, unsigned int channels = 3) {
            if (size % 2 == 0) { throw std::invalid_argument("Kernel size must be odd"); }
            Kernel k(size, size, channels);
            k[0] = 1.0; k[k.length - 1] = 1.0;
            return k;
        }

        /**
        * @brief Calculates a dynamic kernel size relative to an image dimensions.
        * Uses the smaller dimension to determine the base size based on a defined ratio constant.
        * This ensures consistent visual impact of filters across different resolutions.
        * @param image The reference image to calculate the scale against.
        * @param ratio The fraction of the image dimension to use.
        * @return unsigned int An odd integer representing the kernel size (min 3).
        */
        static unsigned int get_size_by_ratio(const Image& image, double ratio = RATIO) {
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
Image convolute(const Image& image, const Kernel& kernel) {
    // Initialize convolution image
    Image conv(image.w(), image.h(), image.c());
    // Initialize sum vector
    std::vector<double> sums(kernel.c(), 0.0);
    // Precompute kernel sums
    for (unsigned int c = 0; c < kernel.c(); ++c) {
        // Loop through kernel width
        for (unsigned int i = 0; i < kernel.w(); ++i) {
            // Loop through kernel height
            for (unsigned int j = 0; j < kernel.h(); ++j) {
                sums[c] += kernel[i, j, c];
            }
        } // Check if zero with some leeway
        if (std::abs(sums[c]) <= 1e-5) { sums[c] = 1.0; } else { sums[c] = 1.0 / sums[c]; }
    }
    // Loop through image height
    for (unsigned int y = kernel.hh(); y < (image.h() - kernel.hh()); ++y) {
        // Loop through image width
        for (unsigned int x = kernel.hw(); x < (image.w() - kernel.hw()); ++x) {
            // Loop through channels
            for (unsigned int c = 0; c < image.c(); ++c) {
                // Operation buffer
                double buffer = 0;
                // Loop through kernel height
                for (unsigned int j = 0; j < kernel.h(); ++j) {
                    // Loop through kernel width
                    for (unsigned int i = 0; i < kernel.w(); ++i) {
                        // Accumulate operations
                        buffer += image[x + (i - kernel.hw()), y + (j - kernel.hh()), c] * kernel[i, j, c];
                    }
                }
                // Normalize, sums[c] is never 0
                buffer = (buffer * sums[c]);
                // Clamp buffer to ensure its in range and cast to unsigned char
                conv[x, y, c] = static_cast<unsigned char>(std::max(0.0, std::min(buffer, 255.0)));
            }
        }
    } return conv;
}

/**
* @brief Scans a directory for supported image files.
* * @param folder The directory path to scan.
* @return std::vector<std::string> List of full paths to found images.
*/
std::vector<std::string> get_image_paths(const std::string& folder) {
    std::vector<std::string> files;

    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        // Skip non-file entries
        if (!entry.is_regular_file()) { continue; };
        // Get extension
        const std::string ext = entry.path().extension().string();
        // Check if image extension
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" ||
            ext == ".tga" || ext == ".hdr" || ext == ".psd" || ext == ".pic" || ext == ".pgm" || ext == ".ppm") {
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
int main() {

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
    std::vector<std::pair<std::string, std::function<Kernel(unsigned int, unsigned int)>>> kernel_list = {
        {"identity",       [](unsigned int s, unsigned int c){ return Kernel::identity(c); }},
        {"box_blur",     [](unsigned int s, unsigned int c){ return Kernel::box_blur(s, c); }},
        {"gaussian",     [](unsigned int s, unsigned int c){ return Kernel::gaussian(s, c); }},
        {"sharpen_3",      [](unsigned int s, unsigned int c){ return Kernel::sharpen(c); }},
        {"sharpen_5",     [](unsigned int s, unsigned int c){ return Kernel::sharpen5(c); }},
        {"laplacian_3",   [](unsigned int s, unsigned int c){ return Kernel::laplacian(c); }},
        {"laplacian_5",   [](unsigned int s, unsigned int c){ return Kernel::laplacian5(c); }},
        {"sobel_x",       [](unsigned int s, unsigned int c){ return Kernel::sobel_x(c); }},
        {"sobel_y",       [](unsigned int s, unsigned int c){ return Kernel::sobel_y(c); }},
        {"emboss",        [](unsigned int s, unsigned int c){ return Kernel::emboss(c); }},
        {"motion_blur_x", [](unsigned int s, unsigned int c){ return Kernel::motion_blur_x(s, c); }},
        {"motion_blur_y", [](unsigned int s, unsigned int c){ return Kernel::motion_blur_y(s, c); }},
        {"glitch_x",      [](unsigned int s, unsigned int c){ return Kernel::glitch_x(s, c); }},
        {"glitch_y",      [](unsigned int s, unsigned int c){ return Kernel::glitch_y(s, c); }},
        {"ghost",         [](unsigned int s, unsigned int c){ return Kernel::ghost(s, c); }}
    };

    // Output start message
    std::cout << "Starting batch processing...\n";

    // Loop through each kernel definition
    for (const auto& kv : kernel_list) {
        // Extract pair
        std::string k_name = kv.first;
        const auto& k_factory = kv.second;

        std::cout << "[Kernel] " << k_name << "...\n";

        // Create subdirectory for this kernel
        std::string out_dir = OUTPUT_DIRECTORY + k_name + "/";
        std::filesystem::create_directory(out_dir);

        // Process all images with kernel
        for (const std::string& file : files) {
            try {
                // Load Image
                const Image image(file);

                // Create the kernel object
                const Kernel kernel = k_factory(Kernel::get_size_by_ratio(image), image.c());

                // Apply Convolution
                Image conv = convolute(image, kernel);

                // Generate Output Path
                const std::string filename = std::filesystem::path(file).filename().replace_extension(".png").string();
                const std::string out_path = out_dir + filename;

                // Write to disk
                conv.write(out_path);

                // Print processing info
                printf(" -> Processed '%s' (%dx%dx%d)\n", filename.c_str(), image.w(), image.h(), image.c());

            } catch (const std::exception& e) {
                std::cerr << " [ERROR] Failed to process '" << file << "': " << e.what() << "\n";
            }
        }
        std::cout << " Done.\n";
    }

    std::cout << "All operations completed successfully.\n";
    return 0;
}