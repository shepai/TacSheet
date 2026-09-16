#include <SPI.h>
#include <mcp_can.h>
#include <avr/io.h>

#define LED_PIN 8
#define CAN_CS  0

int previousSensor1 = 0;
int previousSensor2 = 0;
int previousSensor3 = 0;

#define CHANGE_THRESHOLD 10

MCP_CAN CAN(CAN_CS);
uint8_t uniqueID[10];
uint32_t canID = 0;

void blink(byte count)
{
  for (byte i = 0; i < count; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(25);
    digitalWrite(LED_PIN, LOW);
    delay(25);
  }
}


void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // 1 blink = ATtiny started
  blink(1);

  // Start SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  // 2 blinks = SPI.begin() executed
  blink(2);

  // Initialize MCP2515
  byte result = CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ);

  if (result == CAN_OK)
  {
    // 3 blinks = MCP2515 responded
    blink(3);
  }
  else
  {
    // 5 blinks = MCP2515 did NOT respond
    blink(5);
    while(1);
  }
 
  CAN.setMode(MCP_NORMAL);
  //1. Read the unique factory ID into your array
  uint8_t *sernum_ptr = (uint8_t *)&SIGROW.SERNUM0;
  for (uint8_t i = 0; i < 10; i++) {
      uniqueID[i] = sernum_ptr[i];
  }

  // Hash/Fold the 10 bytes down into a single 32-bit integer
  uint32_t hash = 2166136261U;
  for (uint8_t i = 0; i < 10; i++) {
      hash ^= uniqueID[i];
      hash *= 16777619U;
  }

  // Force the layout to fit a 29-bit CAN frame limit
  canID = hash & 0x7FF;
}

void read_sensors()
{
    int sensor1 = analogRead(2);
    if (abs(sensor1 - previousSensor1) >= CHANGE_THRESHOLD)
    {
        sendSensorEvent(1, sensor1);
        previousSensor1 = sensor1;
    }
    int sensor2 = analogRead(3);
    if (abs(sensor2 - previousSensor2) >= CHANGE_THRESHOLD)
    {
        sendSensorEvent(2, sensor2);
        previousSensor2 = sensor2;
    }
    int sensor3 = analogRead(5);
    if (abs(sensor3 - previousSensor3) >= CHANGE_THRESHOLD)
    {
        sendSensorEvent(3, sensor3);
        previousSensor3 = sensor3;
    }
    delay(25);
}
void sendSensorEvent(byte sensor, int value)
{
    byte data[3];

    data[0] = sensor;
    data[1] = highByte(value);
    data[2] = lowByte(value);

    byte result = CAN.sendMsgBuf(canID, 0, 3, data);
    if (result == CAN_OK)
  {
    blink(1);
  }
}

void loop()
{
  read_sensors();

  //delay(1000); 
}