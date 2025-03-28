.PHONY: apack
.PHONY: build
.PHONY: jwlink
.PHONY: nasm
.PHONY: nasmfmt
.PHONY: pre-commit

define copy-executables
    @echo "Copying executable files from $(1) to $(2)..."
    @find $(1) -type f -iname "*.exe" -exec cp {} $(2) \;
endef

TOOLS_DIR = tools

APACK_DIR = apack
APACK_ZIP = $(APACK_DIR).zip
APACK_URL = https://ibsensoftware.com/files/apack-1.00.zip

JWLINK_DIR = jwlink
JWLINK_ZIP = $(JWLINK_DIR).zip
JWLINK_URL = https://www.japheth.de/Download/JWlink/JWlink_v19b13_dos.zip

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
	mkdir -p $(TOOLS_DIR)
	make apack
	make jwlink
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
	@echo "Downloading aPACK"
	@wget -q $(APACK_URL) -O $(APACK_ZIP)
	@echo "Unpacking aPACK..."
	@unzip -q $(APACK_ZIP) -d $(APACK_DIR)
	@echo "aPACK has been unpacked."
	$(call copy-executables,$(APACK_DIR),$(TOOLS_DIR))
	@rm -r $(APACK_DIR)
	@rm -f $(APACK_ZIP)
	@echo "aPACK is installed."

jwlink:
	@echo "Downloading JWlink..."
	@wget -q $(JWLINK_URL) -O $(JWLINK_ZIP)
	@echo "Unpacking JWlink..."
	@unzip -q $(JWLINK_ZIP) -d $(JWLINK_DIR)
	@echo "JWlink has been unpacked."
	$(call copy-executables,$(JWLINK_DIR),$(TOOLS_DIR))
	@rm -r $(JWLINK_DIR)
	@rm -f $(JWLINK_ZIP)
	@echo "JWlink is installed."

nasm:
	@echo "Downloading NASM..."
	@wget -q $(NASM_URL) -O $(NASM_ZIP)
	@echo "Unpacking NASM..."
	@unzip -q $(NASM_ZIP) -d .
	@echo "NASM has been unpacked."
	@rm -f $(NASM_ZIP)
	$(call copy-executables,$(NASM),$(TOOLS_DIR))
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
