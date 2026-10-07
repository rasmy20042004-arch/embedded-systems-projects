# Knight Rider Circuit — PIC16F877A

Six LEDs create a left-to-right and right-to-left **Knight Rider** scanning effect. A pushbutton changes the animation speed while the pattern is running.

## Hardware / pin mapping

- PIC16F877A
- 10 MHz crystal
- RD0–RD5 → six LED outputs
- RB0 → active-low speed-selection pushbutton
- Current-limiting resistor in series with each LED (the original project documentation uses approximately 440 Ω)
- **MCLR/VPP pin 1 → +5 V through 10 kΩ**

> The 10 kΩ resistor on MCLR is required for reliable hardware operation. Do not leave pin 1 floating.

## Firmware behavior

Four speed presets are implemented: approximately **100 ms, 300 ms, 500 ms, and 800 ms**. The button is polled during the delay routine, allowing the speed to change without waiting for a complete LED sweep.

## Files

```text
firmware/Night_Rider.c
firmware/Night_Rider.hex
simulation/Night_Rider.pdsprj
```

## Run

Compile `Night_Rider.c` for PIC16F877A using XC8 with a 10 MHz clock, then load the HEX into the PIC model in Proteus or program the physical PIC.
