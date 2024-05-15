// 1 GIỜ 35 PHÚT SÁNG NGÀY 24/05/2023

#include <Keypad.h> // thư viện bàn phím

#include <EEPROM.h> // thư viện eeprom

#include <LiquidCrystal_I2C.h> // thư viện ìc
LiquidCrystal_I2C lcd(0X3F,16,2);

#include <SPI.h> // thư viện SPI
#include <MFRC522.h> // thư viện MFRC522

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

const byte ROWS = 4; // 4 hàng
const byte COLS = 3; //3 cột
//các nút trên bàn phím
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'u','0','s'}
};
byte rowPins[ROWS] = {8,7,6,5}; //chân nối với hàng
byte colPins[COLS] = {4,3,2}; //chân nối với cột

//khởi tạo bàn phím
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

char customkey;

char sonhapvao;

byte vitri = 1;

byte luuxacthuc;

char matkhau[4]; 

byte nuidPICC[4]; 

long time ; 

boolean  ktid;

void TAO_MAT_KHAU(){
  lcd.setCursor(0,0);
  lcd.print("TAO MAT KHAU"); // xuất ra màn hình 

  lcd.setCursor(0,1);
  lcd.print("MAT KHAU:_ _ _ _"); // xuất ra màn hình 

  NHAP_MAT_KHAU();
  for(int i = 0; i < 4; i ++){
  EEPROM.write(i, matkhau[i]); delay(5);}
  lcd.clear();EEPROM.write(4,1);delay(5);EEPROM.write(5,10);delay(5);
  lcd.setCursor(2,0);lcd.print("TAO MAT KHAU");
  lcd.setCursor(3,1);lcd.print("THANH  CONG");delay(1500);
}

void NHAP_MAT_KHAU(){ 
  for(byte i = 0; i < 4; i++){ // tạo vòng lặp 4 số cần nhập

  sonhapvao = '0'; // mặc định số nhập vào = 0
  matkhau[i] = sonhapvao;
  lcd.setCursor(9+i*2,1);
  lcd.print(sonhapvao); 

  while(1){
  sonhapvao = customKeypad.getKey();
  lcd.setCursor(9+i*2,1);lcd.blink();delay(50);lcd.noBlink();
  if(sonhapvao && sonhapvao != 's' && sonhapvao != 'u'){ 
  matkhau[i] = sonhapvao;
  lcd.setCursor(9+i*2,1);
  lcd.print(matkhau[i]);  }

  else if(sonhapvao == 's'){   break;  }   }   }
}

void setup(){
  Serial.begin(9600);

  SPI.begin();         // khởi động SPI
  rfid.PCD_Init(); // Init MFRC522 

  lcd.init(); // khởi động lcd
  lcd.backlight(); // bật đèn nền
  if(EEPROM.read(4)==0){
  TAO_MAT_KHAU(); }

  MAN_HINH_CHO(); // màn hình chờ

  pinMode(A0, OUTPUT); analogWrite(A0, 0);
  pinMode(A1, INPUT_PULLUP); vitri = 1;
}

void MAN_HINH_CHO(){
  lcd.setCursor(0,0);
  lcd.print(" DAT   THE   VAO");
  lcd.setCursor(0,1);
  lcd.print("MINH DOAN  DDT02");
}
  
void loop(){

  customkey = customKeypad.getKey();

  QUET_THE(); // CT quét thẻ từ

  if(customkey && customkey ^ 'u' && customkey ^ 's') { lcd.clear(); XAC_THUC_MK(); delay(1000); lcd.clear();MAN_HINH_CHO(); }

  if(customkey == 'u'){ lcd.clear();MAT_KHAU_CT(); delay(1000); lcd.clear();
  if(luuxacthuc == 4) {
  while(1) {
  customkey = customKeypad.getKey();
  if(customkey == 'u'){ vitri ++; delay(10); lcd.clear();} 
    if(vitri < 3){
    lcd.setCursor(1,0); lcd.print("DOI MAT KHAU");
    lcd.setCursor(1,1); lcd.print("THEM ID THE");

    lcd.setCursor(0,vitri-1); lcd.print(">");  }

    if(vitri > 2 && vitri < 5){
    lcd.setCursor(1,0); lcd.print("XOA ID THE");
    lcd.setCursor(1,1); lcd.print("RESET HE THONG");
    lcd.setCursor(0,vitri-3); lcd.print(">"); }

    if( vitri == 5){
    lcd.setCursor(1,0); lcd.print("THOAT MENU");
    lcd.setCursor(0,vitri-5); lcd.print(">");   }

    if(vitri == 6) { vitri = 1; } Serial.print("VI TRI: "); Serial.println(vitri); 

    if(vitri == 1 && customkey == 's' ) { lcd.clear(); DOI_MAT_KHAU();delay(500); vitri = 1;lcd.clear(); }
    if(vitri == 2 && customkey == 's' ) { lcd.clear(); delay(100); while(1) { THEM_ID(); customkey = customKeypad.getKey(); if(customkey == 'u') { break; } }  delay(1000);  vitri = 2; lcd.clear(); }
    if(vitri == 3 && customkey == 's' ) { lcd.clear(); delay(100); while(1) { XOA_ID(); customkey = customKeypad.getKey(); if(customkey == 'u') { break; } }  delay(1000); vitri = 3; lcd.clear(); }
    if(vitri == 4 && customkey == 's' ) { lcd.clear(); delay(100); RESET_HE_THONG(); delay(1000); vitri = 1; lcd.clear(); break; }
    if(vitri == 5 && customkey == 's' ) { lcd.clear(); delay(500); vitri = 1; MAN_HINH_CHO(); break; } } } }
    else { MAN_HINH_CHO(); }
}

