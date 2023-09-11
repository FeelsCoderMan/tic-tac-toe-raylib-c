#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "include/raylib.h"

#define NUM_OF_ROWS 3
#define WINNER_SCORE 10
#define TIE_SCORE 0
#define LOSER_SCORE -10
#define STARTING_PLAYER PLAYER_USER

#define DEFAULT_TILE_COLOR CLITERAL(Color){0, 102, 102, 255}
#define BACKGROUND_COLOR BLACK
#define HOVER_COLOR CLITERAL(Color){25, 51, 0, 255}
#define CIRCLE_COLOR RED
#define CROSS_COLOR BLUE
#define OUTCOME_COLOR WHITE

#define SCREEN_WIDTH 770
#define SCREEN_HEIGHT 770
#define TILE_MARGIN_X 20
#define TILE_MARGIN_Y 20
#define TILE_WIDTH 250
#define TILE_HEIGHT 250
#define RESTART_BUTTON_START_X 240
#define RESTART_BUTTON_START_Y 360
#define RESTART_BUTTON_WIDTH 300
#define RESTART_BUTTON_HEIGHT 100

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

typedef enum {
    PLAYER_USER,
    PLAYER_AI
} player_state;

typedef struct {
    int score;
    Vector2 tile_index;
} Movement;

void draw_tiles(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position);
Vector2 get_tile_from_pos(Vector2 *mouse_position);
void update_tiles_on_mouse_position(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position, player_state *current_player);
match_state check_win_condition(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS]);
match_state check_tie_condition(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS]);
match_state check_diagonal_line(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], char *direction);
match_state check_straight_lines(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], char *direction);
void reset_game(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], match_state *current_match_state, player_state *current_player);
void render_outcome_ui(match_state *current_match_state);
Movement find_best_move_ai(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS]);
int minimax(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], int depth, bool isMax);

int main (void) {
    tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS] = {EMPTY};
    player_state current_player = STARTING_PLAYER;
    Vector2 hovered_tile_position = {-1, -1};
    match_state current_match_state;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Tic Tac Toe Game");
    SetTargetFPS(60);
    // TODO: Update makefile with pkg config of raylib, remove lib static file
    // TODO: (Optional) Trigger a sound when user makes a move
    while (!WindowShouldClose()) {

        BeginDrawing();

        current_match_state = check_win_condition(tiles);

        if (current_match_state != CONTINUE) {
            render_outcome_ui(&current_match_state);
            reset_game(tiles, &current_match_state, &current_player);
        } else {
            update_tiles_on_mouse_position(tiles, &hovered_tile_position, &current_player);
            ClearBackground(BACKGROUND_COLOR);
            draw_tiles(tiles, &hovered_tile_position);
        }
        
        EndDrawing();

    }

    CloseWindow();

    return 0;
}

void render_outcome_ui(match_state *current_match_state) {
    ClearBackground(BACKGROUND_COLOR);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_COLOR);

    if (*current_match_state == WINNER_USER) {
        DrawText("User wins", SCREEN_WIDTH * 2 / 7, SCREEN_HEIGHT / 3, 75, OUTCOME_COLOR);
    } else if (*current_match_state == WINNER_AI) {
        DrawText("AI wins", SCREEN_WIDTH * 2 / 7, SCREEN_HEIGHT / 3, 75, OUTCOME_COLOR);
    } else if (*current_match_state == TIE) {
        DrawText("No winner", SCREEN_WIDTH * 2 / 7, SCREEN_HEIGHT / 3, 75, OUTCOME_COLOR);
    }

    Vector2 mouse_position = GetMousePosition();

    if (mouse_position.x > RESTART_BUTTON_START_X && mouse_position.x < RESTART_BUTTON_START_X + RESTART_BUTTON_WIDTH &&
            mouse_position.y > RESTART_BUTTON_START_Y && mouse_position.y < RESTART_BUTTON_START_Y + RESTART_BUTTON_HEIGHT) {
        DrawRectangle(RESTART_BUTTON_START_X, RESTART_BUTTON_START_Y, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, WHITE);
        DrawText("Restart", SCREEN_WIDTH * 3 / 8, SCREEN_HEIGHT / 2, 50, BLACK);
    } else {
        DrawRectangleLines(RESTART_BUTTON_START_X, RESTART_BUTTON_START_Y, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, WHITE);
        DrawText("Restart", SCREEN_WIDTH * 3 / 8, SCREEN_HEIGHT / 2, 50, WHITE);
    }

}

