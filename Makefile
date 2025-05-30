.PHONY: apack
.PHONY: build
.PHONY: jwlink
.PHONY: nasm
.PHONY: nasmfmt
.PHONY: onekpaq
.PHONY: pre-commit

define copy-executables
    @echo "Copying executable files from $(1) to $(2)..."
    @find $(1) -type f -iname "*.exe" -exec cp {} $(2) \;
endef

TOOLS_DIR = tools

GO_CHECK := $(shell command -v go 2> /dev/null)
INSTALL_NASM_FMT := GO111MODULE=on go install github.com/yamnikov-oleg/nasmfmt@latest
NASMFMT_INSTALLED := $(shell command -v nasmfmt 2> /dev/null)

build:
	@echo "Building the project..."
	@mkdir -p build
	@cd build && cmake .. && make --no-print-directory

clean:
	@echo "Cleaning the project..."
	@mkdir -p build
	@cd build && cmake .. && make clean --no-print-directory

install:
	mkdir -p $(TOOLS_DIR)
	make nasm
	make nasmfmt
	make onekpaq
	make pre-commit

config:
	python scripts/config.py

song:
	python scripts/song.py

consts:
	python scripts/constants.py

nasmfmt:
	@if [ -z "$(GO_CHECK)" ]; then \
		echo "Go is not installed. Please install Go to continue."; \
		exit 1; \
	fi

	@if [ -z "$(NASMFMT_INSTALLED)" ]; then \
		echo "Installing nasmfmt..."; \
		$(INSTALL_NASM_FMT); \
		if [ -z "$$(command -v nasmfmt 2> /dev/null)" ]; then \
			echo "nasmfmt not found. Check your PATH variable."; \
		else \
			echo "nasmfmt installed successfully."; \
		fi \
	else \
		echo "nasmfmt is already installed."; \
	fi

onekpaq:
	@mkdir -p tools
	@echo "Downloading oneKpaq..."
	@cd tools && git clone git@github.com:temisu/oneKpaq.git
	@echo "Compiling from sources..."
	@cd tools/oneKpaq && make
	@echo "oneKpaq is installed."

pre-commit:
	make config
	pip install pre-commit
	pre-commit autoupdate
