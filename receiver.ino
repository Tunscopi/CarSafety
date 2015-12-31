// receiver.ino
// Modified by: Ayotunde Odejayi (Nov. 2015)
//
// See transmitter code for credits for VirtualWire libraries and pure receiving module
//
// Dependencies:
// VirtualWire libraries
// rgb lcd libraries
// 
// Description:
// Implements receiver code and controls an rgb lcd screen
// When the information received is a coherent form of what is expected from the transmitter
// This script lights an LED (for alert) turns on a buzzer and displays corresponding messages to an LCD screen


#include <Wire.h>

#include "VirtualWire.h"
#include "rgb_lcd.h"
int led_pin = 13;

// Create lcd object & set color values
rgb_lcd lcd;           
 int colorR = 255;   
 int colorG = 255;
 int colorB = 0;

// Create shaped character
byte armsUp[8] = {
    0b00100,
    0b01010,
    0b00100,
    0b10101,
    0b01110,
    0b00100,
    0b00100,
    0b01010
};

// Map output pin, J4 to Buzzer
#define J4 8   

void setup()
{
    pinMode(led_pin, OUTPUT);
    pinMode(J4, OUTPUT);
    Serial.begin(9600);	// Debugging only
    Serial.println("Setting up RX..."); //Prints "Setup" to the serial monitor
    Serial.println("-------------"); Serial.println("-------------");
    vw_set_rx_pin(7);       //Sets pin D12 as the RX Pin
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(4000);	     // Bits per sec
    vw_rx_start();           // Start the receiver PLL running  
    
    // Set up the LCD's number of columns and rows and colors
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    
    #if 1 
    lcd.createChar(4, armsUp);
    #endif
}

void loop()
{
    
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i;
        digitalWrite(13, true);  // Flash LED to show received coherent message
        delay(1000);
        digitalWrite(13, false);
        
	// Message with a good checksum received, dump it.
	Serial.println("RX Packet: ");
	for (i = 0; i < buflen; i++)
	{
            char c = (buf[i]);
            Serial.println(c);
	}
          
        digitalWrite(J4, HIGH);
        delay(1000);
        digitalWrite(J4, LOW);
        
        // Print a message to the LCD.
        for (i = 0; i < 4; i++)
        {
          lcd.print(" Drunk Driver"); delay(1000); lcd.clear();
          lcd.print("   Detected!  "); delay(1000); lcd.clear();
          lcd.setCursor(10, 6); lcd.clear();
          lcd.write(4);   // Draw warning sign (stick man with arms up
          lcd.blink();
          delay(1000);
          lcd.clear();
          lcd.noBlink();
        }
        
	Serial.println("");
        digitalWrite(13, false);
    }
}
