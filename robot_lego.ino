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
#define TEST_MODE    (1u)

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
  Serial.begin(9600);
  while (!Serial);
  
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

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

  Serial.println(("Bluetooth® device active, waiting for connections..."));
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
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print("Characteristic event, written: ");

  if (switchCharacteristic.value()) {
    Serial.println("LED Green");
    update_rgb(GREEN);
  } else {
    Serial.println("LED Blue");
    update_rgb(BLUE);
  }
}

void motionCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, drive the robot according to the value
  Serial.print("Characteristic event, written: ");

  switch (motionCharacteristic.value()) {
    case 1:
      robot_lego.auto_mode = AUTO_MODE;
      Serial.println("Auto mode");
      update_rgb(RED);
      break;
    case 2:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[BACKWARD].action();
#endif
        Serial.println(motion[BACKWARD].name);
      }
      break;
    case 3:
      robot_lego.auto_mode = MANU_MODE;
      Serial.println("Manual mode");
      update_rgb(BLUE);
      break;
    case 4:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[LEFT].action();
#endif
        Serial.println(motion[LEFT].name);
      }
      break;
    case 6:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[RIGHT].action();
#endif
        Serial.println(motion[RIGHT].name);
      }
      break;
    case 8:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[FORWARD].action();
#endif
        Serial.println(motion[FORWARD].name);
      }
      break;
    default:
      if (!robot_lego.auto_mode) {
#if (TEST_MODE == 0u)
        motion[STOP].action();
#endif
        Serial.println(motion[STOP].name);
      }
  }
}

void speedCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update the speed of the robot
  Serial.print("Characteristic event, written: ");

  robot_lego.speed = speedCharacteristic.value();

  if (robot_lego.speed < MIN_SPEED) {
    robot_lego.speed = MIN_SPEED;
  } else if (robot_lego.speed > MAX_SPEED) {
    robot_lego.speed = MAX_SPEED;
  }
  Serial.println(robot_lego.speed);

  update_speed(robot_lego.speed);
}

void luxCharacteristicRead(BLEDevice central, BLECharacteristic characteristic) {
  // central read the characteristic, update the lux value
  Serial.print("Characteristic event, read: ");
  Serial.println(lux);

  luxCharacteristic.writeValue(lux);
}
