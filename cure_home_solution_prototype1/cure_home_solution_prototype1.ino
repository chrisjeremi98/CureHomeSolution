#include <MQ2.h>
#include <NTPClient.h>                                                          //timestamp library
#include <ESP8266WiFi.h>                                                        //library wifi
#include <WiFiUdp.h>
#include <FirebaseArduino.h>                                                    //library firebase-arduino


#define WIFI_ID "net_print"                                                  //ganti isi "" dengan nama wifi
#define WIFI_PASS "03634509"                                                    //ganti isi "" dengan password wifi

#define FIREBASE_HOST "curehome-ee497.firebaseio.com"                         //Firebase database
#define FIREBASE_AUTH "MrYkqOc4XLHuH6lOGyexQDnAmoZyAvsoO0cmHIIq"                //Firebase secret key

#define BUZZER D3                                                               //BUZZER
#define MQ A0                                                                   //SENSOR ASAP
#define RELAYCH1 D0                                                             //RELAY PINTU SELENOID CHANNEL 1
#define RELAYCH2 D1                                                             //RELAY LAMPU
#define NTP_OFFSET 60 * 60 // detik
#define NTP_ADDRESS "2.th.pool.ntp.org"                                         //waktu indonesia
#define NTP_INTERVAL 120 * 1000 

//TODO : WIFI UDP 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,NTP_ADDRESS,NTP_OFFSET,NTP_INTERVAL);
String statusLampu;
String statusPintu;

StaticJsonBuffer<200> jsonBuffer;                                               //JSON buffer
MQ2 mq2(MQ); 

unsigned long interval = 60000 * 1;
unsigned long previousMillis = 0;
int asapBahaya =1;

void setup() {
  //inisiasi pin
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAYCH1, OUTPUT);
  pinMode(RELAYCH2, OUTPUT);
  
  pinMode(MQ, INPUT);

  Serial.begin(9600);
  
  //inisiasi wifi
  WiFi.begin(WIFI_ID, WIFI_PASS);
  while(WiFi.status()!= WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  
  //inisiasi firebase
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  mq2.begin();
  timeClient.begin();
  Serial.begin(9600);
}

void loop() {

  

  timeClient.update();
  
  cekLampu();
  cekPintu();
  cekAsap();  
     
}

void cekLampu(){
  //  cek lampu Relay NO
  statusLampu = Firebase.getString("/Lampu/status");
//  String statusLampu = "Off";
  Serial.print("Lampu : ");
  Serial.println(statusLampu);
  if (statusLampu == "On"){
    digitalWrite(RELAYCH2,HIGH);
  } else if(statusLampu == "Off"){
    digitalWrite(RELAYCH2,LOW);
  }
}

void cekPintu(){
  //  cek pintu relay NO
  
  statusPintu = Firebase.getString("/Pintu/status");
  Serial.print("Pintu : ");
  Serial.println(statusPintu);
  if(statusPintu=="On"){
    digitalWrite(RELAYCH1, HIGH);
  }else if(statusPintu=="Off"){
     digitalWrite(RELAYCH1,LOW);
  }
}

void cekAsap(){
  //  cek asap;
  
   int asap = mq2.readSmoke();
//  int asap = analogRead(MQ);

   Serial.print("Kadar asap (ppm) : ");
   Serial.println(asap);

// TODO kalau kebakaran otomatis mengirim tanpa jeda sekuensial
   if(asap>asapBahaya){
      //TODO ALARM SOUND
      tone(BUZZER, 2500);
      logAsap(asap);
   } else{
    unsigned long currentMillis = millis();
//    if((unsigned long)(currentMillis - previousMillis) >= interval){
//      logAsap(asap);
//    }
      logAsap(asap);
      digitalWrite(BUZZER,LOW);
   }
   }

  


void logAsap(int asap){
 
//  log X asap

// TODO coba pakai millis
  
  String logTime = timeClient.getFormattedTime();
  
//  JsonObject& logUser = jsonBuffer.createObject();
//  logUser["Kadar Asap"] = asap;
//  logUser["Waktu"]=logTime;
  
//  Firebase.push("/Log/Asap",logUser);
//  delay(1000);

  Firebase.setInt("Asap/status",asap);
  Firebase.setString("Asap/time_stamp",logTime);

}


  
