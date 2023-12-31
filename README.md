# Smart House System

<img src="./circuit.jpg">

The Smart House System is designed to enhance home automation by incorporating various sensors and devices to provide monitoring and control features. This system integrates an LM35 Temperature Sensor, Ultrasonic Sensor (HC-SR04), LCD Display (LM016L), LED control, and a DC Motor (L293D). The system enables temperature monitoring, distance sensing, and control over appliances.



## Project Functionality
1. **Temperature Monitoring:**
- The LM35 Temperature Sensor measures room temperature and displays it on the LCD.
2. **Distance Sensing:**
- The HC-SR04 Ultrasonic Sensor measures distances.
- The distance data is displayed on the LCD.
3. **Appliance Control:**
- The system allows for control of appliances via the DC Motor (L293D).
- Appliances can be turned on and off using the provided interface.
4. **Visual Feedback:**
- LED indicators provide visual feedback for various states of the system.
- The LCD Display presents data clearly for monitoring purposes.

## Project Benefits

The Smart House System with integrated LM35, Ultrasonic Sensor, LCD Display, and Motor Control offers the following benefits:

- Real-time temperature monitoring for comfort control.
- Accurate distance sensing for security and automation.
- Appliance automation through motor control.
- User-friendly visual interface through the LCD Display.
- Visual feedback through LEDs for system status.

This project showcases the potential of integrating various sensors and devices into a smart home system, offering convenience and control for homeowners.

## Hardware Connections of AtMega64

### LM35 Temperature Sensor:
- Connect LM35's Vcc to a 5V pin.
- Connect LM35's GND to a ground pin.
- Connect LM35's output to PF0/ADC0.

### LCD (LM016L):
- Connect LCD's VDD to a 5V pin.
- Connect LCD's VSS and VEE to a ground pin.
- Connect LCD's RS pin to PD4.
- Connect LCD's RW pin to PD5.
- Connect LCD's E pin to PD6.
- Connect LCD's data lines (D0-D7) to digital I/O pins (PD0 to PD3).

### DC Motor (L293D):
- Connect L293D's VSS to a 5V pin.
- Connect L293D's VS to VM (VM connects to 12V).
- Connect L293D's IN1 to PA0.
- Connect L293D's IN2 to PA1.
- Connect L293D's OUT1 to Motor+.
- Connect L293D's OUT2 to Motor-.
- Connect L293D's OUT3 to Motor+.
- Connect L293D's OUT4 to Motor-.
- EN1 connects to PWM1.
- EN2 connects to PWM2.
- PWM1 connects to PB4 and A of Oscillascope.
- PWM2 connects to PB7 and B of Oscillascope.

### External Interrupt:
- Connect the interrupt source (e.g., switch) to PE7.
- Connect the GND of the switch to ground.

### Ultrasonic Sensor (HC-SR04):
- Connect the sensor's Vcc and Trig to a 5V pin.
- Connect the sensor's GND to ground.
- Connect the sensor's TR to PA6.
- Connect the sensor's ECHO to PA7.

### LEDs:
- LED-Yellow (LED1) connects to PC0.
- LED-Yellow (LED2) connects to PC1.
- LED-Yellow (LED3) connects to PC2.
- LED-Green connects to PC3.
- LED-Red connects to PC4.

### Buttons (to control interrupt and LEDs):
- Button1 (PB0) to control LED1.
- Button2 (PB1) to control LED2.
- Button3 (PB2) to control LED3.
- Button4 (PB3) to control the motor (on/off).

### SW-SPDT (Interrupt)
- Initially connect to ground.
- To activate interrupt, the switch is pressed to connect to Vcc.

### Like this project? You can show your appreciation by buying Hovah a coffee ☕
<a target="_blank" rel="noopener noreferrer" href="https://www.buymeacoffee.com/hovahyii">
<img src="https://github.com/appcraftstudio/buymeacoffee/raw/master/Images/snapshot-bmc-button.png" width="300" style="max-width:100%;">
</a>

## Group Photo
<img src="./group-photo.jpg">

