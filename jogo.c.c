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
        jogo->x += 14;
        jogo->direcao = 3;
    } else if (jogo->keys[ALLEGRO_KEY_LEFT]) {
        jogo->x -= 14;
        jogo->direcao = 1;
    } else if (jogo->keys[ALLEGRO_KEY_DOWN]) {
        jogo->y += 14;
        jogo->direcao = 0;
    } else if (jogo->keys[ALLEGRO_KEY_UP]) {
        jogo->y -= 14;
        jogo->direcao = 2;
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
