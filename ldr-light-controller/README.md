# LDR Light Controller — PIC16F877A

An automatic light controller that measures ambient light with an **LDR**, calculates a light-intensity percentage and approximate lux value, and switches an output using an adjustable threshold with hysteresis.

## Hardware / pin mapping

- PIC16F877A
- 10 MHz crystal
- LDR voltage-divider signal → RA0 / AN0
- Controlled LED/output → RC2
- Setpoint UP → RD1, active LOW
- Setpoint DOWN → RD2, active LOW
- LCD RS → RB0
- LCD EN → RB1
- LCD D4–D7 → RB4–RB7
- **MCLR/VPP pin 1 → +5 V through 10 kΩ**

> Fit the 10 kΩ MCLR pull-up on physical hardware. Pin 1 must not float.

## Firmware features

- 10-sample moving-average ADC filter
- Adjustable setpoint stored in EEPROM
- 10% hysteresis band to prevent rapid output switching
- Light-intensity percentage display
- Approximate lux calculation

The lux calculation in the source assumes a **10 kΩ fixed resistor** in the LDR divider.

## Files

```text
firmware/LDR.c
firmware/LDR.hex
simulation/LDR_Light_Controller.pdsprj
```
