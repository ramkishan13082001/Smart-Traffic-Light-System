#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000

// --- Global State Variables ---
volatile int emergency_mode = 0;

// --- Function Prototypes ---
void lcd_init();
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_string(const char *str);
void adc_init();
int  adc_read();
void pwm_init();
void pwm_set_duty(unsigned int duty);
void uart_init(long baud_rate);
void uart_send_string(const char *str);
void timer1_delay_ms(unsigned int ms);

// --- Hardware Interrupt Service Routine ---
void __interrupt() ISR() {
    if (INTCONbits.INT0IF) {          // If Emergency Button (RB0) pressed
        emergency_mode = 1;           // Set emergency flag
        INTCONbits.INT0IF = 0;        // Clear interrupt flag
    }
}

void main() {
    // --- 1. Pin Configuration ---
    TRISA = 0x01;  // RA0 Input (ADC), RA1/RA2/RA3 Output (Traffic LEDs)
    TRISB = 0x01;  // RB0 Input (INT0 Emergency Button)
    TRISC = 0x80;  // RC7 RX In, others out (RC0 Buzzer, RC2 PWM, RC4/5 LCD, RC6 TX)
    TRISD = 0x00;  // PORTD Output (LCD Data)
    
    // Default outputs OFF
    PORTA = 0x00;
    RC0 = 0; // Buzzer OFF
    
    // --- 2. Subsystem Initialization ---
    lcd_init();
    adc_init();
    pwm_init();
    uart_init(9600);
    
    // --- 3. Interrupt Configuration ---
    INTCONbits.INT0IE = 1;     // Enable INT0 External Interrupt
    INTCON2bits.INTEDG0 = 0;   // Trigger on Falling Edge (Active Low Button)
    INTCONbits.GIE = 1;        // Enable Global Interrupts
    INTCONbits.PEIE = 1;       // Enable Peripheral Interrupts
    
    uart_send_string("\r\n============================\r\n");
    uart_send_string("SMART TRAFFIC SYSTEM BOOTED\r\n");
    uart_send_string("============================\r\n");
    
    lcd_string("SYSTEM BOOTING..");
    timer1_delay_ms(2000);

    // --- Main RTOS-style Control Loop ---
    while(1) {
        // --- EMERGENCY OVERRIDE CHECK ---
        if (emergency_mode) {
            PORTAbits.RA1 = 1; // RED ON
            PORTAbits.RA2 = 0; // YELLOW OFF
            PORTAbits.RA3 = 0; // GREEN OFF
            RC0 = 1;           // Buzzer ON
            pwm_set_duty(1023); // Streetlights max brightness
            
            lcd_command(0x01);
            lcd_string("EMERGENCY OVERRIDE");
            uart_send_string("ALERT: AMBULANCE DETECTED. FORCING RED!\r\n");
            
            while(emergency_mode); // System locks here until manually reset
        }
        
        // --- NORMAL OPERATION ---
        int density = adc_read(); // Read IR sensor on AN0
        unsigned int green_time = 5000; // 5 seconds default
        
        // Dynamic Logic Check
        if (density > 120) {      // If high traffic detected
            green_time = 10000;   // Extend green to 10 seconds
            pwm_set_duty(1023);   // Increase streetlight visibility
            uart_send_string("TRAFFIC: HIGH. Extending Green Time.\r\n");
            lcd_command(0x01); lcd_string("TRAFFIC: HEAVY");
        } else {
            pwm_set_duty(300);    // Dim streetlights to save power
            uart_send_string("TRAFFIC: NORMAL.\r\n");
            lcd_command(0x01); lcd_string("TRAFFIC: NORMAL");
        }
        
        // --- TRAFFIC SEQUENCE ---
        
        // 1. GREEN SIGNAL
        PORTAbits.RA1 = 0; PORTAbits.RA2 = 0; PORTAbits.RA3 = 1; 
        lcd_command(0xC0); lcd_string("SIGNAL: GREEN ");
        timer1_delay_ms(green_time); // Custom delay that aborts if interrupt fires
        if (emergency_mode) continue; // Instantly abort sequence
        
        // 2. YELLOW SIGNAL
        PORTAbits.RA1 = 0; PORTAbits.RA2 = 1; PORTAbits.RA3 = 0; 
        lcd_command(0xC0); lcd_string("SIGNAL: YELLOW");
        timer1_delay_ms(3000); 
        if (emergency_mode) continue;
        
        // 3. RED SIGNAL
        PORTAbits.RA1 = 1; PORTAbits.RA2 = 0; PORTAbits.RA3 = 0; 
        lcd_command(0xC0); lcd_string("SIGNAL: RED   ");
        timer1_delay_ms(5000); 
    }
}

