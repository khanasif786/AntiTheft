/* This is a prototype Only and Extandable to many useful automations
   so a button is used over sensors to demonstrate that as a sensor giving
   0 or 1 , to properly use a Button it is necessary to Use debounce code*/
// Author: Asif Khan
// github: khanasif786

#include <TinyGPS++.h> // GPS library
#include <SoftwareSerial.h> // serial communication Library
#define Button 7
#define Alarm 4
TinyGPSPlus gps; // using Tiny gps as gps
SoftwareSerial gsm(9, 10); // RX TX For Arduino , TX RX for GSM module
SoftwareSerial gpsSerial(2, 3); // gps serial at pin 2 and 3

char str[100] ;
String textMessage;
int  i ;
// variables for Debouncing of Button
unsigned long PreviousTime ;
unsigned long CurrentTime ;
long DebounceDelay = 100 ;
long lastDebounceTime = 0 ;
int MessageJustSent = 0 ;
int ButtonState = 1 ;

void setup() {
  pinMode(Button, INPUT);
  pinMode(Alarm, OUTPUT);
  gsm.begin(9600);   // Setting the baud rate of GSM Module
  delay(100);
  Serial.begin(9600); // only for testing purpose  // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
  gpsSerial.begin(9600); // setting the baud rate of gps serial
  delay(100);
  gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  digitalWrite(4, HIGH);
}

void loop() {

  ButtonState =  digitalRead(Button) ;

  if (gps.encode(gpsSerial.read())) {
    if (gps.location.isValid())
    {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("Altitude: ");
      Serial.println(gps.altitude.meters());
    }
    else
    {
      Serial.println("Location: Not Available");
    }
  }

  if ( (millis() - lastDebounceTime) > DebounceDelay) {
    if ((ButtonState == 0) && (MessageJustSent == 0)) {
      lastDebounceTime = millis();
      Serial.println("sending message");
      SendMessage();
      MessageJustSent = 1 ;
      RecieveMessage();
    }
    else if ((ButtonState == 1) && (MessageJustSent == 1)) {
      MessageJustSent = 0 ;
      lastDebounceTime = millis();
    }
  }

  if (gsm.available() > 0) {
    textMessage = gsm.readString();
    Serial.println(textMessage);
  }

  if (textMessage.indexOf("ALARM ON") >= 0 ) {
    digitalWrite(Alarm, LOW);
    textMessage = "NUL"  ;
  }

  else if (textMessage.indexOf("ALARM OFF") >= 0 ) {
    digitalWrite(Alarm, HIGH);
    textMessage = "NUL"  ;
  }

  else if (textMessage.indexOf("LIVE") >= 0 ) {
    SendLocation();
    textMessage = "NUL"  ;
  }
}

void SendMessage() {
  delay(2000);
  gsm.println("AT+CMGS=\"xxxxxxxxxx\"\r"); // Replace xxxxxxxxxx with mobile number
  delay(1000);
  gsm.println("Your vehicle is Started at this location Click:");
  delay(100);
  gsm.print("http://maps.google.com/maps?q=loc:");// The SMS text you want to send
  delay(100);
  gsm.print(gps.location.lat(), 6);
  delay(100);
  gsm.print(",");
  delay(100);
  gsm.println(gps.location.lng(), 6);
  delay(100);
  gsm.println("if its not you please reply this message");
  delay(100);
  gsm.println("ALARM ON : to switch on Siren");
  delay(100);
  gsm.println("ALARM OFF : to switch off Siren");
  delay(100);
  gsm.println("LIVE : to show Current Location");
  delay(100);
  gsm.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  RecieveMessage();
}

void SendLocation() {
  gsm.println("AT+CMGS=\"+xxxxxxxxxx\"\r"); // Replace x with mobile number
  delay(1000);
  gsm.print("http://maps.google.com/maps?q=loc:");// The SMS text you want to send
  delay(100);
  gsm.print(gps.location.lat(), 6);
  delay(100);
  gsm.print(",");
  delay(100);
  gsm.print(gps.location.lng(), 6);
  delay(100);
  gsm.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void RecieveMessage() {
  gpsSerial.end();
  delay(1000);
  gsm.begin(9600);
  delay(2000);
  gsm.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(1000);
}
