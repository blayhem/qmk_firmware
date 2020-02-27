# How to compile a new keymap for (my) whitefox.

1. install dependencies (macOS):
```
brew tap osx-cross/avr
brew tap osx-cross/arm
brew update
brew install avr-gcc@8
brew link --force avr-gcc@8
brew install dfu-programmer
brew install dfu-util
brew install arm-gcc-bin@8
brew link --force arm-gcc-bin@8
brew install avrdude
```

2. [build firmware](https://docs.qmk.fm/#/newbs_building_firmware):
go to `keyboards/whitefox/keymaps`, edit `keymap.c`, then run `make whitefox:dfr`

3. [flash firmware](https://docs.qmk.fm/#/newbs_flashing):
use `QMK Toolbox`, load bin file, reset keyboard \*, hit flash (atmega32u4).
Expected result:

```
    Download done.
    state(7) = dfuMANIFEST, status(0) = No error condition is present
    dfu-util: unable to read DFU status after completion
*** Kiibohd device disconnected
```

\* if 'bytes dfu-util error during download'
> Usually this means the WhiteFox is in secure bootloader mode. All you have to do if this happens is to press the reset button on the bottom of the keyboard.