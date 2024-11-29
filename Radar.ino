#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_BUILTIN 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Servo myservo;

const int trigPin = 4;
const int echoPin = 3;
const int buzzerPin = 10;
int pos = 0;
float duration, distance;
bool increasing = true; // Tracks your servo's direction

float calculateFunc();
void updateServo();
void displayFunc(float dist);

unsigned long previousServoMillis = 0;
const long servoInterval = 20; // Interval for servo movement
unsigned long previousDisplayMillis = 0;
const long displayInterval = 250; // Interval for display update (1 second)

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  myservo.attach(9);
  myservo.write(0);  
}

float calculateFunc() {  //pretty self explanatory, it calculates the distance of the obj
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  return distance;
}

void updateServo() {  //updates the position of the servo
  unsigned long currentMillis = millis();
  if (currentMillis - previousServoMillis >= servoInterval) {
    previousServoMillis = currentMillis;

    if (increasing) {
      pos++;
      if (pos >= 180) increasing = false;
    } else {
      pos--;
      if (pos <= 0) increasing = true;
    }
    myservo.write(pos);
  }
}

void displayFunc(float dist) {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 9);

  if (dist <= 40){
  display.print("Object detected!");
  display.setCursor(0, 30);
  display.print("Distance: ");
  display.print(dist);
  } else {
    display.print("No object detected");
  }


  display.display();
}

void loop() {
  unsigned long currentMillis = millis();

  // Update distance and display every 1 second
  if (currentMillis - previousDisplayMillis >= displayInterval) {
    previousDisplayMillis = currentMillis;

    distance = calculateFunc();
    Serial.print("Distance: ");
    Serial.println(distance);

    displayFunc(distance);

    if (distance < 40) {
      tone(buzzerPin, 1000);   //Make noise with the buzzer
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      noTone(buzzerPin);
      digitalWrite(LED_BUILTIN, LOW);
    }

  }

  // Continuously update the servo position
  updateServo();
}