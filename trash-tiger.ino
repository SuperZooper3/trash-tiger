#include <Servo.h>

#define ENA 10
#define IN1 11
#define IN2 12
#define IN3 6
#define IN4 5
#define ENB 7
#define SERVO 8

Servo servo;
int angle = 45;
int last_time = 0;
const int max_angle = 60;
int left_direction = 0;
int right_direction = 0;

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO);
  // servo.write(0);
  // delay(1000);
  // servo.write(180);
  // delay(1000);
  // servo.write(0);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

}

int rbCount = 0;

void rightBackward(int speed) {
  if (rbCount % speed == 0) {
    digitalWrite(ENA, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    rightStop();
  }
  rbCount++;
}

int rfCount = 0;

void rightForward(int speed) {
  if (rfCount % speed == 0) {
    digitalWrite(ENA, HIGH);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    rightStop();
  }
  rfCount++;
}

void rightStop() {
  digitalWrite(ENA, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

int lbCount = 0;

void leftBackward(int speed) {
  if (lbCount % speed == 0) {
    digitalWrite(ENB, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    leftStop();
  }
  lbCount++;
}

int lfCount = 0;

void leftForward(int speed) {
  if (lfCount % speed == 0) {
    digitalWrite(ENB, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    leftStop();
  }
  lfCount++;
}

void leftStop() {
  digitalWrite(ENB, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println(angle);
  // Serial.println(millis());
  char incomingChar = ' ';
  if (Serial.available() > 0) {
    incomingChar = Serial.read();  // Read the incoming data
    Serial.print("Echo: ");
    Serial.println(incomingChar);
  }

  // arm
  if (incomingChar == 'L') {
    angle+=10;
    if (angle > max_angle) {
      angle = max_angle;
    }
  }
  if (incomingChar == 'K') {
    angle-=10;
    if (angle < 0) {
      angle = 0;
    }
    Serial.print("Angle: ");
    Serial.println(angle);
  }

  // motors
  if (incomingChar == 'E') { // end of payload, write the current states
    Serial.print("Direction (l/r): ");
    Serial.print(left_direction);
    Serial.println(right_direction);
    Serial.print("Angle: ");
    Serial.println(angle);
    switch (left_direction) {
      case 0:
        leftStop();
        break;
      case 1:
        leftForward(2);
        break;
      case -1:
        leftBackward(2);
        break;
    }
    switch (right_direction) {
      case 0:
        rightStop();
        break;
      case 1:
        rightForward(2);
        break;
      case -1:
        rightBackward(2);
        break;
    }

    left_direction = 0;
    right_direction = 0;
  }

  if (incomingChar == 'W') {
    left_direction += 1;
    right_direction += 1;
  }

  if (incomingChar == 'S') {
    left_direction += -1;
    right_direction += -1;
  }

  if (incomingChar == 'A') {
    left_direction += -1;
    right_direction += 1;
  }

  if (incomingChar == 'D') {
    left_direction += 1;
    right_direction += -1;
  }

  servo.write(angle%360);
}
