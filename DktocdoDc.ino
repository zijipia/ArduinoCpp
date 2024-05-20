int TocDo = 50; // 0 -> 255
bool dir = 1; // chieu quay
void setup() {
  // put your setup code here, to run once:
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);
pinMode(10,OUTPUT);
pinMode(11,OUTPUT);
digitalWrite(12,1);
digitalWrite(13,1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(dir){
  analogWrite(10,TocDo);
  digitalWrite(11,0);
  }else{
  analogWrite(11,TocDo);
  digitalWrite(10,0);
  }
}
