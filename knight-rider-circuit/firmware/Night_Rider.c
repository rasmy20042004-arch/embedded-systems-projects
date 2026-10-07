// PIC16F877A Configuration Bit Settings
#pragma config FOSC = HS        // Oscillator Selection (HS oscillator for 10MHz crystal)
#pragma config WDTE = OFF       // Watchdog Timer Disable
#pragma config PWRTE = ON       // Power-up Timer Enable
#pragma config BOREN = ON       // Brown-out Reset Enable
#pragma config LVP = OFF        // Low-Voltage Programming Disable
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection Disable
#pragma config WRT = OFF        // Flash Program Memory Write Enable Disable
#pragma config CP = OFF         // Flash Program Memory Code Protection Disable

#include <xc.h>

#define _XTAL_FREQ 10000000     // Define oscillator frequency as 10MHz

// Global State Variables
unsigned char mode = 0;         // 0: Single Blink, 1: Double Blink, 2: Night Rider
unsigned char speed_level = 0;  // 0: Slow, 1: Medium, 2: Fast
unsigned int delays[3] = {500, 250, 100}; // The 3 delay levels in milliseconds
unsigned char mode_changed = 0; // Flag to immediately break out of patterns

// Custom interruptable delay function
void smart_delay(unsigned int ms) {
    for(unsigned int i = 0; i < ms; i++) {
        __delay_ms(1); // 1 millisecond base unit
        
        // -------------------------------------------------------------
        // Check PIN 34 (RB1) -> Change Mode (3 Levels)
        // -------------------------------------------------------------
        if (PORTBbits.RB1 == 0) {         
            __delay_ms(20);               // 20ms debounce
            if (PORTBbits.RB1 == 0) {
                mode = (mode + 1) % 3;    // Cycle through modes 0, 1, 2
                mode_changed = 1;         // Signal loop to restart
                while(PORTBbits.RB1 == 0);// Wait until button is released
                return;                   // Break delay immediately
            }
        }
        
        // -------------------------------------------------------------
        // Check PIN 33 (RB0) -> Change Speed (3 Levels)
        // -------------------------------------------------------------
        if (PORTBbits.RB0 == 0) {         
            __delay_ms(20);               // 20ms debounce
            if (PORTBbits.RB0 == 0) {
                speed_level = (speed_level + 1) % 3; // Cycle through speeds 0, 1, 2
                mode_changed = 1;                    // Restart with new speed
                while(PORTBbits.RB0 == 0);           // Wait until button is released
                return;                              // Break delay immediately
            }
        }
    }
}

void main(void) {
    // 1. Hardware Initialization
    TRISD = 0x00;           // Configure entire PORTD as OUTPUT
    PORTD = 0x00;           // Initialize all LEDs to OFF
    
    TRISBbits.TRISB0 = 1;   // Configure RB0 (Pin 33) as INPUT
    TRISBbits.TRISB1 = 1;   // Configure RB1 (Pin 34) as INPUT
    
    // 2. Main Infinite Loop
    while(1) {
        mode_changed = 0;   // Reset interruption flag
        unsigned int current_delay = delays[speed_level]; // Fetch active speed
        
        // --- MODE 0: SINGLE LED BLINK ---
        // Based on schematic, LEDs start at RD2 (Pin 21)
        if (mode == 0) {
            PORTD = (unsigned char)(1 << 2);   // Turn ON D3 (RD2)
            smart_delay(current_delay);
            if (mode_changed) continue;
            
            PORTD = 0x00;                      // Turn OFF
            smart_delay(current_delay);
            if (mode_changed) continue;
        }
        
        // --- MODE 1: TWO LEDs BLINKING ---
        else if (mode == 1) {
            PORTD = (unsigned char)((1 << 2) | (1 << 3)); // Turn ON D3 & D4 (RD2, RD3)
            smart_delay(current_delay);
            if (mode_changed) continue;
            
            PORTD = 0x00;                                 // Turn OFF
            smart_delay(current_delay);
            if (mode_changed) continue;
        }
        
        // --- MODE 2: NIGHT RIDER PATTERN (RD2 to RD7) ---
        else if (mode == 2) {
            // Forward movement: RD2 to RD7
            for(int i = 2; i <= 7; i++) {
                PORTD = (unsigned char)(1 << i); // Turn ON specific LED
                smart_delay(current_delay);
                if (mode_changed) break;
                
                PORTD = 0x00;                    // Turn OFF
                smart_delay(current_delay);
                if (mode_changed) break;
            }
            
            if (mode_changed) continue;  
            
            // Reverse movement: RD6 down to RD3 (prevents double-delay at the ends)
            for(int i = 6; i >= 3; i--) {
                PORTD = (unsigned char)(1 << i); // Turn ON specific LED
                smart_delay(current_delay);
                if (mode_changed) break;
                
                PORTD = 0x00;                    // Turn OFF
                smart_delay(current_delay);
                if (mode_changed) break;
            }
        }
    }
}