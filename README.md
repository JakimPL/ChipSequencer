# _ChipSequencer_

_ChipSequencer_ is a chiptune tracker that aims for creating small 32-bit executable music programs.

## Prerequisites

### Packers

This project would not exist without executable compressors on which this project is based.

#### oneKpaq

_oneKpaq_ is a Linux executable compressor developed by _temisu_ and _PoroCYon_.

Installation requires compilation from source. Run `make onekpaq` for the complete installation.

Since the compression algorithm takes time, the default compression settings is `fast`.

### Code formatters

* `nasmfmt` (NASM)
* `clang-format` (C++)
* `pre-commit` (automatization)

To install all of these, run `make install`.

### Build & compile

The project consists of two parts:
* assembly `core` code
* tracker source code

To prepare a workspace, run the `build.sh` script. After successful preparation, one can build the project via `make build` or, simply, `make`. The executable should be created in the `bin` directory.

## Configuration

The project uses the `config.yaml` file for generating `config.asm` used by the assembler. To generate the assembler configuration file, run `make config`.
