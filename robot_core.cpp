#include "robot_core.h"

#include <Arduino.h>

/**
 * @brief Initialize robot IO
 * @param robot Robot state
*/
void init_io_robot(robot_state_t *robot)
{
  pinMode(IN1, OUTPUT);
  digitalWrite(IN1, LOW);

  pinMode(IN2, OUTPUT);
  digitalWrite(IN2, LOW);

  pinMode(IN3, OUTPUT);
  digitalWrite(IN3, LOW);

  pinMode(IN4, OUTPUT);
  digitalWrite(IN4, LOW);

  pinMode(PWML, OUTPUT);
  pinMode(PWMR, OUTPUT);

  update_speed(robot->speed);
}

/**
 * @brief Move robot right
 * Only right motors are moving
*/
void right()
{
  analogWrite(PWML, ZERO_SPEED);

  digitalWrite(IN2,LOW);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN4,LOW);
  digitalWrite(IN3,HIGH);
}

/**
 * @brief Move robot left
 * Only left motors are moving
*/
void left()
{
  analogWrite(PWMR, ZERO_SPEED);

  digitalWrite(IN2,HIGH);
  digitalWrite(IN1,LOW);
  digitalWrite(IN4,HIGH);
  digitalWrite(IN3,LOW);
}

/**
 * @brief Move robot backward
*/
void backward()
{
  digitalWrite(IN2,LOW);
  digitalWrite(IN1,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

/**
 * @brief Move robot forward
*/
void forward()
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN4,LOW);
  digitalWrite(IN3,HIGH);
}

/**
 * @brief Stop robot
*/
void stop()
{
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}

/**
 * @brief Update robot speed
 * 
 * @param speed Speed value
*/
void update_speed(uint8_t speed)
{
  analogWrite(PWML, speed);
  analogWrite(PWMR, speed);
}