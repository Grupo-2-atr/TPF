/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: pipi
 *
 * Created on August 16, 2020, 3:01 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h> // NO OLVIDAR AGREGAR EN EL LINKER DEL PROYECTO
#include <allegro5/allegro_primitives.h> // NO OLVIDAR AGREGAR EN EL LINKER DEL PROYECTO
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>// NO OLVIDAR AGREGAR EN EL LINKER DEL PROYECTO
#include <allegro5/allegro_image.h> //NO OLVIDAR INCLUIR ALLEGRO_IMAGE EN LINKER
#include <allegro5/allegro_color.h> // NO OLVIDAR AGREGAR EN EL LINKER DEL PROYECTO
#include <allegro5/allegro_native_dialog.h>//PARA USAR EL SPRITE (IMAGEN SIN FONDO)
#include <stdbool.h>



#define FPS             60.0
#define SCREEN_W        800
#define SCREEN_H        530
#define NAVE_SIZE_X     40
#define NAVE_SIZE_Y     40
#define ALIEN_SIZE_X    32
#define ALIEN_SIZE_Y    32
#define BALA_SIZE_X     5
#define BALA_SIZE_Y     14
#define COLUMNAS        3   //COLUMNAS Y FILAS MÁXIMAS
#define FILAS           3
#define COLUMNAS_MIN    1
#define FILAS_MIN       1
#define MOVE_RATE       5.0 //Para el movimiento
#define RANGO_X         30
#define RANGO_Y         60

#define true    1
#define false   0

//DEFINO COORDENADAS DE LOS BOTONES
#define MOUSE_IN_PLAY ((ev.mouse.x>244)&(ev.mouse.x<539)&(ev.mouse.y>245)&(ev.mouse.y<286))
#define MOUSE_IN_EXIT ((ev.mouse.x>335)&(ev.mouse.x<462)&(ev.mouse.y>308)&(ev.mouse.y<345))
#define MOUSE_IN_HIGH_SCORE ((ev.mouse.x>307)&(ev.mouse.x<465)&(ev.mouse.y>390)&(ev.mouse.y<412))

 enum MYKEYS {
 KEY_LEFT, KEY_RIGHT, KEY_SPACE //arrow keys
};

  typedef struct {
        float x;          //COORDENADA DE LA NAVE EN X.
        float y;          //COORDENADA DE LA NAVE EN Y.
        int shot;    //PARA SABER SI HAY UN DISPARO O NO.    
    }nave_t;
    
    
    typedef struct{
        float x;        //POSICIÓN DE LA BALA EN X.
        float y;        //POSICIÓN DE LA BALA EN Y.
        float dy;       //INCREMENTO DE LA BALA (MOVIMIENTO).
    }bullet_t;
    
    
    typedef struct{
        float x;
        float y;
        int alive;
    }alien_t; //PARA MUCHOS PUEDE SER UN ARREGLO.
int imprimo_alien(int bullet_x, int bullet_y, int nave_shot, alien_t arr_alien[][COLUMNAS], ALLEGRO_BITMAP *alien_tipo_1, ALLEGRO_BITMAP *alien_tipo_2, ALLEGRO_BITMAP *alien_fin);
int play_space_invaders(ALLEGRO_EVENT_QUEUE *event_queue,ALLEGRO_TIMER *timer );
void ordeno(alien_t arr[][COLUMNAS], int filas, int columnas);


/*
 * 
 */
