#ifndef RGB_H
#define RGB_H

#include <stdint.h>

// RGB LED pins
#define RED_PIN   14
#define GREEN_PIN 13
#define BLUE_PIN  10

/**
 * @brief RGB LED colors with associated pin
*/
typedef enum {
    BLUE = BLUE_PIN,
    GREEN = GREEN_PIN,
    RED = RED_PIN
} rgb_color_t;

// Public Functions Prototypes
void init_rgb(void);
void update_rgb(rgb_color_t color);

#endif // RGB_H