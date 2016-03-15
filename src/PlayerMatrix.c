/* Partner(s) Name & E­mail: Edward Hernandez ehern044@ucr.edu
*  Partner(s) Name & E­mail: Richard Heng rheng001@ucr.edu
*  Partner(s) Name & E­mail: Vincent Chang vchan019@ucr.edu
* Lab Section: 21
* Assignment: Custom Project
* Exercise Description: This controls each players 8x8 LED matrix
*
* I acknowledge all content contained herein, excluding template or example
* code, is my own original work.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart_ATmega1284.h"
#include <util/delay.h>

/////////////////GLOBAL VARIABLES//////////////////////////////

uint8_t receiveData = 0x00;
unsigned char Right_Pressed = 0x00;
unsigned char Left_Pressed = 0x00;
unsigned char startGame = 0x00;
unsigned char debuff = 0;
unsigned char hit = 0;
unsigned char playerLives = 3;
unsigned char gameOver = 0;

unsigned char X_Coordinate_Power = 0x00;
unsigned char Y_Coordinate_Green = 0x00;
unsigned char Y_Coordinate_Red = 0x00;
unsigned char X_Coordinate_Power_player = 0x00;
unsigned char Y_Coordinate_Green_player = ~0x00;
unsigned char Y_Coordinate_Red_player = ~0x00;
unsigned char X_Coordinate_Power_obstacles = 0x00;
unsigned char Y_Coordinate_Green_obstacles = ~0x00;
unsigned char Y_Coordinate_Red_obstacles = ~0x00;


unsigned char character = ~(0x00);
unsigned char bounds = 0x00;

unsigned char i = 0x00;
unsigned char k = 0x00;
unsigned char pattern = 0x00;

unsigned char Hit_checker = 0x00;

/////////////////END GLOBAL VARIABLES//////////////////////////////


///////////////TIMER STUFF//////////////////////////////////////////////
volatile unsigned char TimerFlag=0; // ISR raises, main() lowers
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn()
{
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff()
{
	
	TCCR1B = 0x00;
}
ISR(TIMER1_COMPA_vect)
{
	
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

typedef struct task
{
	unsigned char state;
	unsigned long period;
	unsigned long elapsedTime;
	int(*TickFct)(int);
}	task;
task tasks[7];
const unsigned char tasksNum = 7;
const unsigned long tasksPeriodGCD = 1;

void TimerISR() { //Look at
	unsigned char i;
	for (i = 0; i < tasksNum; ++i)
	{
		if ( tasks[i].elapsedTime >= tasks[i].period ) // Task is ready to tick
		{
			tasks[i].state = tasks[i].TickFct(tasks[i].state); // Setting next state for task
			tasks[i].elapsedTime = 0; // Reset elapsed time for next tick.
		}
		tasks[i].elapsedTime += tasksPeriodGCD;
	}
}

///////////////EMD TIMER STUFF//////////////////////////////////////////////


////////////////////SHIFT REGISTER/////////////////////////
void transmit_data(unsigned char data) {
	int i;
	for (i = 7; i >= 0 ; --i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}

	// set RCLK = 1. Rising edge copies data from the “Shift” register to the “Storage” register
	PORTC |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}
////////////////////SHIFT REGISTER/////////////////////////

///////////////GLOBAL FUNCTIONS///////////////////////////////////////////


void RED_ALL()
{
	PORTA =   0b11111111; // POWER
	PORTB = ~(0b00000000); //Green
	PORTC = ~(0b11111111); // RED
	hit = 1;
	_delay_ms(10);
	
	if(playerLives <= 0)
	{
		gameOver = 1;
	}
}
void RED_NONE()
{
	hit = 0;
}
void CLEAR_obstacles()
{
	X_Coordinate_Power_obstacles =   0b00000000; // POWER
	Y_Coordinate_Green_obstacles = ~(0b00000000); //Green
	Y_Coordinate_Red_obstacles = ~(0b00000000); // RED
}
void CLEAR_player()
{
	X_Coordinate_Power_player =	  0b00000000; // POWER X-coordinate
	Y_Coordinate_Green_player = ~(0b00000000); //Green y-coordinate
	Y_Coordinate_Red_player = ~(0b00000000); // RED y-coordinate
}
void CLEAR_ALL()
{
	PORTA =   0b00000000; // POWER
	PORTB = ~(0b00000000); //Green
	PORTC = ~(0b00000000); // RED
}


void GREEN1X1_obstacles()
{
	X_Coordinate_Power_obstacles =   0b00000001; // POWER
	Y_Coordinate_Green_obstacles = ~(0b00000001); //Green
	Y_Coordinate_Red_obstacles = ~(0b00000000); // RED
}
void RED_player_start()
{
	X_Coordinate_Power_player =	  0b00000001; // POWER X-coordinate
	Y_Coordinate_Green_player = ~(0b00000000); //Green y-coordinate
	Y_Coordinate_Red_player = ~(0b00001000); // RED y-coordinate
}
void GREEN_drop_obstacles1()
{
	X_Coordinate_Power_obstacles =   0b10000000; // POWER
	Y_Coordinate_Green_obstacles = ~(0b10001001); //Green
	Y_Coordinate_Red_obstacles = ~(0b00000000); // RED
}


unsigned char GetBit(unsigned char x, unsigned char k)
{
	return ((x & (0x01 << k)) != 0);
}
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b)
{
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

///////////////END GLOBAL FUNCTIONS///////////////////////////////////////////


enum receive_move_states { has_received, receive, flush } receive_move_state;
int receive_move( int state )
{
	switch( state )
	{
		case has_received:
		if( USART_HasReceived(0) )
		{
			state = receive;
		}
		else
		{
			state = has_received;
		}
		break;
		
		case receive:
		state = flush;
		break;
		
		case flush:
		state = has_received;;
		break;
		
		default:
		state = has_received;
		break;
		
	}
	
	switch( state )
	{
		case has_received:
		break;
		
		case receive:
		receiveData = USART_Receive(0);
		if(receiveData == 0b11111100) //Not Pressed
		{
			PORTD = 0x00;
			Right_Pressed = 0;
			Left_Pressed = 0;
			//debuff = 1;
		}
		else if(receiveData == 0b11110000) //Left Pressed
		{
			PORTD |= 0x40;
			Left_Pressed = 1;
		}
		else if(receiveData == 0b11111000) //Right Pressed
		{
			PORTD |= 0x40;
			Right_Pressed = 1;
		}
		else if(receiveData == 0b11111110) //Start Game
		{
			PORTD |= 0x40;
			startGame = 1;
		}
		else if(receiveData == 0b10101010) //reset game
		{
			gameOver = 0;
			playerLives = 3;
		}
		else if(receiveData == 0b11001100) //receive game over
		{
			PORTD |= 0x40;
			gameOver = 1;
		}
		else if (receiveData == 0b11101110) //receive power up
		{
			PORTD |= 0x40;
			CLEAR_obstacles();
			CLEAR_obstacles();
		}
		break;
		
		case flush:
		USART_Flush(0);
		break;
		
		default:
		break;
		
	}
	
	return state;
}

enum Player_states {Start_Player_start, Player_start, B_Release, main_state, intialization, right, left} Player_state;
void Player()
{
	switch (Player_state) //State transitions
	{
		case Start_Player_start:
		if(startGame == 1 && gameOver == 0)
		{
			Player_state = Player_start;
		}
		else
		{
			Player_state = Start_Player_start;
		}
		break;
		
		case Player_start:
		Player_state = intialization;
		break;
		
		case intialization:
		Player_state = main_state;
		break;
		
		case main_state:
		if (Left_Pressed == 1)
		{
			Player_state == left;
			_delay_ms(100);
		}
		if(Right_Pressed == 1)
		{
			Player_state = right;
			_delay_ms(100);
		}
		break;
		
		case B_Release:
		if(Right_Pressed == 1 && Left_Pressed == 0) //If A0 is 1(pressed) and A1 is 0(not pressed) increment
		{
			Player_state = right;
		}
		else if(Left_Pressed == 1 && Right_Pressed == 0) //If A0 is 1(pressed) and A1 is 0(not pressed) increment
		{
			Player_state = left;
		}
		break;
		
		case left:
		if(gameOver == 1)
		{
			CLEAR_player();
			Player_state = Start_Player_start;
		}
		else
		{
			Player_state = B_Release;
		}
		break;
		
		case right:
		if(gameOver == 1)
		{
			CLEAR_player();
			Player_state = Start_Player_start;
		}
		else
		{
			Player_state = B_Release;
		}
		break;
		
		default:
		Player_state = Player_start;
		break;
	}

	switch (Player_state) //State actions
	{
		case Player_start:
		RED_player_start();
		break;
		
		case intialization:
		RED_player_start();
		bounds = 0x04;
		break;
		
		case main_state:
		break;
		
		case left:
		if (bounds > 0x01)
		{
			if(debuff != 1)
			{
				bounds--;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character >> 1) ^ 0x80);
				_delay_ms(10);
			}
			else if (debuff == 1)
			{
				bounds--;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character >> 1) ^ 0x80);
				_delay_ms(50);
			}
		}
		break;
		
		case right:
		if (bounds < 0x08)
		{
			if(debuff != 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(10);
			}
			else if (debuff == 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(50);
			}
		}
		else if (bounds < 0x08)
		{
			if(debuff != 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(10);
			}
			else if (debuff == 1)
			{
				bounds++;
				character = Y_Coordinate_Red_player;
				Y_Coordinate_Red_player = ((character << 1) ^ 0x01);
				_delay_ms(50);
			}
		}
		
		break;
		
		default:
		break;
	}
}

enum Obstacles_states {Start_Obstacles_start, Obstacles_start, Obstacles_initalization, Obstacles_drop, Obstacles_next} Obstacles_state;
void Obstacles()
{
	switch (Obstacles_state) //State transitions
	{
		case Start_Obstacles_start:
		if(startGame == 1 && gameOver == 0)
		{
			Obstacles_state = Obstacles_start;
		}
		else
		{
			Obstacles_state = Start_Obstacles_start;
		}
		break;
		
		case Obstacles_start:
		Obstacles_state = Obstacles_initalization;
		break;
		
		case Obstacles_initalization:
		Obstacles_state = Obstacles_drop;
		break;
		
		case Obstacles_drop:
		if (i > 6)
		{
			k++;
			if (pattern == 0x09)
			{
				pattern = 0x00;
			}
			i = 0x00;
			X_Coordinate_Power_obstacles = (X_Coordinate_Power_obstacles >> 1);
			Obstacles_state = Obstacles_next;
		}
		else if (k == 8)
		{
			k = 0x00;
			pattern++;
			Obstacles_state = Obstacles_initalization;
		}
		
		else if(gameOver == 1)
		{
			CLEAR_obstacles();
			Obstacles_state = Start_Obstacles_start;
		}
		else
		{
			Obstacles_state = Obstacles_drop;
		}
		break;
		
		case Obstacles_next:
		Obstacles_state = Obstacles_drop;
		break;
		
		
		default:
		Obstacles_state = Obstacles_start;
		break;
	}

	switch (Obstacles_state) //State actions
	{
		case Obstacles_start:
		GREEN_drop_obstacles1();
		break;
		
		case Obstacles_initalization:
		GREEN_drop_obstacles1();
		Y_Coordinate_Green_obstacles = ~(0b00000001); //Green
		if (pattern == 0x01)
		Y_Coordinate_Green_obstacles = ~(0b10001001); //Green <-------------This does not match for some reason
		if (pattern == 0x02)
		Y_Coordinate_Green_obstacles = ~(0b00000011); //Green
		if (pattern == 0x03)
		Y_Coordinate_Green_obstacles = ~(0b00001100); //Green
		if (pattern == 0x04)
		Y_Coordinate_Green_obstacles = ~(0b00110000); //Green
		if (pattern == 0x05)
		Y_Coordinate_Green_obstacles = ~(0b11000000); //Green
		if (pattern == 0x06)
		Y_Coordinate_Green_obstacles = ~(0b00110000); //Green
		if (pattern == 0x07)
		Y_Coordinate_Green_obstacles = ~(0b00001100); //Green
		if (pattern == 0x08)
		Y_Coordinate_Green_obstacles = ~(0b00000011); //Green*/
		break;
		
		case Obstacles_drop:
		i++;
		break;
		
		Obstacles_next:
		break;
		
		default:
		break;
		}
		}

		unsigned char character_hit = 0x00;
		unsigned char music_play = 0x00;
		enum Matrix_Output_states {Start_Matrix_Output, Matrix_Output_player,  Matrix_Output_obstacles} Matrix_Output_state;
		void Matrix_Output()
		{
		switch (Matrix_Output_state) //State transitions
		{
		
		case Start_Matrix_Output:
		if(startGame == 1 && gameOver == 0)
		{
			music_play = 0x00;
			PORTD |=music_play;
		Matrix_Output_state = Matrix_Output_player;
		}
		else
		{
			music_play = 0x10;
			PORTD |=music_play;
		Matrix_Output_state = Start_Matrix_Output;
		}
		break;
		
		
		case Matrix_Output_player:
		Matrix_Output_state = Matrix_Output_obstacles;
		if(gameOver == 1)
		{
		CLEAR_ALL();
		Matrix_Output_state = Start_Matrix_Output;
		}
		break;
		
		case Matrix_Output_obstacles:
		Matrix_Output_state = Matrix_Output_player;
		break;
		
		default:
		Matrix_Output_state = Matrix_Output_obstacles;
		break;
		}

		switch (Matrix_Output_state) //State actions
		{
		case Matrix_Output_player:
		PORTA = X_Coordinate_Power_player;
		PORTB = Y_Coordinate_Green_player;
		transmit_data(Y_Coordinate_Red_player); //Shift Register for PORTC
		
		if(X_Coordinate_Power_obstacles == X_Coordinate_Power_player &&	(	(Y_Coordinate_Red_player & ~0x01) == (Y_Coordinate_Green_obstacles & ~0x01) ||
		(Y_Coordinate_Red_player & ~0x02) == (Y_Coordinate_Green_obstacles & ~0x02) ||
		(Y_Coordinate_Red_player & ~0x04) == (Y_Coordinate_Green_obstacles & ~0x04) ||
		(Y_Coordinate_Red_player & ~0x08) == (Y_Coordinate_Green_obstacles & ~0x08) ||
		(Y_Coordinate_Red_player & ~0x10) == (Y_Coordinate_Green_obstacles & ~0x10) ||
		(Y_Coordinate_Red_player & ~0x20) == (Y_Coordinate_Green_obstacles & ~0x20) ||
		(Y_Coordinate_Red_player & ~0x40) == (Y_Coordinate_Green_obstacles & ~0x40) ||
		(Y_Coordinate_Red_player & ~0x80) == (Y_Coordinate_Green_obstacles & ~0x80)	))
		{
		RED_ALL();
		character_hit = 1;
		}
		else
		{
		RED_NONE();
		}
		break;
		
		case Matrix_Output_obstacles:
		PORTA = X_Coordinate_Power_obstacles;
		PORTB = Y_Coordinate_Green_obstacles;
		PORTC = Y_Coordinate_Red_obstacles;
		break;
		
		default:
		break;
		}
		}
		
		enum Hit_states {Start_Hit, Record_Hit} Hit_state;
		void Hit_Output()
		{
		switch (Hit_state) //State transitions
		{
		case Start_Hit:
		if (character_hit = 1)
		{
		playerLives--;
		character_hit = 0x00;
		Hit_state = Record_Hit;
		}
		else
		{
		Hit_state = Start_Hit;
		}
		break;
		
		case Record_Hit:
		if (Hit_checker >= 4)
		{
		Hit_checker = 0x00;
		Hit_state = Start_Hit;
		}
		else
		{
		Hit_state = Record_Hit;
		}
		break;
		
		default:
		Hit_state = Start_Hit;
		break;
		}

		switch (Hit_state) //State actions
		{
		case Start_Hit:
		break;
		
		case Record_Hit:
		Hit_checker++;
		break;
		
		default:
		break;
		}
		}

		enum send_score_states { check_send_rdy, has_transmitted, send } send_score_state;
		int send_score( int state )
		{
		
		switch( state )
		{
		case check_send_rdy:
		if( USART_IsSendReady(0) )
		{
		state = send;
		}
		else
		{
		state = check_send_rdy;
		}
		break;
		
		case has_transmitted:
		if( USART_HasTransmitted(0) )
		{
		state = check_send_rdy;
		}
		else
		{
		state = has_transmitted;
		}
		break;
		
		case send:
		state = has_transmitted;
		break;
		
		default:
		state = check_send_rdy;
		break;
		}
		
		switch( state )
		{
		case check_send_rdy:
		break;
		
		case has_transmitted:
		break;
		
		case send:
		USART_Send(0b11111111, 0);
		break;
		
		default:
		break;
		}
		
		return state;
		}

		enum send_lives_states { check_lives_rdy, has_transmittedLives, sendLives } send_live_state;
		int send_lives( int state )
		{
		
		switch( state )
		{
		case check_lives_rdy:
		if( USART_IsSendReady(0) )
		{
		state = sendLives;
		}
		else
		{
		state = check_send_rdy;
		}
		break;
		
		case has_transmittedLives:
		if( USART_HasTransmitted(0) )
		{
		state = check_lives_rdy;
		}
		else
		{
		state = has_transmittedLives;
		}
		break;
		
		case sendLives:
		state = has_transmittedLives;
		break;
		
		default:
		state = check_lives_rdy;
		break;
		
		}
		
		switch( state )
		{
		case check_lives_rdy:
		break;
		
		case has_transmittedLives:
		break;
		
		case sendLives:
		if(hit == 1) //Hit yes
		{
		USART_Send(0b10111111, 0);
		_delay_ms(1);
		}
		
		else if(hit == 0) //No hits
		{
		USART_Send(0b10011111, 0);
		}
		break;
		
		default:
		break;
		}
		return state;
		}

		///////////////END GLOBAL FUNCTIONS///////////////////////////////////////////


		int main(void)
		{
		DDRA = 0xFF; PORTA = 0x00; //Set Port B to output, init to 0's
		DDRB = 0xFF; PORTB = 0x00;
		DDRC = 0xFF; PORTC = 0x00;
		DDRD = 0xFF; PORTD = 0x00;
		
		TimerSet(1);
		TimerOn();
		
		initUSART(0);
		USART_Flush(0);
		
		unsigned char i = 0;
		
		//Player
		tasks[i].state = Player_start;
		tasks[i].period = 10;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &Player;
		i++;
		
		//Obstacles
		tasks[i].state = Obstacles_start;
		tasks[i].period = 10;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &Obstacles;
		i++;
		
		//Matrix-output
		tasks[i].state = Matrix_Output_player;
		tasks[i].period = 10;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &Matrix_Output;
		i++;
		
		//Send Score
		tasks[i].state = check_send_rdy;
		tasks[i].period = 1;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &send_score;
		i++;
		
		//Send Lives
		tasks[i].state = check_lives_rdy;
		tasks[i].period = 1;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &send_lives;
		i++;
		
		//Receive Moves
		tasks[i].state = has_received;
		tasks[i].period = 1;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &receive_move;
		i++;
		
		//Hit Output
		tasks[i].state = Start_Hit;
		tasks[i].period = 1;
		tasks[i].elapsedTime = 0;
		tasks[i].TickFct = &Hit_Output;
		i++;
		
		while (1){}
		}