# Digital Stopwatch — PIC16F877A

A PIC16F877A stopwatch using **Timer1 interrupts**, a 16×2 LCD, start/pause control, reset control, and a running-status LED.

## Hardware / pin mapping

- PIC16F877A
- 10 MHz crystal
- LCD RS → RB0
- LCD EN → RB1
- LCD data D4–D7 → RB4–RB7
- Start/Pause button → RD1, active LOW
- Reset button → RD2, active LOW
- Running LED → RC2
- **MCLR/VPP pin 1 → +5 V through 10 kΩ**

> The 10 kΩ MCLR pull-up must be fitted on the real circuit so the PIC does not remain in or randomly enter reset.

## Timing

Timer1 is configured to generate an interrupt approximately every **50 ms**. Twenty ticks form one second. The display is refreshed only when the stopwatch state changes, reducing unnecessary LCD writes.

## Files

```text
firmware/Time_test.c
firmware/Stopwatch.hex
simulation/Stopwatch.pdsprj
```
