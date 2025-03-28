.PHONY: apack
.PHONY: build
.PHONY: nasm
.PHONY: nasmfmt
.PHONY: pre-commit

APACK_DIR = apack
APACK_ZIP = $(APACK_DIR).zip
APACK_URL = https://ibsensoftware.com/files/apack-1.00.zip

NASM := nasm-2.16.03
NASM_DIR := $(NASM)-dos
NASM_ZIP := $(NASM_DIR).zip
NASM_URL := https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/dos/$(NASM_ZIP)

GO_CHECK := $(shell command -v go 2> /dev/null)
INSTALL_NASM_FMT := GO111MODULE=on go install github.com/yamnikov-oleg/nasmfmt@latest
NASMFMT_INSTALLED := $(shell command -v nasmfmt 2> /dev/null)

build:
	@echo "Building the project..."
	@mkdir -p build
	@cd build && cmake .. && make --no-print-directory

install:
	make apack
	make nasm
	make nasmfmt
	make pre-commit

config:
	python scripts/config.py

song:
	python scripts/song.py

consts:
	python scripts/constants.py

apack:
	@echo "Downloading APACK"
	@wget -q $(APACK_URL) -O $(APACK_ZIP)
	@echo "Unpacking APACK..."
	@unzip -q $(APACK_ZIP) -d $(APACK_DIR)
	@echo "APACK has been unpacked."
	@rm -f $(APACK_ZIP)
	@echo "APACK is installed."

nasm:
	@echo "Downloading NASM..."
	@wget -q $(NASM_URL) -O $(NASM_ZIP)
	@echo "Unpacking NASM..."
	@unzip -q $(NASM_ZIP) -d .
	@echo "NASM has been unpacked."
	@rm -f $(NASM_ZIP)
	@cp $(NASM)/*.exe .
	@rm -r $(NASM)
	@echo "NASM is installed."

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

pre-commit:
	make config
	pip install pre-commit
	pre-commit autoupdate
