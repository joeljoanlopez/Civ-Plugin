BUILD_DIR := build
UNITY_PLUGINS_DIR := ../Unity_Project/Assets/Plugins

OS := $(shell uname -s)
ifeq ($(OS), Darwin)
    LIB_FILE := libMapGenCore.dylib
else ifeq ($(OS), Linux)
    LIB_FILE := libMapGenCore.so
else
    LIB_FILE := MapGenCore.dll
endif

configure:
	cmake -S . -B $(BUILD_DIR)

compile:
	cmake --build $(BUILD_DIR) --parallel

test: compile
	$(BUILD_DIR)/bin/UnitTests

clean:
	cmake --build $(BUILD_DIR) --target clean

rebuild: clean compile

deploy: compile
	cp $(BUILD_DIR)/bin/$(LIB_FILE) $(UNITY_PLUGINS_DIR)/
