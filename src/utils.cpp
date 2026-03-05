#include "utils.hpp"
#include <filesystem>
#include <vector>

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