int main(int argc, char** argv) {
    


    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE * event_queue = NULL;
    ALLEGRO_BITMAP *buffer = NULL;
    ALLEGRO_BITMAP *fondo_menu = NULL;
    ALLEGRO_BITMAP *fondo_play = NULL;
    ALLEGRO_BITMAP *fondo_exit = NULL;
    ALLEGRO_BITMAP *fondo_high_score = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_SAMPLE *sample_high_score = NULL;
    ALLEGRO_SAMPLE *sample_play = NULL;
    ALLEGRO_SAMPLE *sample_menu = NULL;
    ALLEGRO_SAMPLE *sample_exit = NULL;
    ALLEGRO_BITMAP *fondo_juego = NULL;
    
    int redraw = false;
    int do_exit = false;
    
    int control_play=0;
    int control_exit=0;
    int control_high_score=0;
    int play=1;
    

//****CARGO ALLEGRO Y LO QUE VOY A USAR (MOUSE, IMAGENES, SONIDO...).****
    //----> INSTALO ALLEGRO 
    if (!al_init()) {
        fprintf(stderr, "failed to initialize allegro!\n");
        return -1;
    }
    //----> CARGO LO NECESARIO PARA CONTROLAR EL MOUSE.
    if (!al_install_mouse()) {
        fprintf(stderr, "failed to initialize the mouse!\n");
        return -1;
    }
    //----> CARGO LO RELACIONADO CON AUDIO Y AÑADIR AUDIOS.
    
     if (!al_install_audio()) {
        fprintf(stderr, "failed to initialize audio!\n");
        return -1;
    }
    if (!al_init_acodec_addon()) {
        fprintf(stderr, "failed to initialize audio codecs!\n");
        return -1;
    }
    //ESTE NÚMERO INDICA LA CANTIDAD DE AUDIOS QUE SE PUEDEN REPRODUCIR AL MISMO TIEMPO.
    //PUSE 3 PARA QUE SE SUPERPONGAN EL SONIDO DEL MENU Y EL DE DOS SONIDOS (SINO CUANDO MOVES RÁPIDO EL MOUSE NO SUENA EL BOTÓN).
      if (!al_reserve_samples(3)) {
        fprintf(stderr, "failed to reserve samples!\n");
        return -1;
    }
    
    
    //----> CARGO LO NECESARIO PARA USAR EL TIMER.
    timer = al_create_timer(1.0 / FPS);
    if (!timer) {
        fprintf(stderr, "failed to create timer!\n");
        return -1;
    }
    
    
    //----> CARGO LO NECESARIO PARA AÑADIR IMÁGENES.
      if (!al_init_image_addon()) { // ADDON necesario para manejo(no olvidar el freno de mano) de imagenes 
        fprintf(stderr, "failed to initialize image addon !\n");
        return -1;
    }

    
    // CREO EL DISPLAY Y VERIFICO QUE FUNCIONE.
    display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        fprintf(stderr, "failed to create display!\n");
        al_destroy_timer(timer);
        return -1;
    }

    
    // CREO LA COLA DE EVENTOS Y VERIFICO QUE FUNCIONE.
    event_queue = al_create_event_queue();
    if (!event_queue) {
        fprintf(stderr, "failed to create event_queue!\n");
        al_destroy_bitmap(fondo_menu);
        al_destroy_bitmap(fondo_play);
        al_destroy_bitmap(fondo_exit);
        al_destroy_bitmap(fondo_high_score);
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    
    
    //CARGO LAS IMÁGENES Y VERIFICO QUE SE HAYAN CARGADO CORRECTAMENTE.
    fondo_menu = al_load_bitmap("fondo_menu.bmp");
    if (!fondo_menu) {
        fprintf(stderr, "failed to load fondo_menu !\n");
        return 0;
    }
        fondo_play = al_load_bitmap("fondo_play.bmp");
    if (!fondo_play) {
        fprintf(stderr, "failed to load fondo_play !\n");
        return 0;
    }
         fondo_exit = al_load_bitmap("fondo_exit.bmp");
    if (!fondo_exit) {
        fprintf(stderr, "failed to load fondo_exit !\n");
        return 0;
    }
         fondo_high_score = al_load_bitmap("fondo_high_score.bmp");
    if (!fondo_high_score) {
        fprintf(stderr, "failed to load fondo_high_score !\n");
        return 0;
    }
             fondo_juego = al_load_bitmap("fondo_juego.bmp");
    if (!fondo_juego) {
        fprintf(stderr, "failed to create fondo_juego bitmap!\n");
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }
    
         
        al_convert_mask_to_alpha(fondo_menu,al_map_rgb(0, 0, 0));
        al_convert_mask_to_alpha(fondo_play,al_map_rgb(0, 0, 0));
        al_convert_mask_to_alpha(fondo_exit,al_map_rgb(0, 0, 0));
        al_convert_mask_to_alpha(fondo_high_score,al_map_rgb(0, 0, 0));
         
    // CARGO LOS AUDIOS Y VERIFICO QUE SE HAYAN CARGADO CORRECTAMENTE.     
       sample_high_score = al_load_sample("high_score.wav");
    if (!sample_high_score) {
        printf("Audio clip sample_high_score not loaded!\n");
        return -1;
    }
        sample_play = al_load_sample("play.wav");
    if (!sample_play) {
        printf("Audio clip sample_play not loaded!\n");
        return -1;
    }
        sample_menu = al_load_sample("menu.wav");
    if (!sample_menu) {
        printf("Audio clip sample_menu not loaded!\n");
        return -1;
    }
        sample_exit = al_load_sample("exit.wav");
    if (!sample_exit) {
        printf("Audio clip sample_exit not loaded!\n");
        return -1;
    }
         
    //AÑADO EL TIMER, DISPLAY Y MOUSE A LA COLA DE EVENTOS.     
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    
    // INICIO EL TIMER Y CARGO LA IMÁGEN PRINCIPAL.
    al_draw_bitmap(fondo_juego, 0, 0, 0);
    al_draw_bitmap(fondo_menu, 0, 0, 0);
    al_flip_display();
    al_start_timer(timer);

    
    /* PERMANEZCO DENTRO DE ESTE CICLO HASTA QUE LA VARIABLE do_exit INDIQUE QUE SALGA. 
     DENTRO DEL CICLO SE REVISA LA COLA DE EVENTOS Y DE ACUERDO A ESTO SE REALZAN CAMBIOS EN EL DISPLAY
     Y EN LAS VARIABLES do_exit Y redraw.
     */
    //ACÁ VA LA MÚSICA QUE QUIERO QUE SUENE TODO EL TIEMPO 
    //al_play_sample(la música que quiero, volumen, No sé, valocidad, ALLEGRO_PLAYMODE_LOOP(cuantas veces), NULL);
     al_play_sample(sample_menu, 1.5, 0.0, 2.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    
    //ESTA SUENA UNA SOLA VEZ.
    // al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    while (!do_exit) 
    {
        ALLEGRO_EVENT ev;
        if (al_get_next_event(event_queue, &ev)) //Toma un evento de la cola, VER RETURN EN DOCUMENT.
        {
            if (ev.type == ALLEGRO_EVENT_TIMER) // por que tenemos un timer para redibujar?
                redraw = true;

            else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)//Si se cierra el display cerrar
                do_exit = true;

            else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY)
            {
                if(MOUSE_IN_PLAY)
                {
                    al_draw_bitmap(fondo_juego, 0, 0, 0);
                    al_draw_bitmap(fondo_play, 0, 0, 0);
                    al_flip_display();
                    
                    //ESTO ES PARA QUE NO SE REPITA LA MÚSICA CADA VEZ QUE MUEVO EL MOUSE.
                    if(control_play==0)
                    {
                        al_play_sample(sample_play, 8.0, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
                        control_play=1;
                        control_exit=0;
                        control_high_score=0;
                    }
                }
                else if(MOUSE_IN_EXIT)
                {
                     al_draw_bitmap(fondo_juego, 0, 0, 0);
                    al_draw_bitmap(fondo_exit, 0, 0, 0);
                    al_flip_display();
                    
                    if(control_exit==0)
                    {
                        al_play_sample(sample_exit, 8.0, 0.0, 1.5, ALLEGRO_PLAYMODE_ONCE, NULL);
                        control_exit=1;
                        control_play=0;
                        control_high_score=0;
                    }
                    
                } 
                else if(MOUSE_IN_HIGH_SCORE)
                {
                    al_draw_bitmap(fondo_juego, 0, 0, 0);
                    al_draw_bitmap(fondo_high_score, 0, 0, 0);
                    al_flip_display();
                    
                    if(control_high_score==0)
                    {
                        al_play_sample(sample_high_score, 8.0, 0.0, 2.5, ALLEGRO_PLAYMODE_ONCE, NULL);
                        control_high_score=1;
                        control_play=0;
                        control_exit=0;
                    }
                }    
                else
                {
                    al_draw_bitmap(fondo_juego, 0, 0, 0);
                    al_draw_bitmap(fondo_menu, 0, 0, 0);
                    al_flip_display();
                    control_play=0;
                    control_exit=0;
                    control_high_score=0;
                }    

            }
            
            else if ( ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP&&(MOUSE_IN_EXIT))
            {
                do_exit= true;
                
            }
             else if ( ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP&&(MOUSE_IN_PLAY))
             {
                play=play_space_invaders(event_queue, timer);
                    if (play) {
                    printf("Error en play");
                    return -1;
                    }
                
             }     
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
           
            
        }

    }

    al_destroy_bitmap(fondo_menu);
    al_destroy_bitmap(fondo_play);
    al_destroy_bitmap(fondo_exit);
    al_destroy_bitmap(fondo_high_score);
    al_destroy_bitmap(fondo_juego);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_uninstall_audio();
    al_destroy_sample(sample_high_score);
    
    return 0;
}

