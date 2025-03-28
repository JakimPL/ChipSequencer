# _ChipSequencer_

_ChipSequencer_ is a chiptune tracker that aims for creating small 16-bit DOS executable music programs.

## Prerequisites

* `nasm`

To install NASM for DOS, use `make nasm`.

### aPACK

_aPACK_ is a DOS 16-bit executable compressor written by [Jørgen Ibsen](https://ibsensoftware.com/products_aPACK.html) that is being used for reducing the executable size.

### Code formatters

* `nasmfmt`
* `pre-commit`

To install all of these, run `make install`.

### Build & compile

The project consists of two parts:
* assembly DOS code
* tracker source code

To prepare a workspace, run the `build.sh` script. After successful preparation, one can build the project via `make build` or, simply, `make`. The executable should be created in the `bin` directory.

To compile only DOS sources, use `compile.bat` script. It should create a DOS executable.

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
