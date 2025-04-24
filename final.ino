#include <SPI.h>
#include <MFRC522.h>
#include <NTPClient.h>
#include "WiFiEsp.h"
#include "ThingSpeak.h"
#include "WiFiEspUdp.h"
#include <WiFiEspClient.h>
#define SS_PIN 53
#define RST_PIN 5
#define ESP_BAUDRATE 115200

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

int card1Count = 0;
int card2Count = 0;
int card1LEDPin = 11; // Replace with the actual pin number for the card 1 LED
int card2LEDPin = 10;
int card3LEDPin = 9;// Replace with the actual pin number for the card 2 LED
int buzz= 12;

int inputPin = 6;               // choose the input pin (for PIR sensor)
int val = 0;  

const int DOOR_SENSOR_PIN = 13;  // Arduino pin connected to door sensor's pin
int doorState; 

char ssid[] = "KiT"; // your network SSID (name) //EE3070_P1404_1
char pass[] = "aA051400"; // your network password  //EE3070P1404
int status = WL_IDLE_STATUS; // the Wifi radio's status

unsigned long myChannelNumber = "2301676";
const char * myWriteAPIKey = "56NRO960HAIX22K1";
int number = 0;
WiFiEspClient  client;  ///create a wifi client
WiFiEspClient client1;
char HOST_NAME[] = "223.122.106.124";
String time;
int    HTTP_PORT   = 80;
String cardID;

WiFiEspUDP  ntpUDP;
// initialized to a time offset of 10 hours
NTPClient timeClient(ntpUDP,"pool.ntp.org",28800,999999); //pool.ntp.org
int hour;

String data;

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}


bool verifyCardID(String cardID) {
  if (client1.connect(HOST_NAME, HTTP_PORT)) {
    Serial.println("Connected to server");
    
    // Make HTTP request to the PHP script
    client1.print("GET /verify_card.php?cardID=" + cardID + " HTTP/1.1\r\n");
    client1.print("Host: " + String(HOST_NAME) + "\r\n");
    client1.print("Connection: close\r\n\r\n");

    while(!client.read()){}
     String response;
        char c = client.read();
        response += c;
        Serial.print("Received response: ");
        Serial.println(response);

    while (client1.connected()) {
      if (client1.available()) {

        String line = client1.readStringUntil('\n');
        if (line.indexOf("Access granted") != -1) {
          client1.stop();
          return true;
        }
      }
    }

    client1.stop();
    return false;
  } else {
    Serial.println("Connection failed");
    return false;
  }
}




