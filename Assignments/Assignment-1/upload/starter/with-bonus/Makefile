BIN_DIR := bin
LOADER_TARGET := loader
LAUNCHER_TARGET := launcher
TEST_TARGET := test

all: $(BIN_DIR)/$(LOADER_TARGET) $(BIN_DIR)/$(LAUNCHER_TARGET) $(BIN_DIR)/$(TEST_TARGET)
	@echo "Build done."

$(BIN_DIR)/$(LOADER_TARGET):
	@mkdir -p $(BIN_DIR)
	@$(MAKE) -C loader
	@mv $(LOADER_TARGET)/lib_simpleloader.so $(BIN_DIR)

$(BIN_DIR)/$(LAUNCHER_TARGET):
	@$(MAKE) -C launcher
	@mv launcher/launch $(BIN_DIR)

$(BIN_DIR)/$(TEST_TARGET):
	@$(MAKE) -C test

bonus:
	cd bin && ./launch ../test/fib

clean:
	@$(MAKE) -C test clean
	@$(MAKE) -C loader clean
	@$(MAKE) -C launcher clean
	@rmdir $(BIN_DIR)