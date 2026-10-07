# PWM LED Dimming — PIC16F877A

This project reads a potentiometer through the PIC ADC and maps the 10-bit ADC value to **CCP1 hardware PWM** for smooth LED brightness control. A 16×2 LCD displays the ADC reading and brightness percentage.

## Hardware / pin mapping

- PIC16F877A
- 20 MHz crystal
- Potentiometer analog input → RA0 / AN0
- PWM output → RC2 / CCP1
- LCD RS → RD2
- LCD EN → RD3
- LCD D4–D7 → RD4–RD7
- **MCLR/VPP pin 1 → +5 V through 10 kΩ**

> Do not leave MCLR floating on physical hardware. Use the 10 kΩ pull-up from pin 1 to +5 V.

## Firmware features

- 10-bit ADC measurement
- CCP1 PWM output
- Real-time brightness percentage
- LCD refresh only when the ADC value changes to reduce visible flicker

## Files

```text
firmware/pwm_dimming.c
firmware/PWM_Dimming.hex
simulation/PWM_Dimming.pdsprj
```
