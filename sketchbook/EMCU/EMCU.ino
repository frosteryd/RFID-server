/*
 * 
 * All the resources for this project: https://www.hackster.io/Aritro
 * Modified by Aritro Mukherjee
 * 
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

int working = 0;  
int mode = 0;

int green_led = 3;
int red_led = 4;
int blue_led = 5;

int mode_button = 6;

int seg_bit_0 = 7;
int seg_bit_1 = 8;

void setup() 
{
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(blue_led, OUTPUT);

  pinMode(mode_button, INPUT);
  
  pinMode(seg_bit_0, OUTPUT);
  pinMode(seg_bit_1, OUTPUT);

  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop()
{
  if(working == 0) { // Might be used to prevent mulitple things running at the same time
    /* Check if the button is pressed and change mode */
    if(digitalRead(mode_button) == LOW) {
      setBusy(1);
      mode++;
      delay(100);
      Serial.print("Changed mode to " + (int)mode);
      delay(100);
      setBusy(0);
    }

    /* Perform action based on what mode is set */
    if(mode == 0) { // Mode 0 is default and i looking for a card to be swiped
      segDisplay(0);
      // Look for new cards
      if ( ! mfrc522.PICC_IsNewCardPresent()) 
      {
        return;
      }
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) 
      {
        return;
      }
      setBusy(1);
      //Show UID on serial monitor
      Serial.print("UID tag :");
      String content= "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      Serial.println();
      Serial.print("Message : ");
      content.toUpperCase();
      if (content.substring(1) == "EE A3 41 D5") //change here the UID of the card/cards that you want to give access
      {
        Serial.println("Authorized access");
        Serial.println();
        ledBlink(green_led, 1000);
        delay(3000);
      }

      else   {
        Serial.println(" Access denied");
        ledBlink(red_led, 1000);
        delay(3000);
      }
      
      setBusy(0);
      
    } 
    else if(mode == 1) { // Mode 1 is for adding new cards to the list of auth users
      segDisplay(1);

    } 
    else if(mode == 2) { // Mode 2 is for removing access for cards
      segDisplay(2);

    } 
    else { // If the mode doesn't match any exsiting it resets to 0
      mode = 0;
    }
  }
}

void ledOn(int led_nr) {
  digitalWrite(led_nr, HIGH);
}

void ledOff(int led_nr) {
  digitalWrite(led_nr, LOW);
}

void ledBlink(int led_nr, int delay_time) {
  digitalWrite(led_nr, HIGH);
  delay(delay_time);
  digitalWrite(led_nr, LOW);
}

void segDisplay(int nr) {
  if(nr == 1) {
    ledOn(seg_bit_0);
    ledOff(seg_bit_1);
  } else if(nr == 2) {
    ledOn(seg_bit_0);
    ledOn(seg_bit_1);
  } else {
    ledOff(seg_bit_0);
    ledOff(seg_bit_1);
  }
}

void setBusy(int busy) {
  if(busy == 1) {
    working = 1;
    ledOn(blue_led);
  } else {
    working = 0;
    ledOff(blue_led);
  }
}
