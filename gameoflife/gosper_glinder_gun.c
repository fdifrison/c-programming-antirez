#include <stdio.h>
#include <unistd.h>

// Expanded grid size so the gliders have room to travel!
#define ROWS 40
#define COLS 80
#define GRID_SIZE (COLS*ROWS)
#define ALIVE '*'
#define DEAD  '.'
#define CLEAR "\x1b[3J\x1b[H\x1b[2J"

/* 2D to 1D transformation, mapping x (row) and y (column) coordinates.
 * Robust mathematical wrapping for positive and negative numbers.
 */
int cell_to_index(int x, int y) {
    // True mathematical modulo that works correctly for negative numbers in C
    x = (x % ROWS + ROWS) % ROWS;
    y = (y % COLS + COLS) % COLS;

    return x * COLS + y;
}

/* set the state of the cell at position x,y */
void set_cell(char *grid, int x, int y, char state) {
    grid[cell_to_index(x, y)] = state;
}

/* return the state at position x,y */
char get_cell_status(char *grid, int x, int y) {
    return grid[cell_to_index(x, y)];
}

void print_grid(char *grid) {
    printf(CLEAR);
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLS; y++) {
            printf("%c", get_cell_status(grid, x, y));
        }
        printf("\n");
    }
}

void init_grid(char *grid) {
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLS; y++) {
            set_cell(grid, x, y, DEAD);
        }
    }
}

int count_living_neighbors(char *grid, int x, int y) {
    int alive = 0;
    for (int x_offset = -1; x_offset <= 1; x_offset++) {
        for (int y_offset = -1; y_offset <= 1; y_offset++) {
            if (x_offset == 0 && y_offset == 0) continue;
            if (get_cell_status(grid, x + x_offset, y + y_offset) == ALIVE) alive++;
        }
    }
    return alive;
}

void compute_next_state(char *old_grid, char *new_grid) {
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLS; y++) {
            const int n_alive = count_living_neighbors(old_grid, x, y);
            int next_state = DEAD;
            if (get_cell_status(old_grid, x, y) == ALIVE) {
                if (n_alive == 2 || n_alive == 3) {
                    next_state = ALIVE;
                }
            } else {
                if (n_alive == 3) {
                    next_state = ALIVE;
                }
            }
            set_cell(new_grid, x, y, next_state);
        }
    }
}

int main(void) {
    char old_grid[GRID_SIZE];
    char new_grid[GRID_SIZE];
    init_grid(old_grid);
    init_grid(new_grid);

    // Shifted coordinates slightly down (+5) and right (+5)
    // so it sits beautifully inside our larger 40x80 canvas
    int dx = 5;
    int dy = 5;

    // Left Block (Stabilizer)
    set_cell(old_grid, dx+4, dy+0, ALIVE);
    set_cell(old_grid, dx+4, dy+1, ALIVE);
    set_cell(old_grid, dx+5, dy+0, ALIVE);
    set_cell(old_grid, dx+5, dy+1, ALIVE);

    // Left Queen Bee Shuttle (Left Engine)
    set_cell(old_grid, dx+4, dy+10, ALIVE);
    set_cell(old_grid, dx+5, dy+10, ALIVE);
    set_cell(old_grid, dx+6, dy+10, ALIVE);
    set_cell(old_grid, dx+3, dy+11, ALIVE);
    set_cell(old_grid, dx+7, dy+11, ALIVE);
    set_cell(old_grid, dx+2, dy+12, ALIVE);
    set_cell(old_grid, dx+8, dy+12, ALIVE);
    set_cell(old_grid, dx+2, dy+13, ALIVE);
    set_cell(old_grid, dx+8, dy+13, ALIVE);
    set_cell(old_grid, dx+5, dy+14, ALIVE);
    set_cell(old_grid, dx+3, dy+15, ALIVE);
    set_cell(old_grid, dx+7, dy+15, ALIVE);
    set_cell(old_grid, dx+4, dy+16, ALIVE);
    set_cell(old_grid, dx+5, dy+16, ALIVE);
    set_cell(old_grid, dx+6, dy+16, ALIVE);
    set_cell(old_grid, dx+5, dy+17, ALIVE);

    // Right Queen Bee Shuttle (Right Engine)
    set_cell(old_grid, dx+2, dy+20, ALIVE);
    set_cell(old_grid, dx+3, dy+20, ALIVE);
    set_cell(old_grid, dx+4, dy+20, ALIVE);
    set_cell(old_grid, dx+2, dy+21, ALIVE);
    set_cell(old_grid, dx+3, dy+21, ALIVE);
    set_cell(old_grid, dx+4, dy+21, ALIVE);
    set_cell(old_grid, dx+1, dy+22, ALIVE);
    set_cell(old_grid, dx+5, dy+22, ALIVE);
    set_cell(old_grid, dx+0, dy+24, ALIVE);
    set_cell(old_grid, dx+1, dy+24, ALIVE);
    set_cell(old_grid, dx+5, dy+24, ALIVE);
    set_cell(old_grid, dx+6, dy+24, ALIVE);

    // Right Block (Stabilizer)
    set_cell(old_grid, dx+2, dy+34, ALIVE);
    set_cell(old_grid, dx+3, dy+34, ALIVE);
    set_cell(old_grid, dx+2, dy+35, ALIVE);
    set_cell(old_grid, dx+3, dy+35, ALIVE);

    while (1) {
        compute_next_state(old_grid, new_grid);
        print_grid(new_grid);
        usleep(6000); // 60ms delay for smooth animation
        compute_next_state(new_grid, old_grid);
        print_grid(old_grid);
        usleep(6000);
    }
    return 0;
}