void setup() { 
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  pinMode(card1LEDPin, OUTPUT);
  pinMode(card2LEDPin, OUTPUT);
  pinMode(card3LEDPin, OUTPUT);
  pinMode(buzz, OUTPUT);
  
  pinMode(inputPin, INPUT);

  //------------------------WIFI part------------ ------------
    Serial1.begin(ESP_BAUDRATE);    // initialize serial for debugging
    WiFi.init(&Serial1);  // initialize ESP module
    
    ThingSpeak.begin(client); // Initialize ThingSpeak
    
    
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network
        Serial.print(".");
        delay(5000);
      }
      Serial.println("\nConnected");
    } 

}


 
void loop() {
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  hour = timeClient.getHours();
  delay(1000);  
  
  if(hour>=6 || hour<=22){
    
      // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
      if (!rfid.PICC_IsNewCardPresent())
        return;
    
      // Verify if the NUID has been read
      if (!rfid.PICC_ReadCardSerial())
        return;
    
      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.println(rfid.PICC_GetTypeName(piccType));
    
      // Check if the PICC is of Classic MIFARE type
      if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
          piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
          piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
      }
    

         // if (rfid.PICC_IsNewCardPresent()) {
            // Read card ID
            cardID = "";
            for (byte i = 0; i < 4; i++) {
              cardID += String(rfid.uid.uidByte[i]);
            }
            // Send card ID to server for verification
            if (verifyCardID(cardID)) {
              Serial.println("Access granted!");
                        digitalWrite(card1LEDPin, LOW); // Turn off card 1 LED
          digitalWrite(card2LEDPin, HIGH);
          digitalWrite(card3LEDPin, LOW);// Turn on card 2 LED
          Serial.println(F("The NUID tag is:"));
          Serial.print(F("In hex: "));
          printHex(rfid.uid.uidByte, rfid.uid.size);
          Serial.println();
          Serial.print(F("In dec: "));
          printDec(rfid.uid.uidByte, rfid.uid.size);
          Serial.println();
          card2Count++;
          Serial.print(F("Card 2 count: "));
          Serial.println(card2Count);
     
          delay(1000);
          timeClient.update();
          Serial.println(timeClient.getFormattedTime());
          delay(1000);

          time = timeClient.getFormattedTime();
          cardID="";
          cardID += rfid.uid.uidByte[0];
          cardID += rfid.uid.uidByte[1];
          cardID += rfid.uid.uidByte[2];
          cardID += rfid.uid.uidByte[3];


            if(client1.connect(HOST_NAME, HTTP_PORT)) {
                // if connected:
                Serial.println("Connected to server");
                // make a HTTP request:
                // send HTTP header
                String httpRequest = "GET /insert_record.php?time=" + String(time) + "&cardID=" + String(cardID) + "&authorized=" + String("Yes") + " HTTP/1.1";
                client1.println(httpRequest);
                client1.println("Host: " + String(HOST_NAME));
                client1.println("Connection: close");
                client1.println(); // end HTTP header

                while(client1.connected()) {
                  if(client1.available()){
                    // read an incoming byte from the server and print it to serial monitor:
                    char c = client1.read();
                    Serial.print(c);
                  }
                }

                // the server's disconnected, stop the client:
                client1.stop();
                Serial.println();
                Serial.println("disconnected");
              } else {// if not connected:
                Serial.println("connection failed");
              }
            } else {
              Serial.println("Access denied!");
                          digitalWrite(card1LEDPin, HIGH); // Turn on card 1 LED
            digitalWrite(card2LEDPin, LOW);
            digitalWrite(card3LEDPin, LOW);// Turn off card 2 LED
                Serial.println(F("The NUID tag is:"));
          Serial.print(F("In hex: "));
          printHex(rfid.uid.uidByte, rfid.uid.size);
          Serial.println();
          Serial.print(F("In dec: "));
          printDec(rfid.uid.uidByte, rfid.uid.size);
          Serial.println();
          card1Count++;
            Serial.print(F("Card 1 count: "));
          Serial.println(card1Count);
      
          for(int i = 0 ; i<1; i++){
          analogWrite(buzz, 255); 
          delay(500);
          analogWrite(buzz, 3000); 
            delay(500);
          
          }
          analogWrite(buzz, 0);
          delay(2);
              // Perform actions for denied access
              timeClient.update();
              time = timeClient.getFormattedTime();
              if(client1.connect(HOST_NAME, HTTP_PORT)) {
                // if connected:
                Serial.println("Connected to server");
                // make a HTTP request:
                // send HTTP header
                String httpRequest = "GET /insert_record.php?time=" + String(time) + "&cardID=" + String(cardID) + "&authorized=" + String("No") + " HTTP/1.1";
                client1.println(httpRequest);
                client1.println("Host: " + String(HOST_NAME));
                client1.println("Connection: close");
                client1.println(); // end HTTP header

                while(client1.connected()) {
                  if(client1.available()){
                    // read an incoming byte from the server and print it to serial monitor:
                    char c = client1.read();
                    Serial.print(c);
                  }
                }

                // the server's disconnected, stop the client:
                client1.stop();
                Serial.println();
                Serial.println("disconnected");
              } else {// if not connected:
                Serial.println("connection failed");
              }
            }

            delay(2000); // Delay before next scan
          //}
        
    
      // Halt PICC
      rfid.PICC_HaltA();
    
      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();
    
    
    if (!rfid.PICC_IsNewCardPresent()) {
      delay(500);
      digitalWrite(card1LEDPin, LOW); // Turn off card 1 LED
      digitalWrite(card2LEDPin, LOW); // Turn off card 2 LED
    }
 }
  else{ //night

    val = 0;
    doorState = 0;
    val = digitalRead(inputPin);  // read input value
    doorState  = digitalRead(DOOR_SENSOR_PIN);

    if (val == HIGH) {            // check if the input is HIGH{
      Serial.println("Motion detected!");
      ThingSpeak.writeField(myChannelNumber, 4, 1, myWriteAPIKey);
      delay(2000);
    
      while(true){
        analogWrite(buzz, 255); 
        delay(500);
        analogWrite(buzz, 3000); 
        delay(500);

        if (rfid.PICC_IsNewCardPresent()){
          rfid.PICC_ReadCardSerial();
          cardID = "";
          for (byte i = 0; i < 4; i++) {
            cardID += String(rfid.uid.uidByte[i]);
          }
          if (verifyCardID(cardID)) {
            analogWrite(buzz, 0); 
            val = 0;
            delay(2000);
            break;
          }
        }
      }
    }
    
    if(doorState == HIGH){
      Serial.println("Door is opened!");
      ThingSpeak.writeField(myChannelNumber, 5, 1, myWriteAPIKey);
      delay(2000);
      
      while(true){
        analogWrite(buzz, 255); 
        delay(500);
        analogWrite(buzz, 3000); 
        delay(500);

        if (rfid.PICC_IsNewCardPresent()){
          rfid.PICC_ReadCardSerial();
          cardID = "";
          for (byte i = 0; i < 4; i++) {
            cardID += String(rfid.uid.uidByte[i]);
          }
          if (verifyCardID(cardID)) {
            analogWrite(buzz, 0); 
            doorState = 0;
            delay(2000);
            break;
          }
        }
      }

    }

  } 
    
    
}

 

