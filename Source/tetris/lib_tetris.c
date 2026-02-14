/* tetris_logic.c */
#include "tetris.h"
#include "LPC17xx.h"
#include "GLCD.h" 
#include "TouchPanel.h"
#include "timer.h"
#include "stdio.h"
#include "tetris.h"
#include "button.h"
#include "RIT.h"


Tetromino all_shapes[7] = {
    // 1. I-shaped: Centered on the Y-axis
    {{{0,-1}, {0,0}, {0,1}, {0,2}}, 1}, 
    // 2. O-shaped: O-shape is usually not rotated, but this keeps it tight
    {{{0,0}, {1,0}, {0,1}, {1,1}}, 2}, 
    // 3. T-shaped: (0,0) is the middle of the "flat" side
    {{{0,0}, {-1,0}, {1,0}, {0,1}}, 3},
    // 4. J-shaped
    {{{0,0}, {0,-1}, {0,1}, {-1,1}}, 4},
    // 5. L-shaped
    {{{0,0}, {0,-1}, {0,1}, {1,1}}, 5}, 
    // 6. S-shaped
    {{{0,0}, {1,0}, {0,1}, {-1,1}}, 6},
    // 7. Z-shaped
    {{{0,0}, {-1,0}, {0,1}, {1,1}}, 7}  
};

Tetromino piece;
Tetromino abs_piece;

Point anchor;

int delta_w;
int delta_h;
int soft_drop;
int top_score;
int curr_score;
int lines_cleared;
int game_width;
int score_height = 30;
int LCD_width = 240;
int LCD_height = 320;
int grid_width = 10;
int grid_height = 20;

GameState game_state;

unsigned char grid[20][10]; // Current state of the game
unsigned char prev_grid[20][10]; // State of the last rendered frame
unsigned char start_grid[20][10]; 

void init_Grid(int game_width, int LCD_height, int grid_width, int grid_height) {
	int row;
	int col;
	int delta_w;
	int delta_h;
	int counter_h;
	int counter_w;
	
	LCD_Clear(Black);
	LCD_DrawLine( game_width, 0, game_width, LCD_height , Blue);
	
	delta_w = game_width/grid_width;
	delta_h = LCD_height/grid_height;
	// Horizontal lines
	for (counter_h=0;counter_h<=LCD_height;counter_h+=delta_h) {
		LCD_DrawLine( 0, counter_h, game_width, counter_h , White);
	}
	// Vertical lines
	for (counter_w=0;counter_w<game_width;counter_w+=delta_w) {
		LCD_DrawLine( counter_w, 0, counter_w, LCD_height , White);
	}
	// Init. prev_grid and grid matrices
	for (row = 0; row < 20; row++) {
		for (col = 0; col < 10; col++) {
			grid[row][col]=0;
			prev_grid[row][col]=0;
		}
	}
}

void set_Score(int game_width,int score_height, int top_score, int curr_score, int lines_cleared){
	char top_score_str[10];
	char curr_score_str[10];
	char lines_cleared_str[10];
	sprintf(top_score_str, "%d", top_score);
	sprintf(curr_score_str, "%d", curr_score);
	sprintf(lines_cleared_str, "%d", lines_cleared);
	GUI_Text(game_width+10, score_height, (uint8_t *) "TOP", White, Black);
	GUI_Text(game_width+10, score_height+20, (uint8_t *) top_score_str, White, Black);
	GUI_Text(game_width+10, score_height+40, (uint8_t *) "SCORE", White, Black);
	GUI_Text(game_width+10, score_height+60, (uint8_t *) curr_score_str, White, Black);
	GUI_Text(game_width+10, score_height+80, (uint8_t *) "LINES", White, Black);
	GUI_Text(game_width+10, score_height+100, (uint8_t *) lines_cleared_str, White, Black);
}

