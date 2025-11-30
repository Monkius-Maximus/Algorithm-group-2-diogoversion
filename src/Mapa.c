#include "Mapa.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void criar_inimigo(Inimigo *inimigo, const char* nome, int hp, int atk, int def, int xp);

void map_init(Map *mapa) {
    // Limpa o mapa e gera paredes aleatórias
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            int r = rand() % 100;
            if (r < 15) { // 15% de chance de ser uma parede
                mapa->grid[y][x] = TILE_WALL;
            } else {
                mapa->grid[y][x] = TILE_EMPTY;
            }
        }
    }

    //  Inicializa e cria os inimigos com atributos variados
    mapa->num_inimigos = 0;
    int inimigos_a_criar = 7; 

    for (int i = 0; i < inimigos_a_criar; i++) {
        // Para se não houver mais espaço no  array de inimigos
        if (mapa->num_inimigos >= MAX_INIMIGOS) {
            break;
        }

        // Escolhe uma posição aleatória que esteja vazia (TILE_EMPTY)
        int x, y;
        do {
            x = rand() % MAP_W;
            y = rand() % MAP_H;
        } while (mapa->grid[y][x] != TILE_EMPTY);

        // Pega um espaço na lista de inimigos
        Inimigo *novo_inimigo = &mapa->inimigos[mapa->num_inimigos];

        // Decide aleatoriamente qual tipo de inimigo criar
        int tipo_inimigo = rand() % 3;
        if (tipo_inimigo == 0) {
            criar_inimigo(novo_inimigo, "Zumbi Lento", 40, 8, 2, 10);
        } else if (tipo_inimigo == 1) {
            criar_inimigo(novo_inimigo, "Corredor Agil", 25, 12, 0, 15);
        } else {
            criar_inimigo(novo_inimigo, "Zumbi Robusto", 60, 6, 5, 20);
        }
        
        // Guarda a posição do inimigo na sua própria struct
        novo_inimigo->pos_x = x;
        novo_inimigo->pos_y = y;
        
        // Coloca um 'Z' no mapa para representar o inimigo visualmente
        mapa->grid[y][x] = TILE_ZOMBIE; 
        mapa->num_inimigos++; 
    }

 int itens_a_criar = 5;
    for (int i = 0; i < itens_a_criar; i++) {
        int x, y;
        do {
            x = rand() % MAP_W;
            y = rand() % MAP_H;
        } while (mapa->grid[y][x] != TILE_EMPTY);

        mapa->grid[y][x] = TILE_ITEM;
    }
}

void map_print(const Map *m, const Player *p){
    printf("\nMapa (P = jogador, Z = zumbi, # = obstaculo, * = item)\n");
    for(int y = 0; y < MAP_H; y++){
        for(int x = 0; x < MAP_W; x++){
            if(p->pos_x == x && p->pos_y == y){
                putchar('P');
                continue;
            }
            switch (m->grid[y][x])
            {
            case TILE_EMPTY:
                putchar('.');
                break;
            case TILE_WALL:
                putchar('#');
                break;
            case TILE_ZOMBIE:
                putchar('Z');
                break;
            case TILE_ITEM:
                putchar('*');
                break;
            default:
                putchar('?');
            }
        }
        putchar('\n');
    }
};

// Função auxiliar para criar um tipo de inimigo 
void criar_inimigo(Inimigo *inimigo, const char* nome, int hp, int atk, int def, int xp) {
    strncpy(inimigo->nome, nome, 49);
    inimigo->nome[49] = '\0';
    inimigo->hp = hp;
    inimigo->ataque = atk;
    inimigo->defesa = def;
    inimigo->xp = xp;
    inimigo->ativo = 1; // Nasce vivo
}

int map_move_player(Map *m, Player *p, int dx, int dy){
    int nx = p->pos_x + dx;
    int ny = p->pos_y + dy;
    if(nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H) return 0;
    if(m->grid[ny][nx] == TILE_WALL) return 0;

    // Movimentos:
    p->pos_x = nx;
    p->pos_y = ny;
    return 1;
};

int map_check_encounter(Map *m, const Player *p){
    if(m->grid[p->pos_y][p->pos_x] == TILE_ZOMBIE) return 1;
    return 0;
};

