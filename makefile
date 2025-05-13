# torontocast-player/makefile

ifneq ($(shell uname), Linux)
	$(error only linux is supported)
endif


MAKEFLAGS        += --no-print-directory

BUILD_DIR        := bin
SOURCE_DIR       := src
MAIN             := $(SOURCE_DIR)/main.c
PROGRAM          := torontocast-player
DEBUG_PROGRAM    := $(PROGRAM)_debug
EXECUTABLE       := $(BUILD_DIR)/$(PROGRAM)
DEBUG_EXECUTABLE := $(EXECUTABLE)_debug

PREPARE          := mkdir -p $(BUILD_DIR)

COMPILER         := gcc
FLAGS            := -std=c89 -O3 -DNDEBUG -march=native -fwhole-program -flto
DEBUG_FLAGS      := -std=c89 -pedantic -g3 -Wall -Wextra -Wpedantic -Wconversion -Wunused-result -Wno-overlength-strings -Werror

CPPCHECK_FLAGS   := --cppcheck-build-dir=$(BUILD_DIR) --check-level=exhaustive --enable=all --inconclusive --fsigned-char -j 1 --language=c --max-ctu-depth=0 --platform=unix64 --std=c89 --verbose --suppress=unmatchedSuppression --suppress=missingIncludeSystem --suppress=checkersReport --suppress=variableScope --error-exitcode=1
VALGRIND_FLAGS   := --tool=memcheck --track-fds=all --error-limit=no --show-error-list=yes --keep-debuginfo=yes --show-below-main=yes --default-suppressions=no --smc-check=all --read-inline-info=yes --read-var-info=yes --show-emwarns=yes --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --track-origins=yes --expensive-definedness-checks=yes


TARGET:
	@$(PREPARE)
	@$(COMPILER) $(FLAGS) $(MAIN) -o $(EXECUTABLE) \
	&& echo -e "\x1b[32mok:\x1b[0m compiled successfully (run with \`make run\`, or install with \`make install\`)" ||:

debug:
	@$(PREPARE)
	@cppcheck $(CPPCHECK_FLAGS) $(MAIN) > /dev/null \
	&& $(COMPILER) $(DEBUG_FLAGS) $(MAIN) -o $(DEBUG_EXECUTABLE) \
	&& echo -e "\x1b[32mok:\x1b[0m compiled successfully (run with \`make run_debug\`)" ||:

run:
	@if [ -s "$(EXECUTABLE)" ]; then \
		$(EXECUTABLE) ||: ; \
	else \
		echo -e "\x1b[31merror:\x1b[0m "$(PROGRAM)" is clean (build with \`make\`)"; \
	fi

run_debug:
	@if [ -s "$(DEBUG_EXECUTABLE)" ]; then \
		valgrind $(VALGRIND_FLAGS) $(DEBUG_EXECUTABLE) ||: ; \
	else \
		echo -e "\x1b[31merror:\x1b[0m "$(DEBUG_PROGRAM)" is clean (build with \`make debug\`)"; \
	fi

install:
	@if grep -m 1 "^NAME" /etc/os-release | awk -F'=' '{printf tolower($$2)}' | tr -d '"' | grep -q "nixos"; then \
		echo -e "\x1b[31merror:\x1b[0m on NixOS, making an alias in your shell config instead is recommended (since there is no \"/usr/bin/\")"; \
	else \
		if [ -s "$(EXECUTABLE)" ]; then \
			if [ -s "/usr/bin/$(PROGRAM)" ]; then \
				echo -e "\x1b[33mwarning:\x1b[0m already installed, overwriting"; \
				sudo cp $(EXECUTABLE) /usr/bin ||: ; \
			else \
				sudo cp $(EXECUTABLE) /usr/bin && echo -e "\x1b[32mok:\x1b[0m installed (you can now run this from any path with \`"$(PROGRAM)"\`)" ||: ; \
			fi; \
		else \
			echo -e "\x1b[31merror:\x1b[0m "$(PROGRAM)" is clean (build with \`make\`)"; \
		fi; \
	fi

uninstall:
	@if [ -s "/usr/bin/$(PROGRAM)" ]; then \
		sudo rm "/usr/bin/"$(PROGRAM) && echo -e "\x1b[32mok:\x1b[0m uninstalled" ||: ; \
	else \
		echo -e "\x1b[33mwarning:\x1b[0m nothing to uninstall"; \
	fi

clean:
	@rm -r $(BUILD_DIR) 2> /dev/null && echo -e "\x1b[32mok:\x1b[0m cleaned" || echo -e "\x1b[33mwarning:\x1b[0m nothing to clean"

