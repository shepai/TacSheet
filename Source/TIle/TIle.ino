#include <ArduinoUniqueID.h>


int pad1  = analogRead(1);
int pad2  = analogRead(3);
int pad3 = analogRead(9);
int x= analogRead(6);
int y = analogRead(7);
int z  = analogRead(4);
//int pad4  = analogRead(11);
//int pad5  = analogRead(12);
//int pad6 = analogRead(13);
int led = 11;
String id;

void getDeviceID() {
  id = "";

  for (size_t i = 0; i < UniqueIDsize; i++) {
    if (UniqueID[i] < 0x10) {
      id += "0";
    }

    if (UniqueID[i] < 0x10) {
      id += "0";
    }

    id += String(UniqueID[i], HEX);

    // Optional: don't put spaces into the actual ID
  }

  id.toUpperCase();
}


void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
  getDeviceID();
  Serial.print("Device ID: ");
  Serial.println(id);
}

void loop() {
  Serial.print("Pin 6: ");
  Serial.print(pad1);

  Serial.print("  Pin 8: ");
  Serial.print(pad2);

  Serial.print("  Pin 14: ");
  Serial.print(pad3);

  Serial.print("  Pin 11: ");
  //Serial.print(pad4);

  Serial.print("  Pin 10: ");
  //Serial.print(pad5);

  Serial.print("  Pin 9: ");
  //Serial.println(pad6);

  delay(100);
  digitalWrite(led, HIGH);
  delay(1000);

  digitalWrite(led, LOW);
  delay(1000);
}