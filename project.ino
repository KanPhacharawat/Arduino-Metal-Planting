#include <TaskScheduler.h>
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6OHY7yPns"
#define BLYNK_TEMPLATE_NAME "my Project"
#define BLYNK_AUTH_TOKEN "9Bh0MoM2AMdi93dSzvGvJxgyestoZ9uQ"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <ESP32SharpIR.h>
ESP32SharpIR sensor( ESP32SharpIR::GP2Y0A21YK0F, 35);

char ssid[] = "IREKMITL_2.4G";
char pass[] = "5687862020";

const int freq = 5000;
const int ledChannel = 0;

const int freq2 = 22700;
const int ledChannel2 = 1;

const int resolution = 8;

//////////////

#define dir1 13
#define pwm1 12

#define dir2 14
#define pwm2 27

//////////////

#define ENCODER_A 32
#define ENCODER_B 33
volatile int encoder_value = 0;

int buzzer = 22;
int digital = 5;
int limit_left = 26;
int limit_right = 25;
int analog = 35;
int sum_analog;

int val_digital = 0;
float val_analog = 0;
int val_limit_left;
int val_limit_right;
bool btn0, btn1, btn2, btn3, btn4 ;
bool sw_up, sw_down, sw_left, sw_right;
bool state1;
String text_In;



Scheduler runner;

void t1Callback();
Task t1(10, TASK_FOREVER, &t1Callback);
void t2Callback();
Task t2(1, TASK_FOREVER, &t2Callback);
void t3Callback();
Task t3(100, TASK_FOREVER, &t3Callback);
void t4Callback();
Task t4(200, TASK_FOREVER, &t4Callback);

void t1Callback() {
  Blynk.run();
}
void t2Callback() {
  float val_analog = sensor.getDistanceFloat();
  sum_analog = (0.1779 * val_analog) - 0.3085;
  Serial.println(val_analog);
  Blynk.virtualWrite(V7, sum_analog );

  //Serial.println("Encoder value: " + String(encoder_value));
  if (text_In == "2" ) { // บ่อ 3
  }
  if (text_In == "1" ) { // บ่อ 2
    state1 = 1;
  }
  while (state1 > 0) {
    //float val_analog = sensor.getDistanceFloat();
    //sum_analog = map(val_analog, 0, 50, 1, 5);
    //sum_analog = (0.1779 * val_analog) - 0.3085;
    Serial.println(sum_analog);
    if (val_analog > 12 && val_analog <= 12.5) {
      ledcWrite(ledChannel2, 0);
      digitalWrite(dir1, LOW);
      ledcWrite(ledChannel, 100);
      delay(10000);
      digitalWrite(dir1, HIGH);
      ledcWrite(ledChannel, 100);
      delay(10000);
      state1 = 0;
      text_In = "0";
    }
    else if ( val_analog > 12.5 && val_analog <= 40) {
      digitalWrite(dir2, HIGH);
      ledcWrite(ledChannel2, 60);
    }
    else if (val_analog > 9 && val_analog < 12) {
      digitalWrite(dir2, LOW);
      ledcWrite(ledChannel2, 60);
    }
  }

}
void t3Callback() { // ซ้ายขวา HIGH ขวา
  int val_limit_left = digitalRead(limit_left);
  int val_limit_right = digitalRead(limit_right);
  //Serial.println(val_limit_right);
  if (sw_left == 1 && val_limit_left == 1) {
    digitalWrite(dir2, LOW);
    ledcWrite(ledChannel2, 100);
  }
  if ( sw_right == 1 && val_limit_right == 1) {
    digitalWrite(dir2, HIGH);
    ledcWrite(ledChannel2, 100);
  }
  if (sw_right == 0 && sw_left == 0) {
    ledcWrite(ledChannel2, 0);
  }
}
void t4Callback() {
  int val_digital = digitalRead(digital);
  //Serial.println(val_digital);
  delay(500);
  if (sw_up == 1) {
    digitalWrite(dir1, LOW);
    ledcWrite(ledChannel, 200);
  }
  if (val_digital == 1 && sw_down == 1) {
    digitalWrite(dir1, HIGH);
    ledcWrite(ledChannel, 200);
  }
  if (sw_up == 0 && sw_down == 0) {
    ledcWrite(ledChannel, 0);
  }
}
void setup()
{
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(dir1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwm2, OUTPUT);

  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  pinMode(buzzer, OUTPUT);
  sensor.setFilterRate(0.1f);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoder, CHANGE);

  ledcSetup(ledChannel, freq, resolution);
  ledcSetup(ledChannel2, freq2, resolution);

  ledcAttachPin(pwm1, ledChannel);
  ledcAttachPin(pwm2, ledChannel2);

  pinMode(limit_right, INPUT_PULLUP);
  pinMode(limit_left, INPUT_PULLUP);

  runner.init();
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);
  runner.addTask(t4);
  t1.enable();
  t2.enable();
  t3.enable();
  t4.enable();
}

void loop() {
  //Serial.println(encoder_value);
  delay(500);
  runner.execute();
}
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V7);
  Blynk.syncAll();
}
BLYNK_WRITE(V0) {
  int btn0 = param.asInt();
  if (btn0 == 1) {
    sw_left = 1;
    Serial.println(" sw_left");
  }
  else {
    sw_left = 0;
  }
}

BLYNK_WRITE(V1) {
  int btn1 = param.asInt();
  if (btn1 == 1) {
    sw_right = 1;
    Serial.println(" sw_right");
  }
  else {
    sw_right = 0;
  }
}

BLYNK_WRITE(V2) { // ขึ้น
  int btn2 = param.asInt();
  if (btn2 == 1) {
    sw_up = 1;
    Serial.println(" sw_up");
  }
  else {
    sw_up = 0;
  }
}
BLYNK_WRITE(V3) {  // ลง
  int btn3 = param.asInt();
  if (btn3 == 1) {
    sw_down = 1;
    Serial.println(" sw_down");
  }
  else {
    sw_down = 0;
  }
}
BLYNK_WRITE(V6) {
  text_In = param.asStr();
  Blynk.virtualWrite(V8, text_In);
}

void encoder() {
  int A = digitalRead(ENCODER_A);
  int B = digitalRead(ENCODER_B);
  if ((A == HIGH) != (B == LOW)) {
    encoder_value--;
  } else {
    encoder_value++;
  }
}
