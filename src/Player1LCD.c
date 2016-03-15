/* Partner(s) Name & E­mail: Richard Heng rheng001@ucr.edu
*  Partner(s) Name & E­mail: Edward Hernandez ehern044@ucr.edu
* Lab Section: 21
* Assignment: Custom Project
* Exercise Description: This controls the LCD, keypad, led bar, and 7 segment display
*
* I acknowledge all content contained herein, excluding template or example
* code, is my own original work.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "io.c"
#include "usart_ATmega1284.h"

/////////////////GLOBAL VARIABLES//////////////////////////////

unsigned char x;
unsigned char movement = 0; //Movement values
unsigned short powerup_time = 0x00;
unsigned char flag = 0;
unsigned char powerTmp = 0x00;
unsigned char startPower = 0x00;

unsigned char Number0 = 0b00000001;
unsigned char Number1 = 0b01001111;
unsigned char Number2 = 0b00010010;
unsigned char Number3 = 0b00000110;
unsigned char PlayerLives = 0x03;
unsigned char hitRecieved = 0;

uint8_t score = 0x00;
uint8_t getScore = 0x00;
uint8_t getLives = 0x00;
uint8_t getPlayer2 = 0x00;

unsigned char gameOver = 0;
unsigned char reset = 0;

unsigned char player2Lose = 0;
unsigned char player2Received = 0x00;
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
task tasks[9];
const unsigned char tasksNum = 9;
const unsigned long tasksPeriodGCD = 1;
const unsigned long periodKeypad = 1;

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


///////////////GLOBAL FUNCTIONS///////////////////////////////////////////

void returnLeft()
{
	movement = 1;
}
void returnRight()
{
	movement = 2;
}
void returnNo()
{
	movement = 0;
}
void returnStart()
{
	movement = 3;
}
void returnPowerUse()
{
	movement = 4;
}
void returnReset()
{
	movement = 5;
}
void returnPower()
{
	startPower = 1;
}
void setReset()
{
	gameOver = 0;
	score = 0;
	PlayerLives = 0x03;
	player2Lose = 0;
}
void addScore()
{
	score++;
	_delay_ms(10);
}
void returnHit()
{
	hitRecieved = 1;
}
void returnNoHit()
{
	hitRecieved = 0;
}
void loseLives()
{
	PlayerLives--;
	_delay_ms(1000);
}
void sendGameOver()
{
	movement = 6;
}
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

unsigned char GetKeypadKey() {
	// Check keys in col 1
	// Enable col 4 with 0, disable others with 1’s
	// The delay allows PORTC to stabilize before checking
	PORTB = 0xEF;
	asm("nop");
	if (GetBit(PINB,0)==0) { return('1'); }
	if (GetBit(PINB,3)==0) { return('*'); }
	// Check keys in col 2
	// Enable col 5 with 0, disable others with 1’s
	// The delay allows PORTC to stabilize before checking
	PORTB = 0xDF;
	asm("nop");
	// ... *****FINISH*****
	// Check keys in col 3
	// Enable col 6 with 0, disable others with 1’s
	// The delay allows PORTC to stabilize before checking
	PORTB = 0xBF;
	asm("nop");
	if (GetBit(PINB,3)==0) { return('#'); }
	// ... *****FINISH*****
	// Check keys in col 4
	PORTB = 0x7F;
	asm("nop");
	if (GetBit(PINB,0)==0) { return('A'); }
	if (GetBit(PINB,3)==0) { return('D'); }
	// ... *****FINISH*****
	else
	{
		return (' ');
	}
	return('\0'); // default value
}

///////////////END GLOBAL FUNCTIONS///////////////////////////////////////////


///////////////TASK FUNCTIONS///////////////////////////////////////////
void displayScreen()
{
	unsigned char tmpScore = score;
	if(gameOver != 1)
	{
		LCD_DisplayString(1, "Player 1");
		LCD_Cursor(17);
		LCD_WriteData('S');
		LCD_Cursor(18);
		LCD_WriteData('C');
		LCD_Cursor(19);
		LCD_WriteData('O');
		LCD_Cursor(20);
		LCD_WriteData('R');
		LCD_Cursor(21);
		LCD_WriteData('E');
		LCD_Cursor(22);
		LCD_WriteData(':');
		
		//Current Score
		LCD_Cursor(23);
		LCD_WriteData( tmpScore / 100  + '0'); //hundreds place
		tmpScore = tmpScore - (tmpScore / 100) * 100;

		LCD_Cursor(24);
		LCD_WriteData(tmpScore / 10 + '0'); //tens place

		LCD_Cursor(25);
		LCD_WriteData(tmpScore % 10 + '0'); //ones place*/
		}
		else if(gameOver == 1 && player2Lose != 1)
		{
			sendGameOver();
			LCD_DisplayString(1, "You Lose        Player 2 Wins"); //I lose
			_delay_ms(1);
		}
		else if (gameOver == 1 && player2Lose == 1) //I win
		{
			sendGameOver();
			LCD_DisplayString(1, "You Win         Player 2 Loses");
			_delay_ms(1);
		}
}
		
	enum Keypad_States { Key_Start, Key_press, wait, waitMenu };
	int TickFct_Keypad(int state);
	int TickFct_Keypad(int state)
	{
	x = GetKeypadKey();
		
	switch (state)
	{
	case Key_Start:
	state = wait;
	break;
		
	case wait:
	if (x == '\0')
	{state = wait;}
	else
	{state = Key_press;}
	break;
		
	case Key_press:
	if (x == '\0')
	{state = wait;}
	else if (flag == 0)
	{
	state = waitMenu;
	}
	else
	{
	state = Key_press;
	}
	break;
		
	case waitMenu:
	if(flag == 2)
	{
	state = Key_press;
	}
	else
	{
	state = waitMenu;
	}
	break;
		
		
	default:
	state = Key_Start;
	break;
	}
	switch (state)
	{
	case Key_Start:
	break;
		
	case wait:
	break;
		
	case waitMenu:
	LCD_DisplayString(1, "Press Start (D)");
	_delay_ms(1000);
		
	case Key_press:
	switch (x)
	{
	case ' ': //Nothing Pressed
	returnNo();
	break;
		
	case '1':  //Right
	returnRight();
	break;
		
	case 'A': //Left
	returnLeft();
	break;
		
	case 'D': //Start
	returnStart();
	returnPower();
	flag = 2;
	break;
		
	case '*': //Use Powerup
	returnPowerUse();
	break;
		
	case '#': //Reset
	returnReset();
	setReset();
	break;
	}
	break;
		
	default:
	break;
	}
	return state;
	}

	enum send_movement_states { check_send_rdy, has_transmitted, move } send_movement_state;
	int send_move( int state );
	int send_move( int state )
	{
	switch( state ) //Transitions
	{
	case check_send_rdy:
	if( USART_IsSendReady(0) )
	{
	state = move;
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
		
	case move:
	state = has_transmitted;
	break;
		
	default:
	state = check_send_rdy;
	break;
	}
		
	switch( state )  //Actions
	{
		
	case check_send_rdy:
	break;
		
	case has_transmitted:
	break;
		
	case move:
	if(movement == 0) //Nothing Pressed
	{
	if(gameOver == 1)
	{
	USART_Send(0b11001100, 0);
	_delay_ms(100);
	}
	else
	{
	USART_Send(0b11111100, 0);
	_delay_ms(100);
	}
	}
	else if(movement == 1) //A
	{
	USART_Send(0b11110000, 0);
	_delay_ms(100);
	}
	else if (movement == 2) //1
	{
	USART_Send(0b11111000, 0);
	_delay_ms(100);
	}
	else if (movement == 3) //Start Game
	{
	USART_Send(0b11111110, 0);
	_delay_ms(100);
	}
	else if (movement == 4 && powerup_time >= 3000) //Send Powerup
	{
	USART_Send(0b11101110, 0);
	powerup_time = 0x00;
	_delay_ms(100);
	}
	else if (movement == 5) //Reset
	{
	USART_Send(0b10101010, 0);
	_delay_ms(100);
	}
	break;
		
	default:
	break;
	}
	return state;
	}

	enum Player_stat_states {Start_Player_stat_start, Player_stat_start,  Player_stat_powerup} Player_stat_state;
	void Player_stat()
	{
	switch (Player_stat_state) //State transitions
	{
	case Start_Player_stat_start:
	if(startPower == 1 && gameOver != 1)
	{
	Player_stat_state = Player_stat_start;
	}
	else
	{
	Player_stat_state = Start_Player_stat_start;
	}
	break;
		
	case Player_stat_start:
	Player_stat_state = Player_stat_powerup;
	break;
		
	case Player_stat_powerup:
	if (powerup_time == 1500  )
	{
	powerTmp = SetBit(powerTmp, 5, 1);
	PORTD = powerTmp;
	_delay_ms(1);
	}
	if (powerup_time == 3000)
	{
	powerTmp = SetBit(powerTmp, 5, 1);
	powerTmp = SetBit(powerTmp, 4, 1);
	PORTD = powerTmp;
	_delay_ms(1);
	}
	if(powerup_time > 3000 && movement == 4 || gameOver == 1) //Pressed D and meter full
	{
		powerTmp = SetBit(powerTmp, 5, 0);
		powerTmp = SetBit(powerTmp, 4, 0);
		powerTmp = SetBit(powerTmp, 3, 0);
		PORTD = powerTmp;
	}
		
	else
	{
	Player_stat_state = Player_stat_powerup;
	}
		
	break;
		
	default:
	Player_stat_state = Player_stat_start;
	break;
	}

	switch (Player_stat_state) //State actions
	{
	case Player_stat_start:
	break;
		
	case Player_stat_powerup:
	powerup_time++;
	break;
		
	default:
	break;
	}
	}

	enum Lives_states {Start_Lives_start, Lives_start, Game_over} Lives_state;
	void Lives()
	{
	switch (Lives_state) //State transitions
	{
	case Lives_start:
	Lives_state = Lives_start;
	break;
		
		
	default:
	Lives_state = Lives_start;
	break;
	}

	switch (Lives_state) //State transitions
	{
	case Lives_start:
	if(hitRecieved == 1)
	{
	loseLives();
	}
	if (PlayerLives == 3)
	{
	PORTA = Number3;
	}
	if (PlayerLives == 2)
	{
	PORTA = Number2;
	}
	if (PlayerLives == 1)
	{
	PORTA = Number1;
	}
	if (PlayerLives <= 0)
	{
	PORTA = Number0;
	gameOver = 1;
	}
	break;
		
	default:
	break;
	}
	}

	enum receive_score_states { has_received, receive, flush } receive_score_state;
	int receive_score( int state )
	{
		
	switch( state ) //Transitions
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
		
	switch( state )  //Actions
	{
	case has_received:
	break;
		
	case receive:
	getScore = USART_Receive(0);
	if(getScore == 0b11111111)
	{
	addScore();
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

	enum receive_lives_states { has_received_lives, receiveLives, flushLives } receive_lives_state;
	int receive_lives (int state )
	{
		
	switch( state )
	{
	case has_received_lives:
	if( USART_HasReceived(0) )
	{
	state = receiveLives;
	}
	else
	{
	state = has_received_lives;
	}
	break;
		
	case receiveLives:
	state = flushLives;
	break;
		
	case flushLives:
	state = has_received_lives;;
	break;
		
	default:
	state = has_received_lives;
	break;
		
	}
		
	switch( state )
	{
	case has_received_lives:
	break;
		
	case receiveLives:
	getLives = USART_Receive(0);
	if(getLives == 0b10111111)
	{
	returnHit();
	}
	else if (getLives == 0b10011111)
	{
	returnNoHit();
	}
	break;
		
	case flushLives:
	USART_Flush(0);
	break;
		
	default:
	break;
		
	}
		
	return state;
	}
		
	enum send_Loser_states { check_Loser_rdy, has_transmittedLoser, moveLoser } send_Loser_state;
	int send_Loser( int state );
	int send_Loser( int state )
	{
	switch( state ) //Transitions
	{
	case check_Loser_rdy:
	if( USART_IsSendReady(1) )
	{
	state = moveLoser;
	}
	else
	{
	state = check_Loser_rdy;
	}
	break;
		
	case has_transmittedLoser:
	if( USART_HasTransmitted(1) )
	{
	state = check_Loser_rdy;
	}
	else
	{
	state = has_transmittedLoser;
	}
	break;
		
	case moveLoser:
	state = has_transmittedLoser;
	break;
		
	default:
	state = check_Loser_rdy;
	break;
	}
		
	switch( state )  //Actions
	{
		
	case check_Loser_rdy:
	break;
		
	case has_transmittedLoser:
	break;
		
	case moveLoser:
	if(gameOver != 1 && player2Received == 1)
	{
	USART_Send(0b01100110, 1); //Send P1 Not Lose
	player2Lose = 1;
	_delay_ms(100);
	}
	else
	{
	USART_Send(0b00000000, 1);
	player2Lose = 0;
	_delay_ms(100);
	}
	default:
	break;
	}
	return state;
	}

	enum receive_Loserstates { has_receivedLoser, receiveLoser, flushLoser } receive_Loser_state;
	int receive_Loser( int state ) {
		
	switch( state ) {
		
	case has_receivedLoser:
	if( USART_HasReceived(1) ) {
	state = receiveLoser;
	} else {
	state = has_receivedLoser;
	}
	break;
		
	case receiveLoser:
	state = flushLoser;
	break;
		
	case flushLoser:
	state = has_receivedLoser;;
	break;
		
	default:
	state = has_receivedLoser;
	break;
		
	}
		
	switch( state ) {
		
	case has_receivedLoser:
	break;
		
	case receiveLoser:
	getPlayer2 = USART_Receive(1);
	if(getPlayer2 > 0) //Player 2 Loses
	{
		player2Received = 1;
	}
	else
	{
		player2Received = 0;
	}
	break;
		
	case flushLoser:
	USART_Flush(1);
	break;
		
	default:
	break;
		
	}
		
	return state;
	}

	///////////////END TASK FUNCTIONS///////////////////////////////////////////

	int main(void)
	{
	DDRA = 0xFF; PORTA = 0x00; //7 Segment display
	DDRB = 0xF0; PORTB = 0x0F; //Numpad PCB..4 outputs init 0s, PCA..0 inputs init 1s
	DDRC = 0xFF; PORTC = 0x00; //LED
	DDRD = 0xFF; PORTD = 0x00; //LED and LED BAR
		
	initUSART(0);
	USART_Flush(0);
		
	initUSART(1);
	USART_Flush(1);
		
	unsigned char i = 0;
	LCD_init();
		
	// Keypad
	tasks[i].state = Key_Start;
	tasks[i].period = periodKeypad;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_Keypad;
	i++;
		
	//Send Key Presses
	tasks[i].state = check_send_rdy;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &send_move;
	i++;
		
	//Get Score
	tasks[i].state = has_received;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &receive_score;
	i++;
		
	//Get Lives
	tasks[i].state = has_received_lives;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &receive_lives;
	i++;
		
	//Display Screen
	tasks[i].state = 0;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &displayScreen;
	i++;
		
	//Display LED BAR (POWERUPS)
	tasks[i].state = 0;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Player_stat;
	i++;
		
	//Display 7-seg display (Lives)
	tasks[i].state = Lives_start;
	tasks[i].period = 10;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Lives;
	i++;
		
	//Send Loser
	tasks[i].state = check_Loser_rdy;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &send_Loser;
	i++;
		
	//Receive Loser
	tasks[i].state = has_receivedLoser;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &receive_Loser;
	i++;
		
	// Set the timer and turn it on
	TimerSet(tasksPeriodGCD);
	TimerOn();
		
	while(1){} //Need this
	return 0;
	}