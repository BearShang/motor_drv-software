/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the AT32M412-LV-Motor-EV board with AT32M421 MCU to develop a foc control mode project for controlling a PMSM motor with or without sensor.
  in this demo, shows how to setup the MCU peripheral and make a control program with Artery motor control library and comm. functions.
  the test devices and main peripheral pin definitions are shown as below.

/*******************************************AT32M412-LV-Motor-EV board V1.0**********************************************/
  Board: AT32M412-LV-Motor-EV board V1.0
  Motor: JK42BLS01-X038ED
  Motor connection:
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x Phase U -> P3 OUT1¢x  (PWM_U_H: PA8, PWM_V_H: PA9, PWM_W_H: PA10)
 ¢x Phase V -> P4 OUT2¢x  (PWM_U_L: PC13, PWM_V_L: PB8, PWM_W_L: PB9)
 ¢x Phase W -> P5 OUT3¢x
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}
  The hall sensor is required, use the following pin definitions to connect the corresponding sensor.
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x hall sensor -> CN3¢x  ( Hall_1: PB4, Hall_2: PB5, Hall_3: PB3)
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}

  If user interface(UI) is needed, use a AT-LINK(TX/RX pins) to connect PC USB port and CN5(UART_TX/RX) of AT32M412-LV-Motor-EV board as follow:
 ¢z¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢{
 ¢x PC -> AT-LINK -> CN5(AT32M412-LV-Motor-EV board)¢x  
 ¢|¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢w¢}
  for more detailed information. please refer to the quick start guide document AN0271.
/*********************************************************************************************************************/