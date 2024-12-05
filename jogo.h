#ifndef JOGO_H
#define JOGO_H

#include <allegro5/allegro.h>

// Estrutura que representa o estado do jogo
typedef struct {
    int x, y;                // Coordenadas do personagem
    int si;                  // Índice do frame do sprite
    int direcao;             // Direção do personagem (0 = baixo, 1 = lado, 2 = cima, 3 = direita)
    bool keys[ALLEGRO_KEY_MAX];  // Estado das teclas
    ALLEGRO_BITMAP* sprite;  // Sprite do personagem
    ALLEGRO_BITMAP* chao;   // Imagem da grama
    ALLEGRO_BITMAP* parede;  // Imagem da parede
    ALLEGRO_BITMAP* agua; //imagem da agua
    ALLEGRO_BITMAP* cenario_1;
    ALLEGRO_BITMAP* cenario_2;
} Jogo;

// Funções do jogo
Jogo* inicializar_jogo();            // Inicializa o jogo
void processar_entrada(Jogo* jogo, ALLEGRO_EVENT event);  // Processa as entradas do teclado
void atualizar_jogo(Jogo* jogo);     // Atualiza o estado do jogo
void renderizar_jogo(Jogo* jogo);    // Renderiza o jogo na tela
void finalizar_jogo(Jogo* jogo);     // Finaliza o jogo e libera recursos
bool verificar_posicao_valida(int x, int y);

#endif // JOGO_H
