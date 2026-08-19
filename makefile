CC = gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror -g \
          -fsanitize=address,undefined
INCLUDES := -Iinclude -I"$(HOME)/quictls-install/include"
LDFLAGS  := -L"$(HOME)/quictls-install/lib64" -Wl,-rpath,"$(HOME)/quictls-install/lib64"
LDLIBS   := -lssl -lcrypto

SRC := $(wildcard src/*.c)
TESTS := $(wildcard test/*.c)
BUILD_DIR := builds
TARGETS := $(patsubst test/%.c,$(BUILD_DIR)/%,$(TESTS))

.PHONY: all test clean
all: test
test: $(TARGETS)
	@for t in $(TARGETS); do \
		echo "Running $$t"; \
		./$$t; \
	done

$(BUILD_DIR)/%: test/%.c $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC) $< -o $@ $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf $(BUILD_DIR)