#ifndef __TETRIS_H
#define __TETRIS_H

#include <stdint.h>

// Tetromino
typedef struct {
	int x;
	int y;
} Point;

typedef struct {
    Point shape[4];     // The 4 blocks making up the tetromino
    uint16_t color_id;  // The ID (1-7) to match grid colors
} Tetromino;

// Grid control
extern unsigned char grid[20][10]; // Current state of the game
extern unsigned char prev_grid[20][10]; // State of the last rendered frame
extern unsigned char start_grid[20][10]; // Used to compare piece position with respect to grid starting values for colision detection

// Measures
extern int delta_w;
extern int delta_h;
extern int game_width;
extern int score_height;
extern int LCD_width;
extern int LCD_height;
extern int grid_width;
extern int grid_height;

// Tetromino control
extern Tetromino all_shapes[7];
extern Point anchor;
extern Tetromino piece;
extern Tetromino abs_piece;
extern int soft_drop;
extern int hard_drop;

// Game state
typedef enum { INITIAL, PLAYING, PAUSED, GAME_OVER } GameState;
extern GameState game_state;

// Scoring
extern int top_score;
extern int curr_score;
extern int lines_cleared;

// Functions
void init_Grid(int game_width, int LCD_height, int grid_width, int grid_height);
void set_Score(int game_width,int score_height, int top_score, int curr_score, int lines_cleared);
void update_Full_Grid(unsigned char grid[20][10], unsigned char prev_grid[20][10], int delta_w, int delta_h);
Tetromino get_abs_positions(Tetromino relative_piece, Point anchor);
void set_Tetromino_on_Grid(Tetromino abs_piece, unsigned char grid[20][10]);
void rotate_current_piece(Tetromino *piece, int current_anchor_x, int current_anchor_y, unsigned char grid[20][10]);
int check_colisions(Tetromino piece, int anchor_x, int anchor_y, unsigned char grid[20][10]);
void spawn_new_piece(Point *anchor, Tetromino *piece, Tetromino *abs_piece, unsigned char grid[20][10], int delta_w, int delta_h);
void check_and_clear_lines(unsigned char grid[20][10], unsigned char prev_grid[20][10]);
void check_gameover(Tetromino abs_piece);
void handle_gameover(void);
void display_title(void);
#endif