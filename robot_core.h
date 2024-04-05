#ifndef ROBOT_CORE_H
#define ROBOT_CORE_H

#include <stdint.h>

// L298 Pinout with Arduino MKR WIFI1010
#define IN1 11 //Forward right
#define IN2 12 //Backward right
#define IN3 2 //Forward left
#define IN4 3 //Backward left
#define PWML 4 //ENA
#define PWMR 5 //ENB
#define MAX_SPEED 255u
#define MIN_SPEED 120u

/**
 * @brief Robot mode
 * Manu is BLE command driven
 * Auto is autonomous mode (line following)
*/
enum e_robot_mode {
    MANU_MODE = 0,
    AUTO_MODE = 1
};

/**
 * @brief Robot motion
*/
enum e_robot_motion {
    STOP = 0,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

/**
 * @brief Robot motion function pointer
*/
typedef void (*fp_robot_motion_t)(void);

/**
 * @brief Robot state
*/
typedef struct {
    e_robot_mode auto_mode;
    uint8_t speed;
    bool isBlack;
} robot_state_t;

/**
 * @brief Robot motion map
*/
typedef struct {
    fp_robot_motion_t action;
    const char *name;
} robot_motion_map_t;

#define INIT_STATE {MANU_MODE, MIN_SPEED, false}

// Public Functions Prototypes
void init_io_robot(robot_state_t *robot);
void forward(void);
void backward(void);
void left(void);
void right(void);
void stop(void);
void update_speed(uint8_t speed);

#endif // ROBOT_CORE_H