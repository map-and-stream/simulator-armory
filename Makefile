BUILD_DIR = build
OUTPUT_DIR = $(BUILD_DIR)/output
INSTALL_DIR = $(BUILD_DIR)/install

CLANG_BUILD_DIR = build_clang
CLANG_OUTPUT_DIR = $(CLANG_BUILD_DIR)/output


.PHONY: clean build build-clang rebuild install test

build:
	@echo "Starting build process... $(shell nproc) cores"
	cmake -B $(BUILD_DIR) -S . -DSIMULATOR_ARMORY_BUILD_TESTS=ON -DSIMULATOR_ARMORY_BUILD_EXAMPLE=ON
	cmake --build $(BUILD_DIR) -j$(shell nproc)
	cp $(BUILD_DIR)/example/radar $(OUTPUT_DIR)
	cp $(BUILD_DIR)/example/lrad $(OUTPUT_DIR)


build-clang:
	@echo "Starting Clang build process... $(shell nproc) cores"
	cmake -B $(CLANG_BUILD_DIR) -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DSIMULATOR_ARMORY_BUILD_TESTS=ON -DSIMULATOR_ARMORY_BUILD_EXAMPLE=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(CLANG_BUILD_DIR) -j$(shell nproc)
	mkdir -p $(CLANG_OUTPUT_DIR)
	cp $(CLANG_BUILD_DIR)/example/camera_example $(CLANG_OUTPUT_DIR)


clean:
	rm -rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)
	rm -rf $(OUTPUT_DIR)
	mkdir -p $(OUTPUT_DIR)
	rm -rf $(CLANG_BUILD_DIR)
	mkdir -p $(CLANG_BUILD_DIR)
	rm -rf $(CLANG_OUTPUT_DIR)
	mkdir -p $(CLANG_OUTPUT_DIR)

rebuild: clean build

install:
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR)
	make build
	cmake --install build

test:
	make build
	cd ${BUILD_DIR}/test; ctest