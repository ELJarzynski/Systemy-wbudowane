// KONFIGURACJA BITÓW
#pragma config POSCMOD = NONE             // Wybór głównego oscylatora (główny oscylator wyłączony)
#pragma config OSCIOFNC = OFF             // Funkcja wyjścia głównego oscylatora (OSC2/CLKO/RC15 działa jako CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD             // Przełączanie zegara i monitor (Przełączanie zegara i monitor awarii zegara są wyłączone)
#pragma config FNOSC = FRC                // Wybór oscylatora (Szybki oscylator RC bez postscalera)
#pragma config IESO = OFF                 // Tryb przełączania wewnętrzno-zewnętrznego (Tryb IESO (dwustopniowe uruchamianie) wyłączony)
#pragma config WDTPS = PS32768            // Postscaler timera Watchdog (1:32,768)
#pragma config FWPSA = PR128              // Preskaler WDT (1:128)
#pragma config WINDIS = ON                // Tryb okna timera Watchdog wyłączony
#pragma config FWDTEN = OFF               // Timer Watchdog wyłączony
#pragma config ICS = PGx2                 // Emulator/debugger używa EMUC2/EMUD2
#pragma config GWRP = OFF                 // Zapisy do pamięci programu dozwolone
#pragma config GCP = OFF                  // Ochrona kodu wyłączona
#pragma config JTAGEN = OFF               // Port JTAG wyłączony

#include "xc.h"
#include <libpic30.h>
#include <stdbool.h>

// DEFINICJE MAKRO
#define FCY         4000000UL   // Częstotliwość pracy oscylatora
#define LCD_ENABLE  LATDbits.LATD4  
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE
#define LCD_CMD_CLEAR   0x01
#define LCD_CMD_HOME    0x02
#define LCD_CMD_ON      0x0C
#define LCD_CMD_OFF     0x08
#define LCD_CMD_CONFIG  0x38
#define LCD_CMD_CURSOR  0x80
#define LINIA_PIERWSZA  0x00
#define LINIA_DRUGA     0x40
#define LCD_CUSTOM_CHAR 0x40
#define LCD_SHIFT_RIGHT 0x1D

// Deklaracje funkcji
void delay_microseconds(unsigned long us);
void delay_milliseconds(unsigned long ms);
void LCD_sendCmd(unsigned char cmd);
void LCD_sendDataByte(unsigned char data);
void LCD_displayString(unsigned char* string);
void LCD_setPosition(unsigned char row, unsigned char col);
void LCD_initialize();
unsigned int readAnalog();
void showPower(unsigned int power);
void showTime(unsigned int time);

// Funkcja opóźnienia w mikrosekundach
void delay_microseconds(unsigned long us){
    __delay32(us * FCY / 1000000);
}

// Funkcja opóźnienia w milisekundach
void delay_milliseconds(unsigned long ms){
    __delay32(ms * FCY / 1000);
}

// Funkcje LCD
void LCD_sendCmd(unsigned char cmd){
    LCD_RW = 0;     
    LCD_RS = 0;     
    LCD_ENABLE = 1;      
    LCD_DATA = cmd;
    delay_microseconds(50);
    LCD_ENABLE = 0;
}

void LCD_sendDataByte(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;     
    LCD_ENABLE = 1;
    LCD_DATA = data;
    delay_microseconds(50);
    LCD_ENABLE = 0;
}

void LCD_displayString(unsigned char* string){
    while(*string){
        LCD_sendDataByte(*string++);
    }
}

void LCD_setPosition(unsigned char row, unsigned char col){
    unsigned char address;
    if (row == 1){
        address = LCD_CMD_CURSOR + LINIA_PIERWSZA + col;
    }
    if(row == 2){
        address = LCD_CMD_CURSOR + LINIA_DRUGA + col;
    }
    LCD_sendCmd(address);
}

void LCD_initialize(){
    delay_milliseconds(20);
    LCD_sendCmd(LCD_CMD_CONFIG);
    delay_microseconds(50);
    LCD_sendCmd(LCD_CMD_ON);
    delay_microseconds(50);
    LCD_sendCmd(LCD_CMD_CLEAR);
    delay_milliseconds(2);
}

// Funkcje ADC
unsigned int readAnalog(void){
    AD1CON1bits.SAMP = 1;
    while(!AD1CON1bits.DONE);
    return ADC1BUF0;
}

void showPower(unsigned int power){
    LCD_setPosition(1, 0);
    LCD_displayString("Power: ");
    LCD_sendDataByte('0' + power / 100);
    LCD_sendDataByte('0' + (power % 100) / 10);
    LCD_sendDataByte('0' + power % 10);
}

void showTime(unsigned int time){
    LCD_setPosition(2, 0);
    LCD_displayString("Time: ");
    LCD_sendDataByte('0' + time / 60);
    LCD_sendDataByte(':');
    LCD_sendDataByte('0' + (time % 60) / 10);
    LCD_sendDataByte('0' + time % 10);
}

int main(void) {
    TRISB = 0x7FFF;     // Ustawienie rejestrów kierunku
    TRISD = 0xFFE7;
    TRISE = 0x0000;
    TRISA = 0xFFFF;
    
    AD1CON1 = 0x80E4;   // Ustawienia ADC
    AD1CON2 = 0x0404;
    AD1CON3 = 0x0F00;
    AD1CHS = 0;
    AD1CSSL = 0x0020;
    
    LCD_initialize();   // Inicjalizacja wyświetlacza
    
    unsigned int power = 0;
    unsigned int time = 0;
    bool running = false;
    
    // Flagi przycisków
    unsigned char buttonState6 = 0, prevButtonState6 = 0;
    unsigned char buttonState7 = 0, prevButtonState7 = 0;
    unsigned char buttonStateA7 = 0, prevButtonStateA7 = 0;
    
    unsigned int loopCounter = 0;
    const unsigned int loopsPerSecond = 100; // Liczba pętli na około jedną sekundę

    while(1) {
        power = readAnalog() / 10; // Skalowanie wartości ADC do zakresu 0-100
        
        buttonState6 = PORTDbits.RD6;
        buttonState7 = PORTDbits.RD7;
        buttonStateA7 = PORTAbits.RA7;
        
        // Sprawdzanie stanu przycisków i odpowiednie działania
        if(buttonState6 && !prevButtonState6){  // Przycisk dodawania czasu
            time += 10;
        }
       
        if(buttonState7 && !prevButtonState7){  // Przycisk start/stop
            running = !running;
        }
        
        if(buttonStateA7 && !prevButtonStateA7){  // Przycisk resetu
            power = 0;
            time = 0;
            running = false;
        }
        
        // Aktualizacja poprzednich stanów przycisków
        prevButtonState6 = buttonState6;      
        prevButtonState7 = buttonState7;
        prevButtonStateA7 = buttonStateA7;
        
        // Aktualizacja wyświetlacza
        showPower(power);
        showTime(time);
        
        // Obsługa stanu pracy
        if(running && time > 0 && power > 0){
            delay_milliseconds(10); // Sprawdzanie stanu przycisków co 10ms
            loopCounter++;
            if(loopCounter >= loopsPerSecond){
                loopCounter = 0;
                time--;
            }
        } else {
            delay_milliseconds(10); // Sprawdzanie stanu przycisków co 10ms
        }
        
        // Zatrzymanie, jeśli moc jest zerowa lub czas się skończył
        if(power == 0 || time == 0){
            running = false;
        }
    }
    
    return 0;
}
