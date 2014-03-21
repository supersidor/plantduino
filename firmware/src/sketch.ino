#include <OneWire.h>
#include <dht.h>
#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>
typedef void (* IntervalFuncPtr) ();

typedef struct
{
      unsigned long lastRun;
      unsigned long interval;
      IntervalFuncPtr func;
}  interval_run_type;



//light 
int lightPIN = 3;

int oneWirePIN = 2;
float boardTemp = 0.0;
byte onBoardSensorAddress[] = {0x28,0xA4,0x8D,0xBE,0x03,0,0,0xD9};
OneWire  dsOneWire(oneWirePIN);

void temperatureUpdate(){
   boardTemp = getTemperature(onBoardSensorAddress);
}
int pinDHT11 = 7;
float humidity = 0.0;
dht DHT;

void humidityUpdate(){
  int chk = DHT.read22(pinDHT11);
  switch (chk)
  {
    case DHTLIB_OK:  
      //Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
      //Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
      //Serial.print("Time out error,\t"); 
    break;
    default: 
      //Serial.print("Unknown error,\t"); 
    break;
  }
  // DISPLAY DATA
  //Serial.print(DHT.humidity, 1);
  //Serial.print(",\t");
  //Serial.println(DHT.temperature, 1);



   if (chk==DHTLIB_OK){
     humidity = DHT.humidity;
   }
}
float illumination = 0.0 ;
 byte buff[2];
 int BH1750address = 0x23; 

int BH1750_Read(int address) //

{

  int i=0;

  Wire.beginTransmission(address);

  Wire.requestFrom(address, 2);

  while(Wire.available()) //

  {

    buff[i] = Wire.read();  // receive one byte

    i++;

  }

  Wire.endTransmission();  

  return i;

}




void BH1750_Init(int address) 

{

  Wire.beginTransmission(address);

  Wire.write(0x10);//1lx reolution 120ms

  Wire.endTransmission();

}

void illuminationUpdate(){
int i;

 uint16_t val=0;

 BH1750_Init(BH1750address);

 delay(200);

 if(2==BH1750_Read(BH1750address))

  {

   val=((buff[0]<<8)|buff[1])/1.2;
   illumination = val;
   //Serial.print(val,DEC);     
   //Serial.println("[lx]"); 

  }
  /*  
 Serial.println("illuminationUpdate");
 int BH1750address = 0x23; 
 int i;
 uint16_t val=0;
 Wire.beginTransmission(BH1750address);
 Wire.write(0x10);//1lx reolution 120ms
 Wire.endTransmission(); 
 delay(200);
 if(2==BH1750_Read(BH1750address))
 {
   val=((buff_ill[0]<<8)|buff_ill[1])/1.2;
   Serial.println(val);
   illumination = (float)val;     
   Serial.println(illumination);

  }*/

}

interval_run_type interval_run[3] = {
   {0,10000,temperatureUpdate},
   {0,10000,humidityUpdate},
   {0,5000,illuminationUpdate}
};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  //light
  pinMode(lightPIN,OUTPUT);
  digitalWrite(lightPIN,HIGH);

 
  Serial.println("Arduino initialized");
}
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
void serialSensor(const char* name,float value){
  Serial.print(name);
  Serial.print(":");
  Serial.println(value);
}
char command[10];
void loop(){
   tmElements_t tm;

   int incomingByte;
   for (int i=0;i<sizeof(interval_run)/sizeof(interval_run_type);i++){
       interval_run_type* run = &interval_run[i];
       if (run->lastRun==0 || ( millis() - run->lastRun  > run-> interval) ){
          run->func();
          run->lastRun = millis();
       }
   }   
   if (Serial.available()){
      /*while (Serial.available())
         Serial.println(Serial.read());*/
      byte read = Serial.readBytesUntil('\n',command,sizeof(command));
      command[read] = '\0';
      if (strcmp(command,"state")==0){
         serialSensor("air_temp",boardTemp);
         serialSensor("humidity",humidity); 
         serialSensor("illumination",illumination); 
         Serial.println();
      }
   }




/*
 // READ DATA

  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);


   delay(5000);
  */
  
  /*digitalWrite(lightPIN,LOW);
  delay(5000);
  digitalWrite(lightPIN,HIGH);
  delay(5000);*/


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
