#include <TimerOne.h>
int outA =9;
int outB =10;
int total_sample =38.33333;
int f_sine =60;
int f_pwm =4600;
float sine[200];
 float phi=3.14;
 int flag = 0;
 int sample=0;
 int feedback = A0;
 int feedbackval;
 int A=1;
 
void setup() {
Serial.begin(9600);
 for (int angle=0;angle<total_sample;angle++){
     float deg= angle*(180./total_sample)*phi/180;
     sine[angle]=sin(deg); Serial.println(angle);
 }
  float t_pwm=(1000./f_pwm)*1000;
  delay(100);
  pinMode(feedback, INPUT);
   A = 0;
 Timer1.initialize(t_pwm); 
  Timer1.attachInterrupt(create_sine);
  }

void loop() {
   
   feedbackval = analogRead(feedback);

   while (512 > feedbackval) {
     if (A == 995) {
       feedbackval = analogRead(feedback);
     }
     else {
       A = A + 1;
       feedbackval = analogRead(feedback);
     }    Serial.println(A);
   }
   while (512 < feedbackval) {
     if (A == 0) {
       feedbackval = analogRead(feedback);
     }
     else {
       A = A - 1;
       feedbackval = analogRead(feedback);
     }   Serial.println(A);
   }
 }

void create_sine(){
     if(sample>total_sample && flag==0){
         flag=1;
        sample=0;
     }
          if(sample>total_sample && flag==1){
        flag=0;
        sample=0;
      }
      sample++; 
      if(flag==0){
          Timer1.pwm(outA,(sine[sample]*A));
          Timer1.pwm(outB,0);
  }
      if(flag==1){
           Timer1.pwm(outB,(sine[sample]*A));
          Timer1.pwm(outA,0);
      }
 }
