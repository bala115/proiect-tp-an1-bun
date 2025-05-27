#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define GRID_SIZE 10
#define CELL_SIZE 30
#define LEFT_GRID_X 50
#define RIGHT_GRID_X 550
#define GRID_Y 100

typedef enum { EMPTY, SHIP, HIT, MISS } CellState;

typedef struct {
    CellState cells[GRID_SIZE][GRID_SIZE];
    int ships_to_place; // câte nave trebuie plasate
} Board;

typedef struct {
    int x, y; // coordonate grilă (0..9)
} GridPos;

// Liste fixe de nave (lungimi)
const int ship_sizes[] = {3, 2, 1};
const int ships_count = sizeof(ship_sizes) / sizeof(ship_sizes[0]);

// Prototipuri funcții
void draw_grid(SDL_Renderer* renderer, int x_offset, int y_offset, Board* board, bool hide_ships);
bool inside_grid(float mx, float my, int x_offset, int y_offset, GridPos* pos_out);
void draw_cell(SDL_Renderer* renderer, int x, int y, CellState state, int x_offset, int y_offset);

int main(int argc, char **argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("Battleships 2 Players", 1200, 800, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    Board boards[2] = {0};
    // Inițial toate celulele goale
    for (int p = 0; p < 2; p++) {
        for (int i = 0; i < GRID_SIZE; i++)
            for (int j = 0; j < GRID_SIZE; j++)
                boards[p].cells[i][j] = EMPTY;
        boards[p].ships_to_place = ships_count;
    }

    int current_player = 0;
    bool placing_phase = true;
    int current_ship_idx = 0; // care navă plasăm acum

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = event.button.x;
                float my = event.button.y;

                if (placing_phase) {
                    GridPos pos;
                    // Plasare doar pe grila jucătorului curent (stânga pentru juc 0, dreapta pentru juc 1)
                    int x_offset = (current_player == 0) ? LEFT_GRID_X : RIGHT_GRID_X;
                    int y_offset = GRID_Y;

                    if (inside_grid(mx, my, x_offset, y_offset, &pos)) {
                        // Verifică dacă spațiul este liber și dacă putem plasa navă orizontală (spre dreapta)
                        int ship_len = ship_sizes[current_ship_idx];
                        bool can_place = true;
                        if (pos.x + ship_len > GRID_SIZE) can_place = false;

                        for (int i = 0; i < ship_len && can_place; i++) {
                            if (boards[current_player].cells[pos.y][pos.x + i] != EMPTY)
                                can_place = false;
                        }

                        if (can_place) {
                            // Plasează nava
                            for (int i = 0; i < ship_len; i++) {
                                boards[current_player].cells[pos.y][pos.x + i] = SHIP;
                            }
                            current_ship_idx++;
                            if (current_ship_idx >= ships_count) {
                                // S-a terminat plasarea pentru acest jucător
                                current_player = 1 - current_player;
                                current_ship_idx = 0;
                                boards[current_player].ships_to_place = ships_count;
                                // Dacă jucătorul 1 a terminat, închidem faza de plasare
                                if (current_player == 0) {
                                    placing_phase = false;
                                    current_player = 0; // începe jucătorul 0 să tragă
                                }
                            }
                        }
                    }
                }
                else {
                    // Faza de joc (tragere)
                    GridPos pos;
                    int opponent = 1 - current_player;
                    int x_offset = (opponent == 0) ? LEFT_GRID_X : RIGHT_GRID_X;
                    int y_offset = GRID_Y;

                    if (inside_grid(mx, my, x_offset, y_offset, &pos)) {
                        CellState cell = boards[opponent].cells[pos.y][pos.x];
                        if (cell == EMPTY) {
                            boards[opponent].cells[pos.y][pos.x] = MISS;
                            current_player = opponent; // schimbă turnul
                        } else if (cell == SHIP) {
                            boards[opponent].cells[pos.y][pos.x] = HIT;
                            // jucătorul curent continuă la lovitură
                        }
                        // Dacă HIT sau MISS deja, click nu face nimic
                    }
                }
            }
        }

        // Desenare fundal
        SDL_SetRenderDrawColor(renderer, 28, 28, 209, 255);
        SDL_RenderClear(renderer);

        // Desenare grile și starea lor
        draw_grid(renderer, LEFT_GRID_X, GRID_Y, &boards[0], !placing_phase);
        draw_grid(renderer, RIGHT_GRID_X, GRID_Y, &boards[1], !placing_phase);

        // Desenare indicator turn curent
        SDL_Rect turn_rect = {20, 20, 200, 40};
        if (current_player == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        }
        SDL_RenderFillRect(renderer, &turn_rect);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void draw_grid(SDL_Renderer* renderer, int x_offset, int y_offset, Board* board, bool hide_ships) {
    // Desenează grila și starea celulelor
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            CellState state = board->cells[i][j];
            if (hide_ships && state == SHIP) {
                state = EMPTY; // ascunde navele adversarului
            }
            draw_cell(renderer, j, i, state, x_offset, y_offset);
        }
    }
}

void draw_cell(SDL_Renderer* renderer, int x, int y, CellState state, int x_offset, int y_offset) {
    SDL_FRect cell = { x_offset + x * CELL_SIZE, y_offset + y * CELL_SIZE, CELL_SIZE-2, CELL_SIZE-2 };
    switch (state) {
        case EMPTY:
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            break;
        case SHIP:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            break;
        case HIT:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            break;
        case MISS:
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            break;
    }
    SDL_RenderFillRect(renderer, &cell);
}

bool inside_grid(float mx, float my, int x_offset, int y_offset, GridPos* pos_out) {
    if (mx < x_offset || mx >= x_offset + CELL_SIZE * GRID_SIZE) return false;
    if (my < y_offset || my >= y_offset + CELL_SIZE * GRID_SIZE) return false;
    pos_out->x = (int)((mx - x_offset) / CELL_SIZE);
    pos_out->y = (int)((my - y_offset) / CELL_SIZE);
    return true;
}