// AI assisted function
void update_Full_Grid(unsigned char grid[20][10], unsigned char prev_grid[20][10], int delta_w, int delta_h) {
    int row, col, i;
    uint16_t current_color;
    int x_start, y_start;

    for (row = 0; row < 20; row++) {
			for (col = 0; col < 10; col++) {
				if (grid[row][col] != prev_grid[row][col]) {
					// 1. Assign color based on the shape ID in the matrix
					switch (grid[row][col]) {
						case 0:  current_color = Black;   break; // Empty 
						case 1:  current_color = Cyan;    break; // I-shaped 
						case 2:  current_color = Yellow;  break; // O-shaped 
						case 3:  current_color = Magenta; break; // T-shaped 
						case 4:  current_color = Blue;    break; // J-shaped 
						case 5:  current_color = Green;  break; // L-shaped 
						case 6:  current_color = Orange;   break; // S-shaped 
						case 7:  current_color = Red;     break; // Z-shaped
						default: current_color = Black;   break;
					}

					// 2. Calculate pixel offsets 
					x_start = col * delta_w;
					y_start = row * delta_h;

					// 3. Fill the square cell 
					// We use vertical lines to fill the width of the cell
					for (i = 1; i < delta_w; i++) {
						LCD_DrawLine(x_start + i, 
												 y_start + 1, 
												 x_start + i, 
												 y_start + delta_h - 1, 
												 current_color);
					}
					prev_grid[row][col] = grid[row][col];
				}
			}
    }
}

// AI assisted function
Tetromino get_abs_positions(Tetromino relative_piece, Point anchor){
	Tetromino abs_piece;
	int i;
	
	abs_piece.color_id=relative_piece.color_id;
	for (i=0;i<4;i++) {
		abs_piece.shape[i].x=anchor.x+relative_piece.shape[i].x;
		abs_piece.shape[i].y=anchor.y+relative_piece.shape[i].y;
	}
	return abs_piece;
}

// AI assisted function
int check_colisions(Tetromino piece, int anchor_x, int anchor_y, unsigned char grid[20][10]){
	int i;
	volatile check;
	int max_y=0;
	for (i=0;i<4;i++){
		int abs_x=anchor_x+piece.shape[i].x;
		int abs_y=anchor_y+piece.shape[i].y;
		// Horizontal boundaries
		if (abs_x < 0 || abs_x > 9) {
			check=1;
			return 0;
		}
		// Vertical boundaries
		if (abs_y >= 20) {
			check=2;
			return 0;
		}
		// Overlap with existing blocks
		if (abs_y >= 0) {
			if (grid[abs_y][abs_x] != 0){
				check=3;
				return 0;
			}
		}
	}
	return 1;
}

// AI assisted function
void rotate_current_piece(Tetromino *piece, int current_anchor_x, int current_anchor_y, unsigned char grid[20][10]) {
	int i;
	// 1. Skip rotation for O-shaped piece (ID 2) to save cycles
	if (piece->color_id == 2) return;

	// 2. Create a temporary copy to test the rotation
	Tetromino temp = *piece;

	for (i = 0; i < 4; i++) {
		int oldX = piece->shape[i].x;
		int oldY = piece->shape[i].y;

		// Apply 90-degree clockwise transformation
		temp.shape[i].x = -oldY;
		temp.shape[i].y = oldX;
	}
	// 3. Collision Check (Crucial step before applying)
	// Only update the real piece if the rotated version is valid
	if (check_colisions(temp, current_anchor_x, current_anchor_y, grid)) {
			*piece = temp;
	} 
}

void set_Tetromino_on_Grid(Tetromino abs_piece, unsigned char grid[20][10]) {
	int i;
	int color;
	int x_pos;
	int y_pos;
	for (i=0;i<4;i++) {
		color = abs_piece.color_id;
		x_pos = abs_piece.shape[i].x;
		y_pos = abs_piece.shape[i].y;
		if (x_pos >= 0 & x_pos < 10) {
			if (y_pos >= 0 & y_pos < 20) {
				grid[y_pos][x_pos]=color;
			}
		}
	}
}

