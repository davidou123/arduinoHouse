#include <Wire.h> //調用arduino自帶的I2C庫

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
  // print a tab between values:
  Serial.print("\t");
     Serial.print("y:");
  Serial.print(Yg);
  // print a tab between values:
  Serial.print("\t");
     Serial.print("z:");
  Serial.print(Zg);
  Serial.print("\t");

  Serial.print(Xglast-Xg+Yglast-Yg+Zglast-Zg);
  if(abs(Xglast-Xg+Yglast-Yg+Zglast-Zg) >5 ){//這邊數值可以調，越低靈敏度越高
      Serial.print("振動");

      int i = random(0,2);
      if(i=0){
        ledcircle();
        }
      else if(i=1){
        ledallblink();
        }
  }
 
      
 Serial.println();
 Xglast = Xg;
 Yglast = Yg;
 Zglast = Zg;
 delay(200); //延時0.3秒，刷新頻率這裡進行調整
 }

 void ledcircle(){
    for(int i=1;i<3;i++){
      digitalWrite(LED3, HIGH);
      delay(50);
      digitalWrite(LED5, HIGH);
      delay(50);
      digitalWrite(LED6, HIGH);
      delay(50);
      digitalWrite(LED7, HIGH);
      delay(50);
      digitalWrite(LED8, HIGH);
      delay(50);
      digitalWrite(LED3, LOW);
      delay(50);
      digitalWrite(LED5, LOW);
      delay(50);
      digitalWrite(LED6, LOW);
      delay(50);
      digitalWrite(LED7, LOW);
      delay(50);
      digitalWrite(LED8, LOW);
      delay(50);
    }
  }
 void ledallblink(){
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
