#include "raylib.h"
#include "Mapa.h"
#include "Personagem.h"
#include <stdio.h>

// Definições visuais
#define TILE_SIZE 40
#define SCREEN_WIDTH (MAP_W * TILE_SIZE)
#define SCREEN_HEIGHT (MAP_H * TILE_SIZE)

// 1. Definição de Estados do Jogo
typedef enum { 
    ESTADO_EXPLORANDO, 
    ESTADO_BATALHA, 
    ESTADO_FIM_JOGO 
} GameState;

void DrawGame(const Map *mapa, const Player *jogador, GameState estado) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // --- Desenhar Mapa ---
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            int posX = x * TILE_SIZE;
            int posY = y * TILE_SIZE;

            switch (mapa->grid[y][x]) {
                case TILE_WALL:   DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, DARKGRAY); break;
                case TILE_ZOMBIE: DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, RED); break;
                case TILE_ITEM:   DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, GOLD); break;
                case TILE_EMPTY:  DrawRectangleLines(posX, posY, TILE_SIZE, TILE_SIZE, LIGHTGRAY); break;
            }
        }
    }

    // --- Desenhar Jogador ---
    DrawRectangle(jogador->pos_x * TILE_SIZE, jogador->pos_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);

    // --- Interface Gráfica (UI) Baseada no Estado ---
    if (estado == ESTADO_BATALHA) {
        // Desenha uma caixa semi-transparente preta
        DrawRectangle(50, 100, 300, 200, Fade(BLACK, 0.8f));
        DrawRectangleLines(50, 100, 300, 200, WHITE);
        
        // Texto da batalha
        DrawText("ENCONTROU UM ZUMBI!", 70, 120, 20, RED);
        DrawText("Pressione SPACE para atacar", 70, 160, 10, WHITE);
        
        // Simulação de stats (apenas visual por enquanto)
        DrawText(TextFormat("HP Jogador: %d", jogador->hp), 70, 200, 20, GREEN);
    }

    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zombie Rampage - GUI");
    SetTargetFPS(60);

    Map mapa;
    Player jogador;
    // Estado inicial do jogo
    GameState estadoAtual = ESTADO_EXPLORANDO;
    
    // Configuração inicial (Manual para teste visual)
    jogador = player_creat("Heroi", SOLDADO);
    jogador.pos_x = 1; 
    jogador.pos_y = 1;
    map_init(&mapa);
    
    // Garante que o jogador não nasce em cima de parede ou zumbi
    map_place_player(&mapa, &jogador); 

    while (!WindowShouldClose()) {
        
        // --- LÓGICA ---
        
        if (estadoAtual == ESTADO_EXPLORANDO) {
            // Só permite mover se estiver a explorar
            int dx = 0, dy = 0;
            if (IsKeyPressed(KEY_W)) dy = -1;
            if (IsKeyPressed(KEY_S)) dy = 1;
            if (IsKeyPressed(KEY_A)) dx = -1;
            if (IsKeyPressed(KEY_D)) dx = 1;

            if (dx != 0 || dy != 0) {
                // Tenta mover
                if (map_move_player(&mapa, &jogador, dx, dy)) {
                    // Se moveu, verifica encontro
                    if (map_check_encounter(&mapa, &jogador)) {
                        estadoAtual = ESTADO_BATALHA; // MUDA O ESTADO!
                    }
                }
            }
        }
        else if (estadoAtual == ESTADO_BATALHA) {
            // Se estiver em batalha, espera o Espaço para "vencer"
            if (IsKeyPressed(KEY_SPACE)) {
                // Remove o zumbi da posição do jogador (Vitória simples para teste)
                mapa.grid[jogador.pos_y][jogador.pos_x] = TILE_EMPTY;
                estadoAtual = ESTADO_EXPLORANDO; // Volta a explorar
            }
        }

        // --- DESENHO ---
        DrawGame(&mapa, &jogador, estadoAtual);
    }

    CloseWindow();
    return 0;
}