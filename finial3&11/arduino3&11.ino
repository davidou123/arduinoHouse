//此模組在上傳或開機時 麥克風的5V開關要先關掉才能上傳，不然會傳不上去
//czn-15e 聲音感測器
const int soundPin = 2; //D4=2 7;
int soundVal = 0;
//czn-15e 聲音感測器end
const int ledPin = 14;//14=d5
const int ledPin2 = 5;//d1=5  
//beep蜂鳴器
const int buzzer = 4; //4就是d1 mini的D2
const int toneTable[7][5]={
    { 66, 131, 262, 523, 1046},  // C Do
    { 74, 147, 294, 587, 1175},  // D Re
    { 83, 165, 330, 659, 1318},  // E Mi
    { 88, 175, 349, 698, 1397},  // F Fa
    { 98, 196, 392, 784, 1568},  // G So
    {110, 220, 440, 880, 1760},  // A La
    {124, 247, 494, 988, 1976}   // B Si
};
char toneName[]="CDEFGAB";
const  char starTone[]="CCGGAAGFFEEDDCGGFFEEDGGFFEEDCCGGAAGFFEEDDC";
char beeTone[]="GEEFDDCDEFGGGGEEFDDCEGGEDDDDDEFEEEEEFGGEEFDDCEGGC";
int beeBeat[]={

    1,1,2, 1,1,2, 1,1,1,1,1,1,2,

    1,1,2, 1,1,2, 1,1,1,1,4,

    1,1,1,1,1,1,2, 1,1,1,1,1,1,2,

    1,1,2, 1,1,2, 1,1,1,1,4

};
int starBeat[]={
    1,1,1,1,1,1,2, 1,1,1,1,1,1,2,
    1,1,1,1,1,1,2, 1,1,1,1,1,1,2,
    1,1,1,1,1,1,2, 1,1,1,1,1,1,2
};

int getTone(char symbol) {
    int toneNo = 0;
    for ( int ii=0; ii<7; ii++ ) {
        if ( toneName[ii]==symbol ) {
            toneNo = ii;
            break;
        }
    }
    return toneNo;
}
//beep 蜂鳴器end
//超聲波測距
#define echoPin D7 // Echo Pin
#define trigPin D6 // Trigger Pin
long duration, cm, inches;
//超聲波測距end
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ASUS_iot_2G";
const char* password = "1121314151";

ESP8266WebServer server(80);
void homepage() {
  server.send(200, "text/html",  SendHTML());
  Serial.println("同學開啟了網頁");
}

void setup() {
  Serial.begin (9600);

  pinMode(buzzer,OUTPUT);
  noTone(buzzer);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //初始化網絡
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192,168,60,203), // IP位址
             IPAddress(192,168,60,254),  // 閘道（gateway）位址
             IPAddress(255,255,255,0));  // 網路遮罩（netmask）
             
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //初始化WebServer
  server.on("/", homepage);
  server.on("/openmusic", singstar);//打開音樂
  server.on("/openmusicbee", singbee);//打開音樂 bee
  server.begin();
  Serial.println("HTTP server started");
  pinMode(soundPin, INPUT);
}
 
void loop() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  soundVal = digitalRead(soundPin);
  if (soundVal == HIGH)
  {
     Serial.println("loud");
     singbee();
     delay(3000);//要delay一下 以免麥克風又收到自己的聲音又開始放了
  }
  else
  {
     Serial.println("quiet");
  }

  
  if (cm <20)
  {
     Serial.println("move in 10cm");
     Serial.println(cm);
     singstar();
     delay(3000);//要delay一下 以免麥克風又收到自己的聲音又開始放了
  }

  delay(250);
  server.handleClient();//監聽客戶請求並處理
}

void singstar() {
     server.send(200, "text/html",  SendHTML());
  // star
   int  ii,length,toneNo,duration;
    length = sizeof(starTone)-1;
    for ( ii=0; ii<length; ii++ ) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(ledPin2, LOW);
        toneNo = getTone(starTone[ii]);
        duration = starBeat[ii]*133;
        tone(buzzer,toneTable[toneNo][3]);
        delay(duration);
        noTone(buzzer);
        digitalWrite(ledPin, LOW);
        digitalWrite(ledPin2, HIGH);
        delay(50);
    }
    noTone(buzzer);
}
void singbee() {
     server.send(200, "text/html",  SendHTML());
  // bee
   int  ii,length,toneNo,duration;
  length = sizeof(beeTone)-1;

    for ( ii=0; ii<length; ii++ ) {
        digitalWrite(ledPin, HIGH);
        digitalWrite(ledPin2, LOW);
        toneNo = getTone(beeTone[ii]);
        duration = beeBeat[ii]*133;
        tone(buzzer,toneTable[toneNo][3]);
        delay(duration);
        noTone(buzzer);
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin, LOW);
        delay(50);
    }
}
String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"><meta charset=\"UTF-8\">\n";
  ptr +="<title>雲端控制器</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1> ESP8266 網頁伺服器</h1>\n";
  ptr +="<h3>勞動部勞動力發展署中彰投分署</h3>\n";
  ptr +="<a class=\"button button-off\" href=\"/openmusic\">放音樂</a>\n";
  ptr +="<a class=\"button button-off\" href=\"/openmusicbee\">放小蜜蜂音樂</a>\n";
   ptr +="<hr><h3>power by kunlex ,davidou 2019 </h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
