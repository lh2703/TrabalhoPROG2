#include "Jogo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include <stdio.h>

#define CENARIO_LARGURA 12  // Número de colunas do cenário
#define CENARIO_ALTURA 8    // Número de linhas do cenário
#define LARGURA_IMG 70   // Tamanho da célula do cenário (em pixels)
#define ALTURA_IMG 70

struct Jogo {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* fila_eventos;
    ALLEGRO_TIMER* timer;
    ALLEGRO_BITMAP* sprite;
    bool rodando;
    float x, y;            // Posição do personagem
    float velocidade;
    int direcao;           // 0 = baixo, 1 = esquerda, 2 = cima, 3 = direita
    int si;                // Quadro atual do sprite
    float tempo_quadro;    // Controla a troca de quadros
    bool teclas[4];        // Estado das teclas (cima, baixo, esquerda, direita)

    // Cenário
    int cenario[CENARIO_ALTURA][CENARIO_LARGURA];
    ALLEGRO_BITMAP* chao;
    ALLEGRO_BITMAP* parede;
    ALLEGRO_BITMAP* agua;
};

bool carregar_cenario(Jogo* J, const char* arquivo) {
    FILE* file = fopen(arquivo, "r");
    if (!file) {
        fprintf(stderr, "Erro ao abrir o arquivo do cenário: %s\n", arquivo);
        return false;
    }

    for (int i = 0; i < CENARIO_ALTURA; i++) {
        for (int j = 0; j < CENARIO_LARGURA; j++) {
            if (fscanf(file, "%d", &J->cenario[i][j]) != 1) {
                fprintf(stderr, "Erro ao ler o cenário no arquivo %s\n", arquivo);
                fclose(file);
                return false;
            }
        }
    }

    fclose(file);
    return true;
}

void desenhar_cenario(Jogo* J) {
    // Desenha o chão (areia) para todo o cenário
    for (int i = 0; i < CENARIO_ALTURA; i++) {
        for (int j = 0; j < CENARIO_LARGURA; j++) {
            float x = j * LARGURA_IMG;
            float y = i * ALTURA_IMG;

            // Desenha o chão (areia.png) por toda a área do mapa
            al_draw_bitmap(J->chao, x, y, 0);
        }
    }

    // Agora desenha as outras imagens (paredes, água, etc.) sobre o chão
    for (int i = 0; i < CENARIO_ALTURA; i++) {
        for (int j = 0; j < CENARIO_LARGURA; j++) {
            float x = j * LARGURA_IMG;
            float y = i * ALTURA_IMG;

            switch (J->cenario[i][j]) {
                case 1: // Parede
                    al_draw_bitmap(J->parede, x, y, 0); // Desenha a parede sobre o chão
                    break;
                case 2: // Água
                    al_draw_bitmap(J->agua, x, y, 0); // Desenha a água sobre o chão
                    break;
            }
        }
    }
}

Jogo* novo_jogo() {
    if (!al_init()) return NULL;
    if (!al_install_keyboard()) return NULL;

    ALLEGRO_DISPLAY* display = al_create_display(840, 560);
    if (!display) return NULL;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    if (!timer) {
        al_destroy_display(display);
        return NULL;
    }

    ALLEGRO_EVENT_QUEUE* fila_eventos = al_create_event_queue();
    if (!fila_eventos) {
        al_destroy_timer(timer);
        al_destroy_display(display);
        return NULL;
    }

    al_init_primitives_addon();
    al_init_image_addon();

    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    ALLEGRO_BITMAP* sprite = al_load_bitmap("sprites.png");
    if (!sprite) {
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return NULL;
    }

    Jogo* jogo = malloc(sizeof(Jogo));
    if (!jogo) {
        al_destroy_bitmap(sprite);
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return NULL;
    }

    jogo->display = display;
    jogo->fila_eventos = fila_eventos;
    jogo->timer = timer;
    jogo->sprite = sprite;
    jogo->rodando = true;
    jogo->x = 400;
    jogo->y = 300;
    jogo->velocidade = 2.7;
    jogo->direcao = 0;
    jogo->si = 0;
    jogo->tempo_quadro = 0;

    for (int i = 0; i < 4; i++) {
        jogo->teclas[i] = false;
    }

    // Carregar cenário
    if (!carregar_cenario(jogo, "cenario.txt")) {
        al_destroy_bitmap(sprite);
        finalizar_jogo(jogo);
        return NULL;
    }

    // Carregar texturas
    jogo->chao = al_load_bitmap("areia.png");
    jogo->parede = al_load_bitmap("pedra.png");
    jogo->agua = al_load_bitmap("cacto.png");
    if (!jogo->chao || !jogo->parede || !jogo->agua) {
        fprintf(stderr, "Erro ao carregar texturas do cenário.\n");
        finalizar_jogo(jogo);
        return NULL;
    }

    al_start_timer(timer);

    return jogo;
}

