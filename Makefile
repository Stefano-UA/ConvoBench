# Compiler to use
CXX      := g++
# Lib directory
LIB_DIR  := lib
# Flags used:
# -std=c++23 : Use standard C++23 (minimum for this project)
# -Wall -Wextra : Show all warnings
# -MMD -MP : Generate .d files to track header dependencies automatically
# -I$(LIB_DIR) : Look for header files in the $(LIB_DIR) directory
# -fopt-info-vec-missed -fopt-info-vec-optimized : Print information about automatic vectorization
CXXFLAGS := -std=c++23 -MMD -MP -march=native -mtune=native -I$(LIB_DIR) -fopt-info-vec-missed -fopt-info-vec-optimized

# Uncomment to include expensive bound checks
# CXXFLAGS := $(CXXFLAGS) -DCHECK_BOUNDS -DPADDED_ACCESS

# Uncomment to disable all prints
CXXFLAGS := $(CXXFLAGS) -DSILENT

# Uncomment to disable all safety error throwing checks
CXXFLAGS := $(CXXFLAGS) -DNO_THROWS

# Uncomment to disable all error handling
CXXFLAGS := $(CXXFLAGS) -DHARD_ERRORS

# Uncomment to enable more specific optimization flags at the cost error reporting, math precision, and compile time
CXXFLAGS := $(CXXFLAGS) -fno-math-errno -ffast-math -funroll-loops -flto -finline-functions -fno-exceptions -fno-rtti

BASENAME    := main
BUILD_DIR   := build
SRCS        := main.cpp
RESOURCES   := input output

# Default rule
all: O0 O1 O2 O3

# OG Profile: -OG, adds .oG suffix
OG: CXXFLAGS := $(CXXFLAGS) -O0 -g
OG: PROFILE  := OG
OG: $(BUILD_DIR)/$(BASENAME).oG

# O0 Profile: -O0, adds .o0 suffix
O0: CXXFLAGS := $(CXXFLAGS) -O0
O0: PROFILE  := O0
O0: $(BUILD_DIR)/$(BASENAME).o0

# O1 Profile: -O1, adds .o1 suffix
O1: CXXFLAGS := $(CXXFLAGS) -O1
O1: PROFILE  := O1
O1: $(BUILD_DIR)/$(BASENAME).o1

# O2 Profile: -O2, adds .o2 suffix
O2: CXXFLAGS := $(CXXFLAGS) -O2
O2: PROFILE  := O2
O2: $(BUILD_DIR)/$(BASENAME).o2

# O3 Profile: -O3, adds .o3 suffix
O3: CXXFLAGS := $(CXXFLAGS) -O3
O3: PROFILE  := O3
O3: $(BUILD_DIR)/$(BASENAME).o3

# Compilation: Matches .o0, .o1, etc.
# $< is the source file, $@ is the target file
$(BUILD_DIR)/$(PROFILE)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(foreach dir, $(RESOURCES), \
		if [ ! -L $(BUILD_DIR)/$(dir) ]; then \
			ln -sfr $(dir) $(BUILD_DIR)/$(dir); \
			echo "Linked $(BUILD_DIR)/$(dir) -> $(dir)"; \
		fi; \
	)
	@echo "Compiling $< [$@]"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linking: Matches .o0, .o1, etc.
$(BUILD_DIR)/$(BASENAME).o%: FORCE
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) $(BUILD_DIR)/$(PROFILE)/$(SRCS:.cpp=.o) PROFILE=$(PROFILE) CXXFLAGS="$(CXXFLAGS)"
	@echo "Linking $@..."
	$(CXX) $(BUILD_DIR)/$(PROFILE)/$(SRCS:.cpp=.o) $(CXXFLAGS) -o $@

# Include dependencies
-include $(BUILD_DIR)/*/*.d

# Clean all output
clean:
	@echo "Cleaning $(BUILD_DIR)..."
	rm -fr $(BUILD_DIR)

# Helper to force rules to run (needed for recursive make check)
FORCE:

# Specify that they are not real files
.PHONY: all clean O0 O1 O2 O3 FORCE