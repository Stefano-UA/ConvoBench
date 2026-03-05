#include "config.hpp"
#include "utils.hpp"
#include "image.hpp"
#include "kernel.hpp"
#include "convolution.hpp"
#include <functional>
#include <filesystem>
#include <iostream>
#include <clocale>
#include <vector>

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

    #ifdef DO_BENCHMARK
        double read_t = 0, write_t = 0, conv_t = 0, ker_t = 0;
        uint64_t read_i = 0, write_i = 0, conv_i = 0, ker_i = 0;
    #endif

    // Process all images with kernel
    for (const std::string& file : files) {
        #ifndef NO_THROWS
            try {
        #endif

            BENCHMARK_CHRONO(read_t,
                BENCHMARK_CYCLES(read_i,
                    // Load Image
                    const Image<IMG_DTYPE> image(file);
                )
            )

            // Loop through each kernel definition
            for (const auto& kv : kernel_list) {
                // Extract pair
                std::string k_name = kv.first;
                const auto& k_factory = kv.second;

                BENCHMARK_CHRONO(ker_t,
                    BENCHMARK_CYCLES(ker_i,
                        // Create the kernel object
                        const Kernel<CNV_DTYPE> kernel = k_factory(Kernel<CNV_DTYPE>::get_size_by_ratio(image), image.c());
                    )
                )

                BENCHMARK_CHRONO(conv_t,
                    BENCHMARK_CYCLES(conv_i,
                        // Apply Convolution
                        Image<IMG_DTYPE> conv = convolute(image, kernel);
                    )
                )

                BENCHMARK_CHRONO(write_t,
                    BENCHMARK_CYCLES(write_i,
                        // Get output folder path
                        std::string out_dir = OUTPUT_DIRECTORY + k_name + "/";

                        // Generate Output Path
                        const char* ext;
                        if constexpr (std::floating_point<IMG_DTYPE>) { ext = ".hdr"; } else { ext = ".png"; }
                        const std::string filename = std::filesystem::path(file).filename().replace_extension(ext).string();
                        const std::string out_path = out_dir + filename;

                        // Write to disk
                        conv.write(out_path);
                    )
                )

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

    #ifdef DO_BENCHMARK
        setlocale(LC_NUMERIC, "");
        printf(" %-7s | %'14.4f s | %'15lu cyc\n", "[READ]", read_t / 1e6, read_i);
        printf(" %-7s | %'14.4f s | %'15lu cyc\n", "[WRITE]", write_t / 1e6, write_i);
        printf(" %-7s | %'14.4f s | %'15lu cyc\n", "[CONV]", conv_t / 1e6, conv_i);
        printf(" %-7s | %'14.4f s | %'15lu cyc\n", "[KER]", ker_t / 1e6, ker_i);
    #endif

    #ifndef SILENT
        std::cout << "All operations completed successfully.\n";
    #endif
    return 0;
}