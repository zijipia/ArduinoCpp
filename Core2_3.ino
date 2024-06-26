#include <TimerOne.h>
/* ===================================================
          Nguyễn Thanh Phú - DH32112380
      Sine use TimerOne - arduino - ir2101s
 ===================================================*/
const int outAH = 9;   // Chân PWM đầu ra A Hin
const int outBH = 10;  // Chân PWM đầu ra B Hin
const int outAL = 5;   // Chân PWM đầu ra A Lin
const int outBL = 6;   // Chân PWM đầu ra B Lin
const int total_sample = 50; // Số mẫu để tạo một chu kỳ sóng sine
const int f_sine = 50; // Tần số sóng sine (50Hz)
const int feedback = A7; // Chân đầu vào đọc giá trị phản hồi
const int source12v = A1; // Chân đầu vào đọc giá 12v

float sine[total_sample]; // Mảng chứa các giá trị sóng sine
int flag = 0; // Biến cờ để đổi chiều sóng sine
int sample = 0; // Biến đếm mẫu
int feedbackval; // Biến lưu giá trị đọc từ đầu vào phản hồi.
int source12vval; // Biến lưu giá trị đọc từ đầu vào phản hồi
int A; // Biến điều chỉnh độ lớn của sóng sine

void setup() {
  Serial.begin(9600); // Khởi động Serial baudrate 9600

  // Khởi tạo mảng sine với các giá trị sóng sine
  for (int angle = 0; angle < total_sample; angle++) {
    float deg = angle * (180.0 / total_sample) * 3.14159265358979323846 / 180.0; // Tính góc theo radian
    sine[angle] = sin(deg);
  Serial.println(sine[angle]);
  }

  pinMode(outAL, OUTPUT); // Đặt chân outAL là đầu ra
  pinMode(outBL, OUTPUT); // Đặt chân outBL là đầu ra
  pinMode(outAH, OUTPUT); // Đặt chân outAH là đầu ra
  pinMode(outBH, OUTPUT); // Đặt chân outBH là đầu ra
  pinMode(feedback, INPUT); // Đặt chân feedback là đầu vào,
  pinMode(source12v, INPUT); // Đặt chân source12v là đầu vào
  digitalWrite(outAL, LOW); // Khởi tạo trạng thái thấp cho chân outAL
  digitalWrite(outBL, LOW); // Khởi tạo trạng thái thấp cho chân outBL
  digitalWrite(outAH, LOW); // Khởi tạo trạng thái thấp cho chân outAH
  digitalWrite(outBH, LOW); // Khởi tạo trạng thái thấp cho chân outBH
  // -> 4 chân kích fet ở mức thấp ( datasheet ir2101s )
  A = 0; // Khởi tạo biến A với giá trị 0
  delay(5000);
  /* ===================================================
    Số mẫu (total_sample / 2 ): 100 mẫu cho mỗi chu kỳ.
    Thời gian mỗi mẫu: 200µs, tương ứng với tần số ngắt Timer1 là 5kHz (1/200µs).
    Một chu kỳ: 100 mẫu x 200µs = 20ms, tương đương với tần số 50Hz.
  ===================================================*/

  // Thiết lập Timer1 để tạo ngắt mỗi 200µs (200 microseconds)
  Timer1.initialize(200); // 200µs cho mỗi mẫu
  Timer1.attachInterrupt(create_sine); // Đính kèm hàm tạo sóng sine vào ngắt Timer1
}

void loop() {
  feedbackval = analogRead(feedback); // Đọc giá trị từ chân feedback
  source12vval = analogRead(source12v); // Đọc giá trị từ chân 12v
  /* ===================================================
    12v -> 10k - Out -> 1k -> GND
    (Vout) = (Vin * R2) / (R1 + R2)
    Vout = Vin * (1k / (10k + 1k)) = Vin * (1 / 11)
  ===================================================*/
  long mappedVal = map(source12vval, 0, 1024, 0, 5000);
  float vin = (float(mappedVal) * 11) / 1000;

  if (vin > 15.0 || vin < 10.0){
    A = 0;
  } else if (feedbackval < 512 && A < 950) { // Nếu giá trị feedback nhỏ hơn 512 và A nhỏ hơn 950
    A++; // Tăng giá trị A
  } else if (feedbackval > 512 && A > 0) { // Nếu giá trị feedback lớn hơn 512 và A lớn hơn 0
    A--; // Giảm giá trị A
  }

  Serial.println(A); // In giá trị A ra Serial để debug
}

void create_sine() {
  if (sample >= total_sample) { // Nếu số mẫu đã đạt tổng số mẫu
    flag = !flag; // Đảo chiều sóng sine
    sample = 0; // Reset mẫu về 0
  } 

  /* ===================================================
    2 Chân Hin của ir2101s nhận được xung pwm điều chế theo sin 50hz
    2 Chân Lin của ir2101s nhận được xung vuông 50hz
    flag = 0 -> AH + BL ~~ ngược lại
  ===================================================*/
  
  if (flag == 0) { // Nếu cờ flag bằng 0, tạo nửa chu kỳ dương của sóng sine
    digitalWrite(outAL, LOW); // Đặt chân outAL thấp
    digitalWrite(outBL, HIGH); // Đặt chân outBL cao
    Timer1.pwm(outAH, sine[sample] * A); // Điều chỉnh độ rộng xung PWM cho chân outAH
    Timer1.pwm(outBH, 0); // Đặt độ rộng xung PWM cho chân outBH bằng 0
  } else { // Nếu cờ flag bằng 1, tạo nửa chu kỳ âm của sóng sine
    digitalWrite(outBL, LOW); // Đặt chân outBL thấp
    digitalWrite(outAL, HIGH); // Đặt chân outAL cao
    Timer1.pwm(outBH, sine[sample] * A); // Điều chỉnh độ rộng xung PWM cho chân outBH
    Timer1.pwm(outAH, 0); // Đặt độ rộng xung PWM cho chân outAH bằng 0
  }

  sample++; // Tăng giá trị mẫu
}
