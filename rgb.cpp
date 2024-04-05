#include "rgb.h"

#include <Arduino.h>

/**
 * @brief Initialize RGB pins as output
*/
void init_rgb(void)
{
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

/**
 * @brief Update RGB color
 * 
 * @param color RGB color
*/
void update_rgb(rgb_color_t color)
{
  digitalWrite(RED_PIN, color == RED);
  digitalWrite(GREEN_PIN, color == GREEN);
  digitalWrite(BLUE_PIN, color == BLUE);
}