void reset_game(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], match_state *current_match_state, player_state *current_player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_position = GetMousePosition();

        if (mouse_position.x > RESTART_BUTTON_START_X && mouse_position.x < RESTART_BUTTON_START_X + RESTART_BUTTON_WIDTH &&
                mouse_position.y > RESTART_BUTTON_START_Y && mouse_position.y < RESTART_BUTTON_START_Y + RESTART_BUTTON_HEIGHT) {
            memset(tiles, EMPTY, sizeof(tiles[0][0]) * NUM_OF_ROWS * NUM_OF_ROWS);
            *current_match_state = CONTINUE;
            *current_player = STARTING_PLAYER;
        }
    }
}

match_state check_diagonal_line(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], char *direction) {
    unsigned int count_tile_user = 0;
    unsigned int count_tile_ai = 0;

    for (size_t i = 0; i < NUM_OF_ROWS; i++) {
        tile_state tile_status; 

        if (strcmp(direction, "LEFT") ==  0) {
            tile_status = tiles[i][i];
        } else if (strcmp(direction, "RIGHT") == 0) {
            tile_status = tiles[i][NUM_OF_ROWS - 1 - i];
        }
            
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

match_state check_straight_lines(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], char *direction) { 
    unsigned int count_tile_user;
    unsigned int count_tile_ai;

    for (size_t i = 0; i < NUM_OF_ROWS; i++) {
        count_tile_user = 0;
        count_tile_ai = 0;

        for (size_t j = 0; j < NUM_OF_ROWS; j++) {
            tile_state tile_status;

            if (strcmp(direction, "ROW") ==  0) {
                tile_status = tiles[i][j];
            } else if (strcmp(direction, "COLUMN") == 0) {
                tile_status = tiles[j][i];
            }

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

match_state check_win_condition(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS]) {
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

match_state check_tie_condition(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS]) {
    unsigned int count_pressed_tiles = 0;

    for (size_t i = 0; i < NUM_OF_ROWS; i++) {
        for (size_t j = 0; j < NUM_OF_ROWS; j++) {
            tile_state tile_status = tiles[i][j];

            if (tile_status == USER || tile_status == AI) {
                count_pressed_tiles++;
            }
        }
    }

    return count_pressed_tiles == NUM_OF_ROWS * NUM_OF_ROWS ? TIE : CONTINUE;
}

int minimax(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], int depth, bool isMax) {
    match_state current_match_state = check_win_condition(tiles);

    switch (current_match_state) {
            case WINNER_USER:
                return LOSER_SCORE + depth;
            case WINNER_AI:
                return WINNER_SCORE - depth;
            case TIE:
                return TIE_SCORE;
            default:
                break;
    }

    if (isMax) {
        int best_score = INT_MIN;

        for (size_t i = 0; i < NUM_OF_ROWS; i++) {
            for (size_t j = 0; j < NUM_OF_ROWS; j++) {
                if (tiles[i][j] == EMPTY) {
                    tiles[i][j] = AI;
                    int score = minimax(tiles, depth + 1, false);
                    tiles[i][j] = EMPTY;

                    if (score > best_score) {
                        best_score = score;
                    }
                }
            }
        }
        
        return best_score;
    } else {
        int best_score = INT_MAX;

        for (size_t i = 0; i < NUM_OF_ROWS; i++) {
            for (size_t j = 0; j < NUM_OF_ROWS; j++) {
                if (tiles[i][j] == EMPTY) {
                    tiles[i][j] = USER;
                    int score = minimax(tiles, depth + 1, true);
                    tiles[i][j] = EMPTY;

                    if (score < best_score) {
                        best_score = score;
                    }
                }
            }
        }

        return best_score;
    }
}


Movement find_best_move_ai(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS]) {
    Movement best_move;
    best_move.score = INT_MIN;
    best_move.tile_index.x = -1;
    best_move.tile_index.y = -1;

    for (size_t i = 0; i < NUM_OF_ROWS; i++) {
        for (size_t j = 0; j < NUM_OF_ROWS; j++) {
            if (tiles[i][j] == EMPTY) {
                tiles[i][j] = AI;
                int score = minimax(tiles, 0, false);
                tiles[i][j] = EMPTY;

                if (score > best_move.score) {
                    best_move.score = score;
                    best_move.tile_index.x = i;
                    best_move.tile_index.y = j;
                }
            }
        }
    }

    return best_move;
}


void update_tiles_on_mouse_position(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position, player_state *current_player) {
    if (*current_player == PLAYER_AI) {
        Movement best_move_ai = find_best_move_ai(tiles);
        tiles[(int)best_move_ai.tile_index.x][(int)best_move_ai.tile_index.y] = AI;
        *current_player = PLAYER_USER;
    } else {
        Vector2 mouse_position = GetMousePosition();
        Vector2 tile_position = get_tile_from_pos(&mouse_position);

        if (tile_position.x > -1 && tile_position.y > -1) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                tile_state pressed_tile_state = tiles[(int)tile_position.x][(int)tile_position.y];

                if (pressed_tile_state == EMPTY) {
                    tiles[(int)tile_position.x][(int)tile_position.y] = USER;
                    *current_player = PLAYER_AI;
                }
            } else {
                hovered_tile_position->x = tile_position.x;
                hovered_tile_position->y = tile_position.y;
            }
        }
    }
}

