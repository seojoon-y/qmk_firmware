# jason_orbital
Encoder order:
1. GND (white)
2. VCC 5V (red)
3. Channel B (green)
4. Channel A (black)

    qmk flash -kb jason_orbital -km default -bl uf2-split-right

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
