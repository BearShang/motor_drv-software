/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the AT-MOTOR-EVB board with AT32F421 MCU to develop a six-step control mode project for controlling a BLDC motor with Hall sensor.
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
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x hall sensor -> J1¢x  ( Hall_A: PB4, Hall_B: PB5, Hall C: PB0) 
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}

  If user interface(UI) is needed, use a micro USB cable to connect PC USB port and CN4 of board as follow:
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x PC -> CN4          ¢x  
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}

  for more detailed information. please refer to the library user guide document AN0064, AN0169 and AN0213.