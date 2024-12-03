#include "Jogo.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <stdlib.h>
#include <stdio.h>

#define LARGURA_MAPA 20
#define ALTURA_MAPA 10

int mapa[LARGURA_MAPA][ALTURA_MAPA];

Jogo* inicializar_jogo() {
    Jogo* jogo = malloc(sizeof(Jogo));

    jogo->x = 0;
    jogo->y = 0;
    jogo->si = 0;
    jogo->direcao = 0;

    jogo->sprite = al_load_bitmap("sprites2.png");
    jogo->grama = al_load_bitmap("grama.png");
    jogo->parede = al_load_bitmap("parede.png");

    if (!jogo->sprite || !jogo->grama || !jogo->parede) {
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

    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
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

    if (jogo->keys[ALLEGRO_KEY_RIGHT] || jogo->keys[ALLEGRO_KEY_LEFT] ||
        jogo->keys[ALLEGRO_KEY_UP] || jogo->keys[ALLEGRO_KEY_DOWN]) {
        anim_counter++;
        if (anim_counter % 2 == 0) {
            jogo->si = (jogo->si + 1) % 3;
        }
    } else {
        jogo->si = 1;
    }

    if (jogo->keys[ALLEGRO_KEY_RIGHT]) {
        jogo->x += 12;
        jogo->direcao = 3;
    } else if (jogo->keys[ALLEGRO_KEY_LEFT]) {
        jogo->x -= 12;
        jogo->direcao = 1;
    } else if (jogo->keys[ALLEGRO_KEY_DOWN]) {
        jogo->y += 12;
        jogo->direcao = 0;
    } else if (jogo->keys[ALLEGRO_KEY_UP]) {
        jogo->y -= 12;
        jogo->direcao = 2;
    }
}

void renderizar_jogo(Jogo* jogo) {
    // desenha o cenario
    for (int i = 0; i < ALTURA_MAPA; i++) {
        for (int j = 0; j < LARGURA_MAPA; j++) {
            al_draw_bitmap(jogo->grama, j * 90, i * 90, 0);

            if (mapa[i][j] == 1) {
                al_draw_bitmap(jogo->parede, j * 90, i * 90, 0);
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
    al_destroy_bitmap(jogo->grama);
    al_destroy_bitmap(jogo->parede);
    free(jogo);
}
