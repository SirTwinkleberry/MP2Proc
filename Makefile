CXX      := -c++ -std=c++17
# CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -Wconversion -Wno-sign-conversion -Wdouble-promotion
CXXFLAGS := -pedantic-errors -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wconversion -Wno-sign-conversion -Wdouble-promotion
LDFLAGS  := -L"./externals/lib" -lRNifti -lboost_timer
BUILD    := ./build
BIN      := ./bin
OBJ_DIR  := $(BUILD)/objects
TARGET   := main
INCLUDE  := -I. -I"./externals/include/" -I"/usr/local/include/eigen3/"
SRC      :=                      \
   $(wildcard ./src/*.cpp) \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(BIN)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(BIN)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(BIN)/$(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(BIN)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g3
debug: all

release: CXXFLAGS += -O3
release: all

clean:
	-@rm -rvf $(BUILD)
	-@rm -rvf $(BIN)

info:
	@echo "[*] Application dir: ${BIN}         "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"