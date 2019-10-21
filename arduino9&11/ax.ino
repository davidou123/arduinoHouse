#include <Wire.h> //調用arduino自帶的I2C庫
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ASUS_iot_2G";
const char* password = "1121314151";

ESP8266WebServer server(80);
void homepage() {
  server.send(200, "text/html",  SendHTML());
  Serial.println("同學開啟了網頁");
}

#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

int LED3=D3;
int LED5=D5;
int LED6=D6;
int LED7=D7;
int LED8=D8;

 
int ADXAddress = 0xA7>>1; //轉換為7位地址
int reading = 0;
int val = 0;
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
double Xg,Yg,Zg;

double Xglast = 0;
double Yglast = 0;
double Zglast = 0;
void setup()
{
 Serial.begin(9600);
 Wire.begin(); //初始化I2C
 delay(100);
 Wire.beginTransmission(ADXAddress);
 Wire.write(Register_2D);
 Wire.write(8);
 Wire.endTransmission();

 pinMode(LED3, OUTPUT);
 pinMode(LED5, OUTPUT);
 pinMode(LED6, OUTPUT);
 pinMode(LED7, OUTPUT);
 pinMode(LED8, OUTPUT);
 randomSeed(analogRead(A0));//亂數種子

 //初始化網絡
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(IPAddress(192,168,60,204), // IP位址
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
  server.on("/openLED", ledallblink);//開啟led
  server.on("/openLED2", ledcircle);//開啟led2
  
  server.begin();
  Serial.println("HTTP server started");
  ledcircle();
}
 
void loop()
{
 Wire.beginTransmission(ADXAddress);
 Wire.write(Register_X0);
 Wire.write(Register_X1);
 Wire.endTransmission();
 Wire.requestFrom(ADXAddress,2);
 if(Wire.available()<=2);
 {
   X0 = Wire.read();
   X1 = Wire.read();
   X1 = X1<<8;
   X_out = X0+X1;
 }
 
 Wire.beginTransmission(ADXAddress);
 Wire.write(Register_Y0);
 Wire.write(Register_Y1);
 Wire.endTransmission();
 Wire.requestFrom(ADXAddress,2);
 if(Wire.available()<=2);
 {
   Y0 = Wire.read();
   Y1 = Wire.read();
   Y1 = Y1<<8;
   Y_out = Y0+Y1;
 }
 
 Wire.beginTransmission(ADXAddress);
 Wire.write(Register_Z0);
 Wire.write(Register_Z1);
 Wire.endTransmission();
 Wire.requestFrom(ADXAddress,2);
 if(Wire.available()<=2);
 {
   Z0 = Wire.read();
   Z1 = Wire.read();
   Z1 = Z1<<8;
   Z_out = Z0+Z1;
 }
 
 Xg = X_out/256.00;//把輸出結果轉換為重力加速度g,精確到小數點後2位。
 Yg = Y_out/256.00;
 Zg = Z_out/256.00;

  Serial.print("X:");
  Serial.print(Xg);
  Serial.print("\t");
  
  Serial.print("Y:");  
  Serial.print(Yg);
  Serial.print("\t");
  
  Serial.print("z:");
  Serial.print(Zg);
  Serial.print("\t");

  int i = random(2); 
  Serial.print("亂數:");
  Serial.print(i);
  if(abs(Xglast-Xg+Yglast-Yg+Zglast-Zg) >5 ){//這邊數值可以調，越低靈敏度越高
      Serial.print("振動");
      
      if(i==0){
        ledcircle();
        }
      else if(i==1){
        ledallblink();
        }
  }

 Serial.println();
 Xglast = Xg;
 Yglast = Yg;
 Zglast = Zg;
 //監聽客戶請求並處理
  server.handleClient();
 delay(200); //延時0.3秒，刷新頻率這裡進行調整
 
 }

 void ledcircle(){
    server.send(200, "text/html",  SendHTML());
    Serial.print("ledcircle");
    for(int i=1;i<4;i++){
      digitalWrite(LED3, HIGH);
      delay(100);
      digitalWrite(LED5, HIGH);
      delay(100);
      digitalWrite(LED6, HIGH);
      delay(100);
      digitalWrite(LED7, HIGH);
      delay(100);
      digitalWrite(LED8, HIGH);
      delay(200);
      digitalWrite(LED3, LOW);
      delay(100);
      digitalWrite(LED5, LOW);
      delay(100);
      digitalWrite(LED6, LOW);
      delay(100);
      digitalWrite(LED7, LOW);
      delay(100);
      digitalWrite(LED8, LOW);
      delay(200);
    }
  }
 void ledallblink(){
    server.send(200, "text/html",  SendHTML());
    Serial.print("ledallblink");
    for(int i=1;i<5;i++){
      digitalWrite(LED3, HIGH);
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      digitalWrite(LED7, HIGH);
      digitalWrite(LED8, HIGH);
      delay(100);
      digitalWrite(LED3, LOW);
      digitalWrite(LED5, LOW);
      digitalWrite(LED6, LOW);
      digitalWrite(LED7, LOW);
      digitalWrite(LED8, LOW);
      delay(100);
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
  ptr +="X:"+String(Xg)+"Y:"+String(Yg)+"z:"+String(Zg)+"<br>";
  
  ptr +="<a class=\"button button-off\" href=\"/openLED\">開啟led閃爍</a>\n";
  ptr +="<a class=\"button button-on\" href=\"/openLED2\">開啟led繞圈</a>\n";

   ptr +="<hr><h3>power by kunlex ,davidou 2019 </h3>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
