#include "OneWire.h"
int oneWirePIN = 2;
float boardTemp = 0.0;



byte onBoardSensorAddress[] = {0x28,0xA4,0x8D,0xBE,0x03,0,0,0xD9};
OneWire  dsOneWire(oneWirePIN);

void setup() {
  Serial.begin(9600);
  
}
void serialSensor(const char* name,float value){
  Serial.print("sensor:");
  Serial.print(name);
  Serial.print(":");
  Serial.println(value);
}
void loop(){
   boardTemp = getTemperature(onBoardSensorAddress);
   serialSensor("air_temp",boardTemp);
   delay(60000);
}
/////////////////////////////
float getTemperature(byte address[]){
  byte i;
  byte present = 0;
  byte data[12];
  
  // read the value from the sensor:
 // int sensorValue = analogRead(sensorPin);   
  //Serial.println(sensorValue); 
   // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  // print the number of seconds since reset:
 // lcd.print(String("M")+String(sensorValue));
 /*
  if ( !dsOneWire.search(onBoardSensorAddress)) {
      Serial.print("No more addresses.\n");
      dsOneWire.reset_search();
      return -1;
  }
  dsOneWire.reset_search();
  

  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(onBoardSensorAddress[i], HEX);
    Serial.print(" ");
  }
*/  

  if ( OneWire::crc8( address, 7) != address[7]) {
      Serial.print("CRC is not valid!\n");
      return -1;
  }

  if ( address[0] == 0x10) {
      //Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( address[0] == 0x28) {
     // Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(address[0],HEX);
      return -1;
  }

  dsOneWire.reset();
  dsOneWire.select(address);
  dsOneWire.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(750);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = dsOneWire.reset();
  dsOneWire.select(address);    
  dsOneWire.write(0xBE);         // Read Scratchpad

 // Serial.print("P=");
 // Serial.print(present,HEX);
 // Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = dsOneWire.read();
 ///   Serial.print(data[i], HEX);
 //   Serial.print(" ");
  }
//  Serial.print(" CRC=");
//  Serial.print( OneWire::crc8( data, 8), HEX);
//  Serial.println();
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;

  float temperature = Whole;
  temperature+=Fract/100.0;
  if (SignBit) // If its negative
     temperature = -temperature;
  return temperature;
}
