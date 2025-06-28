.PHONY: apack
.PHONY: build
.PHONY: jwlink
.PHONY: nasm
.PHONY: nasmfmt
.PHONY: onekpaq
.PHONY: pre-commit
.PHONY: venv

GO_CHECK := $(shell which go 2> /dev/null)
INSTALL_NASM_FMT := GO111MODULE=on go install github.com/yamnikov-oleg/nasmfmt@latest
NASMFMT_INSTALLED := $(shell which nasmfmt 2> /dev/null)

REQUIREMENTS_FILE = scripts/requirements.txt
TOOLS_DIR = tools
VENV_DIR = venv

ifeq ($(OS),Windows_NT)
	PYTHON = python
	VENV_BIN = $(VENV_DIR)/Scripts
	VENV_PYTHON = $(VENV_BIN)/python.exe
	VENV_PIP = $(VENV_BIN)/pip.exe
	ACTIVATE_SCRIPT = $(VENV_BIN)/activate.bat
else
	PYTHON = python3
	VENV_BIN = $(VENV_DIR)/bin
	VENV_PYTHON = $(VENV_BIN)/python
	VENV_PIP = $(VENV_BIN)/pip
	ACTIVATE_SCRIPT = $(VENV_BIN)/activate
endif


build:
	@echo "Building the project..."
ifeq ($(OS),Windows_NT)
	@cmake -S . -B build -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug
else
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
endif
	@cmake --build build --config Debug -- --no-print-directory

clean:
	@echo "Cleaning the project..."
	@if exist build (cmake --build build --target clean) || (echo No build directory.)

install:
	@echo "Downloading dependencies..."
	@git submodule update --init --recursive
	@echo "Installing tools..."
	@$(MAKE) nasm
	@$(MAKE) nasmfmt
	@$(MAKE) onekpaq
	@$(MAKE) venv
	@$(MAKE) activate

consts:
	@echo "Generating constants..."
	@$(VENV_PYTHON) scripts/constants.py

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
	@$(VENV_PIP) install pre-commit
	@$(VENV_PYTHON) -m pre_commit install
	@$(VENV_PYTHON) -m pre_commit autoupdate

venv:
	@echo "Creating virtual environment..."
	@if [ ! -d "$(VENV_DIR)" ]; then \
		$(PYTHON) -m venv $(VENV_DIR) && \
		echo "Virtual environment created at $(VENV_DIR)"; \
	else \
		echo "Virtual environment already exists at $(VENV_DIR)"; \
	fi
	@if [ -f "$(REQUIREMENTS_FILE)" ]; then \
		echo "Installing Python dependencies..." && \
		$(VENV_PIP) install -r $(REQUIREMENTS_FILE); \
	else \
		echo "No requirements.txt found, skipping Python dependencies"; \
	fi

activate:
	@echo "To activate the virtual environment, run:"
ifeq ($(OS),Windows_NT)
	@echo "  $(ACTIVATE_SCRIPT)"
else
	@echo "  source $(ACTIVATE_SCRIPT)"
endif
