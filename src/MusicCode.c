/* Partner(s) Name & E­mail: Richard Heng rheng001@ucr.edu
*  Partner(s) Name & E­mail: Edward Hernandez ehern044@ucr.edu
* Lab Section: 21
* Assignment: Custom Project
* Exercise Description: This controls music playing
*
* I acknowledge all content contained herein, excluding template or example
* code, is my own original work.
*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

void set_PWM(double frequency)
{
	// Keeps track of the currently set frequency
	// Will only update the registers when the frequency
	// changes, plays music uninterrupted.
	static double current_frequency;
	if (frequency != current_frequency)
	{
		if (!frequency) TCCR3B &= 0x08; //stops timer/counter
		else TCCR3B |= 0x03; // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) OCR3A = 0xFFFF;
		
		// prevents OCR3A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) OCR3A = 0x0000;
		
		// set OCR3A based on desired frequency
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;

		TCNT3 = 0; // resets counter
		current_frequency = frequency;
	}
}

void PWM_on()
{
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off()
{
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{start, look, wait, startSong} State;

void Tick(){
	switch(State)
	{
		case start:
		State = look;
		break;
		
		case look:
		State = startSong;
		break;
		
		case wait:
		if((~PINA & 0x01) == 0x01)
		State = wait;
		else
		State = look;
		break;
		
		
		case startSong:
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D6);
		_delay_ms(450*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D6);
		_delay_ms(450*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_E6);
		_delay_ms(400*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F6);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_E6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_E6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_C6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(100*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_G5);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(600*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_G5);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(600*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D6);
		_delay_ms(450*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D6);
		_delay_ms(350*8);
		PWM_off();
		_delay_ms(100*8);
		PWM_on();
		//-------------------------------------------
		
		
		PWM_on();
		set_PWM(NOTE_E6);
		_delay_ms(400*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F6);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_E6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_E6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_C6);
		_delay_ms(180*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(400*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		//-------------------------------------------
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_F5);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_G5);
		_delay_ms(200*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(400*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_A5);
		_delay_ms(300*8);
		PWM_off();
		_delay_ms(0*8);
		PWM_on();
		
		PWM_on();
		set_PWM(NOTE_D5);
		_delay_ms(500*8);
		PWM_off();
		_delay_ms(100*8);
		PWM_on();
		//-------------------------------------------
		
		
		State = wait;
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	State = start;
	
	while(1)
	{
		Tick();
	}
}