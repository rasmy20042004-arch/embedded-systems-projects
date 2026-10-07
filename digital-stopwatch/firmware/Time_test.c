#pragma config FOSC = HS        // High-Speed Crystal Oscillator (10 MHz)
#pragma config WDTE = OFF       // Watchdog Timer Disabled
#pragma config PWRTE = ON       // Power-up Timer Enabled
#pragma config BOREN = ON       // Brown-out Reset Enabled
#pragma config LVP = OFF        // Low-Voltage Programming Disabled
#pragma config CPD = OFF        // Data EEPROM Code Protection Off
#pragma config WRT = OFF        // Flash Program Memory Write Disable
#pragma config CP = OFF         // Flash Program Memory Code Protection Off

#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 10000000     // 10 MHz System Clock

// LCD Pins (PORTB)
#define RS PORTBbits.RB0        // Pin 33
#define EN PORTBbits.RB1        // Pin 34
#define LCD_DATA PORTB          // RB4-RB7 (Pins 37-40)

// Push Buttons (PORTD - Pins 20 & 21)
#define BTN_START_PAUSE PORTDbits.RD1 // Pin 20
#define BTN_RESET       PORTDbits.RD2 // Pin 21

// Status LED (PORTC - Pin 17)
#define LED_RUNNING PORTCbits.RC2     // Pin 17

// Global Stopwatch Variables
volatile unsigned char seconds = 0;
volatile unsigned char minutes = 0;
volatile unsigned char running_state = 0; // 0 = Paused, 1 = Running
volatile unsigned char timer_ticks = 0;   // 20 ticks * 50ms = 1 sec
volatile unsigned char update_display_flag = 1; // Flag to trigger screen refresh

// Function Prototypes
void Lcd_Cmd(unsigned char cmd);
void Lcd_Data(unsigned char data);
void Lcd_Init(void);
void Lcd_Set_Cursor(unsigned char row, unsigned char col);
void Lcd_Print_String(const char *str);
void Update_Display(void);

// Timer1 Interrupt Service Routine (50ms tick at 10 MHz)
void __interrupt() ISR(void) {
    if (PIR1bits.TMR1IF) {
        // Preload Timer1 for 50 ms delay (65536 - 15625 = 49911 = 0xC2F7)
        TMR1H = 0xC2;
        TMR1L = 0xF7;
        
        if (running_state) {
            timer_ticks++;
            if (timer_ticks >= 20) { // 20 * 50ms = 1 Second
                timer_ticks = 0;
                seconds++;
                if (seconds >= 60) {
                    seconds = 0;
                    minutes++;
                    if (minutes >= 60) {
                        minutes = 0;
                    }
                }
                update_display_flag = 1; // Trigger LCD update once per second
            }
        }
        PIR1bits.TMR1IF = 0; // Clear interrupt flag
    }
}

void main(void) {
    // Disable Analog Inputs
    ADCON1 = 0x06; 

    // Configure Port Directions
    TRISB = 0x00; // PORTB Outputs (LCD)
    TRISC = 0x00; // PORTC Outputs (LED at RC2)
    TRISD = 0x06; // RD1 & RD2 as Inputs (0b00000110)

    // Clear initial outputs
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    // Timer1 Setup (1:8 Prescaler)
    T1CON = 0x30; 
    TMR1H = 0xC2;
    TMR1L = 0xF7;
    
    // Enable Interrupts
    PIR1bits.TMR1IF = 0;
    PIE1bits.TMR1IE = 1; 
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE  = 1; 

    T1CONbits.TMR1ON = 1; // Start Timer1

    // Initialize LCD
    Lcd_Init();
    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("STOPWATCH");
    
    LED_RUNNING = 0;

    while (1) {
        // Instant Detection: START/PAUSE Button (RD1 / Pin 20)
        if (BTN_START_PAUSE == 0) {
            __delay_ms(10); // Quick 10ms debounce
            if (BTN_START_PAUSE == 0) {
                running_state = !running_state; // Toggle run/pause
                LED_RUNNING = running_state;
                update_display_flag = 1;
                while (BTN_START_PAUSE == 0);   // Wait for release
            }
        }

        // Instant Detection: RESET Button (RD2 / Pin 21)
        if (BTN_RESET == 0) {
            __delay_ms(10); // Quick 10ms debounce
            if (BTN_RESET == 0) {
                running_state = 0;
                timer_ticks = 0;
                seconds = 0;
                minutes = 0;
                LED_RUNNING = 0;
                update_display_flag = 1;
                while (BTN_RESET == 0); // Wait for release
            }
        }

        // Update LCD display only when data changes
        if (update_display_flag) {
            update_display_flag = 0;
            Update_Display();
        }
    }
}

// Fast LCD Drivers (50us delay instead of blocking 2ms)
void Lcd_Cmd(unsigned char cmd) {
    RS = 0;
    LCD_DATA = (LCD_DATA & 0x0F) | (cmd & 0xF0);
    EN = 1; __delay_us(5); EN = 0;
    
    LCD_DATA = (LCD_DATA & 0x0F) | ((cmd << 4) & 0xF0);
    EN = 1; __delay_us(5); EN = 0;
    __delay_us(50);
}

void Lcd_Data(unsigned char data) {
    RS = 1;
    LCD_DATA = (LCD_DATA & 0x0F) | (data & 0xF0);
    EN = 1; __delay_us(5); EN = 0;
    
    LCD_DATA = (LCD_DATA & 0x0F) | ((data << 4) & 0xF0);
    EN = 1; __delay_us(5); EN = 0;
    __delay_us(50);
}

void Lcd_Init(void) {
    __delay_ms(20);
    Lcd_Cmd(0x02); // 4-bit mode initialization
    Lcd_Cmd(0x28); // 2 lines, 5x7 matrix
    Lcd_Cmd(0x0C); // Display ON, Cursor OFF
    Lcd_Cmd(0x06); // Entry mode set
    Lcd_Cmd(0x01); // Clear display screen
    __delay_ms(2);  // Clear requires longer execution time
}

void Lcd_Set_Cursor(unsigned char row, unsigned char col) {
    unsigned char pos = (row == 1) ? (0x80 + col - 1) : (0xC0 + col - 1);
    Lcd_Cmd(pos);
}

void Lcd_Print_String(const char *str) {
    while (*str) Lcd_Data(*str++);
}

void Update_Display(void) {
    char timeStr[16];
    sprintf(timeStr, "TIME: %02d:%02d", minutes, seconds);
    Lcd_Set_Cursor(2, 1);
    Lcd_Print_String(timeStr);
}