
#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd.h"

// Ultrasonic sensor pins
#define TRIGGER_PIN PA6
#define ECHO_PIN PA7

volatile uint16_t fanSpeed = 0;

void initUltrasonic() {
	// Set trigger pin as output
	DDRA |= (1 << TRIGGER_PIN);
	// Set echo pin as input
	DDRA &= ~(1 << ECHO_PIN);
}

uint16_t measureDistance() {
	// Trigger ultrasonic sensor
	PORTA |= (1 << TRIGGER_PIN);
	_delay_us(10);
	PORTA &= ~(1 << TRIGGER_PIN);

	// Measure distance
	uint16_t pulse_width = 0;
	
	// Wait for the rising edge
	while (!(PINA & (1 << ECHO_PIN))) {
		// Do nothing
	}
	
	// Measure the pulse width (time taken by the sound wave to return)
	while (PINA & (1 << ECHO_PIN)) {
		pulse_width++;
		_delay_us(1);
	}

	// Calculate distance in centimeters
	uint16_t distance_cm = (pulse_width * 10) / 96;

	return distance_cm;
}


void adc_init() {
	ADMUX = (1 << REFS0); // Set reference voltage to AVcc and left-justify result
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC Enable and prescaler of 128
}

uint16_t adc_read(uint8_t ch) {
	ADMUX = (ADMUX & 0xF8) | (ch & 0x07); // Clear the channel selection bits and select the desired channel
	ADCSRA |= (1 << ADSC); // Start single conversion by setting ADSC
	while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
	return ADC; // Read and return the ADC result
}

void pwm_init() {
	// Initialize timer0 in PWM mode
	TCCR0 |= (1 << WGM00) | (1 << COM01) | (1 << WGM01) | (1 << CS00);
	TCCR2 |= (1 << WGM00) | (1 << COM01) | (1 << WGM01) | (1 << CS00);
	// Make sure to make OC0,OC2 pin as output pin
	DDRA = 0xFF;
	PORTA = 0X00;
	DDRB = 0XFF;
}

void led_init() {
	DDRC = 0x1F; // Set PC0, PC1, PC2, PC3, PC4 as output pins for LEDs
	PORTC = 0x00; // Turn off LEDs initially
}

void external_interrupt_init() {
	// External interrupt (INT7) initialization code
	EICRB |= (1 << ISC71) | (1 << ISC70); // Trigger on rising edge
	EIMSK |= (1 << INT7); // Enable external interrupt INT7
}

void lcd_display_welcome() {
	lcd_clrscr();
	lcd_gotoxy(0, 0);
	lcd_puts("Welcome Home,");
	lcd_gotoxy(0, 1);
	lcd_puts("Master");
	_delay_ms(2000);
	lcd_clrscr();
}

void lcd_display_detection() {
	lcd_gotoxy(0, 0);
	lcd_puts("Human detected.");
	lcd_gotoxy(0, 1);
	lcd_puts("Auto Switch On");
	_delay_ms(1000);
}

void lcd_display_temperature_fan(int temp) {
	lcd_clrscr();
	char buffer[16]; // Create a buffer to hold the string
	sprintf(buffer, "Room Temp: %d C", temp); // Format temperature
	lcd_puts(buffer);

	lcd_gotoxy(0, 1);
	sprintf(buffer, "Fan Speed: %d%%", fanSpeed); // Format fan speed
	lcd_puts(buffer);

	_delay_ms(1000);
}

void lcd_display_no_detection() {
	lcd_clrscr();
	lcd_gotoxy(0, 0);
	lcd_puts("No one detected,");
	lcd_gotoxy(0, 1);
	lcd_puts("Auto Switch off");
	_delay_ms(500);
}

ISR(INT7_vect) {
	// Display a message on the LCD when the interrupt is activated
	lcd_clrscr();
	lcd_gotoxy(0, 0);
	lcd_puts("Interrupt detected,");
	lcd_gotoxy(0, 1);
	lcd_puts("Auto Switch off");
	PORTC = 0x00; // Turn off all LEDs
	PORTA = 0x00; // Turn off all fans
	_delay_ms(5000);
}

void normalMode(){
		lcd_display_welcome();

		// Turn on LEDs and display a message
		PORTC = 0x0F; // Turn on LEDs
		lcd_display_detection();

		// Read temperature from LM35 (ADC0/PF0)
		int adcValue = adc_read(0); // Read from ADC0/PF0

		// Calculate temperature in degrees Celsius
		// The LM35 has a sensitivity of 10 mV/°C and a reference voltage of 5V (AVcc)
		float temp = (adcValue / 1024.0) * 500.0; // 500 mV/°C

		// Control fan speed based on temperature as you did before
		if (temp <= 20) {
			PORTA = 0x00;
			OCR0 = 0;
			OCR2 = 0;
			fanSpeed = 0; // Set fan speed to 0%
			} else if ((temp >= 21) && (temp <= 30)) {
			PORTA = 0x05;
			OCR0 = 155;
			OCR2 = 155;
			fanSpeed = 50; // Set fan speed to 50%
			} else if ((temp >= 31) && (temp <= 50)) {
			PORTA = 0x05;
			OCR0 = 255;
			OCR2 = 255;
			fanSpeed = 100; // Set fan speed to 100%
			} else {
			PORTA = 0x00;
			OCR0 = 0;
			OCR2 = 0;
			fanSpeed = 0; // Turn off fan
			lcd_clrscr();
			lcd_gotoxy(0, 0);
			lcd_puts("Error");
			lcd_gotoxy(0, 1);
			lcd_puts("Invalid Temp");
			_delay_ms(500);
		}
		
		// Display temperature on LCD
		lcd_display_temperature_fan((int)temp);
}


