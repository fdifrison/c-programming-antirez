#include <stdio.h>
#include <unistd.h>
// https://www.youtube.com/watch?v=c5atNuYdKK8&list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY&index=13

#define COLS 11
#define ROWS 11
#define GRID_SIZE (COLS*ROWS)
#define ALIVE '*'
#define DEAD  '.'
#define CLEAR "\x1b[3J\x1b[H\x1b[2J"

/* 2D to 1D transformation in rows, mapping a x,y coordinates into grid
 * index. Support wrapping for negative and overflow numbers
 */
int cell_to_index(int x, int y) {
    if (x >= COLS) x = x % COLS;
    if (y >= ROWS) y = y % ROWS;
    if (x < 0) x = COLS + (x % COLS);
    if (y < 0) y = ROWS + (y % ROWS);
    // 2D to 1D transformation in rows
    return y * COLS + x;
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
    // clear | hexdump -C 00000000  1b 5b 33 4a 1b 5b 48 1b  5b 32 4a                 |.[3J.[H.[2J|
    printf(CLEAR);
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            printf(" %c ", get_cell_status(grid, x, y));
        }
        printf("\n");
    }
}

void init_grid(char *grid) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
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
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
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
    set_cell(old_grid, 4, 4, ALIVE);
    set_cell(old_grid, 6, 4, ALIVE);
    set_cell(old_grid, 4, 5, ALIVE);
    set_cell(old_grid, 5, 5, ALIVE);
    set_cell(old_grid, 5, 6, ALIVE);
    while (1) {
        compute_next_state(old_grid, new_grid);
        print_grid(new_grid);
        usleep(100000);
        compute_next_state(new_grid, old_grid);
        print_grid(old_grid);
        usleep(100000);
    }
    printf("Number of living neighbors: %d\n", count_living_neighbors(new_grid, 2, 2));
    return 0;
}
