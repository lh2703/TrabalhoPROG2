#include "Jogo.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <stdlib.h>
#include <stdio.h>

#define LARGURA_MAPA 12
#define ALTURA_MAPA 8


int mapa[ALTURA_MAPA][LARGURA_MAPA];

Jogo* inicializar_jogo() {
    Jogo* jogo = malloc(sizeof(Jogo));

    jogo->x = 0;
    jogo->y = 0;
    jogo->si = 0;
    jogo->direcao = 0;

    jogo->sprite = al_load_bitmap("sprites.png");
    jogo->chao = al_load_bitmap("areia.png");
    jogo->parede = al_load_bitmap("pedra.png");
    jogo->agua = al_load_bitmap ("agua2.png");
    jogo->cenario_1 = al_load_bitmap ("caveira.png");
    jogo->cenario_2 = al_load_bitmap ("cacto.png");


    if (!jogo->sprite || !jogo->chao || !jogo->parede || !jogo->agua ) {
        printf("Erro ao carregar uma das imagens\n");
        exit(1);
    }

    for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
        jogo->keys[i] = false;
    }

    FILE* file = fopen("cenario.txt", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo de cenário\n");
        exit(1);
    }

    for (int i = 0; i < ALTURA_MAPA; i++) {
        for (int j = 0; j < LARGURA_MAPA; j++) {
            fscanf(file, "%d", &mapa[i][j]);
        }
    }
    fclose(file);

    ALLEGRO_DISPLAY* display = al_create_display(840, 560);
    if (!display) {
        printf("Erro ao criar o display\n");
        exit(1);
    }

    return jogo;
}

void processar_entrada(Jogo* jogo, ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        jogo->keys[event.keyboard.keycode] = true;
    } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        jogo->keys[event.keyboard.keycode] = false;
    }
}

void atualizar_jogo(Jogo* jogo) {
    static int anim_counter = 0;

    int novo_x = jogo->x;
    int novo_y = jogo->y;

    // Tamanho do mapa e das células
    const int TAMANHO_CELULA = 70;

    // Dimensões e margens internas do personagem
    const int largura_personagem = 94;
    const int altura_personagem = 99;
    const int margem_esquerda = 30;
    const int margem_direita = 30;
    const int margem_superior = 20;
    const int margem_inferior = 20;

    // Movimenta o personagem com base nas teclas pressionadas
    if (jogo->keys[ALLEGRO_KEY_RIGHT]) {
        novo_x += 14;
        jogo->direcao = 3;
    } else if (jogo->keys[ALLEGRO_KEY_LEFT]) {
        novo_x -= 14;
        jogo->direcao = 1;
    } else if (jogo->keys[ALLEGRO_KEY_DOWN]) {
        novo_y += 14;
        jogo->direcao = 0;
    } else if (jogo->keys[ALLEGRO_KEY_UP]) {
        novo_y -= 14;
        jogo->direcao = 2;
    }

    // Verificação de colisão
    bool colisao = false;

    // Calcula as posições dos cantos do personagem
    int canto_superior_esquerdo_x = novo_x + margem_esquerda;
    int canto_superior_esquerdo_y = novo_y + margem_superior;

    int canto_inferior_direito_x = novo_x + largura_personagem - margem_direita;
    int canto_inferior_direito_y = novo_y + altura_personagem - margem_inferior;

    // Converte coordenadas para índices no mapa
    int coluna_inicial = canto_superior_esquerdo_x / TAMANHO_CELULA;
    int linha_inicial = canto_superior_esquerdo_y / TAMANHO_CELULA;

    int coluna_final = canto_inferior_direito_x / TAMANHO_CELULA;
    int linha_final = canto_inferior_direito_y / TAMANHO_CELULA;

    // Verifica cada célula ocupada pelo personagem
    for (int linha = linha_inicial; linha <= linha_final; linha++) {
        for (int coluna = coluna_inicial; coluna <= coluna_final; coluna++) {
            // Checa os limites do mapa
            if (linha < 0 || linha >= ALTURA_MAPA || coluna < 0 || coluna >= LARGURA_MAPA) {
                colisao = true;
            } else {
                // Verifica tipos de células que impedem o movimento
                int tipo_celula = mapa[linha][coluna];
                if (tipo_celula == 1 || tipo_celula == 2 || tipo_celula == 4) {
                    colisao = true;  // Obstáculos: parede, água e cacto
                }
            }
        }
    }

    // Atualiza posição apenas se não houve colisão
    if (!colisao) {
        jogo->x = novo_x;
        jogo->y = novo_y;
    }

    // Atualiza animação do sprite
    if (jogo->keys[ALLEGRO_KEY_RIGHT] || jogo->keys[ALLEGRO_KEY_LEFT] ||
        jogo->keys[ALLEGRO_KEY_UP] || jogo->keys[ALLEGRO_KEY_DOWN]) {
        anim_counter++;
        if (anim_counter % 2 == 0) {
            jogo->si = (jogo->si + 1) % 3;
        }
    } else {
        jogo->si = 1;
    }
}


