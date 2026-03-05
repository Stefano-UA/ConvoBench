#include "config.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// Disable certain implementations
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM

// Force stb lib to be memory aligned
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

// Disable asserts to optimize further
#ifdef NO_THROWS
    #define STBI_ASSERT(x)
#endif

#include "stb_image.h"
#include "stb_image_write.h"