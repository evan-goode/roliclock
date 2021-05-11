PLATFORM = Linux

# C++ compiler.
CXX := g++ -std=c++11

# Build configuration (Debug or Release).
ifndef CONFIG
    CONFIG := Debug
endif
ifeq ($(CONFIG),Debug)
    CXXFLAGS += -O0 -g -DDEBUG=1 -D_DEBUG=1
else
    CXXFLAGS += -O3 -DNDEBUG
endif

# The name of your application.
APP_NAME := roliclock

# The path to the modules directory in the BLOCKS SDK directory.
SDK_PATH := ./BLOCKS-SDK/SDK

# The path to temporary build files.
OBJECT_DIR := build/$(CONFIG)

# The path to the compiled BLOCKSSDK library.
BLOCKS_LIBRARY := $(SDK_PATH)/Build/$(PLATFORM)/$(CONFIG)/libBLOCKS-SDK.a

# The source code for this application.
SOURCE_FILES := $(wildcard ../*.cpp) $(foreach EXT,.cpp .mm,$(wildcard *$(EXT)))

# Make a list of object files from .cpp files.
SOURCE_OBJ := $(addprefix $(OBJECT_DIR)/,$(notdir $(addsuffix .o,$(basename $(SOURCE_FILES)))))

# Header include paths (prefix with -I).
INCLUDES := -I$(SDK_PATH)

# Frameworks and libraries.
LIBS := -L/usr/X11R6/lib/ $(shell pkg-config --libs alsa libcurl x11) -ldl -lpthread -lrt
CXXFLAGS += -DLINUX=1

##############################################################################
# Build rules                                                                #
##############################################################################

.PHONEY: clean

$(CONFIG)/$(APP_NAME): $(SOURCE_OBJ) $(BLOCKS_LIBRARY)
	@mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(LIBS)

$(OBJECT_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -rf $(CONFIG) $(OBJECT_DIR)
