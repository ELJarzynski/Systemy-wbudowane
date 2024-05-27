// CONFIGURATION BITS
#pragma config POSCMOD = NONE             // Primary Oscillator Select (primary oscillator disabled)
#pragma config OSCIOFNC = OFF             // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD             // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC                // Oscillator Select (Fast RC Oscillator without Postscaler)
#pragma config IESO = OFF                 // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)
#pragma config WDTPS = PS32768            // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128              // WDT Prescaler (1:128)
#pragma config WINDIS = ON                // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF               // Watchdog Timer disabled
#pragma config ICS = PGx2                 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF                 // Writes to program memory allowed
#pragma config GCP = OFF                  // Code protection is disabled
#pragma config JTAGEN = OFF               // JTAG port is disabled

#include "xc.h"
#include <libpic30.h>
#include <stdbool.h>
#include <stdio.h>

// DEFINICJE MAKRO
#define FCY         4000000UL   // Częstotliwość pracy oscylatora
#define LCD_E       LATDbits.LATD4  
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE
#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_ON          0x0C
#define LCD_OFF         0x08
#define LCD_CONFIG      0x38
#define LCD_CURSOR      0x80
#define LINE1           0x00
#define LINE2           0x40
#define LCD_CUST_CHAR   0x40
#define LCD_SHIFT_R     0x1D

// Funckje delay
void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}

// Funkcje LCD
void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;     
    LCD_RS = 0;     
    LCD_E = 1;      
    LCD_DATA = command;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;     
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if (row == 1){
        address = LCD_CURSOR + LINE1 + col;
    }
    if(row == 2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

// Funkcje obsługi przycisków i potencjometru
unsigned int read_ADC(void){
    AD1CON1bits.SAMP = 1;
    while(!AD1CON1bits.DONE);
    return ADC1BUF0;
}

void clear_line(unsigned char row){
    LCD_setCursor(row, 0);
    for (int i = 0; i < 16; i++) { // Assuming a 16x2 LCD
        LCD_sendData(' ');
    }
}

void display_power(unsigned char power){
    char buffer[16]; // Bufor do przechowywania sformatowanego tekstu
    sprintf(buffer, "Power: %u", power);
    clear_line(1);
    LCD_setCursor(1, 0);
    LCD_print((unsigned char*)buffer);
}

void display_time(unsigned char time){
    char buffer[16]; // Bufor do przechowywania sformatowanego tekstu
    sprintf(buffer, "Time: %02u:%02u", time / 60, time % 60);
    clear_line(2);
    LCD_setCursor(2, 0);
    LCD_print((unsigned char*)buffer);
}

int main(void) {
    TRISB = 0x7FFF;     // Ustawienie rejestrów kierunku
    TRISD = 0xFFFF;
    TRISE = 0x0000;
    
    AD1CON1 = 0x80E4;   // Ustawienia ADC
    AD1CON2 = 0x0404;
    AD1CON3 = 0x0F00;
    AD1CHS = 0;
    AD1CSSL = 0x0020;
    
    LCD_init();         // Inicjalizacja wyświetlacza
    
    unsigned char power = 0;
    unsigned char time = 0;
    bool running = false;
    bool reset = false;
	
	char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0, current8 = 0, prev8 = 0; //variables for buttons
    
    while(1) {
        power = read_ADC() / 10; // Skalowanie wartości ADC do zakresu 0-255 (unsigned char)
		
		prev6 = PORTDbits.RD6;      //scanning for a change of buttons' state
        prev7 = PORTDbits.RD7;
		prev8 = PORTDbits.RD8
        __delay32(150000);
        current6 = PORTDbits.RD6;
        current7 = PORTDbits.RD7;
		current8 = PORTDbits.RD8;
        
        if(current6 - prev6 == 1){  // Przycisk dodawania czasu
            if (time <= 245) { // Zapobiega przekroczeniu maksymalnej wartości unsigned char
                time += 10;
            }
        }
        
        if(current7 - prev7 == 1){  // Przycisk start/stop
            running = !running;
			__delay32(100);
        }
        
        if(current8 - prev8 == 1){  // Przycisk reset
            power = 0;
            time = 0;
            running = false;
        }
        
        if(running && time > 0){
            __delay_ms(1000); // Odliczanie w dół co sekundę
            time--;
        }
        
        display_power(power);
        display_time(time);
    }
    
    return 0;
}