void QUET_THE(){
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) {return; }
  for(int idquet = 0; idquet < EEPROM.read(5); idquet=idquet+4){
  if (rfid.uid.uidByte[0] == EEPROM.read(10+idquet) && rfid.uid.uidByte[1] == EEPROM.read(11+idquet) && rfid.uid.uidByte[2] == EEPROM.read(12+idquet) && rfid.uid.uidByte[3] == EEPROM.read(13+idquet) ){
  ktid = true; Serial.print(" ID QUET : "); Serial.println(idquet+10); idquet = EEPROM.read(5); } else { ktid = false; } }

  if( ktid == true ) {lcd.clear(); lcd.setCursor(3, 0); lcd.print("THE HOP LE"); analogWrite(A0, 255); delay(3000); analogWrite(A0, 0); lcd.clear(); MAN_HINH_CHO();
  }
  else { ktid = false; lcd.clear(); lcd.setCursor(1, 0); lcd.print("KHONG TIM THAY"); analogWrite(A0, 0); delay(500); lcd.clear(); MAN_HINH_CHO();}
  Serial.print(" KT ID : "); Serial.println(ktid); 
  rfid.PICC_HaltA();rfid.PCD_StopCrypto1();
}

void MAT_KHAU_CT(){
  byte xacthuc = 0;
  lcd.setCursor(0,0);lcd.print("NHAP MAT KHAU !");
  lcd.setCursor(0,1);lcd.print("MAT KHAU:_ _ _ _");

  NHAP_MAT_KHAU();

  for(int i = 0; i < 4; i++){
    if( ((char)EEPROM.read(i)) == matkhau[i]){ xacthuc++; Serial.println(xacthuc);} }
    if(xacthuc == 4) { lcd.clear(); lcd.setCursor(0,0); lcd.print(" MAT KHAU DUNG ");}
    if(xacthuc != 4) { lcd.clear(); lcd.setCursor(1,0); lcd.print(" MAT KHAU SAI !");lcd.setCursor(0,1);lcd.print("VUI LONG THU LAI");  }  luuxacthuc = xacthuc; xacthuc = 0;
}

void XAC_THUC_MK(){
  byte xacthuc = 0;
  lcd.setCursor(0,0);lcd.print("NHAP MAT KHAU !");
  lcd.setCursor(0,1);lcd.print("MAT KHAU:_ _ _ _");

  NHAP_MAT_KHAU();

  for(int i = 0; i < 4; i++){
    if( ((char)EEPROM.read(i)) == matkhau[i]){ xacthuc++; Serial.println(xacthuc);} }
    if(xacthuc == 4) { lcd.clear(); lcd.setCursor(0,0); lcd.print(" MAT KHAU DUNG ");lcd.setCursor(1,1);lcd.print("MO KHOA CUA 3s"); analogWrite(A0, 255); delay(3000); analogWrite(A0, 0);}
    if(xacthuc != 4) { lcd.clear(); lcd.setCursor(1,0); lcd.print(" MAT KHAU SAI !");lcd.setCursor(0,1);lcd.print("VUI LONG THU LAI");  }  luuxacthuc = xacthuc; xacthuc = 0;
}

void DOI_MAT_KHAU(){
  MAT_KHAU_CT(); delay(500);
  if(luuxacthuc == 4) { lcd.clear(); TAO_MAT_KHAU(); } luuxacthuc = 0;
}

