// 開發版選Wemos D1 R1
//溫溼度感測器相關
// 你可能需要安裝下列的函式庫 Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
#include "DHT.h"
 
#define DHTPIN D6 // D1 mini的接口請接D6
 
//切換你的溫溼度感應模組
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22
 
DHT dht(DHTPIN, DHTTYPE);
float h,t,f,hif,hic ;//設在這邊當全域變數

//溫溼度感測器結束
//伺服馬達相關
  #include <Servo.h>
  Servo myservo;  // create servo object to control a servo
  int windowstate;//窗戶開關狀態 0是關 1是開
//伺服馬達結束
//LED字幕相關
#include <Wire.h> // Arduino IDE 內建
// LCD I2C Library，從這裡可以下載：
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
#include <LiquidCrystal_I2C.h>
 
// Set the pins on the I2C chip used for LCD connections:
// addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // 設定 LCD I2C 位址
//LED字幕結束

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ASUS_iot_2G";
const char* password = "1121314151";

ESP8266WebServer server(80);


void homepage() {
  server.send(200, "text/html",  SendHTML(windowstate));
  Serial.println("同學開啟了網頁");
}

void setup(void) {
  //初始化串口
 Serial.begin(9600);
 Serial.println(F("DHTxx test!"));
 dht.begin();
 
 lcd.begin(16, 2); // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
 
// 閃爍三次
 for(int i = 0; i < 3; i++) {
  lcd.backlight(); // 開啟背光
  delay(250);
  lcd.noBacklight(); // 關閉背光
  delay(250);
 }
 lcd.backlight();
 
// 輸出初始化文字
 lcd.setCursor(0, 0); // 設定游標位置在第一行行首
 lcd.print("Hello, world!");
 delay(1000);
 lcd.setCursor(0, 1); // 設定游標位置在第二行行首
 lcd.print("davidou.org");
 delay(3000);
 
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("System boot");
 lcd.setCursor(0, 1);
 lcd.print("Wait a sec.."); 

  myservo.attach(D7);  //把接腳接在D7上面
  myservo.write(90); // 一開始先置中90度
  //初始化網絡
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192,168,60,199),    // IP位址
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
  server.on("/openwindow", handle_openwindow);//打開窗戶
  server.on("/closewindow", handle_closewindow);//關閉窗戶
  
  server.begin();
  Serial.println("HTTP server started");
    handle_closewindow() ;//初始關閉窗戶
}

void loop() {
  // 等待幾秒讓感測器感測一下溫溼度
delay(2000);
 
// Reading temperature or humidity takes about 250 milliseconds!
// 感測器的感測秒數需要等待超過2秒才會有新的數值 (DHT系列是很慢的感測元件)
 h = dht.readHumidity();
// Read temperature as Celsius (the default)
 t = dht.readTemperature();
// Read temperature as Fahrenheit (isFahrenheit = true)
 f = dht.readTemperature(true);
 
// Check if any reads failed and exit early (to try again).
if (isnan(h) || isnan(t) || isnan(f)) {
Serial.println(F("Failed to read from DHT sensor!"));
//return;
}
 
// Compute heat index in Fahrenheit (the default)
 hif = dht.computeHeatIndex(f, h);
// Compute heat index in Celsius (isFahreheit = false)
 hic = dht.computeHeatIndex(t, h, false);
 
Serial.print(F("濕度Humidity: "));
Serial.print(h);
Serial.print(F("% 溫度Temperature: "));
Serial.print(t);
Serial.print(F("°C "));
Serial.print(f);
Serial.print(F("°F 體感溫度Heat index: "));
Serial.print(hic);
Serial.print(F("°C "));
Serial.print(hif);
Serial.println(F("°F"));
 
lcd.setCursor(0, 0);
lcd.print("Humidity: " );
lcd.print( h,1);//顯示小數點後一位就好
lcd.print("%" );
lcd.setCursor(0, 1);
lcd.print("Temper: " );
lcd.print( t,1);
lcd.print(" C" );

  //監聽客戶請求並處理
  server.handleClient();

  if(t>29){
    Serial.print(F("高溫開窗"));
   myservo.write(140); 
   windowstate=1;
   server.send(200, "text/html",  SendHTML(windowstate));
   delay(1000);
    }
  if(t<29){
    Serial.print(F("低溫關窗"));
    myservo.write(20); 
    windowstate=0;
    server.send(200, "text/html",  SendHTML(windowstate));
    delay(10000);//手動開窗停留10秒再回復抓溫度
    }
}

void handle_openwindow() {//打開窗戶
   myservo.write(140); 
   windowstate=1;
       Serial.print(F("手動開窗"));
   server.send(200, "text/html",  SendHTML(windowstate));
   delay(3000);
}
void handle_closewindow() {//關閉窗戶
 myservo.write(20); 
 windowstate=0;
 Serial.print(F("手動關窗"));
 server.send(200, "text/html",  SendHTML(windowstate));
 delay(10000);//手動關窗停留10秒再回復抓溫度
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
    ptr +="<hr><h3>現在溫度 : "+ String(t) +"°C  濕度 : "+ String(h) +"% <br> 體感溫度Heat index : "+ String(hif) +"°F  "+ String(hic) +"°C</h3>\n";
   if(windowstate==1)
  {ptr +="<p>窗戶狀態: 開啟</p><a class=\"button button-off\" href=\"/closewindow\">關窗戶</a>\n";}
  else
  {ptr +="<p>窗戶狀態: 關閉</p><a class=\"button button-on\" href=\"/openwindow\">開窗戶</a>\n";}

   ptr +="<hr><h3>power by kunlex ,davidou 2019 </h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
