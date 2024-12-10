#include "Jogo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdbool.h>
#include <stdio.h>

#define CENARIO_LARGURA 12  // Número de colunas do cenário
#define CENARIO_ALTURA 8    // Número de linhas do cenário
#define LARGURA_IMG 70      // Tamanho da célula do cenário (em pixels)
#define ALTURA_IMG 70

struct Jogo {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* fila_eventos;
    ALLEGRO_TIMER* timer;
    ALLEGRO_BITMAP* sprite;
    bool rodando;
    float x, y;            
    float velocidade;
    int direcao;           
    int si;                
    float tempo_quadro;    
    bool teclas[4];        

    
    int cenario[CENARIO_ALTURA][CENARIO_LARGURA];
    ALLEGRO_BITMAP* chao;
    ALLEGRO_BITMAP* parede;
    ALLEGRO_BITMAP* agua;
    ALLEGRO_BITMAP* caveira;
    ALLEGRO_BITMAP* cacto;
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
    
    for (int i = 0; i < CENARIO_ALTURA; i++) {
        for (int j = 0; j < CENARIO_LARGURA; j++) {
            float x = j * LARGURA_IMG;
            float y = i * ALTURA_IMG;

            
            al_draw_bitmap(J->chao, x, y, 0);
        }
    }

    
    for (int i = 0; i < CENARIO_ALTURA; i++) {
        for (int j = 0; j < CENARIO_LARGURA; j++) {
            float x = j * LARGURA_IMG;
            float y = i * ALTURA_IMG;

            switch (J->cenario[i][j]) {
                case 1: 
                    al_draw_bitmap(J->parede, x, y, 0); 
                    break;
                case 2: 
                    al_draw_bitmap(J->agua, x, y, 0); 
                    break;
                case 3:
                    al_draw_bitmap(J->cacto, x, y, 0); 
                    break;
                case 4:
                    al_draw_bitmap(J->caveira, x, y, 0); 
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
    jogo->x = 10;
    jogo->y = 0;
    jogo->velocidade = 2.7;
    jogo->direcao = 0;
    jogo->si = 0;
    jogo->tempo_quadro = 0;



    for (int i = 0; i < 4; i++) {
        jogo->teclas[i] = false;
    }

    
    if (!carregar_cenario(jogo, "cenario2.txt")) {
        al_destroy_bitmap(sprite);
        finalizar_jogo(jogo);
        return NULL;
    }

    
    jogo->chao = al_load_bitmap("areia.png");
    jogo->parede = al_load_bitmap("pedra.png");
    jogo->agua = al_load_bitmap("agua.png");
    jogo->cacto = al_load_bitmap("cacto.png");
    jogo->caveira = al_load_bitmap("caveira.png");
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

bool verifica_colisao(Jogo* J, float nova_x, float nova_y) {

   
    int col_left = nova_x / (LARGURA_IMG-4);
    int col_right = (nova_x + LARGURA_IMG - 1) / LARGURA_IMG;
    int row_top = nova_y / 2;
    int row_bottom = (nova_y + ALTURA_IMG - 2) / ALTURA_IMG;

   
    if (col_left >= 0 && col_left < CENARIO_LARGURA && row_top >= 0 && row_top < CENARIO_ALTURA) {
        if (J->cenario[row_top][col_left] == 1 || J->cenario[row_top][col_left] == 2 || J->cenario[row_top][col_left]== 3) return true; 
    }

    if (col_right >= 0 && col_right < CENARIO_LARGURA && row_top >= 0 && row_top < CENARIO_ALTURA) {
        if (J->cenario[row_top][col_right] == 1 || J->cenario[row_top][col_right] == 2 || J->cenario[row_top][col_right] == 3) return true; 
    }

    if (col_left >= 0 && col_left < CENARIO_LARGURA && row_bottom >= 0 && row_bottom < CENARIO_ALTURA) {
        if (J->cenario[row_bottom][col_left] == 1 || J->cenario[row_bottom][col_left] == 2 || J->cenario[row_bottom][col_left] == 3) return true; 
    }

    if (col_right >= 0 && col_right < CENARIO_LARGURA && row_bottom >= 0 && row_bottom < CENARIO_ALTURA) {
        if (J->cenario[row_bottom][col_right] == 1 || J->cenario[row_bottom][col_right] == 2 || J->cenario[row_bottom][col_right] == 3) return true; 
    }

    return false;
}


 bool verifica_colisao_agua(Jogo* J, float nova_x, float nova_y) {
    
    int col_left = nova_x / LARGURA_IMG;
    int col_right = (nova_x + LARGURA_IMG - 1) / LARGURA_IMG;
    int row_top = nova_y / ALTURA_IMG;
    int row_bottom = (nova_y + ALTURA_IMG - 1) / ALTURA_IMG;

   
    if (col_left >= 0 && col_left < CENARIO_LARGURA && row_top >= 0 && row_top < CENARIO_ALTURA) {
        if (J->cenario[row_top][col_left] == 2) {
            return true; 
        }
    }

    if (col_right >= 0 && col_right < CENARIO_LARGURA && row_top >= 0 && row_top < CENARIO_ALTURA) {
        if (J->cenario[row_top][col_right] == 2) {
            return true; 
        }
    }

    if (col_left >= 0 && col_left < CENARIO_LARGURA && row_bottom >= 0 && row_bottom < CENARIO_ALTURA) {
        if (J->cenario[row_bottom][col_left] == 2) {
            return true; 
        }
    }

    if (col_right >= 0 && col_right < CENARIO_LARGURA && row_bottom >= 0 && row_bottom < CENARIO_ALTURA) {
        if (J->cenario[row_bottom][col_right] == 2) {
            return true; 
        }
    }

    return false; 
}

void atualizar_jogo(Jogo* J) {
    ALLEGRO_EVENT evento;

   
    float limite_esquerdo = 0;
    float limite_direito = (CENARIO_LARGURA * LARGURA_IMG) - LARGURA_IMG; 
    float limite_superior = 0;
    float limite_inferior = (CENARIO_ALTURA * ALTURA_IMG) - ALTURA_IMG;

    while (al_get_next_event(J->fila_eventos, &evento)) {
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            J->rodando = false;
        } else if (evento.type == ALLEGRO_EVENT_TIMER) {
            J->tempo_quadro += 1.0 / 60.0;
            if (J->tempo_quadro >= 0.1) {
                J->si = (J->si + 1) % 5;  
                J->tempo_quadro = 0;
            }

            
            float nova_x = J->x;
            float nova_y = J->y;

            if (J->teclas[0]) { 
                nova_y -= J->velocidade;
                J->direcao = 2; 
            }
            if (J->teclas[1]) { 
                nova_y += J->velocidade;
                J->direcao = 0; 
            }
            if (J->teclas[2]) { 
                nova_x -= J->velocidade;
                J->direcao = 1; 
            }
            if (J->teclas[3]) { 
                nova_x += J->velocidade;
                J->direcao = 3; 
            }

            
            if (!verifica_colisao(J, nova_x, nova_y)) {
                
                if (nova_x < limite_esquerdo) nova_x = limite_esquerdo;
                if (nova_x > limite_direito) nova_x = limite_direito;
                if (nova_y < limite_superior) nova_y = limite_superior;
                if (nova_y > limite_inferior) nova_y = limite_inferior;

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

    
    if (!J->teclas[0] && !J->teclas[1] && !J->teclas[2] && !J->teclas[3]) {
        J->si = 0;
    }


    desenhar_cenario(J); 
    al_draw_bitmap_region(J->sprite, 94 * J->si, 99 * J->direcao, 94, 99, J->x, J->y, 0);
    al_flip_display(); 
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
