#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

const int trig1 = 12;  
const int echo1 = 11;
const int trig2 = 10;  
const int echo2 = 9;

int relay = 6;
unsigned long time1;
unsigned long time2; 

int kccb1bandau;
int kccb2bandau;
//int khoangcach;
int khoangcach1;
int khoangcach2;          
//int gioihan = 25;
//int luotvao;
//int luotra;

int songuoitrongphong;
String queue = "";
String den;
int timeoutcounter=0;
LiquidCrystal_I2C lcd(0x27,16,2);
void setup() {
  // put your setup code here, to run once:
  lcd.begin(); //initialize the lcd
  lcd.clear();
  lcd.backlight(); //open the backlight 
  lcd.setCursor(1,0);
  lcd.print("xin chao");
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT); 
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(relay, OUTPUT);
  Serial.begin(9600);
  songuoitrongphong=0;
  
  delay(500);
  dokhoangcach1();
  kccb1bandau=khoangcach1;
  dokhoangcach2();
  kccb2bandau=khoangcach2;
  Serial.print(kccb1bandau);Serial.print("-");
  Serial.println(kccb2bandau);
  den="";
}

void loop() {
  // put your main code here, to run repeatedly:
  khoangcach1 = 0;
  khoangcach2 = 0;
  dokhoangcach1();
  dokhoangcach2();
  //delay(500);
  if(songuoitrongphong==0){digitalWrite(relay,LOW);den="Tat";}
  else {digitalWrite(relay,HIGH);den="Sang";}
  if(khoangcach1<30 && queue.charAt(0)!='1')
  {
    queue+="1";
  }
  else if(khoangcach2<30 && queue.charAt(0)!='2')
  {
    queue+="2";
  }
  if(queue.equals("12"))
  {
    songuoitrongphong++;
    Serial.print("Hang doi: ");
    Serial.println(queue);
    queue="";
    delay(550);
  }
 else if(queue.equals("21") && songuoitrongphong>0)
 {
    songuoitrongphong--;
    Serial.print("Hang doi: ");
    Serial.println(queue);
    queue="";
    delay(550);
 }
 // Reset hàng đợi nếu giá trị hàng đợi sai (11 hoặc 22) hoặc hết thời gian chờ
  if(queue.length()>2 || queue.equals("11") || queue.equals("22") ||timeoutcounter>200)
  {
      queue="";
  }
  if(queue.length()==1)
  {timeoutcounter++;}
  else {timeoutcounter=0;}

  Serial.print("Hang doi: ");
  Serial.print(queue);
  Serial.print(" CB1: ");
  Serial.print(khoangcach1);
  Serial.print(" CB2: ");
  Serial.println(khoangcach2);
  hienthichuoi(0,0,"So Nguoi: ");
  hienthiso(11,0,songuoitrongphong);
  hienthichuoi(0,1,"Den Dang: ");
  hienthichuoi(11,1,den);
  //Serial.println(songuoitrongphong);
}
void dokhoangcach1()
{

  digitalWrite(trig1, LOW); 
  delayMicroseconds(2);
  digitalWrite(trig1, HIGH);  
  delayMicroseconds(10); 
  digitalWrite(trig1, LOW); 


  // Đo độ rộng xung HIGH ở chân echo./
  time1 = pulseIn(echo1, HIGH);

  khoangcach1 = time1 / 2 / 29.412;

}
void dokhoangcach2()
{

  digitalWrite(trig2, LOW); 
  delayMicroseconds(2);
  digitalWrite(trig2, HIGH);  
  delayMicroseconds(10); 
  digitalWrite(trig2, LOW); 


  // Đo độ rộng xung HIGH ở chân echo.
  time2 = pulseIn(echo2, HIGH);

  khoangcach2 = time2 / 2 / 29.412;

}

void hienthichuoi(int cot, int dong, String chuoi)
{
  lcd.setCursor(cot,dong);
  for(int j=0;j<chuoi.length();j++)
    {
      lcd.print(chuoi[j]);
    } 
    if(chuoi.length()==3){lcd.setCursor(cot+3,dong);lcd.print(" ");}
}
void hienthiso(int cot, int dong, int so)
{
  String chuoi=String(so);
  lcd.setCursor(cot-(chuoi.length()-1),dong);
  for(int j=0;j<chuoi.length();j++)
    {
      lcd.print(chuoi[j]);
    } 
  if(chuoi.length()==2){lcd.setCursor(cot-2,dong);lcd.print(" ");}
  if(chuoi.length()==1){lcd.setCursor(cot-2,dong);lcd.print(" ");lcd.setCursor(cot-1,dong);lcd.print(" ");}
}
