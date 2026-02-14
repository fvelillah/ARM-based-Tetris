/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD.h" 
#include "TouchPanel.h"
#include "timer.h"
#include "stdio.h"
#include "tetris.h"
#include "button.h"
#include "RIT.h"


#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
	game_state=INITIAL; // Game starts in INITIAL state
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	LCD_Initialization();
	BUTTON_init();
	
	// Start grid initialization
	int row;
	int col;
	for (row=0;row<10;row++) {
		for (col=0;col<20;col++){
			start_grid[col][row]=0;
		}
	}
	
	game_width = LCD_width/2+40;
	delta_w = game_width/grid_width;
	delta_h = LCD_height/grid_height;
	
	init_Grid(game_width, LCD_height, grid_width, grid_height);
	display_title();
	
	set_Score(game_width, score_height,0,0,0);
	
	soft_drop = 0;
	
	/* Standard Timers */
	//init_timer(0,0x17D7840); // For the LandTiger 
	init_timer(0, 0x1312D0 ); // For the emulator
	//init_timer(1,0x1312D0); // For the LandTiger 
	init_timer(1, 0x6108 ); // For the emulator
	
	enable_timer(0);
	
	/* Repetitive Interrupt Timer (RIT) */
	init_RIT(0x1312A0);									// For the LandTiger and emulator

	enable_RIT();
		
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	SCB->SCR |= 0x2;											/* set SLEEPONEXIT */
	
	__ASM("wfi");

}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
