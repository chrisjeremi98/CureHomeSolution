   #include <MQ2.h>
#include <NTPClient.h> //timestamp library
#include <ESP8266WiFi.h> //library wifi
#include <WiFiUdp.h>
#include <FirebaseArduino.h> //library firebase-arduino


#define WIFI_ID "WIFI_ATMA_82" //ganti isi "" dengan nama wifi
#define WIFI_PASS "atma*@82" // ganti isi "" dengan password wifi

#define FIREBASE_HOST "curehomesolution.firebaseio.com" //Firebase database
#define FIREBASE_AUTH "xyAkQ8iYUeuf0P4FMH1JIuO06goT4ZRlAWstFzRo" //Firebase secret key

#define BUZZER D3 //BUZZER
#define MQ A0 // SENSOR ASAP
#define RELAYCH1 D0 //RELAY LAMPU CHANNEL 1
#define RELAYCH2 D1 // RELAY LAMPU CHANNEL 2
#define NTP_OFFSET 60 * 60 // detik
#define NTP_ADDRESS "2.th.pool.ntp.org" //waktu indonesia
#define NTP_INTERVAL 120 * 1000 

//TODO : WIFI UDP 
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP,NTP_ADDRESS,NTP_OFFSET,NTP_INTERVAL);
String statusLampu;
String statusPintu;

StaticJsonBuffer<200> jsonBuffer; //jsoN buffer
MQ2 mq2(MQ); 

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
    digitalWrite(RELAYCH1,HIGH);
  } else if(statusLampu == "Off"){
    digitalWrite(RELAYCH1,LOW);
  }
}

void cekPintu(){
  //  cek pintu relay NO
  
  statusPintu = Firebase.getString("/Pintu/status");
  Serial.print("Pintu : ");
  Serial.println(statusPintu);
  if(statusPintu=="On"){
    digitalWrite(RELAYCH2, HIGH);
  }else if(statusPintu=="Off"){
     digitalWrite(RELAYCH2,LOW);
  }
}

void cekAsap(){
  //  cek asap;
  
   int asap = mq2.readSmoke();
//  int asap = analogRead(MQ);

   Serial.print("Kadar asap (ppm) : ");
   Serial.println(asap);
   
   if(asap>200){
      //TODO ALARM SOUND
      digitalWrite(BUZZER, HIGH);
   }

  logAsap(asap);
}

void logAsap(int asap){
 
//  log asap
  
  String logTime = timeClient.getFormattedTime();
  
  JsonObject& logUser = jsonBuffer.createObject();
  logUser["Kadar Asap"] = asap;
  logUser["Waktu"]=logTime;
  
  Firebase.push("/Log/Asap",logUser);
  delay(1000);

  Firebase.setInt("/Asap/status",asap);
  Firebase.setString("/Asap/time_stamp",logTime);

}


  
