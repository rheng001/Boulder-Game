Richard Heng
---

How to run file
------------

1. Clone `https://github.com/rheng001/rshell.git`
2. `cd rshell`
3. `git checkout hw2`
4. `make`
5. `bin/rshell`

 ------------
High Level Description:
-----

The goal of the game is to outlast your opponent by dodging incoming obstacles for as long as possible without dying. Each player starts off with three lives, each time a player gets hit they can lose either one or two lives, some obstacles hit harder than others. Points are accumulated for each player for as long as they survive. As time progresses, a player’s power up meter will increase, at max meter, you can use a ability. The player who outlives their opponent wins the game.

------
User Guide
---

You control a red dot which acts as your player. The controls for the game is as follows:

*   **A** - Move Left
*   **1** - Move Right
*   **D** - Start Game
*  **#** - Reset Game
*   ***** - Use Power up

As time progresses, your power bar will fill up incrementally until it reaches max. If you press your power up while the meter is full, you are able to clear all obstacles presently on your screen. When each player runs out of lives the game will automatically stop and clear everything and display a message of who won the game. Each player get reset the game by pressing the reset button.

-----
Technologies/Components
-----

*   AVR Studio 6
*   LCD Display Screens (2)
*   8x8 Matrix LED (2)
*   2 16 key numpad (2)
*   Atmega1284 Microcontrollers (5)
*   Speaker (1)
*   Shift Registers
*   7 Segment LED Display (2)
*   LED bar 10-seg (2)

-----
Source Files
-----

**Player1LCD.c**

Contains LCD code and other components for player 1.

**Player2LCD.c**

Contains LCD code and other components for player 2.

**PlayerMatrix.c**

Controls each player’s 8x8 Matrix display which controls the gameplay.

**MusicCode.c**

Controls music that plays only during gameplay.

**io.c**

Contains functions to control our LCD. Supplied from lab.

**io.h**

Contains function declaration for LCD functions. Supplied from lab.

**usartATmega1284.h**

Contains USART functions for communicating between two Atmega1284 microcontrollers. Supplied from lab.

-----
Bugs
-----

*   Sometimes when a player gets hit with an obstacle their life will decrease by 2 lives instead of 1.

*   The display signal of who wins may sometime display the wrong player winning but rarely happens.

*   After a few resets of the game, there is a potential to have unexpected results when the game is running, you must power off and on the board for everything to be reset fully.

*   Before you start the game, you can hear the music playing very quietly in the background until you press the start button.

*   When the game starts, the player must move right before he is able to move left initially or else the character will not move on the screen. After doing so you can move freely.
