#include <Servo.h>

#define ENA 10
#define IN1 11
#define IN2 12
#define IN3 6
#define IN4 5
#define ENB 3
#define SERVO 13

Servo servo;
const int angle_offset = 29;
int angle = 0;
int last_time = 0;
const int max_angle = 18;
const int min_angle = -30;
int left_speed = 0;  // -255 to 255, negative = backward
int right_speed = 0;
const int servo_speed = 10;

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO);
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void setRightMotor(int speed) {
  if (speed > 0 && speed < 200) {
    speed = 200;
  } else if (speed < 0 && speed > -200) {
    speed = -200;
  }
  
  if (speed > 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, constrain(speed, 0, 255));
  } else if (speed < 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, constrain(-speed, 0, 255));
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}

void setLeftMotor(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, constrain(speed, 0, 255));
  } else if (speed < 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, constrain(-speed, 0, 255));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void loop() {
  // Input state flags
  static bool forward = false;
  static bool backward = false;
  static bool left = false;
  static bool right = false;
  static bool boost = false;
  
  char incomingChar = ' ';
  if (Serial.available() > 0) {
    incomingChar = Serial.read();  // Read the incoming data
    Serial.print("Echo: ");
    Serial.println(incomingChar);
  }

  // Pince control
  if (incomingChar == 'L') {
    angle += servo_speed;
    if (angle > max_angle) {
      angle = max_angle;
    }
  }
  if (incomingChar == 'K') {
    angle -= servo_speed;
    if (angle < min_angle) {
      angle = min_angle;
    }
  }

  // Accumulate movement commands
  if (incomingChar == 'W') {
    forward = true;
  }
  if (incomingChar == 'S') {
    backward = true;
  }
  if (incomingChar == 'A') {
    left = true;
  }
  if (incomingChar == 'D') {
    right = true;
  }
  if (incomingChar == 'H') {
    boost = true;
  }

  if (incomingChar == 'E') {
    int leftMotorSpeed = 0;
    int rightMotorSpeed = 0;
    
    // SIMPLIFIED LOGIC - ALWAYS FULL FORCE
    if (forward && !backward) {
      // FORWARD - ignore left/right, just go full forward
      leftMotorSpeed = 255;
      rightMotorSpeed = 255;
    } else if (backward && !forward) {
      // BACKWARD - ignore left/right, just go full backward
      leftMotorSpeed = -255;
      rightMotorSpeed = -255;
    } else if (!forward && !backward) {
      // PURE TANK STEERING - full force
      if (left && !right) {
        // Turn left: left motor backward, right motor forward
        leftMotorSpeed = -255;
        rightMotorSpeed = 255;
      } else if (right && !left) {
        // Turn right: left motor forward, right motor backward
        leftMotorSpeed = 255;
        rightMotorSpeed = -255;
      }
      // If both left and right or neither, motors stay at 0
    }
    // If both forward and backward, motors stay at 0 (safety)
    
    setLeftMotor(leftMotorSpeed);
    setRightMotor(rightMotorSpeed);
    
    if (boost) Serial.print(" BOOST!");
    Serial.print("Left: ");
    Serial.println(leftMotorSpeed);
    Serial.print("Right: ");
    Serial.println(rightMotorSpeed);
    Serial.print("Angle: ");
    Serial.println(angle);
    
    forward = false;
    backward = false;
    left = false;
    right = false;
    boost = false;
  }
  
  servo.write((angle_offset + angle) % 360);
}
