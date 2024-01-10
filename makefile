ifeq ($(OS),Windows_NT)
    TARGET=brainfuck.exe
else
    TARGET=brainfuck
    INSTALL_DIR=/usr/local/bin
endif

all: $(TARGET)

$(TARGET): brainfuck.c
	@gcc -o $(TARGET) brainfuck.c

install:
	@UNAME_S=$$(uname -s); \
	if [ "$$UNAME_S" != "Linux" ]; then \
		echo "install is currently only supported on Linux systems"; \
		exit 1; \
	fi; \
	if [ ! -w $(INSTALL_DIR) ]; then \
		echo "Error: No write permission to install directory $(INSTALL_DIR)"; \
		echo "Try running 'sudo make install'"; \
		exit 1; \
	fi; \
	gcc -o $(INSTALL_DIR)/$(TARGET) brainfuck.c; \
	echo "brainfuck compiled and installed to $(INSTALL_DIR)"

uninstall:
	@UNAME_S=$$(uname -s); \
	if [ "$$UNAME_S" != "Linux" ]; then \
		echo "Error: uninstall is only supported on Linux systems"; \
		exit 1; \
	fi; \
	if [ ! -w $(INSTALL_DIR) ]; then \
		echo "Error: No write permission in $(INSTALL_DIR)"; \
		exit 1; \
	fi; \
	rm -f $(INSTALL_DIR)/$(TARGET); \
	echo "brainfuck uninstalled from $(INSTALL_DIR)"

clean:
	@rm -f $(TARGET)