int play_space_invaders( ALLEGRO_EVENT_QUEUE *event_queue,ALLEGRO_TIMER *timer )
{
       /* al_clear_to_color(al_map_rgb(255, 255, 255)); //Hace clear del backbuffer del diplay al color RGB 255,255,255 (blanco)
        al_flip_display();
       */

    nave_t nave;
    bullet_t bullet;
    
    
    ALLEGRO_BITMAP *nave_imagen = NULL;
    ALLEGRO_BITMAP *fondo_juego = NULL;
    ALLEGRO_BITMAP *alien = NULL;
    ALLEGRO_BITMAP *alien_2 = NULL;
    ALLEGRO_BITMAP *alien_muere = NULL;
    ALLEGRO_BITMAP *bala = NULL;
    ALLEGRO_SAMPLE *laser = NULL;
    
    bool redraw = false;
    bool do_exit = false;
    int cont_x,cont_y,muere, filas_min, columnas_min;
    filas_min=1;
    columnas_min=1;
 
    
    alien_t aliens[FILAS][COLUMNAS];
    
    nave.x = SCREEN_W/2 +(NAVE_SIZE_X/2); //Tamaño de nave en x/2
    nave.y = SCREEN_H-NAVE_SIZE_Y; 
    nave.shot=0;
    ordeno(aliens, filas_min,filas_min);
    
     bool key_pressed[3] = {false, false, false}; //Estado de teclas, true cuando esta apretada


     
     //INSTALO LO RELACIONADO CON EL TECLADO.
    if (!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }
     
     
    nave_imagen = al_load_bitmap("nave.bmp");
    if (!nave_imagen) {
        fprintf(stderr, "failed to create nave bitmap!\n");
        al_destroy_timer(timer);
        return -1;
    }
     fondo_juego = al_load_bitmap("fondo_juego.bmp");
    if (!fondo_juego) {
        fprintf(stderr, "failed to create fondo_juego bitmap!\n");
        al_destroy_timer(timer);
        return -1;
    }
       alien = al_load_bitmap("alien_1.bmp");
    if (!alien) {
        fprintf(stderr, "failed to create alien_1 bitmap!\n");
        al_destroy_timer(timer);
        return -1;
    }
          alien_2 = al_load_bitmap("alien_2.bmp");
    if (!alien_2) {
        fprintf(stderr, "failed to create alien_2 bitmap!\n");
        al_destroy_timer(timer);
        return -1;
    }
        alien_muere = al_load_bitmap("alien_muere.bmp");
    if (!alien) {
        fprintf(stderr, "failed to create alien_2 bitmap!\n");
        al_destroy_timer(timer);
        return -1;
    }
          bala = al_load_bitmap("bala.bmp");
    if (!bala) {
        fprintf(stderr, "failed to create bala bitmap!\n");
        al_destroy_timer(timer);
        return -1;
    }
          
        laser = al_load_sample("play.wav");
    if (!laser) {
        printf("Audio clip laser not loaded!\n");
        return -1;
    }
    
    al_convert_mask_to_alpha(nave_imagen,al_map_rgb(0, 255, 0));
    al_convert_mask_to_alpha(bala,al_map_rgb(0, 255, 0));
    al_convert_mask_to_alpha(alien,al_map_rgb(0, 255, 0));
     al_convert_mask_to_alpha(alien_2,al_map_rgb(0, 255, 0));
    al_convert_mask_to_alpha(alien_muere,al_map_rgb(0, 255, 0));

    
    al_register_event_source(event_queue, al_get_keyboard_event_source()); //REGISTRAMOS EL TECLADO

//resetear las x y bala.
    while (!do_exit) // idem anterior
    {   
        ALLEGRO_EVENT ev;
        if (al_get_next_event(event_queue, &ev)) //Toma un evento de la cola, VER RETURN EN DOCUMENT.
        {
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (key_pressed[KEY_LEFT] && nave.x >= MOVE_RATE)
                    nave.x -= MOVE_RATE;

                if (key_pressed[KEY_RIGHT] && nave.x <= SCREEN_W- NAVE_SIZE_X - MOVE_RATE)
                    nave.x += MOVE_RATE;
                
                 if ((key_pressed[KEY_SPACE])&&(!nave.shot)){
                     
                     al_play_sample(laser, 10.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                     nave.shot=1;
                     bullet.x= nave.x + (NAVE_SIZE_X/2)-(BALA_SIZE_X/2);
                     bullet.y= nave.y -(BALA_SIZE_Y/2);
                     al_draw_bitmap(bala, bullet.x,bullet.y, 0);
                     al_flip_display();
                }

                redraw = true;
            }
            else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)//Si se cierra el display cerrar
                do_exit = true;

           
            else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){//Si se cierra el display o click de mouse cerrar
                do_exit = true;

            }
            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                switch (ev.keyboard.keycode) {
/*
                  
*/
                    case ALLEGRO_KEY_LEFT:
                        key_pressed[KEY_LEFT] = true;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        key_pressed[KEY_RIGHT] = true;
                        break;
                    case ALLEGRO_KEY_SPACE:
                        key_pressed[KEY_SPACE] = true;
                        break;
                }
            }
            else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                switch (ev.keyboard.keycode) {
                    
                    case ALLEGRO_KEY_LEFT:
                        key_pressed[KEY_LEFT] = false;
                        break;
                    case ALLEGRO_KEY_RIGHT:
                        key_pressed[KEY_RIGHT] = false;
                        break;
                    case ALLEGRO_KEY_SPACE:
                        key_pressed[KEY_SPACE] = false;
                        break;
            }
            }

            if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
                        //SI TODOS LOS ALIENIGENAS ESTÁN MUERTOS VUELVE A EMPEZAR (ESTO SE PUEDE MODIFICAR OBVIO)
            
            al_draw_bitmap(fondo_juego, 0, 0, 0);
            al_draw_bitmap(nave_imagen, nave.x, nave.y, 0);
            
            nave.shot=imprimo_alien(bullet.x,bullet.y, nave.shot, aliens, alien, alien_2, alien_muere);
                
                
            if(nave.shot)
                {
                    bullet.y-=(2*MOVE_RATE);
                    al_draw_bitmap(bala, bullet.x,bullet.y, 0);

                    //PARA QUE SE PUEDA VOLVER A DISPARAR
                    if(bullet.y<0)
                    {
                        nave.shot=0;
                     
                    }
                     
                }
            //SI TODOS LOS ALIENIGENAS ESTÁN MUERTOS VUELVE A EMPEZAR (ESTO SE PUEDE MODIFICAR OBVIO)
            if( winner(aliens)==1)
            {
                nave.shot=0;
                
                if(filas_min<FILAS){
                filas_min=filas_min +1;
                }
                if(columnas_min<COLUMNAS){
                columnas_min=columnas_min +1;
                }
                ordeno(aliens, filas_min,columnas_min);
            }

            al_flip_display();
            
            }
        }

    }
    
    
    al_destroy_bitmap(nave_imagen);
    al_destroy_bitmap(bala);
    al_destroy_bitmap(fondo_juego);
    al_destroy_bitmap(alien);

    return 0;
}
    
