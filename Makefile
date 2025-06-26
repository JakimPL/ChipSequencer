.PHONY: apack
.PHONY: build
.PHONY: jwlink
.PHONY: nasm
.PHONY: nasmfmt
.PHONY: onekpaq
.PHONY: pre-commit

IS_WINDOWS := $(filter Windows_NT,$(OS))
GO_CHECK := $(shell which go 2> /dev/null)
INSTALL_NASM_FMT := GO111MODULE=on go install github.com/yamnikov-oleg/nasmfmt@latest
NASMFMT_INSTALLED := $(shell which nasmfmt 2> /dev/null)

TOOLS_DIR = tools

ifeq ($(IS_WINDOWS),)
define copy-executables
	@echo "Copying executable files from $(1) to $(2)..."
	@find $(1) -type f -iname "*.exe" -exec cp {} $(2) \;
endef
else
define copy-executables
	@echo Copying executable files from $(1) to $(2)...
	@for %%f in ($(1)\*.exe) do copy "%%f" "$(2)" > nul
endef
endif

build:
	@echo "Building the project..."
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	@cmake --build build --config Debug

clean:
	@echo "Cleaning the project..."
	@if exist build (cmake --build build --target clean) || (echo No build directory.)

install:
	@echo "Installing tools..."
	@$(MAKE) nasm
	@$(MAKE) nasmfmt
	@$(MAKE) onekpaq
	@$(MAKE) pre-commit

consts:
	@echo "Generating constants..."
	@python scripts/constants.py

nasmfmt:
ifeq ($(GO_CHECK),)
	@echo "Go is not installed. Please install Go to continue."
	@exit 1
endif

ifeq ($(NASMFMT_INSTALLED),)
	@echo "Installing nasmfmt..."
	@$(INSTALL_NASM_FMT)
	@which nasmfmt > /dev/null || echo "nasmfmt not found in PATH. Add \$GOPATH/bin to your PATH."
else
	@echo "nasmfmt is already installed."
endif

onekpaq:
	@echo "Installing oneKpaq..."
	@if [ ! -d $(TOOLS_DIR)/oneKpaq ]; then \
		mkdir -p $(TOOLS_DIR); \
		cd $(TOOLS_DIR) && git clone https://github.com/temisu/oneKpaq.git; \
	fi
	@cd $(TOOLS_DIR)/oneKpaq && $(MAKE)

pre-commit:
	@echo "Installing pre-commit..."
	@pip install pre-commit
	@python -m pre_commit autoupdate
