#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include "Jogo.h"

int main() {
    al_init();
    al_init_image_addon();
    al_install_keyboard();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(400, 300);

    //inicia o jogo
    Jogo* jogo = inicializar_jogo();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;
    al_start_timer(timer);

    while (1) {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
            processar_entrada(jogo, event);  //  entrada do teclado
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            atualizar_jogo(jogo);  // atualiza a animacao do naruto
            renderizar_jogo(jogo);  // desenha naruto
            redraw = false;
        }
    }

    finalizar_jogo(jogo);  // Libera recursos do jogo
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
