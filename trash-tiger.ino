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
int left_speed = 0;  // -255 to 255, negative = backward
int right_speed = 0;

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

void setLeftMotor(int speed) {
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

void setRightMotor(int speed) {
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
    angle += 10;
    if (angle > max_angle) {
      angle = max_angle;
    }
  }
  if (incomingChar == 'K') {
    angle -= 10;
    if (angle < 0) {
      angle = 0;
    }
    Serial.print("Angle: ");
    Serial.println(angle);
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
    static int base_speed = 150;  // base speed for movement (0-255)
    static int turn_speed = 100;  // peed difference for turning
    
    // boost for straight
    int current_base_speed = base_speed;
    if (boost && (forward || backward) && !(left || right)) {
      current_base_speed = 255;
    }
    
    if (forward && !backward) {
      leftMotorSpeed = current_base_speed;
      rightMotorSpeed = current_base_speed;
      
      if (left && !right) {
        leftMotorSpeed = current_base_speed - turn_speed;
      } else if (right && !left) {
        rightMotorSpeed = current_base_speed - turn_speed;
      }
    } else if (backward && !forward) {
      leftMotorSpeed = -current_base_speed;
      rightMotorSpeed = -current_base_speed;
      
      if (left && !right) {
        leftMotorSpeed = -(base_speed - turn_speed);
      } else if (right && !left) {
        rightMotorSpeed = -(base_speed - turn_speed);
      }
    } else if (!forward && !backward) {
      if (left && !right) {
        leftMotorSpeed = -turn_speed;
        rightMotorSpeed = turn_speed;
      } else if (right && !left) {
        leftMotorSpeed = turn_speed;
        rightMotorSpeed = -turn_speed;
      }
    }
    
    setLeftMotor(leftMotorSpeed);
    setRightMotor(rightMotorSpeed);
    
    Serial.print("Echo: ");
    if (boost) Serial.print(" BOOST!");
    Serial.print(" -> Left: ");
    Serial.print(leftMotorSpeed);
    Serial.print(", Right: ");
    Serial.print(rightMotorSpeed);
    Serial.println();
    
    forward = false;
    backward = false;
    left = false;
    right = false;
    boost = false;
  }
  
  servo.write(angle % 360);
}