//int alien (ALLEGRO_DISPLAY display, )
void ordeno(alien_t arr[][COLUMNAS], int filas, int columnas)
    {
            int     orden_x, orden_y;
            float   distancia_x;
            
            distancia_x=((SCREEN_W-2*RANGO_X)-((ALIEN_SIZE_X)*columnas))/(columnas+1);
            //distancia_y=ALIEN_SIZE_Y;
            for(orden_y=0; orden_y<FILAS; ++orden_y)
            {
                for(orden_x=0; orden_x<COLUMNAS; ++orden_x)
                {
                    if((orden_x<filas)&&(orden_y<columnas))
                    {
                    arr[orden_y][orden_x].x=RANGO_X+distancia_x+(ALIEN_SIZE_X)*orden_x+(distancia_x*orden_x);
                    arr[orden_y][orden_x].y=RANGO_Y+2*(ALIEN_SIZE_Y*orden_y);
                    arr[orden_y][orden_x].alive=1;
                    }
                    else
                    {
                    arr[orden_y][orden_x].alive=0;

                    }
    
                }
            }
    }
//ESTA FUNCIÓN IMPRIME A LOS ALIENS Y DEVUELVE EL ESTADO DEL DISPARO (1-0). 
int imprimo_alien(int bullet_x, int bullet_y, int nave_shot, alien_t arr_alien[][COLUMNAS], ALLEGRO_BITMAP *alien_tipo_1, ALLEGRO_BITMAP *alien_tipo_2, ALLEGRO_BITMAP *alien_fin)
{
    
    int shot_status, alien_died, cont_x, cont_y;
    
    shot_status=nave_shot;
    
            for(cont_y=0,alien_died=1;cont_y<FILAS;++cont_y)
            {
                for(cont_x=0;cont_x<COLUMNAS;++cont_x)
                {
                        if((arr_alien[cont_y][cont_x].alive)&&((bullet_x -(ALIEN_SIZE_X)<arr_alien[cont_y][cont_x].x)
                           &&(arr_alien[cont_y][cont_x].x<bullet_x +(ALIEN_SIZE_X/2)))&&((bullet_y <(arr_alien[cont_y][cont_x].y+ALIEN_SIZE_Y))))
                        {
                            if(shot_status==1)
                            {
                            arr_alien[cont_y][cont_x].alive=0;
                            shot_status=0;
                            alien_died=0;
                            al_draw_bitmap(alien_fin, arr_alien[cont_y][cont_x].x, arr_alien[cont_y][cont_x].y, 0);
                            }
                        }
                        if(arr_alien[cont_y][cont_x].alive)
                        {   
                            if(cont_y<2)
                             al_draw_bitmap(alien_tipo_1, arr_alien[cont_y][cont_x].x, arr_alien[cont_y][cont_x].y, 0);
                            else
                              al_draw_bitmap(alien_tipo_2, (arr_alien[cont_y][cont_x].x), arr_alien[cont_y][cont_x].y, 0);
                        }
                }
            }
            if(!alien_died)
            {
                al_flip_display();
                al_rest(0.05);//Para que se vea cuando explota
            }
            
            return shot_status;
}


//ESTA FUNCIÓN DEVUELVE UN UNO SI TODOS LOS ALIENIGENAS ESTÁN MUERTOS.
int winner(alien_t arr_lives[][COLUMNAS])
{
    int     contador_filas, contador_columnas, win;
    
    win=1;
    
    for(contador_filas=0;contador_filas<FILAS;++contador_filas)
            {
                for(contador_columnas=0;contador_columnas<COLUMNAS;++contador_columnas)
                {
                    if(arr_lives[contador_filas][contador_columnas].alive==1)
                    {
                        win=0;
                    }
                    
                }
            } 
    
    return win;
}



