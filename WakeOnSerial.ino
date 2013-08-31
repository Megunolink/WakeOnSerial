#include <LowPower.h>
#include <PinChangeInt.h>

// The pin that the Arduino receives 
// serial data on. 
#define SERIAL_RX_PIN 0

// The baud rate for serial communications
#define SERIAL_BAUD 9600

// We won't got to sleep if there has been serial communication recently. 
// record the millis timer value for the last serial activity. 
unsigned long g_uLastSerial = 0;

// Time to remain awake to check for serial port activity.
#define SERIAL_SLEEP_TIMEOUT 500 // [ms]

// Count sheep while we wait to go to sleep. Printout the count periodically. 
unsigned long g_uSheep = 0;
#define SHEEP_COUNT_PERIOD 1000 // [ms]
unsigned long g_uLastSheepCount = 0;

// If we receive a 'w' character on the serial port, we remain
// awake until we get the 's' character. 
bool g_bRemainAwake = false;

void WakeHandler()
{
  // Nothing to do; just wakes the device. 
}

void setup()
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("Hello world!");
}

void loop() 
{
  char chReceived;
  
  chReceived = Serial.read();
  if (chReceived == 's')
  {
    g_bRemainAwake = false;
    Serial.println("I'll sleep soon");
  }
  if (chReceived == 'w')
  {
    g_bRemainAwake = true;
    Serial.println("Time to count some sheep!");
  }
    
  // If we haven't had any serial messages for a while, go to sleep.
  if (!g_bRemainAwake && millis() - g_uLastSerial > SERIAL_SLEEP_TIMEOUT)
  {
    Serial.println("Good night");
    
    // Make sure all serial messages have been sent.
    Serial.flush();
  
    // Enable the pin change interrupt on the receive pin 
    // so that serial activity will wake the device.
    pinMode(SERIAL_RX_PIN, INPUT_PULLUP);
    PCintPort::attachInterrupt(SERIAL_RX_PIN, &WakeHandler, LOW);

    // Enter power down state. We'll wake periodically. 
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
    
    // Detach pinchange interrupts & reconfigure serial port. 
    PCintPort::detachInterrupt(SERIAL_RX_PIN);
    Serial.begin(SERIAL_BAUD);
    
    Serial.println("Yawn. I'm awake!");
    g_uLastSerial = millis();
  }
  
  ++g_uSheep;
  if (millis() - g_uLastSheepCount > SHEEP_COUNT_PERIOD)
  {
    g_uLastSheepCount = millis();
    Serial.print("That's ");
    Serial.print(g_uSheep);
    Serial.println(" sheep. Can I go to sleep now?");
  }
}
