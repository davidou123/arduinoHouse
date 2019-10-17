#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// notes in the melody:
int melody[] = {
NOTE_E4, NOTE_C4, NOTE_G3, NOTE_C4, NOTE_D4, NOTE_G4,
NOTE_D4, NOTE_E4, NOTE_D4, NOTE_G3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
4,4,4,4,4,1.3,
4,4,4,4,1
};
int LEDpin=D4; //LED
int doorpin = D2;//霍爾門磁
int OutputRadioPin=D7; //音樂pin
int windowstate;//大門開關狀態 0是關 1是開
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ASUS_iot_2G";
const char* password = "1121314151";

ESP8266WebServer server(80);
void homepage() {
  server.send(200, "text/html",  SendHTML(windowstate));
  Serial.println("同學開啟了網頁");
}

void setup() {
  //初始化串口
  Serial.begin(9600);
  //初始化網絡
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192,168,60,200),    // IP位址
             IPAddress(192,168,60,254),     // 閘道（gateway）位址
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
  server.on("/openmusic", handle_openmusic);//打開音樂
  server.on("/openLED", handle_openLED);//開啟led
  
  server.begin();
  Serial.println("HTTP server started");
  
  pinMode(doorpin, INPUT);  //霍爾門磁
  pinMode(LEDpin,OUTPUT);

  pinMode(OutputRadioPin,OUTPUT);
  music();
}

void loop() {
  windowstate = digitalRead(doorpin);
  Serial.println(windowstate);

  if(windowstate==1){
    digitalWrite(LEDpin, HIGH);
    server.send(200, "text/html",  SendHTML(windowstate));
    music();
  }
  if(windowstate==0){
    digitalWrite(LEDpin, LOW);
    server.send(200, "text/html",  SendHTML(windowstate));
  }
    //監聽客戶請求並處理
  server.handleClient();
  delay(500);
}

void music() {//放音樂
  for (int thisNote = 0; thisNote < 11; thisNote++) {
   //四分音符= 1000/4，八分音符= 1000/8，依此類推。
   int noteDuration = 1000/noteDurations[thisNote];
   tone(OutputRadioPin, melody[thisNote],noteDuration);


   //為了區分音符，請在它們之間設置最短時間。
    //加速的持續時間+ 30％似乎效果良好：
   int pauseBetweenNotes = noteDuration * 1.0;
   delay(pauseBetweenNotes);
   // 音符間的空白時間
   noTone(8);
  }
}
void blink()
{
    digitalWrite(LEDpin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(100);                       // wait for a second
    digitalWrite(LEDpin, LOW);    // turn the LED off by making the voltage LOW
    delay(100);                       // wait for a second 
}

void handle_openmusic() {//打開音樂
   server.send(200, "text/html",  SendHTML(windowstate));
  music();
    Serial.println("手動放音樂");
  
}
void handle_openLED() {//開啟led
  server.send(200, "text/html",  SendHTML(windowstate));
  blink();
  blink();
  blink();
  Serial.println("手動開啟led");
}

String SendHTML(uint8_t windowstate){
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
   if(windowstate==1)
  {ptr +="<p>大門狀態: 開啟</p>\n";}
  else
  {ptr +="<p>大門狀態: 關閉</p>\n";}
  ptr +="<a class=\"button button-off\" href=\"/openmusic\">放音樂</a>\n";
  ptr +="<a class=\"button button-on\" href=\"/openLED\">開啟led</a>\n";

   ptr +="<hr><h3>power by kunlex ,davidou 2019 </h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