void processar_entrada(Jogo* jogo, ALLEGRO_EVENT event) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        jogo->keys[event.keyboard.keycode] = true;
    } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
        jogo->keys[event.keyboard.keycode] = false;
    }
}

void atualizar_jogo(Jogo* jogo) {
    static int anim_counter = 0;

    int novo_x = jogo->x;
    int novo_y = jogo->y;

    // Tentativa de mover o personagem
    if (jogo->keys[ALLEGRO_KEY_RIGHT]) {
        novo_x += 14;
        jogo->direcao = 3;
    } else if (jogo->keys[ALLEGRO_KEY_LEFT]) {
        novo_x -= 14;
        jogo->direcao = 1;
    } else if (jogo->keys[ALLEGRO_KEY_DOWN]) {
        novo_y += 14;
        jogo->direcao = 0;
    } else if (jogo->keys[ALLEGRO_KEY_UP]) {
        novo_y -= 14;
        jogo->direcao = 2;
    }

    // Margens da área de colisão (ajuste os valores para afinar o resultado)
    const int margem_esquerda = 40;
    const int margem_direita = 40;
    const int margem_superior = 30;
    const int margem_inferior = 30;

    // Dimensões do personagem
    const int largura_personagem = 94;  // Largura total do sprite
    const int altura_personagem = 99;  // Altura total do sprite

    // Verifica se a nova posição está dentro de uma área válida
    if (
        verificar_posicao_valida(novo_x + margem_esquerda, novo_y + margem_superior) &&
        verificar_posicao_valida(novo_x + largura_personagem - margem_direita, novo_y + margem_superior) &&
        verificar_posicao_valida(novo_x + margem_esquerda, novo_y + altura_personagem - margem_inferior) &&
        verificar_posicao_valida(novo_x + largura_personagem - margem_direita, novo_y + altura_personagem - margem_inferior)
    ) {
        jogo->x = novo_x;
        jogo->y = novo_y;
    }

    // Atualiza a animação do sprite
    if (jogo->keys[ALLEGRO_KEY_RIGHT] || jogo->keys[ALLEGRO_KEY_LEFT] ||
        jogo->keys[ALLEGRO_KEY_UP] || jogo->keys[ALLEGRO_KEY_DOWN]) {
        anim_counter++;
        if (anim_counter % 2 == 0) {
            jogo->si = (jogo->si + 1) % 3;
        }
    } else {
        jogo->si = 1;
    }
}


void renderizar_jogo(Jogo* jogo) {

    for (int i = 0; i < ALTURA_MAPA; i++) {
    for (int j = 0; j < LARGURA_MAPA; j++) {
        printf("%d ", mapa[i][j]);
    }
    printf("\n");
}
    // desenha o cenario
for (int i = 0; i < ALTURA_MAPA; i++) {
    for (int j = 0; j < LARGURA_MAPA; j++) {
        al_draw_bitmap(jogo->chao , j * 70, i * 70, 0);

        if (mapa[i][j] == 1) {
            al_draw_bitmap(jogo->parede, j * 70, i * 70, 0);
        }
        if (mapa[i][j] == 2) {
            al_draw_bitmap(jogo->agua, j * 70, i * 70, 0);
        }
        if (mapa[i][j] == 3) {
            al_draw_bitmap(jogo->cenario_1, j * 70, i * 70, 0);
        }
        if (mapa[i][j] == 4) {
            al_draw_bitmap(jogo->cenario_2, j * 70, i * 70, 0);
        }


    }
}


    // desenha o personagem
    int flip = 0;
    if (jogo->direcao == 0) {
        flip = ALLEGRO_FLIP_HORIZONTAL;
    }

    al_draw_bitmap_region(
        jogo->sprite,
        94 * jogo->si,       // X da região do sprite
        99 * jogo->direcao,  // Y da região do sprite
        94,                  // Largura do sprite
        99,                  // Altura do sprite
        jogo->x,             // Posição X do personagem
        jogo->y,             // Posição Y do personagem
        flip                 // Flip horizontal se necessário
    );

    al_flip_display();
}

void finalizar_jogo(Jogo* jogo) {
    al_destroy_bitmap(jogo->sprite);
    al_destroy_bitmap(jogo->chao);
    al_destroy_bitmap(jogo->parede);
    al_destroy_bitmap(jogo->chao);
    al_destroy_bitmap(jogo->agua);
    al_destroy_bitmap(jogo->cenario_1);
    al_destroy_bitmap(jogo->cenario_2);
    free(jogo);
}
