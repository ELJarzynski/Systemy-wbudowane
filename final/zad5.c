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

// Funckje delay
void __delay_us(unsigned long us){
    __delay32(us * FCY / 1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms * FCY / 1000);
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
    if (row == 2){
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

void display_time(unsigned int time, int cursor){
    LCD_setCursor(cursor, 0);
    LCD_print("Time: ");
    LCD_sendData('0' + time / 60);
    LCD_sendData(':');
    LCD_sendData('0' + (time % 60) / 10);
    LCD_sendData('0' + time % 10);
}


int main(void) {
    TRISB = 0x7FFF;     // Ustawienie rejestrow kierunku
    TRISD = 0xFFE7;
    TRISE = 0x0000;
    TRISA = 0x0000;

    LCD_init();         // Inicjalizacja wyświetlacza

    unsigned int time_player1 = 300; // 5 minut w sekundach
    unsigned int time_player2 = 300; // 5 minut w sekundach
    bool player1_turn = true; // flagi na tury
    bool player2_turn = false;
    bool game_over = false; // flaga konca gry
	
    while(1) {
        if(game_over) {
            LCD_setCursor(1, 0);
            LCD_print("Game Over.");
            while(1); // Zatrzymaj program
        }

        if(PORTDbits.RD6==0){  // Przycisk gracza 1
            player1_turn = false;
            player2_turn = true;
            
        }

        if(PORTDbits.RD7==0){  // Przycisk gracza 2
            player2_turn = false;
            player1_turn = true;
        }

        if(player1_turn) {
            __delay_ms(1000); // Odliczanie co sekundę
            time_player1--;
        }

        if(player2_turn) {
            __delay_ms(1000); // Odliczanie co sekundę
            time_player2--;
        }

        display_time(time_player1,1);
        display_time(time_player2,2);
        
        // Koniec czasu
        if(time_player1 == 0) {
            LCD_setCursor(2, 0);
            LCD_print("Player 1 loses");
            game_over = true;
        }

        if(time_player2 == 0) {
            LCD_setCursor(2, 0);
            LCD_print("Player 2 loses");
            game_over = true;
        }
    }

    return 0;
}