void map_place_player(Map *m, Player *p){
    // Encontrar uma posição vazia:
    for(int y = 0; y < MAP_H; y++){
        for(int x = 0; x < MAP_W; x++){
            if(m->grid[y][x] == TILE_EMPTY){
                p->pos_x = x;
                p->pos_y = y;
                return;
            }
        }
    }
    // Voltar a trás (fallback):
    p->pos_x = 0; p->pos_y = 0;
}

// Estrutura para a fila do BFS
typedef struct {
    int x, y;
} Pos;

// Função BFS para encontrar o próximo passo em direção ao jogador
// Retorna 1 se encontrou caminho, 0 se não
static int bfs_find_next_step(const Map *m, int start_x, int start_y, int goal_x, int goal_y, int *next_x, int *next_y) {
    // Se já está adjacente ao jogador, não move
    if (abs(start_x - goal_x) + abs(start_y - goal_y) <= 1) {
        *next_x = start_x;
        *next_y = start_y;
        return 0;
    }
    
    // Matriz de visitados e predecessores
    int visited[MAP_H][MAP_W] = {0};
    int pred_x[MAP_H][MAP_W];
    int pred_y[MAP_H][MAP_W];
    
    // Inicializa predecessores
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            pred_x[y][x] = -1;
            pred_y[y][x] = -1;
        }
    }
    
    // Fila para BFS
    Pos queue[MAP_W * MAP_H];
    int front = 0, back = 0;
    
    // Adiciona posição inicial
    queue[back].x = start_x;
    queue[back].y = start_y;
    back++;
    visited[start_y][start_x] = 1;
    
    // Direções: cima, baixo, esquerda, direita
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    
    while (front < back) {
        Pos current = queue[front++];
        
        // Chegou ao objetivo
        if (current.x == goal_x && current.y == goal_y) {
            // Reconstrói o caminho para encontrar o primeiro passo
            int cx = goal_x, cy = goal_y;
            while (pred_x[cy][cx] != start_x || pred_y[cy][cx] != start_y) {
                int px = pred_x[cy][cx];
                int py = pred_y[cy][cx];
                cx = px;
                cy = py;
            }
            *next_x = cx;
            *next_y = cy;
            return 1;
        }
        
        // Explora vizinhos
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            
            // Verifica limites
            if (nx < 0 || nx >= MAP_W || ny < 0 || ny >= MAP_H) continue;
            
            // Verifica se já foi visitado
            if (visited[ny][nx]) continue;
            
            // Verifica se é passável (vazio, item ou posição do jogador)
            if (m->grid[ny][nx] == TILE_WALL || m->grid[ny][nx] == TILE_ZOMBIE) continue;
            
            visited[ny][nx] = 1;
            pred_x[ny][nx] = current.x;
            pred_y[ny][nx] = current.y;
            queue[back].x = nx;
            queue[back].y = ny;
            back++;
        }
    }
    
    // Não encontrou caminho
    *next_x = start_x;
    *next_y = start_y;
    return 0;
}

// Move todos os inimigos ativos em direção ao jogador usando BFS
void map_move_enemies(Map *m, const Player *p) {
    for (int i = 0; i < m->num_inimigos; i++) {
        Inimigo *inimigo = &m->inimigos[i];
        
        // Ignora inimigos inativos (derrotados)
        if (!inimigo->ativo) continue;
        
        int next_x, next_y;
        
        // Usa BFS para encontrar o próximo passo
        if (bfs_find_next_step(m, inimigo->pos_x, inimigo->pos_y, p->pos_x, p->pos_y, &next_x, &next_y)) {
            // Verifica se a posição destino ainda está disponível (evita colisão entre zumbis)
            if (m->grid[next_y][next_x] == TILE_EMPTY || m->grid[next_y][next_x] == TILE_ITEM) {
                // Atualiza o mapa: remove o Z da posição antiga
                m->grid[inimigo->pos_y][inimigo->pos_x] = TILE_EMPTY;
                
                // Move o inimigo
                inimigo->pos_x = next_x;
                inimigo->pos_y = next_y;
                
                // Coloca o Z na nova posição
                m->grid[next_y][next_x] = TILE_ZOMBIE;
            }
        }
    }
}