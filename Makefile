# Compiler to use
CXX      := g++
# Lib directory
LIB_DIR  := lib

# Flags used:
# -std=c++23 : Use standard C++23 (minimum for this project)
# -Wall -Wextra : Show all warnings
# -MMD -MP : Generate .d files to track header dependencies automatically
# -I$(LIB_DIR) : Look for header files in the $(LIB_DIR) directory
CXXFLAGS := -std=c++23 -Wall -Wextra -MMD -MP -I$(LIB_DIR)

# Uncomment to print vectorization information
# CXXFLAGS := $(CXXFLAGS) -fopt-info-vec-missed -fopt-info-vec-optimized

# Uncomment to hide usual warnings
CXXFLAGS := $(CXXFLAGS) -Wno-missing-field-initializers

# Uncomment to compile with benchmarking code
CXXFLAGS := $(CXXFLAGS) -DDO_BENCHMARK

# Combinatorial axes definition
O_AXIS     := O0 O1 O2 O3		  # Optimization
ARCH_AXIS  := ARCH+ #ARCH-	  # Architecture-Oriented Code
VEC_AXIS   := VEC~ VEC- #VEC+	# Vectorized Code
ALIG_AXIS  := ALIG+ ALIG-	    # Aligned Allocation
IMGDT_AXIS := IMGF IMGC	      # Image Datatype
KERDT_AXIS := KERF KERD KERI  # Kernel Datatype
ERR_AXIS   := E+ E-		        # C++ Errors
FM_AXIS    := FM+ FM-			    # Fast Math
SIL_AXIS   := S+ #S-	        # Silent Code

# Optimization flags
O0_FLAGS := -O0
O1_FLAGS := -O1
O2_FLAGS := -O2
O3_FLAGS := -O3

# Native architecture flags
ARCH+_FLAGS := -march=native -mtune=native
ARCH-_FLAGS :=

# Vectorization flags
VEC+_FLAGS := -DVECTORIZED_CONVO -ftree-vectorize
VEC~_FLAGS := -DVECTORIZABLE_CONVO -ftree-vectorize -fopt-info-vec-missed -fopt-info-vec-optimized
VEC-_FLAGS := -DUNVECTORIZED_CONVO -fno-tree-vectorize

# Aligned allocation flags
ALIG+_FLAGS := -DALIGNED_ALLOCATION
ALIG-_FLAGS := -DNORMAL_ALLOCATION

# Image datatype flags
IMGF_FLAGS := -DIMG_DTYPE=float
IMGC_FLAGS := -DIMG_DTYPE='unsigned char'

# Kernel datatype flags
KERF_FLAGS := -DCNV_DTYPE=float
KERD_FLAGS := -DCNV_DTYPE=double
KERD_FLAGS := -DCNV_DTYPE=int

# Error flags
E+_FLAGS := -DCHECK_BOUNDS -DPADDED_ACCESS
E-_FLAGS := -DHARD_ERRORS -DNO_THROWS -fno-exceptions -fno-rtti -fno-math-errno

# Fast math flags
FM+_FLAGS := -ffast-math
FM-_FLAGS :=

# Silent flags
S+_FLAGS := -DSILENT
S-_FLAGS :=

# Targets
BASENAME    := main
BUILD_DIR   := build
SRCS        := main.cpp
RESOURCES   := input output

.DEFAULT_GOAL := all

# Template to generate rules dynamically
# $(1) is the profile name, $(2)-$(10) are the axes values
define GENERATE_PROFILE_RULES

# Flags for this specific combination
$(1)_CXXFLAGS := $$(CXXFLAGS) $$($(2)_FLAGS) $$($(3)_FLAGS) $$($(4)_FLAGS) $$($(5)_FLAGS) $$($(6)_FLAGS) $$($(7)_FLAGS) $$($(8)_FLAGS) $$($(9)_FLAGS) $$($(10)_FLAGS)

# Compilation: Matches specific profile object files
# $< is the source file, $@ is the target file
$(BUILD_DIR)/v/$(1)/%.o: %.cpp
	@mkdir -p $$(dir $$@)
	@$$(foreach dir, $(RESOURCES), \
	    if [ ! -L $(BUILD_DIR)/$$(dir) ]; then \
	        ln -sfr $$(dir) $(BUILD_DIR)/$$(dir); \
	    fi; \
	)
	@echo "Compiling $$< [$(1)]"
	$$(CXX) $$($(1)_CXXFLAGS) -c $$< -o $$@

# Linking: Matches specific profile executable
$(BUILD_DIR)/$(BASENAME).$(1): $$(addprefix $(BUILD_DIR)/v/$(1)/, $$(SRCS:.cpp=.o))
	@echo "Linking $$@..."
	$$(CXX) $$^ $$($(1)_CXXFLAGS) -o $$@

# Include dependencies
-include $(BUILD_DIR)/v/$(1)/*.d

endef

# Generate the Cartesian product targets
TARGETS :=
$(foreach o, $(O_AXIS), \
  $(foreach a, $(ARCH_AXIS), \
    $(foreach v, $(VEC_AXIS), \
      $(foreach l, $(ALIG_AXIS), \
        $(foreach i, $(IMGDT_AXIS), \
          $(foreach k, $(KERDT_AXIS), \
            $(foreach e, $(ERR_AXIS), \
              $(foreach f, $(FM_AXIS), \
                $(foreach s, $(SIL_AXIS), \
                  $(eval TARGETS += $(BUILD_DIR)/$(BASENAME).$(o).$(a).$(v).$(l).$(i).$(k).$(e).$(f).$(s)) \
                  $(eval $(call GENERATE_PROFILE_RULES,$(o).$(a).$(v).$(l).$(i).$(k).$(e).$(f).$(s),$(o),$(a),$(v),$(l),$(i),$(k),$(e),$(f),$(s),)) \
                ) \
              ) \
            ) \
          ) \
        ) \
      ) \
    ) \
  ) \
)

# Default rule
all: $(TARGETS)

# Clean all output
clean:
	@echo "Cleaning $(BUILD_DIR)..."
	rm -fr $(BUILD_DIR)

# Helper to force rules to run
FORCE:

# Specify that they are not real files
.PHONY: all clean FORCE