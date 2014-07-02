
#include <OneWire.h>
#include <dht.h>
#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>

typedef void (* IntervalFuncPtr) ();
typedef void (* CommandFuncPtr) ();

typedef struct
{
      unsigned long lastRun;
      unsigned long interval;
      IntervalFuncPtr func;
}  interval_run_type;

typedef struct
{
     const char* name;
     CommandFuncPtr func;
} command_type;


//light 
int lightPIN = 3;
// pump
int pumpPIN = 4;
int wateringTime = 6000;

int oneWirePIN = 2;
float boardTemp = 0.0;
float soilTemp = 0.0;
byte onBoardSensorAddress[] = {0x28,0xA4,0x8D,0xBE,0x03,0,0,0xD9};
byte soilSensorAddress[] = {0x28,0x06,0xC5,0xA9,0x03,0,0,0x82};

OneWire  dsOneWire(oneWirePIN);

const int startLightHour = 6;
const int stopLightHour = 20;

boolean bLight = false;

boolean bForceLight = false;
boolean bForceLightState = false;
boolean bExpectedState = false;

const int waterHour = 20;
int lastWateringDay = -1;


void temperatureUpdate(){
   boardTemp = getTemperature(onBoardSensorAddress);
   soilTemp = getTemperature(soilSensorAddress);
}
int pinDHT11 = 7;
float humidity = 0.0;
float temperature = 0.0;

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
     temperature = DHT.temperature;
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
}
// defined later
void water();
void waterUpdate(){
  //int day2 = day();
  //Serial.print("day:");
  //Serial.println(day2);
  if (lastWateringDay==-1)
    lastWateringDay = day();
  if (day()!=lastWateringDay && hour()>=waterHour){
      water();
      lastWateringDay = day();
  }

}
void lightUpdate(){
    //return;
/*    
     Serial.println("lightUpdate");
     Serial.print("bLight:");Serial.println(bLight);
     Serial.print("bExpectedState:");Serial.println(bExpectedState);
     Serial.print("bForceLight:");Serial.println(bForceLight);
     Serial.print("bForceLightState:");Serial.println(bForceLightState);
*/
     boolean bNewLight = false;
     boolean bNewExpectedState =  (hour()>=startLightHour && hour()<stopLightHour);
     //Serial.print("2bNewExpectedState:");Serial.println(bNewExpectedState);

     if (bNewExpectedState!=bExpectedState){
         bForceLight = false;
         bForceLightState = false;
         bNewLight = bExpectedState = bNewExpectedState;
     }else{
        bNewLight = bExpectedState;
     }
     /*Serial.print("3bLight:");Serial.println(bLight);
     Serial.print("3bExpectedState:");Serial.println(bExpectedState);
     Serial.print("3bForceLight:");Serial.println(bForceLight);
     Serial.print("3bForceLightState:");Serial.println(bForceLightState);
     */


     if (bForceLight){
         bNewLight = bForceLightState;
     }
     if (bNewLight!=bLight){
        bLight = bNewLight;
        digitalWrite(lightPIN,bLight?LOW:HIGH);
     }
     //Serial.print("4bLight:");Serial.println(bLight);

}
int moisturePIN =  A0;
int mositureVoltagePin = 8;
float moisture = 0.0;

void moistureUpdate(){
  digitalWrite(mositureVoltagePin,HIGH);
  delay(500);
  analogRead(moisturePIN);
  float iSum = 0;
  const int probeCount = 50;
  for (int i=0;i<probeCount;i++){
    int sensorValue = analogRead(moisturePIN);
    //Serial.println(sensorValue);
    //delay(500);
    iSum = iSum + sensorValue;
  }
  moisture = ((iSum/probeCount)/1024.0)*100.0;
  digitalWrite(mositureVoltagePin,LOW);

}
// commands
void stateCommand(){
         serialSensor("air_temp",temperature);
	 serialSensor("temp_board",boardTemp);
         serialSensor("humidity",humidity); 
         serialSensor("illumination",illumination); 
         serialSensor("light",bLight?1:0); 
         serialSensor("force_light",bForceLight?1:0); 
         serialSensor("force_light_state",bForceLightState?1:0); 
	 serialSensor("soil_temp",soilTemp);
	 serialSensor("soil_moisture",moisture);
         serialSensor("last_water_day",lastWateringDay);
         Serial.println();
}
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits

void setTimeCommand(){
    char timec[TIME_MSG_LEN+1];
    readSerialLine(timec,sizeof(timec));
    time_t pctime = 0;
    tmElements_t tm;
    for(int i=0; i < TIME_MSG_LEN -1; i++){   
      char c = timec[i];          
      if( c >= '0' && c <= '9'){   
	pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
      }
    }   
    breakTime(pctime,tm);
    RTC.write(tm);   
    setTime(RTC.get());   
    Serial.println();


}
void getTimeCommand(){
   tmElements_t tm;

   time_t time_now =  now();
   Serial.println(time_now);
   breakTime(time_now,tm);
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

   Serial.println();
}
void readSerialLine(char* line,int length){
      while (!Serial.available());
      byte read = Serial.readBytesUntil('\n',line,length-1);
      line[read] = '\0';
}
void setLightCommand(){
      char light_state[3];
      readSerialLine(light_state,sizeof(light_state));
      bForceLight = true;
      bForceLightState = (strcmp(light_state,"1")==0);
      lightUpdate();
      Serial.println();
}
void setResetLightCommand(){
      bForceLight = false;
      lightUpdate();
      Serial.println();
}
void water(){
  digitalWrite(pumpPIN,LOW);
  delay(wateringTime);
  digitalWrite(pumpPIN,HIGH);
}
void waterCommand(){
  water();
  Serial.println();
}

interval_run_type interval_run[6] = {
   {0,10000,temperatureUpdate},
   {0,10000,humidityUpdate},
   {0,5000,illuminationUpdate},
   {0,500,lightUpdate},
   {0,20000,moistureUpdate},
   {0,1000,waterUpdate}
};

command_type commands[6] = {
   {"state",stateCommand},
   {"get_time",getTimeCommand},   
   {"set_time",setTimeCommand},
   {"light",setLightCommand},
   {"reset_light",setResetLightCommand},
   {"water",waterCommand}

};

void setup() {
  Serial.begin(9600);
  Wire.begin();
  //light
  pinMode(lightPIN,OUTPUT);
  digitalWrite(lightPIN,HIGH);

  //pump
  pinMode(pumpPIN,OUTPUT);
  digitalWrite(pumpPIN,HIGH);

  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  setSyncInterval(60);

  //moisture
  pinMode(mositureVoltagePin,OUTPUT);
  digitalWrite(mositureVoltagePin,LOW);
 

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
char command[20];
void loop(){

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
      for (int i=0;i<sizeof(commands)/sizeof(command_type);i++){
          command_type* cmd = &commands[i];
          if (strcmp(cmd->name,command)==0){
              cmd->func();
          }

        }
   }





 // READ DATA

  /*if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
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
  }*/
  //Serial.println(RTC.get());
  //delay(1000);
  
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
