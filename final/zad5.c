#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = OFF
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRC
#pragma config IESO = OFF
#pragma config WDTPS = PS32768
#pragma config FWPSA = PR128
#pragma config WINDIS = ON
#pragma config FWDTEN = OFF
#pragma config ICS = PGx2
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF

#include "xc.h"
#include <libpic30.h>
#include <stdbool.h>

#define FCY         4000000UL
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

void delay_microseconds(unsigned long us){
    __delay32(us * FCY / 1000000);
}

void delay_milliseconds(unsigned long ms){
    __delay32(ms * FCY / 1000);
}

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

void showTime(unsigned int time, unsigned char row){
    unsigned char minutes = time / 60;
    unsigned char seconds = time % 60;

    LCD_setPosition(row, 0);
    if (row == 1) {
        LCD_displayString("Player 1: ");
    } else {
        LCD_displayString("Player 2: ");
    }
    LCD_sendDataByte('0' + minutes / 10);
    LCD_sendDataByte('0' + minutes % 10);
    LCD_sendDataByte(':');
    LCD_sendDataByte('0' + seconds / 10);
    LCD_sendDataByte('0' + seconds % 10);
}

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0xFFE7;
    TRISE = 0x0000;
    TRISA = 0xFFFF;
    
    LCD_initialize();
    
    unsigned int timePlayer1 = 3;
    unsigned int timePlayer2 = 300;
    bool activePlayer1 = true;
    bool running = false;
    
    unsigned char buttonState6 = 0, prevButtonState6 = 0;
    unsigned char buttonState7 = 0, prevButtonState7 = 0;
    
    unsigned int loopCounter = 0;
    const unsigned int loopsPerSecond = 100 / 1.3;

    while(1) {
        buttonState6 = PORTDbits.RD6;
        buttonState7 = PORTDbits.RD7;
        
        if(buttonState6 && !prevButtonState6){
            activePlayer1 = !activePlayer1;
            running = true;
        }
        
        if(buttonState7 && !prevButtonState7){
            timePlayer1 = 300;
            timePlayer2 = 300;
            running = false;
        }
        
        prevButtonState6 = buttonState6;      
        prevButtonState7 = buttonState7;
        
        showTime(timePlayer1, 1);
        showTime(timePlayer2, 2);
        
        if(running){
            delay_milliseconds(10);
            loopCounter++;
            if(loopCounter >= loopsPerSecond){
                loopCounter = 0;
                if(activePlayer1){
                    if(timePlayer1 > 0) timePlayer1--;
                } else {
                    if(timePlayer2 > 0) timePlayer2--;
                }
            }
        } else {
            delay_milliseconds(10);
        }
        
        if(timePlayer1 == 0 || timePlayer2 == 0){
            running = false;
        }
    }
    
    return 0;
}