void THEM_ID(){

  if(EEPROM.read(5) > 50) { lcd.setCursor(1,1); lcd.print("FULL"); }
  else {

  lcd.setCursor(1,0); lcd.print("DAT THE VAO !");

  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) { return; }

  for(int idquet = 0; idquet < EEPROM.read(5); idquet=idquet+4){
  if (rfid.uid.uidByte[0] == EEPROM.read(10+idquet) && rfid.uid.uidByte[1] == EEPROM.read(11+idquet) && rfid.uid.uidByte[2] == EEPROM.read(12+idquet) && rfid.uid.uidByte[3] == EEPROM.read(13+idquet) ){
  ktid = true;  break; } else { ktid = false; } }

  Serial.print(" KT ID : "); Serial.println(ktid);
  
  if(ktid == true) { lcd.clear(); lcd.setCursor(0,0); lcd.print("THE DA DUOC LUU"); delay(500); lcd.clear(); }

  if(ktid  == false) {lcd.clear();
  for(int i = 0; i < 4; i++){
  nuidPICC[i] = rfid.uid.uidByte[i];
  EEPROM.write(EEPROM.read(5) + i,nuidPICC[i]); delay(5);
  Serial.print("Ô NHỚ THỨ "); Serial.print(EEPROM.read(5) + i);Serial.print(" ID "); Serial.println(EEPROM.read(i + EEPROM.read(5) ) ); }
  EEPROM.write(5, EEPROM.read(5) + 4);
  lcd.setCursor(1,0); lcd.print("DA LUU THE MOI"); delay(500); lcd.clear();   }

  rfid.PICC_HaltA();rfid.PCD_StopCrypto1(); }
}

void KIEM_TRA_ID(){
  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) {return; }
  for(int idquet = 0; idquet < EEPROM.read(5); idquet=idquet+4){
  if (rfid.uid.uidByte[0] == EEPROM.read(10+idquet) && rfid.uid.uidByte[1] == EEPROM.read(11+idquet) && rfid.uid.uidByte[2] == EEPROM.read(12+idquet) && rfid.uid.uidByte[3] == EEPROM.read(13+idquet) ){
  ktid = true; Serial.print(" ID QUET : "); Serial.println(idquet+10); idquet = EEPROM.read(5);  }
  else { ktid = false;} }
  Serial.print(" KT ID : "); Serial.println(ktid); 
  rfid.PICC_HaltA();rfid.PCD_StopCrypto1();
}

void XOA_ID(){
  int idquet = 0 ; 
  lcd.setCursor(1,0); lcd.print("DAT THE VAO !");
  lcd.setCursor(1,0); lcd.print("");

  if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial()) { return; }

  for(idquet = 0; idquet < EEPROM.read(5); idquet=idquet+4){
  if (rfid.uid.uidByte[0] == EEPROM.read(10+idquet) && rfid.uid.uidByte[1] == EEPROM.read(11+idquet) && rfid.uid.uidByte[2] == EEPROM.read(12+idquet) && rfid.uid.uidByte[3] == EEPROM.read(13+idquet) ){
  ktid = true;  break; } else { ktid = false; } }
  Serial.print("ID QUET "); Serial.println(idquet);

  if(ktid  == true) { lcd.clear();
  for(int i = 0; i < 4; i++){
  EEPROM.write(idquet + 10 + i ,0); delay(5);
  Serial.print("Ô NHỚ THỨ ");  Serial.println(idquet + 10 + i ); Serial.println(" GIA TRI: "); Serial.println( EEPROM.read(idquet + 10 + i) ); }
  lcd.setCursor(3,0); lcd.print("DA XOA THE"); delay(500); lcd.clear();}
  else { 
  lcd.clear(); lcd.setCursor(1, 0); lcd.print("KHONG TIM THAY"); delay(500); lcd.clear();
  } 

  rfid.PICC_HaltA();rfid.PCD_StopCrypto1();

}

void RESET_HE_THONG(){
  byte xacthucrt = 0, phantram;
  lcd.setCursor(0,0);lcd.print("NHAP MAT KHAU !");
  lcd.setCursor(0,1);lcd.print("MAT KHAU:_ _ _ _");

  NHAP_MAT_KHAU();

  for(int i = 0; i < 4; i++){ if( ((char)EEPROM.read(i)) == matkhau[i]){ xacthucrt++; Serial.println(xacthucrt);} } 

    if(xacthucrt == 4) { lcd.clear(); lcd.setCursor(0,0); lcd.print(" MAT KHAU DUNG");
    for(int i = 0; i < 256; i++){
    phantram = map(i, 0, 255, 0, 100);
    EEPROM.write(i, 0);delay(10);
    lcd.setCursor(0, 1);lcd.print(" RESET: "); lcd.print( phantram );lcd.print(" %");   }  setup(); }

    if(xacthucrt != 4) { lcd.clear(); lcd.setCursor(1,0); lcd.print(" MAT KHAU SAI !");lcd.setCursor(0,1);lcd.print("VUI LONG THU LAI");  } 
}


