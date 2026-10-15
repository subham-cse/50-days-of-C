# ==============================================================================
# 50 Days of C - Systems Programming & Core Computer Science Mastery
# Root Build Automation Makefile
# ==============================================================================

CC      ?= gcc
CFLAGS  ?= -Wall -Wextra -std=c99 -pthread -O2
LDFLAGS ?= -pthread -lm

# Directories
HARNESS_DIR = harness
ENGINE_DIR  = engine

# Binary Targets
HARNESS_BIN = $(HARNESS_DIR)/verifier
ENGINE_BIN  = $(ENGINE_DIR)/parser_vm
DAY49_BIN   = Day_49/day49_vm
DAY50_BIN   = Day_50/day50_concurrency

.PHONY: all test test-engine test-harness day49 day50 clean

# Default target: compile both the Test Harness and the Domain Engine
all: $(HARNESS_BIN) $(ENGINE_BIN) $(DAY49_BIN) $(DAY50_BIN)

# Standalone Day 49 & Day 50 Builds
day49: $(DAY49_BIN)
$(DAY49_BIN): Day_49/49_Command_Line_Arguments_code.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

day50: $(DAY50_BIN)
$(DAY50_BIN): Day_50/50_Preprocessor_Directives_Const_Macros_code.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Build the Multi-Threaded Process-Sandboxed Verification Harness
$(HARNESS_BIN): $(HARNESS_DIR)/verifier.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Build the Recursive-Descent Parser & Bytecode Virtual Machine Engine
$(ENGINE_BIN): $(ENGINE_DIR)/vm.c $(ENGINE_DIR)/parser.c $(ENGINE_DIR)/vm.h
	$(CC) $(CFLAGS) $(ENGINE_DIR)/vm.c $(ENGINE_DIR)/parser.c -o $@ $(LDFLAGS)

# Run full test suite: Multi-threaded verification + Domain engine self-tests
test: all
	@echo "======================================================================"
	@echo ">>> Executing Domain Engine Validation..."
	@echo "======================================================================"
	./$(ENGINE_BIN)
	@echo ""
	@echo "======================================================================"
	@echo ">>> Executing Multi-Threaded Verification Harness across All 50 Days..."
	@echo "======================================================================"
	./$(HARNESS_BIN)

test-engine: $(ENGINE_BIN)
	./$(ENGINE_BIN)

test-harness: $(HARNESS_BIN)
	./$(HARNESS_BIN)

clean:
	rm -f $(HARNESS_BIN) $(HARNESS_BIN).exe
	rm -f $(ENGINE_BIN) $(ENGINE_BIN).exe
	rm -f $(DAY49_BIN) $(DAY49_BIN).exe
	rm -f $(DAY50_BIN) $(DAY50_BIN).exe
	rm -f .tmp_day_* .tmp_test_*
	@echo "Cleaned build artifacts."
