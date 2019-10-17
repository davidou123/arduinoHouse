#include <MQ135.h> //https://github.com/MurrayBoz/NodeMCU-MQ135/blob/master/libraries/MQ135-master.zip 需要下載library來用
//本項測試用打火機放瓦斯出來 就可以得到很高的ppm了
#define ANALOGPIN A0

int PPMStatus = 0;
int PPMStatusOld = 0;

float  airTemperature, airHumidity, ppm, ppmbalanced, rzero;
int initStep=1;                 // 1 = Connection in progress / 2 = Connection Done 

MQ135 gasSensor = MQ135(ANALOGPIN);
int led = 2;    //LED 2是接D4
//伺服馬達相關
  #include <Servo.h>
  Servo myservo;  // create servo object to control a servo
  int windowstate=0;//窗戶開關狀態 0是關 1是開
//伺服馬達結束

//LED字幕相關套件
#include <Wire.h>  // Arduino IDE 內建
// LCD I2C Library，從這裡可以下載：
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
#include <LiquidCrystal_I2C.h>

// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // 設定 LCD I2C 位址

//LED字幕套件結束

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
   Serial.begin(9600);
   pinMode(led, OUTPUT);

   lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光

  // 閃爍三次
  for(int i = 0; i < 3; i++) {
    lcd.backlight(); // 開啟背光
    delay(250);
    lcd.noBacklight(); // 關閉背光
    delay(250);
  }
  lcd.backlight();
  myservo.attach(D7);//把接腳接在D7上面
  myservo.write(90); //一開始先置中90度

  // 輸出初始化文字
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Hello, world!");
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("Kunlex corp.");

  //初始化網絡
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192,168,60,202), // IP位址
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
  server.on("/openwindow", handle_openwindow);  //打開窗戶
  server.on("/closewindow", handle_closewindow);//關閉窗戶
  server.on("/api", handle_api);//api
  
  server.begin();
  Serial.println("HTTP server started");
  handle_closewindow() ;//初始關閉窗戶

  delay(5000);
  lcd.clear();
}

void loop() {
  // 當時室內大約的溫度跟濕度，讓 getCorrectPPM進行校正
  airTemperature = 28;
  airHumidity = 50;

  rzero = gasSensor.getRZero(); //取得rzero 校準值
  Serial.print("RZero=");
  Serial.println(rzero);    // 持續顯示 rzero 值
   
  ppm = gasSensor.getPPM(); // 取得 ppm 值
  Serial.print("PPM=");
  Serial.println(ppm); 
  
  ppmbalanced = gasSensor.getCorrectedPPM(airTemperature, airHumidity);  // 取得修正的 ppm 值
  Serial.print("PPM Corrected=");
  Serial.println(ppmbalanced); 
  Serial.println();
  if(ppm>100){
    digitalWrite(led, HIGH);
      // 閃爍5次
    for(int i = 0; i < 5; i++) {
      lcd.noBacklight(); // 關閉背光
      delay(100);
      lcd.backlight(); // 開啟背光
      delay(100);

    }
    Serial.print(F("瓦斯外洩開窗"));
    myservo.write(140); 
    windowstate=1;
    server.send(200, "text/html",  SendHTML(windowstate));

  }else{
    Serial.print(F("關窗"));
    myservo.write(20); 
    windowstate=0;
    server.send(200, "text/html",  SendHTML(windowstate));
    digitalWrite(led, LOW);
  }
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("RZero=");
  lcd.print(rzero);
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("PPM =");
  lcd.print(ppmbalanced);
  delay(1000);
  
   //監聽客戶請求並處理
  server.handleClient();
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
void handle_api() {//api
   server.send(200, "text/html",  api(windowstate));

}
String api(uint8_t windowstate){
  String ptr ="{\"RZero\": \""+ String(rzero) +"\"";
         ptr +=",\"PPM\":\""+ String(ppm) +"\",\"PPM Corrected\":\""+ String(ppmbalanced)+"\""; 
         ptr +=",\"windowstate\":\""+ String(windowstate)+"\"}";
  return ptr;
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
    ptr +="<hr><h3>RZero : "+ String(rzero) +"  PPM="+ String(ppm) +" <br>PPM Corrected= "+ String(ppmbalanced) +"</h3>\n";
   if(windowstate==1)
  {ptr +="<p>窗戶狀態: 開啟</p><a class=\"button button-off\" href=\"/closewindow\">關窗戶</a>\n";}
  else
  {ptr +="<p>窗戶狀態: 關閉</p><a class=\"button button-on\" href=\"/openwindow\">開窗戶</a>\n";}

   ptr +="<hr><h3>power by kunlex ,davidou 2019 </h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
