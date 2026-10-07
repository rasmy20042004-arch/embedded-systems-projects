// Configuration Bits
#pragma config FOSC = HS        // 10MHz High-Speed Crystal Oscillator
#pragma config WDTE = OFF       // Watchdog Timer Disabled
#pragma config PWRTE = ON       // Power-up Timer Enabled
#pragma config BOREN = ON       // Brown-out Reset Enabled
#pragma config LVP = OFF        // Low-Voltage Programming Disabled
#pragma config CPD = OFF        
#pragma config WRT = OFF        
#pragma config CP = OFF         

#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 10000000     // 10MHz Clock Frequency

// LCD Pin Definitions
#define RS RB0
#define EN RB1
#define D4 RB4
#define D5 RB5
#define D6 RB6
#define D7 RB7

// Output Pin (Pin 17 - RC2)
#define LED RC2

// Push Buttons (Active-LOW: Reads 0 when pressed)
#define BTN_UP RD1
#define BTN_DOWN RD2

// LCD Function Prototypes
void LCD_Init();
void LCD_Command(unsigned char cmd);
void LCD_Char(unsigned char dat);
void LCD_String(const char *msg);
void LCD_Port(unsigned char a);

// ADC & Helper Prototypes
void ADC_Init();
unsigned int ADC_Read(unsigned char channel);
unsigned int ADC_Get_Averaged(unsigned char channel);
void EEPROM_Write_Safe(unsigned char addr, unsigned char data);
unsigned int Calculate_Lux(unsigned int raw_adc);

void main(void) {
    char buffer[16];
    unsigned int raw_adc, lux_val;
    unsigned int light_pct;
    unsigned char set_position;
    unsigned char set_high;
    
    // Pin Direction Configuration
    TRISB = 0x00;           // PORTB as output for LCD
    TRISC2 = 0;             // RC2 (Pin 17) as output for LED
    TRISA0 = 1;             // RA0 (AN0) as input for LDR
    TRISD1 = 1;             // RD1 as input for UP button
    TRISD2 = 1;             // RD2 as input for DOWN button

    LED = 0;                // Ensure LED starts OFF

    ADC_Init();
    LCD_Init();

    // Read stored set position from EEPROM
    set_position = eeprom_read(0x00);
    if (set_position > 90) { set_position = 30; EEPROM_Write_Safe(0x00, set_position); }

    while(1) {
        // 1. Moving Average Filter (10 readings)
        raw_adc = ADC_Get_Averaged(0);
        
        // 2. Light Intensity (LI) Percentage & Lux Calculations
        light_pct = ((1023 - (long)raw_adc) * 100) / 1023;
        lux_val = Calculate_Lux(raw_adc);

        // 3. Set Position Adjustment Buttons (Active-LOW: 0 when pressed)
        if (BTN_UP == 0) {
            __delay_ms(50); // Debounce
            if (BTN_UP == 0) {
                if (set_position < 90) set_position += 5; // Limit setpoint to 90% so upper range stays <= 100%
                EEPROM_Write_Safe(0x00, set_position);
                while(BTN_UP == 0); // Wait for release
            }
        }
        
        if (BTN_DOWN == 0) {
            __delay_ms(50); // Debounce
            if (BTN_DOWN == 0) {
                if (set_position > 5) set_position -= 5;
                EEPROM_Write_Safe(0x00, set_position);
                while(BTN_DOWN == 0); // Wait for release
            }
        }

        // 4. Upper Range Threshold (10% higher than set point)
        set_high = set_position + 10;

        // 5. 10% Range Hysteresis Logic:
        // Turns ON when LI drops to or below set_position
        // Turns OFF when LI rises to or above set_high (set_position + 10%)
        // Keeps previous state while LI is inside the 10% band
        if (light_pct <= set_position) {
            LED = 1;    // Turn ON
        } else if (light_pct >= set_high) {
            LED = 0;    // Turn OFF
        }

        // 6. LCD Display Updates
        // Line 1: Show Light Intensity (LI) and target Set Position
        sprintf(buffer, "LI:%2u%%   SET:%2u%% ", light_pct, set_position);
        LCD_Command(0x80);
        LCD_String(buffer);

        // Line 2: Show Lux and the 10% Hysteresis Range (e.g., R:30-40)
        sprintf(buffer, "Lux:%-4u R:%2u-%2u", lux_val, set_position, set_high);
        LCD_Command(0xC0);
        LCD_String(buffer);

        __delay_ms(100);
    }
}

// EEPROM Write Guard
void EEPROM_Write_Safe(unsigned char addr, unsigned char data) {
    if (eeprom_read(addr) != data) {
        eeprom_write(addr, data);
    }
}

// Moving Average Filter (10 samples)
unsigned int ADC_Get_Averaged(unsigned char channel) {
    unsigned long sum = 0;
    for(unsigned char i = 0; i < 10; i++) {
        sum += ADC_Read(channel);
        __delay_ms(2);
    }
    return (unsigned int)(sum / 10);
}

// Convert ADC Reading to Lux Value
unsigned int Calculate_Lux(unsigned int raw_adc) {
    if (raw_adc >= 1020) return 0;   // Dark limit
    if (raw_adc <= 5) return 2000;    // Bright limit
    
    float r_ldr = (10000.0 * (float)raw_adc) / (1023.0 - (float)raw_adc);
    float lux = (500.0 * 1000.0) / r_ldr;
    return (unsigned int)lux;
}

// ADC Driver Setup
void ADC_Init() {
    ADCON0 = 0x41; // ADC ON, Fosc/8, Channel 0 selected
    ADCON1 = 0x8E; // Right Justified, AN0 as Analog
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0xC5;
    ADCON0 |= (channel << 3);
    __delay_ms(1);
    GO_nDONE = 1;
    while(GO_nDONE);
    return ((ADRESH << 8) + ADRESL);
}

// LCD Drivers (4-bit Mode)
void LCD_Port(unsigned char a) {
    D4 = (a & 1) ? 1 : 0;
    D5 = (a & 2) ? 1 : 0;
    D6 = (a & 4) ? 1 : 0;
    D7 = (a & 8) ? 1 : 0;
}

void LCD_Command(unsigned char cmd) {
    RS = 0;
    LCD_Port(cmd >> 4);
    EN = 1; __delay_ms(1); EN = 0;
    LCD_Port(cmd & 0x0F);
    EN = 1; __delay_ms(1); EN = 0;
}

void LCD_Char(unsigned char dat) {
    RS = 1;
    LCD_Port(dat >> 4);
    EN = 1; __delay_ms(1); EN = 0;
    LCD_Port(dat & 0x0F);
    EN = 1; __delay_ms(1); EN = 0;
}

void LCD_Init() {
    LCD_Port(0x00);
    __delay_ms(20);
    LCD_Command(0x02); // 4-bit initialization
    LCD_Command(0x28); // 2 lines, 5x7 matrix
    LCD_Command(0x0C); // Display ON, Cursor OFF
    LCD_Command(0x06); // Auto-increment cursor
    LCD_Command(0x01); // Clear display
    __delay_ms(2);
}

void LCD_String(const char *msg) {
    while(*msg) {
        LCD_Char(*msg++);
    }
}