// ====================================================================
// PERIPHERAL DRIVERS BELOW
// ====================================================================

// --- Custom Timer1 Delay (Interrupt Aware) ---
void timer1_delay_ms(unsigned int ms) {
    while (ms > 0 && emergency_mode == 0) {
        // Fosc/4 = 5MHz. Prescaler 1:8 = 625kHz clock.
        // 1 millisecond = 625 ticks.
        // 65536 - 625 = 64911 = 0xFD8F
        TMR1H = 0xFD;
        TMR1L = 0x8F;
        T1CON = 0x31; // Prescaler 1:8, Timer1 ON
        
        while (!PIR1bits.TMR1IF && emergency_mode == 0); // Wait for 1ms overflow
        
        PIR1bits.TMR1IF = 0; // Clear flag
        T1CONbits.TMR1ON = 0; // Stop Timer
        ms--;
    }
}

// --- UART Communication ---
void uart_init(long baud_rate) {
    SPBRG = (unsigned char)((_XTAL_FREQ / (64 * baud_rate)) - 1);
    TXSTAbits.TXEN = 1; // Enable TX
    RCSTAbits.SPEN = 1; // Enable Serial Port
}

void uart_send_char(char c) {
    while (!TXSTAbits.TRMT); // Wait until transmit shift register is empty
    TXREG = c;
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

// --- Analog-to-Digital Converter ---
void adc_init() {
    ADCON0 = 0x01; // Enable ADC, Channel 0
    ADCON1 = 0x0E; // Set AN0 to Analog, keeping RA1-RA3 Digital for Traffic LEDs
    ADCON2 = 0xA9; // Right justified, Fosc/8, 12 TAD
}

int adc_read() {
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ((ADRESH << 8) + ADRESL);
}

// --- Hardware PWM (CCP1) ---
void pwm_init() {
    PR2 = 0xFF;    
    CCP1CON = 0x0C;
    T2CON = 0x06;  
}

void pwm_set_duty(unsigned int duty) {
    CCPR1L = duty >> 2;
    CCP1CON = (CCP1CON & 0xCF) | ((duty & 0x03) << 4);
}

// --- LCD Display ---
void lcd_init() {
    __delay_ms(15);
    lcd_command(0x38); // 8-bit mode, 2 lines
    lcd_command(0x0C); // Display ON, Cursor OFF
    lcd_command(0x06); // Auto-increment cursor
    lcd_command(0x01); // Clear display
    __delay_ms(2);
}

void lcd_command(unsigned char cmd) {
    PORTD = cmd;
    PORTCbits.RC4 = 0; // RS = 0
    PORTCbits.RC5 = 1; // EN = 1
    __delay_ms(1);
    PORTCbits.RC5 = 0; // EN = 0
}

void lcd_data(unsigned char data) {
    PORTD = data;
    PORTCbits.RC4 = 1; // RS = 1
    PORTCbits.RC5 = 1; // EN = 1
    __delay_ms(1);
    PORTCbits.RC5 = 0; // EN = 0
}

void lcd_string(const char *str) {
    while(*str) lcd_data(*str++);
}