bool jogo_rodando(Jogo* J) {
    return J->rodando;
}

// Verifica se a posição do personagem colide com algo no cenário
bool verifica_colisao(Jogo* J, float nova_x, float nova_y) {
    // Verificar as 4 direções do personagem

    // Lados da célula onde o personagem estará
    int col_left = nova_x / LARGURA_IMG;
    int col_right = (nova_x + LARGURA_IMG - 1) / LARGURA_IMG;
    int row_top = nova_y / ALTURA_IMG;
    int row_bottom = (nova_y + ALTURA_IMG - 1) / ALTURA_IMG;

    // Verifica se a posição está dentro dos limites do cenário e se não colide com parede (1) ou água (2)
    if (col_left >= 0 && col_left < CENARIO_LARGURA && row_top >= 0 && row_top < CENARIO_ALTURA) {
        if (J->cenario[row_top][col_left] == 1 || J->cenario[row_top][col_left] == 2) return true; // Colisão na parte superior esquerda
    }

    if (col_right >= 0 && col_right < CENARIO_LARGURA && row_top >= 0 && row_top < CENARIO_ALTURA) {
        if (J->cenario[row_top][col_right] == 1 || J->cenario[row_top][col_right] == 2) return true; // Colisão na parte superior direita
    }

    if (col_left >= 0 && col_left < CENARIO_LARGURA && row_bottom >= 0 && row_bottom < CENARIO_ALTURA) {
        if (J->cenario[row_bottom][col_left] == 1 || J->cenario[row_bottom][col_left] == 2) return true; // Colisão na parte inferior esquerda
    }

    if (col_right >= 0 && col_right < CENARIO_LARGURA && row_bottom >= 0 && row_bottom < CENARIO_ALTURA) {
        if (J->cenario[row_bottom][col_right] == 1 || J->cenario[row_bottom][col_right] == 2) return true; // Colisão na parte inferior direita
    }

    return false; // Não houve colisão
}

void atualizar_jogo(Jogo* J) {
    ALLEGRO_EVENT evento;

    while (al_get_next_event(J->fila_eventos, &evento)) {
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            J->rodando = false;
        } else if (evento.type == ALLEGRO_EVENT_TIMER) {
            J->tempo_quadro += 1.0 / 60.0;
            if (J->tempo_quadro >= 0.1) {
                J->si = (J->si + 1) % 5;  // Ciclo de animação
                J->tempo_quadro = 0;
            }

            // Movimentos do personagem e direção
            float nova_x = J->x;
            float nova_y = J->y;

            if (J->teclas[0]) { // Cima
                nova_y -= J->velocidade;
                J->direcao = 2; // Atualiza direção para cima
            }
            if (J->teclas[1]) { // Baixo
                nova_y += J->velocidade;
                J->direcao = 0; // Atualiza direção para baixo
            }
            if (J->teclas[2]) { // Esquerda
                nova_x -= J->velocidade;
                J->direcao = 1; // Atualiza direção para esquerda
            }
            if (J->teclas[3]) { // Direita
                nova_x += J->velocidade;
                J->direcao = 3; // Atualiza direção para direita
            }

            // Verificar se há colisão antes de mover
            if (!verifica_colisao(J, nova_x, nova_y)) {
                J->x = nova_x;
                J->y = nova_y;
            }
        } else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP: J->teclas[0] = true; break;
                case ALLEGRO_KEY_DOWN: J->teclas[1] = true; break;
                case ALLEGRO_KEY_LEFT: J->teclas[2] = true; break;
                case ALLEGRO_KEY_RIGHT: J->teclas[3] = true; break;
            }
        } else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP: J->teclas[0] = false; break;
                case ALLEGRO_KEY_DOWN: J->teclas[1] = false; break;
                case ALLEGRO_KEY_LEFT: J->teclas[2] = false; break;
                case ALLEGRO_KEY_RIGHT: J->teclas[3] = false; break;
            }
        }
    }

    // Se nenhuma tecla de direção está pressionada, mantemos o personagem na posição inicial (sem movimento)
    if (!J->teclas[0] && !J->teclas[1] && !J->teclas[2] && !J->teclas[3]) {
        J->si = 0;
    }

    al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa a tela
    desenhar_cenario(J); // Desenha o cenário
    al_draw_bitmap_region(J->sprite, 94 * J->si, 99 * J->direcao, 94, 99, J->x, J->y, 0); // Desenha o personagem com a direção correta
    al_flip_display(); // Atualiza a tela
}

void finalizar_jogo(Jogo* J) {
    al_destroy_bitmap(J->sprite);
    al_destroy_bitmap(J->chao);
    al_destroy_bitmap(J->parede);
    al_destroy_bitmap(J->agua);
    al_destroy_event_queue(J->fila_eventos);
    al_destroy_timer(J->timer);
    al_destroy_display(J->display);
    free(J);
}
