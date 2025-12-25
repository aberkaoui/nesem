# Directories
SRC_DIR = ./src
INCLUDE_DIR = ./include
BIN_DIR = ./bin
OBJ_DIR_rel = $(BIN_DIR)/obj/release
OBJ_DIR_dbg = $(BIN_DIR)/obj/debug

# Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
# (Release mode)
OBJ_FILES_rel = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR_rel)/%.o)
DEP_FILES_rel = $(OBJ_FILES_rel:$(OBJ_DIR_rel)/%.o=$(OBJ_DIR_rel)/%.d)
BINARY_rel = $(BIN_DIR)/release/x86-64_linux-nesem
# (Debug mode)
OBJ_FILES_dbg = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR_dbg)/%.o)
DEP_FILES_dbg = $(OBJ_FILES_dbg:$(OBJ_DIR_dbg)/%.o=$(OBJ_DIR_dbg)/%.d)
BINARY_dbg = $(BIN_DIR)/debug/x86-64_linux-nesem

# Compilation
CXX = clang++
CXXFLAGS = -g0 -I$(INCLUDE_DIR)/lol -MMD -MP -MF $(OBJ_DIR_rel)/$*.d
CXXFLAGS_dbg = -g3 -I$(INCLUDE_DIR)/lol -MMD -MP -MF $(OBJ_DIR_dbg)/$*.d


.PHONY: release debug all clean format

release: $(BINARY_rel)
debug: $(BINARY_dbg)
all: $(BINARY_rel) $(BINARY_dbg)

# Release mode build rule
$(BINARY_rel): $(OBJ_FILES_rel)
	@mkdir -p $(BIN_DIR)/release
	$(CXX) $(OBJ_FILES_rel) -o $(BINARY_rel)
$(OBJ_DIR_rel)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR_rel)
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Debug mode build rule
$(BINARY_dbg): $(OBJ_FILES_dbg)
	@mkdir -p $(BIN_DIR)/debug
	$(CXX) $(OBJ_FILES_dbg) -o $(BINARY_dbg)
$(OBJ_DIR_dbg)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR_dbg)
	$(CXX) -c $< -o $@ $(CXXFLAGS_dbg)

-include $(DEP_FILES_rel)
-include $(DEP_FILES_dbg)
# Interesting note: generated .d dependency files do indeed consider the circular dependency
# between Bus.h/cpp and NES6502.h/cpp.
# So the include directive is "twice" better than just manually adding .h dependencies!

clean:
	rm -rf $(BIN_DIR)

format:
	clang-format -i $(SRC_DIR)/*.cpp $(INCLUDE_DIR)/*.h
