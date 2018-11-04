#include <NTPClient.h> //timestamp library
#include <ESP8266WiFi.h> //library wifi
#include <FirebaseArduino.h> //library firebase-arduino

#define WIFI_ID "starless" //ganti isi "" dengan nama wifi
#define WIFI_PASS "kentanggoreng" // ganti isi "" dengan password wifi

#define FIREBASE_HOST "" //Firebase database
#define FIREBASE_AUTH "" //Firebase secret key

#define BUZZER D3 //BUZZER
#define MQ2 D2 // SENSOR ASAP
#define RELAYCH1 D0 //RELAY LAMPU CHANNEL 1
#define RELAYCH2 D1 // RELAY LAMPU CHANNEL 2
#define NTP_OFFSET 60 * 60 // detik
#define NTP_ADDRESS "id.pool.ntp.org" //waktu indonesia

//TODO : WIFI UDP 

StaticJsonBuffer<200> jsonBuffer; //jsoN buffer

void setup() {
  //inisiasi pin
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAYCH1, OUTPUT);
  pinMode(RELAYCH2, OUTPUT);
  
  pinMode(MQ2, INPUT);

  //inisiasi wifi
  WiFi.begin(WIFI_ID, WIFI_PASS);
  
  //inisiasi firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  cekLampu();
  cekAsap();
  cekPintu();
}

void cekLampu(){
  if (Firebase.get("/Lampu/status") == "On"){
    //TODO BERAPA LAMPU
  }
  delay(1000);
}

void cekAsap(){
   int asap;
   if(Firebase.get("/Asap/status")>500){
      //TODO ALARM SOUND
   }
   logAsap(asap);

   delay(1000);
}

void cekPintu(){
  //TODO SELENOID  
  delay(1000);
}

void logAsap(int asap){
  JsonObject logUser = jsonBuffer.createObject();
  logUser["Kadar Asap"] = asap;
  //TODO TIMESTAMP LOG ASAP
  logUser["Waktu"]=  ;

  Firebase.push("/Log/Lampu",logUser);
  
}
