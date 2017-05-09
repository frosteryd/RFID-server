 * ll the resources for this project: https://www.hackster.io/Aritro
 * Modified by Aritro Mukherjee
 * 
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

int on = 1; // This variable is for testing. If set to 0 the main loop will do nothing which makes it safe to rewire the board.

int mode = 0;
int busyState = 0; // This variable is 1 if the board is working and wont take new tasks

// Temporary LEDs instead of the 7-Sed-Display
int o1 = 2;
int o2 = 3;
int o3 = 4;

int busyLed = 6; // This LED is turned on and kept on when the board is working and is unable to recieve further tasks.
int green_led = 7; // This signals authorized access
int red_led = 8; // This signals unautorized access

int mode_button = 5; // The button which is used to switch modes

void setup() 
{
  // These 3 are only temporary
  pinMode(o1, OUTPUT);
  pinMode(o2, OUTPUT);
  pinMode(03, OUTPUT);
  
  // The main LEDs of the EMCU, all set as outputs
  pinMode(busyLed, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);

  // The button set as input
  pinMode(mode_button, INPUT);

  // Setting up serial connection
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop()
{
  if(on == 1 && busyState == 0) { // Used for testing, read variable description
    /* Check if the button is pressed and change mode */
    if(digitalRead(mode_button) == LOW) {
      busy(1); //
      delay(100);
      mode++;
      delay(100);
      busy(0);
    }

    /* Perform action based on what mode is set */
    if(mode == 0) { // Mode 0 is default and i looking for a card to be swiped
      // Write current mode to the display
      segDisp(0);
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
      busy(1);
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
        delay(100);
      }

      else   {
        Serial.println(" Access denied");
        ledBlink(red_led, 1000);
        delay(100);
      }
      busy(0);
    } 
    else if(mode == 1) { // Mode 1 is for adding new cards to the list of auth users
      // Set disp to current mode
      segDisp(1);
    } 
    else if(mode == 2) { // Mode 2 is for removing access for cards
      // Set display to current mode
      segDisp(2);
    } 
    else { // If the mode doesn't match any exsiting it resets to 0
      mode = 0;
      // Set display to current mode
      segDisp(0);
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

void segDisp(int nr) {
  if(nr == 0) {
    ledOff(o1);
    ledOff(o2);
    ledOff(o3);
  } else if(nr == 1) {
    ledOn(o1);
    ledOff(o2);
    ledOff(o3);
  } else if(nr == 2) {
    ledOn(o1);
    ledOn(o2);
    ledOff(o3);
  } else {
    ledOff(o1);
    ledOff(o2);
    ledOff(o3);
  }
}

void busy(int working) {
  if(working == 1) {
    ledOn(busyLed);
    busyState = 1;
  } else {
    ledOff(busyLed);
    busyState = 0;
  }
}


