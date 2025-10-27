/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the AT-MOTOR-EVB board with AT32F421 MCU to develop a six-step control mode project for controlling a BLDC motor in sensorless with comparators.
  in this demo, shows how to setup the MCU peripheral and make a control program with Artery motor control library and comm. functions.
  the test devices and main peripheral pin definitions are shown as below. 

  Board: AT-MOTOR-EVB V1.1/V1.2/V2.0
  Motor: JK42BLS01
  Motor connection:
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x Phase U -> CN5 OUT1¢x  (PWM_U_H: PA8, PWM_V_H: PA9, PWM_W_H: PA10)
 ¢x Phase V -> CN5 OUT2¢x  (PWM_U_L: PB13, PWM_V_L: PB14, PWM_W_L: PB15)
 ¢x Phase W -> CN5 OUT3¢x
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}
  the pins of 1 and 2 of JP8, JP10, JP12 are shorted respectively
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x JP8  pin1 <-> JP8  pin2¢x  
 ¢x JP10 pin1 <-> JP10 pin2¢x 
 ¢x JP12 pin1 <-> JP12 pin2¢x
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}
  If user interface(UI) is needed, use a micro USB cable to connect PC USB port and CN4 of board as follow:
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x PC -> CN4          ¢x  
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}

  for more detailed information. please refer to the library user guide document AN0064, AN0170/AN0171 and AN0214.