int main(void) {
	// Initialization code for peripherals
	adc_init();
	pwm_init();
	initUltrasonic(); // Initialize ultrasonic sensor
	lcd_init(LCD_DISP_ON);
	led_init();
	external_interrupt_init();
	DDRB = 0x00;
	PORTB = 0xFF;
	
	sei(); // Enable global interrupts

	while (1) {

			uint16_t distance =  measureDistance(); // Calculate distance in centimeters
			
				if (distance <= 150) { // less than 1.5 m

					if (PINB == 0xFE) { // Button 1 (PB0)
						PORTC = 0x0E; // Turn off LED1 (PC0)
						PORTA = 0x05;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("LED1");
						lcd_gotoxy(0, 1);
						lcd_puts("Switched Off");
						_delay_ms(50);
					}
					else if (PINB == 0xFD) { // Button 2 (PB1)
						PORTC = 0x0D; // Turn off LED2 (PC1)
						PORTA = 0x05;
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("LED2");
							lcd_gotoxy(0, 1);
							lcd_puts("Switched Off");
							_delay_ms(50);
					}
					else if (PINB == 0xFB) { // Button 3 (PB2)
						PORTC = 0x0B; // Turn off LED3 (PC2)
						PORTA = 0x05;
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("LED3");
							lcd_gotoxy(0, 1);
							lcd_puts("Switched Off");
							_delay_ms(50);

					}
					else if (PINB == 0xF7) { // Button 4 (Fan Control) (PB3) 
						// Implement code to control the fan based on button press
						// Example: Change fanSpeed variable, update fan speed
						PORTC = 0x0F;
						PORTA = 0x00;
						lcd_clrscr();
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("Fans");
							lcd_gotoxy(0, 1);
							lcd_puts("Switched Off");
							_delay_ms(50);
						_delay_ms(50);
					}
					else if (PINB == 0xFC) { 
						PORTC = 0x0C; // Turn off LED1 (PC0) and LED2 (PC1)
						PORTA = 0x05;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("LED1, LED2");
						lcd_gotoxy(0, 1);
						lcd_puts("Switched Off");
						_delay_ms(50);
		
					} 
					else if (PINB == 0xFA) { 
						PORTC = 0x0A;  // Turn off LED1 (PC0) and LED3 (PC2)
						PORTA = 0x05;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("LED1, LED3");
						lcd_gotoxy(0, 1);
						lcd_puts("Switched Off");
						_delay_ms(50);

					}
					else if (PINB == 0xF9) { 
						PORTC = 0x09;  // Turn off LED2 and LED3 (PC2)
						PORTA = 0x05;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("LED1, LED3");
						lcd_gotoxy(0, 1);
						lcd_puts("Switched Off");
						_delay_ms(50);

					}
					else if (PINB == 0xF8) { // Turn off all LEDs
						PORTC = 0x08; 
						PORTA = 0x05;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("All LEDs");
						_delay_ms(50);

					}
					else if (PINB == 0xF6) { //  Turn off LED1 (PC0) and Button 4 (Fan Control) (PB3) 
						PORTC = 0x0E;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("LED1, Fans");
						_delay_ms(50);
					}
					else if (PINB == 0xF5) { //  Turn off LED2 (PC1) and Button 4 (Fan Control) (PB3)
						PORTC = 0x0D;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("LED2, Fans");
						_delay_ms(50);
					}
					else if (PINB == 0xF3) { //  Turn off LED3 (PC2) and Button 4 (Fan Control) (PB3)
						PORTC = 0x0B;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("LED3, Fans");
						_delay_ms(50);
					}
					else if (PINB == 0xF4) { //  Turn off LED1, LED2 and Button 4 (Fan Control) (PB3)
						PORTC = 0x0C;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("LED1, LED2, Fans");
						_delay_ms(50);
					}
					else if (PINB == 0xF1) { //  Turn off LED2, LED3 and Button 4 (Fan Control) (PB3)
						PORTC = 0x09;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("LED2, LED3, Fans");
						_delay_ms(50);
					}
					else if (PINB == 0xF2) { //  Turn off LED1, LED3 and Button 4 (Fan Control) (PB3)
						PORTC = 0x0A;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off");
						lcd_gotoxy(0, 1);
						lcd_puts("LED1, LED3, Fans");
						_delay_ms(50);
					}
					else if (PINB == 0xF0) { //  Turn off LED1, LED2, LED3 and Button 4 (Fan Control) (PB3)
						PORTC = 0x08;
						PORTA = 0x00;
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Switched Off all");
						lcd_gotoxy(0, 1);
						lcd_puts("Fans and LEDs");
						_delay_ms(50);
					}
					else {
						normalMode();
							
					}
					
					
					
				} else {
					// Turn off LEDs and display a message
					PORTC = 0x10;
					PORTA &= ~((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3)); // Turn off fans
					lcd_display_no_detection();
				}
			
			
	
		_delay_ms(500); // Delay for smoother operation
	}
		
	return 0; // Return 0 to indicate successful execution (optional)
}
