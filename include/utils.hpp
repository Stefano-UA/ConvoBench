#pragma once
#include "config.hpp"
#include <vector>
#include <string>

/**
 * @brief Scans a directory for supported image file paths.
 * @param folder The directory path to scan for images.
 * @return A vector of strings, where each string is the full path to a
 *         found image file (e.g., .png, .jpg, .bmp, .hdr).
 * @note The implementation of this function is in `src/utils.cpp`.
 */
std::vector<std::string> get_image_paths(const std::string& folder) noexcept;