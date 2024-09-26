# _ChipSequencer_

## Prerequisites

* `nasm`

To install NASM for DOS, use `make nasm`.

### Code formatters

* `nasmfmt`
* `pre-commit`

To install all of these, run `make install`.

## DOSBox configuration

To run the project in DOSBox, please set the following _SoundBlaster16_ settings in the `dosbox.conf` file:

```conf
sbtype=sb16
sbbase=220
irq=7
dma=1
hdma=5
sbmixer=true
```
