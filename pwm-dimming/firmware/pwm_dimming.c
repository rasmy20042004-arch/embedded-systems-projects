// PIC16F877A Configuration Bit Settings
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage In-Circuit Serial Programming Enable bit (disabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (disabled)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (disabled)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (disabled)

#include <xc.h>
#include <stdio.h> 

#define _XTAL_FREQ 20000000     // 20MHz crystal oscillator

// --- LCD Pin Definitions ---
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

// --- LCD Drivers ---
void Lcd_Port(char a) {
    D4 = (a & 1) ? 1 : 0;
    D5 = (a & 2) ? 1 : 0;
    D6 = (a & 4) ? 1 : 0;
    D7 = (a & 8) ? 1 : 0;
}

void Lcd_Cmd(char a) {
    RS = 0;             
    Lcd_Port(a >> 4);   
    EN = 1; __delay_us(40); EN = 0;
    Lcd_Port(a);        
    EN = 1; __delay_us(40); EN = 0;
}

void Lcd_Clear(void) {
    Lcd_Cmd(0x01);
    __delay_ms(2);
}

void Lcd_Set_Cursor(char row, char col) {
    if(row == 1)      Lcd_Cmd(0x80 + (col - 1));
    else if(row == 2) Lcd_Cmd(0xC0 + (col - 1));
}

void Lcd_Init(void) {
    Lcd_Port(0);
    __delay_ms(20);
    Lcd_Cmd(0x03); __delay_ms(5);
    Lcd_Cmd(0x03); __delay_ms(11);
    Lcd_Cmd(0x03); 
    Lcd_Cmd(0x02); 
    Lcd_Cmd(0x28); // 4-bit mode, 2 lines, 5x8 font
    Lcd_Cmd(0x0C); // Display ON, Cursor OFF
    Lcd_Cmd(0x06); // Entry mode
    Lcd_Clear();
}

void Lcd_Write_Char(char a) {
    RS = 1;             
    Lcd_Port(a >> 4);   
    EN = 1; __delay_us(40); EN = 0;
    Lcd_Port(a);        
    EN = 1; __delay_us(40); EN = 0;
}

void Lcd_Write_String(const char *a) {
    for(int i = 0; a[i] != '\0'; i++) {
        Lcd_Write_Char(a[i]);
    }
}

// --- Main Program ---
void main(void) {
    unsigned int adc_value = 0;
    unsigned int prev_adc = 9999; // Set dummy value to trigger initial write
    unsigned int brightness_percent = 0;
    char display_buffer[17];

    // 1. Initialize Ports
    TRISA0 = 1;         // RA0 as input for potentiometer
    TRISC2 = 0;         // RC2 as output for LED (PWM)
    TRISD = 0x00;       // PORTD as output for LCD

    // 2. Initialize ADC Module (Fosc/32 for 20MHz clock)
    ADCON1 = 0x8E;      // Right justify result, RA0 Analog
    ADCON0 = 0x81;      // Fosc/32, Select Channel 0, Turn ADC On

    // 3. Initialize PWM Module (CCP1)
    CCP1CON = 0x0C;     // PWM mode
    PR2 = 255;          // Max PWM period (100% duty cycle ref)
    T2CON = 0x04;       // Timer2 ON, 1:1 prescaler

    // 4. Initialize LCD
    Lcd_Init();

    // 5. Main Loop
    while(1) {
        // Read Potentiometer
        __delay_us(20);                 // Acquisition time delay
        ADCON0bits.GO = 1;              
        while(ADCON0bits.GO);           
        adc_value = (ADRESH << 8) | ADRESL; 

        // Update PWM Duty Cycle in Real-Time
        CCPR1L = adc_value >> 2;        
        CCP1CONbits.CCP1Y = (adc_value & 2) >> 1; 
        CCP1CONbits.CCP1X = adc_value & 1;        

        // Only refresh the LCD when ADC reading changes (eliminates flicker)
        if(adc_value != prev_adc) {
            // Prevent 16-bit integer overflow during calculation
            brightness_percent = ((unsigned long)adc_value * 100) / 1023; 
            
            // Line 1: Real-time ADC value
            sprintf(display_buffer, "ADC Val : %-4u  ", adc_value);
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String(display_buffer);

            // Line 2: Real-time Brightness %
            sprintf(display_buffer, "Bright  : %-3u%%  ", brightness_percent);
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String(display_buffer);

            prev_adc = adc_value;
        }
        
        __delay_ms(50);
    }
}