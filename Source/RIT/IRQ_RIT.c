/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "RIT.h"
#include "tetris.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void RIT_IRQHandler (void)
{
	static int select=0;
	Tetromino temp_piece;
	temp_piece=abs_piece;
	
	// JOY_LEFT
	if ((LPC_GPIO1->FIOPIN & (1<<27)) ==0){
		select++;
		switch(select){
			case 1:
				if (game_state==PLAYING && check_colisions(piece,anchor.x-1,anchor.y+1,start_grid)){
					temp_piece.color_id=0;
					set_Tetromino_on_Grid(temp_piece,grid);
					anchor.x--;
					abs_piece=get_abs_positions(piece,anchor);
					set_Tetromino_on_Grid(abs_piece,grid);
					update_Full_Grid(grid,prev_grid,delta_w,delta_h);
				}
				break;
			default:
				break;
		}
	}
	
	// JOY_RIGHT
	else if ((LPC_GPIO1->FIOPIN & (1<<28)) ==0){
		select++;
		switch(select){
			case 1:
				if (game_state==PLAYING && check_colisions(piece,anchor.x+1,anchor.y+1,start_grid)){
					temp_piece.color_id=0;
					set_Tetromino_on_Grid(temp_piece,grid);
					anchor.x++;
					abs_piece=get_abs_positions(piece,anchor);
					set_Tetromino_on_Grid(abs_piece,grid);
					update_Full_Grid(grid,prev_grid,delta_w,delta_h);
				}
				break;
			default:
				break;
		}
	}
	
	// JOY_UP
	else if ((LPC_GPIO1->FIOPIN & (1<<29)) ==0){
		select++;
		switch(select){
			case 1:
				if (game_state==PLAYING) {
					temp_piece.color_id=0;
					set_Tetromino_on_Grid(temp_piece,grid);
					rotate_current_piece(&piece,anchor.x,anchor.y,start_grid);
					abs_piece=get_abs_positions(piece,anchor);
					set_Tetromino_on_Grid(abs_piece,grid);
					update_Full_Grid(grid,prev_grid,delta_w,delta_h);
				}
				break;
			default:
				break;
		}
	}
	
	// JOY_DOWN
	else if ((LPC_GPIO1->FIOPIN & (1<<26)) ==0){
		select++;
		int limit_select=5;
		if (select > limit_select) {
			soft_drop=1;
		} 
	}
	
	else {
		select=0;
		soft_drop=0;
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
