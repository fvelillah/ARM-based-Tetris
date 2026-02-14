
#include "button.h"
#include "lpc17xx.h"

/**
 * @brief  Function that initializes Buttons and Joystick
 */
void BUTTON_init(void) {
	// Joystick initialization
	// JOY_SEL
	LPC_PINCON->PINSEL3 &= ~(1<<18); /* GPIO pin selection */
	LPC_PINCON->PINSEL3 &= ~(1<<19);
	LPC_GPIO1->FIODIR &= ~(1<<25); /* PORT1.25 defined as input          */
	// JOY_DOWN
	LPC_PINCON->PINSEL3 &= ~(1<<20); /* GPIO pin selection */
	LPC_PINCON->PINSEL3 &= ~(1<<21);
	LPC_GPIO1->FIODIR &= ~(1<<26); /* PORT1.26 defined as input          */
	// JOY_LEFT
	LPC_PINCON->PINSEL3 &= ~(1<<22); /* GPIO pin selection */
	LPC_PINCON->PINSEL3 &= ~(1<<23);
	LPC_GPIO1->FIODIR &= ~(1<<27); /* PORT1.27 defined as input          */
	// JOY_RIGHT
	LPC_PINCON->PINSEL3 &= ~(1<<24); /* GPIO pin selection */
	LPC_PINCON->PINSEL3 &= ~(1<<25);
	LPC_GPIO1->FIODIR &= ~(1<<28); /* PORT1.28 defined as input          */
	// JOY_UP
	LPC_PINCON->PINSEL3 &= ~(1<<26); /* GPIO pin selection */
	LPC_PINCON->PINSEL3 &= ~(1<<27);
	LPC_GPIO1->FIODIR &= ~(1<<29); /* PORT1.29 defined as input          */
	
	// Buttons initialization
  LPC_PINCON->PINSEL4    |= (1 << 20);		 /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* PORT2.10 defined as input          */

  LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 11);    /* PORT2.11 defined as input          */
  
  LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
  LPC_GPIO2->FIODIR      &= ~(1 << 12);    /* PORT2.12 defined as input          */

  LPC_SC->EXTMODE = 0x7;

  NVIC_EnableIRQ(EINT2_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(EINT2_IRQn, 1);				 /* priority, the lower the better     */
  NVIC_EnableIRQ(EINT1_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(EINT1_IRQn, 2);				 
  NVIC_EnableIRQ(EINT0_IRQn);              /* enable irq in nvic                 */
	NVIC_SetPriority(EINT0_IRQn, 3);				 /* decreasing priority	from EINT2->0	 */
}
