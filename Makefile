CFLAGS = -g0
CFLAGS_dbg = -g3

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

$(BUILD_DIR)/nesem_dbg : $(BUILD_DIR)/Bus_dbg.o $(BUILD_DIR)/NES6502_dbg.o $(BUILD_DIR)/main_dbg.o
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS_dbg) -o $(BUILD_DIR)/x86-64_linux-nesem-dbg $(BUILD_DIR)/Bus.o $(BUILD_DIR)/NES6502.o $(BUILD_DIR)/main.o

$(BUILD_DIR)/nesem : $(BUILD_DIR)/Bus.o $(BUILD_DIR)/NES6502.o $(BUILD_DIR)/main.o
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS) -o $(BUILD_DIR)/x86-64_linux-nesem $(BUILD_DIR)/Bus.o $(BUILD_DIR)/NES6502.o $(BUILD_DIR)/main.o

$(BUILD_DIR)/Bus_dbg.o: $(SRC_DIR)/Bus.cpp
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS_dbg) -o $(BUILD_DIR)/Bus.o -c $(SRC_DIR)/Bus.cpp

$(BUILD_DIR)/NES6502_dbg.o : $(SRC_DIR)/NES6502.cpp
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS_dbg) -o $(BUILD_DIR)/NES6502.o -c $(SRC_DIR)/NES6502.cpp

$(BUILD_DIR)/main_dbg.o : $(SRC_DIR)/main.cpp
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS_dbg) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.cpp

$(BUILD_DIR)/Bus.o: $(SRC_DIR)/Bus.cpp
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS) -o $(BUILD_DIR)/Bus.o -c $(SRC_DIR)/Bus.cpp

$(BUILD_DIR)/NES6502.o : $(SRC_DIR)/NES6502.cpp
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS) -o $(BUILD_DIR)/NES6502.o -c $(SRC_DIR)/NES6502.cpp

$(BUILD_DIR)/main.o : $(SRC_DIR)/main.cpp
	mkdir $(BUILD_DIR) -p
	clang++ $(CFLAGS) -o $(BUILD_DIR)/main.o -c $(SRC_DIR)/main.cpp

clean:
	rm -r $(BUILD_DIR)
