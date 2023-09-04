#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "include/raylib.h"

#define NUM_OF_ROWS 3
#define DEFAULT_TILE_COLOR SKYBLUE
#define BACKGROUND_COLOR BLACK
#define SCREEN_WIDTH 770
#define SCREEN_HEIGHT 770
#define TILE_MARGIN_X 20
#define TILE_MARGIN_Y 20
#define TILE_WIDTH 250
#define TILE_HEIGHT 250

typedef enum {
    EMPTY,
    USER,
    AI
} tile_state;

typedef enum {
    WINNER_USER,
    WINNER_AI,
    TIE,
    CONTINUE
} match_state;

void draw_tiles(int tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position);
Vector2 get_tile_from_pos(Vector2 *mouse_position);
void update_tiles_on_mouse_position(int tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position, int *current_player);
match_state check_win_condition(int tiles[NUM_OF_ROWS][NUM_OF_ROWS]);
match_state check_tie_condition(int tiles[NUM_OF_ROWS][NUM_OF_ROWS]);

int main () {
    int tiles[NUM_OF_ROWS][NUM_OF_ROWS] = {EMPTY};
    int current_player = 0;
    bool exit = false;
    Vector2 hovered_tile_position = {-1, -1};

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Tic Tac Toe Game");

    while (!exit) {
        if (IsKeyPressed(KEY_Y)) {
            exit = true;
        }

        match_state current_match_state = check_win_condition(tiles);
        switch (current_match_state) {
            case WINNER_USER:
                printf("User wins!\n");
                break;
            case WINNER_AI:
                printf("AI wins!\n");
                break;
            case TIE:
                printf("No winner, try it again!\n");
                break;
            case CONTINUE:
                break;
        }

        update_tiles_on_mouse_position(tiles, &hovered_tile_position, &current_player);
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        draw_tiles(tiles, &hovered_tile_position);
        EndDrawing();
    }

    return 0;
}

match_state check_diagonal_line(int tiles[NUM_OF_ROWS][NUM_OF_ROWS], char* str) {
    unsigned int count_tile_user = 0;
    unsigned int count_tile_ai = 0;

    for (int i = 0; i < NUM_OF_ROWS; i++) {

        tile_state tile_status = strcmp(str, "LEFT") == 0 ? tiles[i][i] : tiles[i][NUM_OF_ROWS - 1 - i];
            
        if (tile_status == USER) {
            count_tile_user++;
        } else if (tile_status == AI) {
            count_tile_ai++;
        }
    }

    if (count_tile_user == NUM_OF_ROWS) {
        return WINNER_USER;
    }

    if (count_tile_ai == NUM_OF_ROWS) {
        return WINNER_AI;
    }

    return CONTINUE;
}

match_state check_straight_lines(int tiles[NUM_OF_ROWS][NUM_OF_ROWS], char* str) { 
    unsigned int count_tile_user;
    unsigned int count_tile_ai;

    for (int i = 0; i < NUM_OF_ROWS; i++) {
        count_tile_user = 0;
        count_tile_ai = 0;

        for (int j = 0; j < NUM_OF_ROWS; j++) {
            tile_state tile_status = strcmp(str, "ROW") == 0 ? tiles[i][j] : tiles[j][i];

            if (tile_status == USER) {
                count_tile_user++;
            } else if (tile_status == AI) {
                count_tile_ai++;
            }
        }

        if (count_tile_user == NUM_OF_ROWS) {
            return WINNER_USER;
        }

        if (count_tile_ai == NUM_OF_ROWS) {
            return WINNER_AI;
        }
    }

    return CONTINUE;
}

match_state check_win_condition(int tiles[NUM_OF_ROWS][NUM_OF_ROWS]) {
    match_state current_state = check_diagonal_line(tiles, "LEFT");
    
    if (current_state != CONTINUE) {
        return current_state;
    }

    current_state = check_diagonal_line(tiles, "RIGHT");

    if (current_state != CONTINUE) {
        return current_state;
    }

    current_state = check_straight_lines(tiles, "ROW");

    if (current_state != CONTINUE) {
        return current_state;
    }

    current_state = check_straight_lines(tiles, "COLUMN");

    if (current_state != CONTINUE) {
        return current_state;
    }

    return check_tie_condition(tiles);
}

match_state check_tie_condition(int tiles[NUM_OF_ROWS][NUM_OF_ROWS]) {
    unsigned int count_pressed_tiles = 0;

    for (int i = 0; i < NUM_OF_ROWS; i++) {
        for (int j = 0; j < NUM_OF_ROWS; j++) {
            tile_state tile_status = tiles[i][j];

            if (tile_status == WINNER_USER || tile_status == WINNER_AI) {
                count_pressed_tiles++;
            }
        }
    }

    return count_pressed_tiles == NUM_OF_ROWS * NUM_OF_ROWS ? TIE : CONTINUE;
}

void update_tiles_on_mouse_position(int tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position, int *current_player) {
    Vector2 mouse_position = GetMousePosition();
    Vector2 tile_position = get_tile_from_pos(&mouse_position);

    if (tile_position.x > -1 && tile_position.y > -1) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            tile_state pressed_tile_state = tiles[(int)tile_position.x][(int)tile_position.y];
            
            if (pressed_tile_state == EMPTY) {
                tiles[(int)tile_position.x][(int)tile_position.y] = *current_player == 0 ? USER : AI;
                *current_player = *current_player ? 0 : 1;
            }
        } else {
            hovered_tile_position->x = tile_position.x;
            hovered_tile_position->y = tile_position.y;
        }
    }

}

void draw_tiles(int tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position) {
    for (size_t i = 0; i < NUM_OF_ROWS; i++) {
        for (size_t j = 0; j < NUM_OF_ROWS; j++) {
            switch (tiles[i][j]) {
                case EMPTY:
                    if (hovered_tile_position->x == i && hovered_tile_position->y == j) {
                        DrawRectangle(i * (TILE_MARGIN_X + TILE_WIDTH), j * (TILE_MARGIN_Y + TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT, GREEN);
                    } else {
                        DrawRectangle(i * (TILE_MARGIN_X + TILE_WIDTH), j * (TILE_MARGIN_Y + TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT, DEFAULT_TILE_COLOR);
                    }
                    break;
                case USER:
                        DrawRectangle(i * (TILE_MARGIN_X + TILE_WIDTH), j * (TILE_MARGIN_Y + TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT, RED);
                        break;
                case AI:
                        DrawRectangle(i * (TILE_MARGIN_X + TILE_WIDTH), j * (TILE_MARGIN_Y + TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT, YELLOW);
                        break;
            }
        }
    }
}

Vector2 get_tile_from_pos(Vector2 *mouse_position) {
    Vector2 tile_pos = {-1, -1};
    int boundary_margin_x = (int)mouse_position->x % (TILE_WIDTH + TILE_MARGIN_X);
    int boundary_margin_y = (int)mouse_position->y % (TILE_HEIGHT + TILE_MARGIN_Y);

    if (boundary_margin_x < TILE_WIDTH && boundary_margin_y < TILE_HEIGHT) {
        int tile_pos_x = mouse_position->x / TILE_WIDTH;
        int tile_pos_y = mouse_position->y / TILE_HEIGHT;

        if (tile_pos_x < NUM_OF_ROWS && tile_pos_y < NUM_OF_ROWS) {
            tile_pos.x = tile_pos_x;
            tile_pos.y = tile_pos_y;
        }
    }

    return tile_pos;
}

