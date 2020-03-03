#include <stdlib.h>
#include <stdio.h>

#include "functions.h"

sem_t full;
sem_t empty;
sem_t mutex;

square_t square;
unsigned char * background;
unsigned char *frames[MAX_NUMBER_FRAMES];

FILE * file_output;
unsigned int grid_width = 0;
unsigned int grid_height = 0;
unsigned int step = 0;

static unsigned int generate_cnt = 0;
static unsigned int save_cnt = 0;

unsigned int colorY;
unsigned int colorU;
unsigned int colorV;


void * generate_frame(void * arg)
{
    while(TRUE) {
        sem_wait (&empty);
        sem_wait (&mutex);

        move_square_diagonally_thread (frames[generate_cnt % MAX_NUMBER_FRAMES],
                                         grid_height,
                                         grid_width,
                                         &square,
                                         step,
                                         background,
                                         &colorY,
                                         &colorU,
                                         &colorV
                                        );

        generate_cnt++;

        sem_post (&mutex);
        sem_post (&full);
    }

    return NULL;
}


void * save_frame(void * arg)
{

    while(TRUE) {
        sem_wait(&full);
        sem_wait(&mutex);

        saveFrame (file_output,
                   frames[save_cnt % MAX_NUMBER_FRAMES],
                   grid_height,
                   grid_width
                   );

        save_cnt++;

        sem_post (&mutex);
        sem_post (&empty);
    }

    return NULL;
}

int main (int argc, char *argv[])
{

    if(argc != 3) {
        fprintf (stderr, "Too many or too few arguments!\n");
        exit (EXIT_FAILURE);
    }

    unsigned int resolution = read_resolution(argv[1]);

    if(resolution < 0) {
        fprintf (stderr, "No such resolution!\n");
        exit (EXIT_FAILURE);
    }

    set_grid_parameters (&grid_width, &grid_height, resolution);
    step = read_step (argv[2], grid_height, grid_width);
    set_initial_square_parameters (&square, resolution);

    int i;

    for (i = 0; i < MAX_NUMBER_FRAMES; i++) {
        frames[i] = (unsigned char *) malloc (grid_width * grid_height * uv_ratio);
        if (!frames[i]) {
            fprintf(stderr, "Could not allocate memory!\n");
            exit(EXIT_FAILURE);

        }
    }

    background = (unsigned char *) malloc (grid_width * grid_height * uv_ratio);

    if (!background) {
        fprintf(stderr, "Could not allocate memory!\n");
        exit(EXIT_FAILURE);
    }


    sem_init (&full, 0, FULL_CNT);
    sem_init (&empty, 0, EMPTY_CNT);
    sem_init (&mutex, 0, 1);

    generate_white_noise(background, grid_height, grid_width);

    colorY = rand () % BASIC_COLOR_CNT;
    colorU = rand () % BASIC_COLOR_CNT;
    colorV = rand () % BASIC_COLOR_CNT;

    pthread_t t1, t2;

    file_output = fopen ("test_yuv.y4m", "wb");
    if (!file_output) {
        fprintf (stderr, "Something went wrong opening the file!\n");
        exit (EXIT_FAILURE);
    }

    write_header (file_output, grid_width, grid_height);


    pthread_create(&t1, NULL, &generate_frame, NULL);
    pthread_create(&t2, NULL, &save_frame, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    fclose (file_output);

    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&mutex);

    for (i = 0; i < MAX_NUMBER_FRAMES; i++) {
        free(frames[i]);
    }

    return 0;
}