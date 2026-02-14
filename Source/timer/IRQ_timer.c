/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "LPC17xx.h"
#include "timer.h"
#include "GLCD.h" 
#include "TouchPanel.h"
#include <stdio.h> /*for sprintf*/
#include "tetris.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
extern unsigned char grid[20][10];
extern unsigned char prev_grid[20][10];
extern int delta_w;
extern int delta_h;
extern int test_h;
extern int new_tetrotype;



void TIMER0_IRQHandler (void)
{
	if (game_state==PLAYING) {
		int drop;
		if (soft_drop && check_colisions(piece,anchor.x,anchor.y+2,start_grid)) {
			drop = 2;
		} 
		else if (soft_drop && check_colisions(piece,anchor.x,anchor.y+1,start_grid)){
			drop = 1;
		}
		else {
			drop =1;
		}
		if (check_colisions(piece,anchor.x,anchor.y+drop,start_grid)) {
			Tetromino temp_piece;
			temp_piece=abs_piece;
			temp_piece.color_id=0;
			set_Tetromino_on_Grid(temp_piece,grid);
			anchor.y +=drop;
			abs_piece=get_abs_positions(piece,anchor);
			set_Tetromino_on_Grid(abs_piece,grid);
			update_Full_Grid(grid,prev_grid,delta_w,delta_h);
		} 
		else {
			int row;
			int col;
			check_and_clear_lines(grid,prev_grid);
		  set_Score(game_width,score_height, top_score, curr_score, lines_cleared);
			for (row=0;row<10;row++) {
				for (col=0;col<20;col++){
					start_grid[col][row]=grid[col][row];
				}
			}
			check_gameover(abs_piece);
			if (game_state==PLAYING){
				spawn_new_piece(&anchor, &piece, &abs_piece, grid,delta_w,delta_h);
			}
		}
	} else if (game_state==GAME_OVER) {
		game_state=INITIAL;
		handle_gameover();
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler for button de-bouncing
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)

{
	static int down1 = 0; /*used for KEY 1*/
	static int down2 = 0; /*used for KEY 2*/
	
	// KEY 1: Game state modification
	if((LPC_PINCON->PINSEL4 & (1 << 22)) == 0){
		down1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){   //key1
			switch(down1){
				case 1:
					if (game_state == INITIAL) {
						srand(LPC_TIM0->TC); 
						init_Grid(game_width, LCD_height, grid_width, grid_height);
						set_Score(game_width, score_height,top_score,0,0);
						spawn_new_piece(&anchor, &piece, &abs_piece, grid,delta_w,delta_h);
						game_state = PLAYING;
						reset_RIT();
					} 
					else if (game_state == PLAYING) {
						game_state = PAUSED;
					}
					else if (game_state == PAUSED) {
						game_state = PLAYING;
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down1=0;			
			disable_timer(1);
			reset_timer(1);
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	// KEY 2: Hard drop implementation
	if((LPC_PINCON->PINSEL4 & (1 << 24)) == 0){
		Tetromino temp_piece;
		down2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){   //key2
			switch(down2){
				case 1:
					if (game_state==PLAYING) {
						temp_piece=abs_piece;
					
						while (check_colisions(piece,anchor.x,anchor.y+1,start_grid)) {
							anchor.y++;
							abs_piece=get_abs_positions(piece,anchor);
						}
						temp_piece.color_id=0;
						set_Tetromino_on_Grid(temp_piece,grid);
						set_Tetromino_on_Grid(abs_piece,grid);
						update_Full_Grid(grid,prev_grid,delta_w,delta_h);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down2=0;			
			disable_timer(1);
			reset_timer(1);
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