void draw_tiles(tile_state tiles[NUM_OF_ROWS][NUM_OF_ROWS], Vector2 *hovered_tile_position) {
    for (size_t i = 0; i < NUM_OF_ROWS; i++) {
        for (size_t j = 0; j < NUM_OF_ROWS; j++) {
            int starting_point_x = i * (TILE_MARGIN_X + TILE_WIDTH);
            int starting_point_y = j * (TILE_MARGIN_Y + TILE_HEIGHT);

            switch (tiles[i][j]) {
                case EMPTY:
                    if (hovered_tile_position->x == i && hovered_tile_position->y == j) {
                        DrawRectangle(starting_point_x, starting_point_y, TILE_WIDTH, TILE_HEIGHT, HOVER_COLOR);
                    } else {
                        DrawRectangle(starting_point_x, starting_point_y, TILE_WIDTH, TILE_HEIGHT, DEFAULT_TILE_COLOR);
                    }
                    break;
                case USER:
                        DrawRectangle(starting_point_x, starting_point_y, TILE_WIDTH, TILE_HEIGHT, DEFAULT_TILE_COLOR);
                        DrawCircle(starting_point_x + TILE_WIDTH / 2, starting_point_y + TILE_HEIGHT / 2, 80, CIRCLE_COLOR);
                        DrawCircle(starting_point_x + TILE_WIDTH / 2, starting_point_y + TILE_HEIGHT / 2, 40, DEFAULT_TILE_COLOR);
                        break;
                case AI:
                        Rectangle left_rect = {starting_point_x + TILE_WIDTH / 3, starting_point_y + TILE_HEIGHT / 8, TILE_WIDTH * 3 / 4, TILE_HEIGHT / 4};
                        Rectangle right_rect = {starting_point_x + TILE_WIDTH * 2 / 3, starting_point_y + TILE_HEIGHT / 8, TILE_WIDTH / 4, TILE_HEIGHT * 3 / 4};
                        Vector2 origin = {0, 0};
                        DrawRectangle(starting_point_x, starting_point_y, TILE_WIDTH, TILE_HEIGHT, DEFAULT_TILE_COLOR);
                        DrawRectanglePro(left_rect, origin, 45.f, CROSS_COLOR);
                        DrawRectanglePro(right_rect, origin, 45.f, CROSS_COLOR);
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

