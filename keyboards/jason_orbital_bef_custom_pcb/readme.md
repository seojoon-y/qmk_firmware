# jason_orbital
Encoder order:
1. GND (white)
2. VCC 5V (red)
3. Channel B (green)
4. Channel A (black)

    qmk flash -kb jason_orbital -km default -bl uf2-split-right


https://cyanophage.github.io/playground.html?layout=bldcvjfoux-nrtsgyhaei%2C%5Cqmw%3D%3Bpkz%2F.*%27&mode=ergo&lan=english&thumb=l

https://github.com/daprice/keyswitches.pretty

Haptic:
https://chatgpt.com/c/698a1fb5-36ac-8395-b19d-59569464c641

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available

homid.
