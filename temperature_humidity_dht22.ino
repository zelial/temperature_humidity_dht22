#include "DHT.h"
#include <utils.h>
#include "configuration.h"


#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
uint8_t DHTPin = D2; 
DHT dht(DHTPin, DHTTYPE);

float temperature;
float humidity;
float voltage;

/*
 *  Main body, all will happen in setup as wake up from deep sleep
 *  is reset so there's no need for the loop function
 */
void setup() {
  init_serial();

  // initialize DHT sensor
  pinMode(DHTPin, INPUT_PULLUP);
  dht.begin();

  temperature = dht.readTemperature(); // Gets the values of the temperature
  humidity = dht.readHumidity(); // Gets the values of the humidity

  wifi_reconnect(ssid, wifi_password, ip_last_byte);

  // get voltage of battery/ power source
  int voltage_raw = analogRead(A0);
  // calibrate by supplying known voltage and come up with coeficient
  voltage = (float)voltage_raw * 0.00448;


  // process all data
  logln("Temperature: "+ String(temperature));
  logln("Humidity: " + String(humidity));
  logln("Voltage raw: " + String(voltage_raw));
  logln("Voltage: "+ String(voltage));
  upload();

  protect_battery(voltage);
  logln("Going to sleep. See you in " + String(refresh_rate) + "s. G'night");
  ESP.deepSleep(refresh_rate * 1000000); 
}

// not used
void loop() {
}

void upload() {
  Broker b = Broker(broker_url);
  b.addProperty("voltage", String(voltage));
  b.addProperty("refresh_rate", String(refresh_rate));
  // report only 1 decimal place - saves space in HA dashboards
  b.addProperty("temperature", String(temperature, 1));
  b.addProperty("humidity", String(humidity, 1));
  b.upload();
}
