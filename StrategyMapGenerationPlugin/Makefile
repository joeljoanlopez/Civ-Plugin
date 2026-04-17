BUILD_DIR := build

configure:
	cmake -S . -B $(BUILD_DIR)

compile:
	cmake --build $(BUILD_DIR) --parallel

test: compile
	$(BUILD_DIR)/bin/UnitTests

clean:
	cmake --build $(BUILD_DIR) --target clean

rebuild: clean compile
