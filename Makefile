.PHONY: build
.PHONY: fftw
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

NUM_PROCESSORS = 4
MAKEFLAGS += --no-print-directory

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
	@cmake --build build --config Debug -- --no-print-directory -j ${NUM_PROCESSORS}

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
	@$(MAKE) crinkler
	@$(MAKE) fftw
	@$(MAKE) venv
	@$(MAKE) activate

consts:
	@echo "Generating constants..."
	@$(VENV_PYTHON) scripts/constants.py

nasmfmt:
ifeq ($(GO_CHECK),)
	@echo "Go is not installed. Installation of nasmfmt is skipped."
else
ifeq ($(NASMFMT_INSTALLED),)
	@echo "Installing nasmfmt..."
	@$(INSTALL_NASM_FMT)
	@which nasmfmt > /dev/null || echo "nasmfmt not found in PATH. Add \$GOPATH/bin to your PATH."
else
	@echo "nasmfmt is already installed."
endif
endif

onekpaq:
	@echo "Installing oneKpaq..."
	@if [ ! -d $(TOOLS_DIR)/oneKpaq ]; then \
		mkdir -p $(TOOLS_DIR); \
		cd $(TOOLS_DIR) && git clone https://github.com/temisu/oneKpaq.git; \
	fi
	@cd $(TOOLS_DIR)/oneKpaq && $(MAKE)
	@rm -rf $(TOOLS_DIR)/oneKpaq/.git 2>/dev/null || true

crinkler:
	@echo "Installing Crinkler..."
	@if [ ! -d "$(TOOLS_DIR)/crinkler" ]; then \
		mkdir -p $(TOOLS_DIR) && \
		echo "Downloading Crinkler 2.3..." && \
		curl -L -o crinkler23.zip https://github.com/runestubbe/Crinkler/releases/download/v2.3/crinkler23.zip && \
		echo "Extracting Crinkler..." && \
		powershell -Command "Expand-Archive -Path crinkler23.zip -DestinationPath $(TOOLS_DIR)\crinkler -Force" && \
		rm crinkler23.zip && \
		echo "Crinkler installed to $(TOOLS_DIR)/crinkler"; \
	else \
		echo "Crinkler directory already exists"; \
	fi

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

fftw:
	@echo "Installing FFTW..."
ifeq ($(OS),Windows_NT)
	@if [ ! -d "fftw" ]; then \
		mkdir -p fftw && \
		echo "Downloading FFTW 3.3.5 for Windows..." && \
		curl -L -o fftw-3.3.5-dll32.zip https://fftw.org/pub/fftw/fftw-3.3.5-dll32.zip && \
		echo "Extracting FFTW..." && \
		powershell -Command "Expand-Archive -Path fftw-3.3.5-dll32.zip -DestinationPath fftw -Force" && \
		rm fftw-3.3.5-dll32.zip && \
		echo "FFTW installed to fftw directory"; \
	else \
		echo "FFTW directory already exists"; \
	fi
else
	@if [ ! -d "fftw" ]; then \
		mkdir -p fftw && \
		echo "Downloading FFTW 3.3.10 for Linux..." && \
		curl -L -o fftw-3.3.10.tar.gz https://www.fftw.org/fftw-3.3.10.tar.gz && \
		echo "Extracting FFTW..." && \
		tar -xzf fftw-3.3.10.tar.gz -C fftw --strip-components=1 && \
		rm fftw-3.3.10.tar.gz && \
		echo "FFTW source installed to fftw directory"; \
	else \
		echo "FFTW directory already exists"; \
	fi
endif