void spawn_new_piece(Point *anchor, Tetromino *piece, Tetromino *abs_piece, unsigned char grid[20][10], int delta_w, int delta_h) {
	int new_shape = rand() % 7;
	*piece=all_shapes[new_shape];
	anchor->x=1+rand()%6;
	anchor->y=-1;
	*abs_piece=get_abs_positions(*piece,*anchor);
	set_Tetromino_on_Grid(*abs_piece, grid);
	update_Full_Grid(grid,prev_grid,delta_w,delta_h);
}

// AI assisted function
void check_and_clear_lines(unsigned char grid[20][10], unsigned char prev_grid[20][10]) {
	int row, col, r_above;
	int lines_cleared_now = 0;
	for (row = 19; row >= 0; row--) { // Start from the bottom 
		int is_full = 1;
		
		for (col = 0; col < 10; col++) {
			if (grid[row][col] == 0) {
					is_full = 0;
					break;
			}
		}
		
		if (is_full) {
			lines_cleared_now++;
			for (r_above = row; r_above > 0; r_above--) {
					for (col = 0; col < 10; col++) {
							grid[r_above][col] = grid[r_above - 1][col];
					}
			}
			// The top-most row must be cleared after the shift
			for (col = 0; col < 10; col++) {
					grid[0][col] = 0;
			}
			// Re-check the current row index because a new row was shifted into it
			row++; 
		}
	}

	// Update scoring based on Spec 9 [cite: 134-137]
	if (lines_cleared_now > 0) {
		lines_cleared += lines_cleared_now;
		
		if (lines_cleared_now == 4) {
			// A "Tetris" awards 600 points + 10 for placement [cite: 132, 137]
			curr_score += 610; 
		} else {
			// Clearing multiple lines: 100 * lines + 10 [cite: 136]
			curr_score += (100 * lines_cleared_now) + 10;
		}
	} else {
		// Just the placement points if no lines cleared [cite: 134]
		curr_score += 10;
	}
}

void check_gameover(Tetromino abs_piece) {
	int i;
	for (i=0;i<4;i++){
		if (abs_piece.shape[i].y < 0) {
			game_state=GAME_OVER;
		} 
	}
}

void handle_gameover(void) {
	int row;
	int col;
	
	if (curr_score>top_score) {
		top_score=curr_score;
	}
	
	curr_score=0;
	lines_cleared=0;
	
	// Grid clearing
	for (row = 0; row < 20; row++) {
		for (col = 0; col < 10; col++) {
			grid[row][col]=0;
			prev_grid[row][col]=0;
			start_grid[row][col]=0;
		}
	}

	
	GUI_Text(30, score_height, (uint8_t *)"  GAME OVER  ", Red, White);
	GUI_Text(30, score_height+20, (uint8_t *)" Press KEY1  ", Red, White);
	GUI_Text(30, score_height+40, (uint8_t *)" to Restart  ", Red, White);
}

void display_title(void) {
	int spacing = 16; // Width of a character + gap
	// T - Magenta
	GUI_Text(30, score_height, (uint8_t *)"T", Magenta, Black);
	// E - Orange
	GUI_Text(30 + spacing, score_height, (uint8_t *)"E", Orange, Black);
	// T - Cyan
	GUI_Text(30 + spacing * 2, score_height, (uint8_t *)"T", Cyan, Black);
	// R - Red
	GUI_Text(30 + spacing * 3, score_height, (uint8_t *)"R", Red, Black);
	// I - Yellow
	GUI_Text(30 + spacing * 4, score_height, (uint8_t *)"I", Yellow, Black);
	// S - Green
	GUI_Text(30 + spacing * 5, score_height, (uint8_t *)"X", Green, Black);
	
	GUI_Text(30, score_height+20, (uint8_t *)" Press KEY1 ", Blue, White);
	GUI_Text(30, score_height+40, (uint8_t *)" to Play    ", Blue, White);
	
	GUI_Text(20, 260, (uint8_t *)"      FVH      ", White, Black);
  GUI_Text(20, 280, (uint8_t *)" (C) 2026 CORP ", Grey, Black);
}


