/*
  @brief Robot Lego BLE controlled
  @author Guillaume CUADRADO
  @date 2024-3-25
*/
// Arduino Includes
#include <ArduinoBLE.h>
#include <Arduino_MKRENV.h>

// User Includes
#include "rgb.h"
#include "robot_core.h"

// No motors in TEST_MODE
#define TEST_MODE    (0u)

BLEService robotService("19B10000-E8F2-537E-4F6C-D104768A1214"); // create service for the Robot

// create switch characteristic and allow remote device to read and write
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic motionCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic speedCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
BLEByteCharacteristic luxCharacteristic("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

robot_state_t robot_lego = INIT_STATE;

robot_motion_map_t motion[] = {
    {stop, "STOP"},
    {forward, "FORWARD"},
    {backward, "BACKWARD"},
    {left, "LEFT"},
    {right, "RIGHT"}
};

int lux = 0;

void setup() {
  // begin initialization
  if (!BLE.begin()) {

    while (1);
  }

  init_io_robot(&robot_lego);
  init_rgb();

  // set the local name peripheral advertises
  BLE.setLocalName("RobotLego");
  // set the UUID for the service this peripheral advertises
  BLE.setAdvertisedService(robotService);

  // add the characteristic to the service
  robotService.addCharacteristic(switchCharacteristic);
  robotService.addCharacteristic(motionCharacteristic);
  robotService.addCharacteristic(speedCharacteristic);
  robotService.addCharacteristic(luxCharacteristic);

  // add service
  BLE.addService(robotService);

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristics
  switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
  motionCharacteristic.setEventHandler(BLEWritten, motionCharacteristicWritten);
  speedCharacteristic.setEventHandler(BLEWritten, speedCharacteristicWritten);
  luxCharacteristic.setEventHandler(BLERead, luxCharacteristicRead);

  // set an initial value for the characteristics
  switchCharacteristic.setValue(0);
  motionCharacteristic.setValue(0);
  speedCharacteristic.setValue(0);

  // start advertising
  BLE.advertise();

  update_rgb(BLUE);
}

void loop() {

  float l = ENV.readLux();
  lux = (int)l;
  
  robot_lego.isBlack = lux < 100 ? true : false;

  // poll for Bluetooth® Low Energy events
  BLE.poll();
}

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  if (switchCharacteristic.value()) {
    update_rgb(GREEN);
  } else {
    update_rgb(BLUE);
  }
}

void motionCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, drive the robot according to the value

  switch (motionCharacteristic.value()) {
    case 1:
      robot_lego.auto_mode = AUTO_MODE;
      update_rgb(RED);
      break;
    case 2:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[BACKWARD].action();
#endif
      }
      break;
    case 3:
      robot_lego.auto_mode = MANU_MODE;
      update_rgb(BLUE);
      break;
    case 4:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[LEFT].action();
#endif
      }
      break;
    case 6:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[RIGHT].action();
#endif
      }
      break;
    case 8:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[FORWARD].action();
#endif
      }
      break;
    default:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[STOP].action();
#endif
      }
  }
}

void speedCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update the speed of the robot

  robot_lego.speed = speedCharacteristic.value();

  if (robot_lego.speed < MIN_SPEED) {
    robot_lego.speed = MIN_SPEED;
  } else if (robot_lego.speed > MAX_SPEED) {
    robot_lego.speed = MAX_SPEED;
  }

  update_speed(robot_lego.speed);
}

void luxCharacteristicRead(BLEDevice central, BLECharacteristic characteristic) {
  // central read the characteristic, update the lux value
  luxCharacteristic.writeValue(lux);
}
