# PIC16F877A Embedded Systems Projects

A compact embedded-systems portfolio built around the **PIC16F877A**, **MPLAB X**, **XC8**, and **Proteus**. This repository groups four practical projects demonstrating digital I/O, timers, ADC, PWM, LCD interfacing, and basic control logic.

## Projects

| Project | What it demonstrates | Clock | Main I/O |
|---|---|---:|---|
| **Knight Rider Circuit** | LED sequencing and button-controlled timing | 10 MHz | RD0-RD5 LEDs, RB0 button |
| **Digital Stopwatch** | Timing, start/pause/reset logic and LCD output | 10 MHz | LCD on PORTB, RD1/RD2 buttons |
| **PWM Dimming** | ADC input and CCP1 hardware PWM | 20 MHz | RA0 ADC, RC2 PWM, LCD |
| **LDR Light Controller** | Analog sensing, threshold control and hysteresis | 10 MHz | AN0 LDR, RC2 output, LCD |

## Critical Hardware Requirement

> **PIC16F877A MCLR/RESET (physical pin 1) must be connected to +5 V through a 10 kOhm pull-up resistor. Do not leave MCLR floating.**

Also verify:

- VDD pins **11 and 32 -> +5 V**
- VSS pins **12 and 31 -> GND**
- Common ground for the complete circuit
- Correct oscillator frequency for each project

## Repository Structure

```text
embedded-systems-projects/
|-- knight-rider-circuit/
|-- digital-stopwatch/
|-- pwm-dimming/
\-- ldr-light-controller/
```

Each subproject contains firmware, Proteus simulation files, and its own README.

## Development Stack

- PIC16F877A
- MPLAB X
- XC8
- Proteus Design Suite
- Embedded C

## Skills Demonstrated

Embedded C, GPIO, timers, ADC, PWM, LCD interfacing, sensor interfacing, switch handling, circuit simulation, and microcontroller debugging.

---

**Author:** Mohamed Rasmy  
**Project type:** Embedded Systems Portfolio