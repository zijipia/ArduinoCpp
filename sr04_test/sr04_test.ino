/*hướng dẫn kết nối:
 *chân D dò line trái trên  : nối với chân A0 của arduino
 *chân D dò line trái dưới  : nối với chân A1 của arduino
 *chân D dò line phải trên  : nối với chân A2 của arduino
 *chân D dò line phải dưới  : nối với chân A3 của arduino
 *INA-INB-INC-IND của module L298 nối với 5-6-9-10 của arduino
 *trig  và echo của SRF-04 nối với 8-7 của arduino
 */
#include <NewPing.h>

//dong co
const int outA = 5;
const int outB = 6;//banh trai

const int outC = 9;
const int outD = 10;//banh phai
//SRF-04
#define TRIGGER_PIN  1
#define ECHO_PIN     2
#define MAX_DISTANCE 50
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//cai dat cac gioi han toc do
int pulseRight=220;
int pulseLeft=220;
int pulseRightmin=150;
int pulseLeftmin=150;
int pulseRightmax=255;
int pulseLeftmax=255;

int kc,kctruoc;
//////////////////////////////////////////////////////////Chuong trinh cai dat cac thong so ban dau
void setup() 
{ 
  ////////control motor DC//////////////
  //Cac chan dieu khien motor la cac chan xuat
  pinMode(outA, OUTPUT);pinMode(outB, OUTPUT);pinMode(outC, OUTPUT);pinMode(outD, OUTPUT);
  //dung 2 dong co
  motor_left_run(0,0);motor_right_run(0,0);
  //Khoi dong UART
  Serial.begin(9600);
  //doc lai gia tri khoang cach va do line
 dockc();
   
}
///// Chuong trinh con doc khoang cach va luu vao bien kc
void dockc()
{ //doc 2 lan de chong nhieu
  kctruoc=sonar.ping_cm();delay(10);
  kc=sonar.ping_cm();delay(10);
  while (((kc==0)&&(kctruoc!=0))||((kc!=0)&&(kctruoc==0))) {kctruoc=kc;kc=sonar.ping_cm();delay(10);}
//Serial.print("Ping: ");Serial.print(kc); Serial.println("cm");
}
//////////////////////////////
/////////Chuong trinh chinh
//////////////////////////////
void loop() 
{ dockc();
  if (kc) {Serial.println("max cong suat");   motor_left_run(pulseLeftmax,0);motor_right_run(pulseRightmax,0);}
  else {Serial.println("xoay ");  motor_left_run(pulseLeft,0);motor_right_run(pulseRight,1);}
}
/////////////////////
/*********************************************************************
        SUBROTINES
*********************************************************************/
//******dieu khien banh Trai******
//********************************
void motor_left_run(int speedmotor,boolean dir)
{// speedmotor 0 --> 255 (0=min  /// 255= max)
 // dir: 0 false  --> run head (quay thuan)
 //      1 true   --> run back (quay nghich)    
 if (dir == 0)
 {// quay thuan
   analogWrite (outB, speedmotor);
   digitalWrite(outA, LOW);
 }
 else
 {// quay nghich
   analogWrite (outB, 255 - speedmotor);
   digitalWrite(outA, HIGH);
 } 
}
//******dieu khien banh Phai******
//********************************
void motor_right_run(int speedmotor,boolean dir)
{// speedmotor 0 --> 255 (0=min  /// 255= max)
 // dir: 0 false  --> run head (quay thuan)
 //      1 true   --> run back (quay nghich)    
 if (dir == 0)
 {// quay thuan
   analogWrite (outD, speedmotor);
   digitalWrite(outC, LOW);
 }
 else
 { // quay nghich
   analogWrite (outD, 255 - speedmotor);
   digitalWrite(outC, HIGH);
 } 
}