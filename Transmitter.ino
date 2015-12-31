// Transmitter Code for Project
// Modified by: Ayotunde Odejayi (Nov. 2015)
//
// This code makes use of Transmitter libraries using VirtualWire to send and receive messages
// Implements a simplex (one-way) transmitter with an TX-C1 module
// Libraries by Author: Mike McCauley (mikem@airspayce.com
//
// Dependencies:
// VirtualWire library
//
// Description:
// Detects motion with an infrared sensor and then activates a gas sensor to start reading BAC levels.
// Some BAC levels have corresponding actions and for a certain range, transmission is made to the receiver unit/station.




#include "VirtualWire.h"

// Initialize pins
int pir_sensor = 2;
int gas_sensor = 5;
int ledGOpin = 12;
int ledSTOPpin = 4;
int safety_guard1 = 25;
int safety_guard2 = 75;
int prev_gas_value, prev_pir_value;


// RGB led pins and variables (digital pins)
int datapin  = 11; // DI
int latchpin = 9; // LI
int enablepin = 7; // EI
int clockpin = 5; // CI
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

void setup()
{
  // Set pinModes
  pinMode(ledGOpin, OUTPUT);
  pinMode(ledSTOPpin, OUTPUT);
  pinMode(pir_sensor, INPUT);
  pinMode(gas_sensor, INPUT);

  // RGB pinModes
  pinMode(datapin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(enablepin, OUTPUT);
  pinMode(clockpin, OUTPUT);

  digitalWrite(latchpin, LOW);
  digitalWrite(enablepin, LOW);
  Serial.begin(9600);       	      // Debugging only
  Serial.println("Setting up TX..."); // Prints "Setup to the serial monitor"
  Serial.println("-------------");
  
  vw_set_tx_pin(4);                  // Sets pin D4 as the TX pin
  vw_set_ptt_inverted(true);         // Required for DR3100
  vw_setup(4000);	             // Bits per sec
}

void SB_SendPacket()
{
  SB_CommandPacket = SB_CommandMode & B11;
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);

  shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
  shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
  shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
  shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

  delay(1); // adjustment may be necessary depending on chain length
  digitalWrite(latchpin, HIGH); // latch data into registers
  delay(1); // adjustment may be necessary depending on chain length
  digitalWrite(latchpin, LOW);
}

void loop()
{
    prev_gas_value = analogRead(gas_sensor);
    Serial.println("Gas sensor:");
    Serial.println(analogRead(gas_sensor));
    Serial.println("Pir sensor:");
    Serial.println(analogRead(pir_sensor));
    Serial.println(" ");

   // RGB led showing BAC level green before Person is detected
   SB_CommandMode = B00; // Write to PWM control registers
   SB_RedCommand = 0; // Min. red
   SB_GreenCommand = 127; // Max. green
   SB_BlueCommand = 0; // Min. blue
   SB_SendPacket();
   
    if (analogRead(pir_sensor) > 700) // If motion is detected (Person is within range)
   {
    Serial.println("Motion Detected!"); Serial.println(" ");
    digitalWrite(ledGOpin, HIGH);
    delay(1000);         // Gives a 3 second period to detect gas after motion
    digitalWrite(ledGOpin, LOW);
    
    if (analogRead(gas_sensor) > 200 && analogRead(gas_sensor) < 300)   // Caution! Alcohol
      {
        // RGB led showing BAC level (Yellow)
        SB_CommandMode = B00; // Write to PWM control registers
        SB_RedCommand = 127; // Min. red
        SB_GreenCommand = 127; // Min. green
        SB_BlueCommand = 0; // Max. blue
        SB_SendPacket();
        
        digitalWrite(ledSTOPpin, HIGH);
        digitalWrite(ledGOpin, HIGH);
        delay(1500);
        digitalWrite(ledGOpin, HIGH);
        digitalWrite(ledSTOPpin, LOW);
        delay(1500);
        digitalWrite(ledGOpin, HIGH);
        digitalWrite(ledSTOPpin, HIGH);
        delay(1500);
        digitalWrite(ledGOpin, LOW);
        digitalWrite(ledSTOPpin, LOW);
        prev_gas_value = analogRead(gas_sensor);
    }
  
     else if (analogRead(gas_sensor) > 300)     // Car Locked! Nail Polish
      {
        
        const char *msg = "M";          // Message to be sent
        Serial.println("Tx Packet:"); 
        Serial.println(*msg);
        Serial.println(" ");
        digitalWrite(13, true);         // Flash a light to show transmitting
        vw_send((uint8_t *)msg, strlen(msg)); //Sending the message
        vw_wait_tx();                   // Wait until the whole message is gone
        digitalWrite(13, false);        // Turn the LED off.
        
       // RGB led showing BAC level (Red)
       SB_CommandMode = B00; // Write to PWM control registers
       SB_RedCommand = 127; // Max. red
       SB_GreenCommand = 0; // Min. green
       SB_BlueCommand = 0; // Min. blue
       SB_SendPacket();
  
       digitalWrite(ledSTOPpin, HIGH);
       digitalWrite(ledGOpin, LOW);
       delay(5000);
       digitalWrite(ledSTOPpin, LOW);
       prev_gas_value = analogRead(gas_sensor);
      }
      else
      {
         // RGB led showing BAC level
         SB_CommandMode = B00; // Write to PWM control registers
         SB_RedCommand = 0; // Min. red
         SB_GreenCommand = 127; // Max. green
         SB_BlueCommand = 0; // Min. blue
         SB_SendPacket();
  
         digitalWrite(ledGOpin, HIGH);
         digitalWrite(ledSTOPpin, LOW);
         delay(1000);
         digitalWrite(ledGOpin, LOW);
         prev_gas_value = analogRead(gas_sensor);
         
      }
   }

} 
