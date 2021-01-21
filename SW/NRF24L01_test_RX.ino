/* Kod prijimace pro LasKKit Simple Joystick - Arduino Micro
 * PWM vystup D2,D3
 * 
 * Pouzita knihovna https://github.com/nRF24/RF24
 * 
 */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

int pwm_width_2 = 0;
int pwm_width_3 = 0;

Servo PWM2;
Servo PWM3;

//Muzeme mit až 32 kanalu
struct MyData {
  byte ch1;      //V tomto pripade mame 4 kanaly
  byte ch2;
  byte ch3;
  byte ch4;
};
MyData data;

const uint64_t pipeIn = 0x0022;   //Identifikator zarizeni - stejny kod musi byt nastaven na vysilaci
RF24 radio(8,10);  //zapojeni CE a CSN pinu

void resetData()
{
  //Definice zakladnich hodnot
  data.ch1 = 127;
  data.ch2 = 127;
  data.ch3 = 0;
  data.ch4 = 0;

}

void setup()
{
  Serial.begin(115200);
  //Nastaveni PWM pinu
  PWM2.attach(2);
  PWM3.attach(3);
  
  //Nastaveni NRF24 
  resetData();
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeIn);
  radio.startListening();
}

//Zde budeme cist prijata data
unsigned long lastRecvTime = 0;
void recvData()
{
  while ( radio.available() ) {
  radio.read(&data, sizeof(MyData));
  lastRecvTime = millis();
  }
}

void loop()
{
  recvData();
  unsigned long now = millis();
  //Zde zkontrolujeme, zda se nam ztratila komunikace
  if ( now - lastRecvTime > 1200 ) {
    //Pokud doslo ke ztrate komunikace, resetujeme prijate hodnoty
    resetData();
  }
  
  //Vypocet hodnot PWM
  pwm_width_2 = map(data.ch1, 0, 255, 200, 2000);  //PWM vystup pin D2
  pwm_width_3 = map(data.ch2, 0, 255, 200, 2000);  //PWM vystup pin D3
  
  Serial.print(pwm_width_2);
  Serial.print(" | ");
  Serial.print(pwm_width_3);
  Serial.print(" | ");
  Serial.print(data.ch3);
  Serial.print(" | ");
  Serial.println(data.ch4);
  
  //Vygenerovani PWM signalu
  PWM2.writeMicroseconds(pwm_width_2);
  PWM3.writeMicroseconds(pwm_width_3);

}
