#include <EEPROM.h>
#include <OneWire.h>

#define SERIAL_BAUD 115200
#define K 15 //pojacanje ventilatora
#define RELAY_PIN 7
#define PWM_PIN 9

byte AU=1;
byte G[2];
byte D[2];
byte T1[2];
byte T2[2];
byte VENT=0;
byte HEAT=0;

OneWire  ds(10); 

void setup() {
  
  Serial.begin(SERIAL_BAUD);
  pinMode(RELAY_PIN, OUTPUT);
  
  G[0] = EEPROM.read(0);
  G[1] = EEPROM.read(1);
  D[0] = EEPROM.read(2);
  D[1] = EEPROM.read(3);
  if(G[0]==0xff) G[0]=30;
  if(G[1]==0xff) G[1]=1;
  if(D[0]==0xff) D[0]=25;
  if(D[1]==0xff) D[1]=1;
}

void loop()
{
  citaj_oba();
  if(AU==1) calc_and_apply();
  
  switch( Serial.read() )
  {
    case 'E':
      Serial.print(T1[0]);
      Serial.print(T1[1]);
      Serial.print(T2[0]);
      Serial.print(T2[1]);
      Serial.print(HEAT);
      Serial.print(VENT);
      break;
    case 'L':
      G[0]=Serial.read();
      G[1]=Serial.read();
      D[0]=Serial.read();
      D[1]=Serial.read();
      EEPROM.write(0,G[0]);
      EEPROM.write(1,G[1]);
      EEPROM.write(2,D[0]);
      EEPROM.write(3,D[1]);
      break;
    case 'V':
      Serial.print(G[0]);
      Serial.print(D[0]);
      Serial.print(G[1]);
      break;
    case 'R':
      HEAT = Serial.read();
      VENT = Serial.read();
      AU=0;
      (HEAT==1)?digitalWrite(RELAY_PIN, HIGH):digitalWrite(RELAY_PIN, LOW);
      analogWrite(PWM_PIN,VENT);
      break;
    case 'A':
      AU=1;
      break;
    case 'I':
      Serial.print('O');
      Serial.print('K');
      break;
  }
}

void calc_and_apply(){
  unsigned int raw1 = (T1[0] << 8) | T1[1];
  unsigned int raw2 = (T2[0] << 8) | T2[1];
  float temp1= (float)raw1/16.0;
  float temp2=(float)raw2/16.0;
  
  // proracun grijaca ----------------
  
  switch (D[1])
  {
   case 0:
     if( (temp1 < D[0]) && (temp2 < D[0]) ){
       HEAT=1;
     }else if ( ((temp1+temp2)/2) >= ((D[0]+G[0])/2) ){
       HEAT=0;
     }
     break;
   case 1:
     if( (temp1 < D[0]) || (temp2 < D[0]) ){
       HEAT=1;
     }else if ( ((temp1+temp2)/2) >= ((D[0]+G[0])/2) ){
       HEAT=0;
     }
     break;
  }
  
  if( HEAT==0 ){
    digitalWrite(RELAY_PIN, LOW);
  }else{
    digitalWrite(RELAY_PIN, HIGH);
  }  
  
  // proracun ventilatora -------------
  
  if(G[1]==1){
    if( (temp1 > G[0]) || (temp2 > G[0]) ){
      float DIF=(  ((temp1+temp2)/2)-((G[0]+D[0])/2) +0.1*K  );
      float pom=DIF*DIF*10.2;
      if (pom>255) pom=255;
      
      VENT=(byte)pom;
    }
    else if ( ((temp1+temp2)/2) <= ((D[0]+G[0])/2) ) VENT=0;
  }
  else if(D[1]==0){
    if( (temp1 > G[0]) && (temp2 > G[0]) ){
      float DIF=(  ((temp1+temp2)/2)-((G[0]+D[0])/2) +0.1*K  );
      float pom=DIF*DIF*10.2;
      if (pom>255) pom=255;
      
      VENT=(byte)pom;
    }
    else if (((temp1+temp2)/2) <= ((D[0]+G[0])/2) ) VENT=0;
  }
  analogWrite(PWM_PIN,VENT);
  
}

void citaj_oba() {
  for(int z=0; z<3; z++)
  {
       byte i;
      byte present = 0;
      byte data[2];
      byte addr[8];
      float celsius, fahrenheit;
      if ( !ds.search(addr)) {
          ds.reset_search();
          return;
      }
      ds.reset();
      ds.select(addr);
        ds.write(0x4E);
        ds.write(0x4B);
        ds.write(0x46);
        ds.write(0x1F);  // 9 bit rezolucija
      ds.reset();
      ds.select(addr);
      ds.write(0x44,1);      
      delay(100);     // za razlicite rezolucije, razlicite vrijednosti
      present = ds.reset();
      ds.select(addr);
      ds.write(0xBE);
      for ( i = 0; i < 2; i++) {           
        data[i] = ds.read();
      }
      //unsigned int raw = (data[1] << 8) | data[0];
      //celsius = (float)raw / 16.0;
      if(z==0){
        T1[0]=data[1];
        T1[1]=data[0];
      }
      else if(z==1){
        T2[0]=data[1];
        T2[1]=data[0];
      }
  }
}

