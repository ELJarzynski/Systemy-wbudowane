/*
 * File:   newmainXC16.c
 * Author: local
 *
 * Created on March 20, 2024, 1:50 PM
 */

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include "xc.h"
#include <libpic30.h>

int number = 0; // zmienna globalna zwiazana z numerem programu

//Licznik binarny w gore
void BinaryCountUp()
{
    unsigned char portValue = 0;
    TRISA = 0x0000; // set all port bits to be output
    TRISD = 0xFFFF;

    while(1) 
    {
        LATA = portValue;
       __delay32(4000000);
       portValue += 1;
       
       // obsluga przyciskow       
       if(PORTDbits.RD6 == 0) 
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
    }
}

// Licznik binarny w dol
void BinaryCountDown()
{
    unsigned char portValue = 255;
    TRISA = 0x0000; // set all port bits to be output
    TRISD = 0xFFFF;

    while(1) 
   {
        LATA = portValue;
        __delay32(4000000);
        portValue -= 1;
        
               
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
    }
}

// Licznik w kodzie gray w gore
void GrayCountUp()
{
    unsigned char portValue = 0;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    
    while(1)
    {
        LATA = (portValue >> 1) ^ portValue;
        __delay32(4000000);
        portValue += 1;
        
               
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
    }
}

// Licznik w kodzie gray w dol
void GrayCountDown()
{
    unsigned char portValue = 0;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    
    while(1)
    {
        LATA = (portValue >> 1) ^ portValue;
        __delay32(4000000);
        portValue -= 1;
        
               
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
    }
    
}

// Licznik w kodzie BCD w gore
void BCDCountUp()
{
    unsigned char portValue = 0;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    
    while(1)
    {
        LATA = (((portValue/10) << 4) | (portValue % 10));
        __delay32(4000000);
        portValue += 1;
        
               
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
        

        
    }
}


// Licznik w kodzie BCD w dol
void BCDCountDown()
{
    unsigned char portValue = 99;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    
    while(1)
    {
        LATA = (((portValue/10) << 4) | (portValue % 10));
        __delay32(4000000);
        portValue -= 1;
        
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
        
    }
}

// Wonsz
void snake()
{
    unsigned char portValue[10] = {7,14,28,56,112,224,112,56,28,14}; // tablica z wartosciami
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    unsigned int i = 0; //index
    
    while(1)
    {
        LATA = portValue[i];
        __delay32(4000000);
        i++;
       
       if(i==10)
       {
           i = 0; // powrot na poczatek tablicy
       }
        
               
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
    }
}

void queue()
{
    unsigned char portValue = 0;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
	
	//flagi na buttony
    int break_button1 = 0;
    int break_button2 = 0;
    
	while(1)
	{
		for(int i=0; i<8; i++)
		{
			int temp = 1;
			for(int j=i+1; j<8; j++)
			{
				LATA = portValue + temp;
				temp = temp << 1;    
				__delay32(4000000);
            
                   
				if(PORTDbits.RD6 == 0)
				{
					break_button1 = 1;
					break;
				}
        
				if(PORTDbits.RD7 == 0)
				{
					break_button2 = 1;
					break;
				}
			}
			portValue += temp;
			
			// wyjscie z petli for
			if(break_button1 == 1)
			{
				break;
			}
        
			if(break_button2 == 1)
			{
				break;
			}
		}
		
		//wyjscie z glownej petli, zmiana programu
		if(break_button1 == 1)
		{
			number -= 1;
			break;
		}
		
		if(break_button2 == 1)
		{
			number += 1;
			break;
		}	
	}   
}

//Generator
void generator()
{
    unsigned char portValue;
    unsigned char seed = 0b1110011;
    TRISA = 0x0000;
    TRISD = 0xFFFF;
    
    while(seed != 0)
    {
        portValue = (seed >> 0) ^ (seed >> 1) ^ (seed >> 4) ^ (seed >> 5) ^ (seed >> 6);
        seed = seed >> 1 | seed << 6;
        LATA = portValue;
        __delay32(4000000);
        
               
       if(PORTDbits.RD6 == 0)
       {
           number -= 1;
           break;
       }
        
       if(PORTDbits.RD7 == 0)
       {
           number += 1;
           break;
       }
    }
}


int main(void) 
{
	//obsluga zmiany programow
    if(number < 0)
    {
        number = 0;
    }
    
    if(number == 0)
    {
        BinaryCountUp();
    }
    
    if(number == 1)
    {
        BinaryCountDown();
    }
    
    if(number == 2)
    {
        GrayCountUp();
    }
    
    if(number == 3)
    {
        GrayCountDown();
    }
    
    if(number == 4)
    {
        BCDCountUp();
    }
    
    if(number == 5)
    {
        BCDCountDown();
    }
    
    if(number == 6)
    {
       snake();
    }
    
    if(number == 7)
    {
        queue();
    }
    
    if(number == 8)
    {
        generator();
    }
    
    if(number > 8)
    {
        number = 0; //powrot do pierwszego programu
    }
    
}