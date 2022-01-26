TARGET_EXEC ?= cpplox #binary name


ifneq ($(wildcard ./src/*), ) #if src exists then use it else use ./
SRC_DIRS := ./src/
else
SRC_DIRS := ./
endif

ifneq ($(wildcard ./build/*), ) #if build doesnt exist create it
$(mkdir ./build)
endif

BUILD_DIR ?= ./build

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -or -name "*.c" -or -name "*.s")
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
#cpp flags
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
CXXFLAGS := -g3 -Wno-switch -Wall -pedantic -std=c++2a
#link flags
LDFLAGS = 

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
