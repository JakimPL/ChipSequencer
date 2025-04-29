# _ChipSequencer_

_ChipSequencer_ is a chiptune tracker that aims for creating small 16-bit DOS executable music programs.

## Prerequisites

* `nasm`

To install NASM for DOS, use `make nasm`.

### Packers

This project would not exist without executable compressors on which this project is based.

#### aPACK

_aPACK_ is a DOS 16-bit executable compressor written by [Jørgen Ibsen](https://ibsensoftware.com/products_aPACK.html) that is being used for reducing the executable size.

To install aPACK, run `make apack`.

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

### DOSBox configuration

To run the project in DOSBox, please set the following _SoundBlaster16_ settings in the `dosbox.conf` file:

```conf
sbtype=sb16
sbbase=220
irq=7
dma=1
hdma=5
sbmixer=true
```
