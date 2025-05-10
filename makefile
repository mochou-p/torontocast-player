# torontocast-player/makefile

ifneq ($(shell uname), Linux)
	$(error only linux is supported)
endif

MAKEFLAGS        += --no-print-directory

BUILD_DIR        := bin
SOURCE_DIR       := src
PROGRAM          := torontocast-player
DEBUG_PROGRAM    := $(PROGRAM)_debug
EXECUTABLE       := $(BUILD_DIR)/$(PROGRAM)
DEBUG_EXECUTABLE := $(EXECUTABLE)_debug

PREPARE          := mkdir $(BUILD_DIR)

COMPILER         := gcc
FLAGS            := -std=c89 -O3 -DNDEBUG -march=native -fwhole-program -flto
DEBUG_FLAGS      := -std=c89 -pedantic -g3 -Wall -Wextra -Wpedantic -Wconversion -Wno-overlength-strings -Werror

TARGET:
	@$(PREPARE) 2> /dev/null ||:
	@$(COMPILER) $(FLAGS) $(SOURCE_DIR)/main.c -o $(EXECUTABLE)
	@echo -e "\x1b[32mok:\x1b[0m compiled successfully (run with \`make run\`)"

debug:
	@$(PREPARE) 2> /dev/null ||:
	@$(COMPILER) $(DEBUG_FLAGS) $(SOURCE_DIR)/main.c -o $(DEBUG_EXECUTABLE)
	@echo -e "\x1b[32mok:\x1b[0m compiled successfully (run with \`make run_debug\`)"

run:
	@if [ -s "$(EXECUTABLE)" ]; then \
		$(EXECUTABLE) ||: ; \
	else \
		echo -e "\x1b[31merror:\x1b[0m "$(PROGRAM)" is clean (build with \`make\`)"; \
	fi

run_debug:
	@if [ -s "$(DEBUG_EXECUTABLE)" ]; then \
		$(DEBUG_EXECUTABLE) ||: ; \
	else \
		echo -e "\x1b[31merror:\x1b[0m "$(DEBUG_PROGRAM)" is clean (build with \`make debug\`)"; \
	fi

clean:
	@rm -r $(BUILD_DIR) 2> /dev/null && echo -e "\x1b[32mok:\x1b[0m cleaned" || echo -e "\x1b[33mwarning:\x1b[0m nothing to clean"

all:
	@$(MAKE) && $(MAKE) run ; $(MAKE) clean

all_debug:
	@$(MAKE) debug && $(MAKE) run_debug ; $(MAKE) clean

