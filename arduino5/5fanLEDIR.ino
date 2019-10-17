int sensor = D7;        //定義紅外線sensor接脚為2
int led = 16;    //LED 16是接D0 
int fan = D5;
int val = 0;           //定義變量儲存傳感器的返回值
int fanState;//風扇開關狀態 0是關 1是開
float h,t,f,hif,hic ;//設在這邊當全域變數
// 開發版選Wemos D1 R1
 
// 你可能需要安裝下列的函式庫 Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
 
#include "DHT.h"
 
#define DHTPIN D6 // D1 mini的接口請接D6
 
//切換你的溫溼度感應模組
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22
 
DHT dht(DHTPIN, DHTTYPE);

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ASUS_iot_2G";
const char* password = "1121314151";

ESP8266WebServer server(80);


void homepage() {
  server.send(200, "text/html",  SendHTML(fanState));
  Serial.println("同學開啟了網頁");
}

void setup()
{
   Serial.begin(9600);    //設置傳輸鮑率為9600
   Serial.println(F("DHTxx test!"));
   dht.begin();
   pinMode(sensor, INPUT);        //設定sensor接脚為輸入狀態
   pinMode(led, OUTPUT);
   pinMode(fan, OUTPUT);

 //初始化網絡
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192,168,60,201),    // IP位址
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
  server.on("/openfan", handle_openfan);//打開風扇
  server.on("/closefan", handle_closefan);//關閉風扇
  server.on("/openLED", handle_openLED);//打開LED
   
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  delay(1000);
  //監聽客戶請求並處理
  server.handleClient();
  
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
 


   val = digitalRead(sensor);    //讀傳感訊息
   if( val== HIGH)
     //如果檢測到人移動
   {
       Serial.println("有人經過");
       digitalWrite(led, HIGH);//開燈
   }
     //如果沒有檢測到人移動
   else
   {
    Serial.println("無人或人靜止不動"); 
    digitalWrite(led, LOW); //關LED
   }

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
  if(t>=29){
    Serial.print(F("高溫開fan"));
    fanState=1;
    server.send(200, "text/html",  SendHTML(fanState));    
    digitalWrite(fan, HIGH);
    }
  if(t<29){
    Serial.print(F("低溫關fan"));
    fanState=0;
    server.send(200, "text/html",  SendHTML(fanState));
    digitalWrite(fan,LOW );
    }
    
}

void handle_openfan() {//開電扇5秒
   fanState=1;
   server.send(200, "text/html",  SendHTML(fanState));
   digitalWrite(fan, HIGH);
   Serial.print(F("手動開風扇"));
   delay(5000);//手動關風扇停留5秒
}
void handle_closefan() {//關電扇
    digitalWrite(fan,LOW );
    fanState=0;
    Serial.print(F("手動關風扇"));
    server.send(200, "text/html",  SendHTML(fanState));
    delay(5000);//手動關風扇停留5秒
}

void handle_openLED() {//開LED10秒
    server.send(200, "text/html",  SendHTML(fanState));
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led,LOW );
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led,LOW );
    delay(100);
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led,LOW );
    delay(100);   
    digitalWrite(led, HIGH);
    delay(5000);//停留5秒
}

String SendHTML(uint8_t fanState){
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
   ptr +="<a class=\"button button-off\" href=\"/openLED\">閃爍LED</a>\n";
   if(fanState==1)
  {ptr +="<p>風扇狀態: 開啟</p><a class=\"button button-off\" href=\"/closefan\">關風扇5秒</a>\n";}
  else
  {ptr +="<p>風扇狀態: 關閉</p><a class=\"button button-on\" href=\"/openfan\">開風扇5秒</a>\n";}

   ptr +="<hr><h3>power by kunlex ,davidou 2019 </h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
