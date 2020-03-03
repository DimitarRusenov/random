#include "functions.h"

static int left = FALSE;
static int right = TRUE;
static int up = FALSE;
static int down = TRUE;

unsigned int read_resolution (char * input)
{

    unsigned int resolution;

    if (strcmp(input, _720_res) == 0) {
        resolution = _720p_width * _720p_height;
    } else if (strcmp (input, _1080_res) == 0) {
        resolution = _1080p_width * _1080p_height;
    } else if (strcmp (input, _1632x1224_res) == 0) {
        resolution = _1632x1224_width * _1632x1224_height;
    } else {
        resolution = 0;
    }

    return resolution;
}


unsigned int read_step (char * input,
                        unsigned int height,
                        unsigned int width)
{

    unsigned int step = 0;

    step = atoi (input);

    if (step < 0 || step > height || step > width) {
	    fprintf (stderr, "Invalid step!\n");
	    exit (EXIT_FAILURE);
    }

    return step;
}


void set_initial_square_parameters (square_t * square,
                                    const unsigned int resolution
                                    )
{

    if (resolution == _720p_width * _720p_height) {
        square->size = square_len_720p;
    } else if (resolution == _1080p_width * _1080p_height) {
        square->size = square_len_1080p;
    } else {
        square->size = square_len_1632p;
    }

    square->col_position = 0;
    square->row_poisiton = 0;

    return;
}


void set_grid_parameters (unsigned int * width,
                          unsigned int * height,
                          const unsigned int resolution
                          )
{
    if(resolution == _720p_width * _720p_height) {
        *width = _720p_width;
        *height = _720p_height;
    } else if(resolution == _1080p_width * _1080p_height) {
        *width = _1080p_width;
        *height = _1080p_height;
    } else {
        *width = _1632x1224_width;
        *height = _1632x1224_height;
    }

    return;
}

void write_header (FILE * file_output,
                  const unsigned int width,
                  const unsigned int height
                  )
{
    if (!(fwrite (YUV_MAGIC, 1, YUV_MAGIC_LEN, file_output))) {
        exit (EXIT_FAILURE);
    }

    write_width_height_in_header (file_output, width, height);
    write_frame_rate (file_output);
    write_interlacing (file_output);
    write_pixel_aspect_ratio (file_output);
}

void write_width_height_in_header (FILE * file_output,
                                  unsigned int width,
                                  unsigned int height
                                  )
{
    int i;

    if (!(fwrite ("W", 1, 1, file_output))) {
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[MAX_LEN_RESOLUTION];
    memset (buffer, 0, MAX_LEN_RESOLUTION);

    for (i = MAX_LEN_RESOLUTION - 1; width > 0; i--) {
        buffer[i] = (width % BASE10_TEN) + '0';
        width /= BASE10_TEN;
    }

    if (buffer[0] == 0) {
        i = 1;
    } else {
        i = 0;
    }

    for (; i < MAX_LEN_RESOLUTION; i++) {
        if (!(fwrite (&buffer[i], 1, 1, file_output))) {
            exit (EXIT_FAILURE);
        }
    }

    if (!(fwrite (" ", 1, 1, file_output))) {
        exit(EXIT_FAILURE);
    }

    memset (buffer, 0, MAX_LEN_RESOLUTION);

    if (!(fwrite ("H", 1, 1, file_output))) {
        exit(EXIT_FAILURE);
    }

    for (i = MAX_LEN_RESOLUTION - 1; height > 0; i--) {
        buffer[i] = (height % BASE10_TEN) + '0';
        height /= BASE10_TEN;
    }

    if (buffer[0] == 0) {
        i = 1;
    } else {
        i = 0;
    }

    for (; i < MAX_LEN_RESOLUTION; i++) {
        if(!(fwrite (&buffer[i], 1, 1, file_output))) {
            exit (EXIT_FAILURE);
        }
    }

    if (!(fwrite (" ", 1, 1, file_output))) {
        exit (EXIT_FAILURE);
    }
}

void write_frame_rate (FILE * file_output)
{
    if (!(fwrite (FRAME_RATE, 1, FRAME_RATE_LEN, file_output))) {
        exit (EXIT_FAILURE);
    }

    if (!(fwrite (" ", 1, 1, file_output))) {
        exit (EXIT_FAILURE);
    }
}

void write_interlacing (FILE * file_output)
{
    if (!(fwrite (INTERLACING, 1, INTERLACING_LEN, file_output))) {
        exit (EXIT_FAILURE);
    }

    if (!(fwrite (" ", 1, 1, file_output))) {
        exit (EXIT_FAILURE);
    }
}

void write_pixel_aspect_ratio (FILE * file_output)
{
    if (!(fwrite (PIXEL_ASPECT_RAT ,1, PIXEL_ASPECT_RAT_LEN, file_output))) {
        exit (EXIT_FAILURE);
    }

    unsigned int space = 0x0A;

    if (!(fwrite (&space, 1, 1, file_output))) {
        exit (EXIT_FAILURE);
    }
}


void fill_grid_Y (unsigned char * buffer,
                  unsigned int height,
                  unsigned int width,
                  unsigned char color
                  )
{
    memset (buffer, color, height * width);

    return;
}

void fill_grid_U (unsigned char * buffer,
                  unsigned int height,
                  unsigned int width,
                  unsigned char color
                  )
{
    memset (buffer + (height * width), color, (height * width)/4);

    return;
}

void fill_grid_V (unsigned char * buffer,
                  unsigned int height,
                  unsigned int width,
                  unsigned char color
                  )
{
    memset (buffer + (height * width) + ((height * width)/4),
           color,
           (height * width)/4
           );

    return;
}

void set_background_white (unsigned char * buffer,
                           unsigned int height,
                           unsigned int width
                           )
{
    fill_grid_Y (buffer, height, width, 0xFF);
    fill_grid_U (buffer, height, width, 0x80);
    fill_grid_V (buffer, height, width, 0x80);

    return;
}

void generate_white_noise (unsigned char * buffer,
                           unsigned int height,
                           unsigned int width
                           )
{
    unsigned char color;

    unsigned int i, j;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            color = rand () % 255;
            buffer[i * width + j] = color;
        }
    }

    fill_grid_U (buffer, height, width, 0x80);
    fill_grid_V (buffer, height, width, 0x80);

    return;
}

int saveFrame (FILE * file_output,
               unsigned char * grid,
               unsigned int height,
               unsigned int width
               )
{
    if (!(fwrite (FRAME_WORD, 1, FRAME_WORD_LEN, file_output))) {
        fprintf (stderr, "Something went wrong in writing the frame!\n");
        return -1;
    }

    static unsigned char space = 0x0A;

    if (!(fwrite (&space, 1, 1, file_output))) {
        return -1;
    }

    if (!(fwrite (grid, 1, width * height + (width * height) / 2, file_output))) {
        fprintf (stderr, "Something went wrong in writing the frame!\n");
        return -1;
    }

    return 1;
}

void draw_square (unsigned char * grid,
                  const square_t * square,
                  const unsigned int width,
                  unsigned char color
                  )
{
    int it, j;
    unsigned int row = square->row_poisiton;
    unsigned int col = square->col_position;
    unsigned int pos = 0;

    for (it = 0; it < square->size; it++) {
        for (j = 0; j < square->size; j++) {
            pos = row * width + col;
            grid[pos] = color;
            col++;
        }
        row++;
        col = square->col_position;
	}

    return;
}

void set_square_Y (unsigned char * buffer,
                   const square_t * square,
                   unsigned int height,
                   unsigned int width,
                   unsigned char color
                   )
{
    int i;
    unsigned int row = square->row_poisiton;

    for (i = 0; i < square->size; i++) {
        memset (buffer + (row * width) + square->col_position, color, square->size);
        row++;
    }

    return;
}

void set_square_U (unsigned char * buffer,
                   const square_t * square,
                   unsigned int height,
                   unsigned int width,
                   unsigned char color
                   )
{
    int i, j;

    unsigned int row = square->row_poisiton;
    unsigned int col = square->col_position;

    unsigned int upos;


    for(i = 0; i < square->size; i++) {
        for(j = 0; j < square->size; j++) {
            upos = (row / 2) * (width/2) + (col / 2) + (width * height);
            buffer[upos] = color;
            col++;
        }
        row++;
        col = square->col_position;
    }

    return;
}

void set_square_V (unsigned char * buffer,
                   const square_t * square,
                   unsigned int height,
                   unsigned int width,
                   unsigned char color
                   )
{
    int i, j;

    unsigned int row = square->row_poisiton;
    unsigned int col = square->col_position;

    unsigned int vpos;

    for (i = 0; i < square->size; i++) {
        for (j = 0; j < square->size; j++) {
            vpos = (row / 2) * (width/2) + (col / 2) +
                   (width * height) + ((width * height) / 4);
            buffer[vpos] = color;
            col++;
        }
        row++;
        col = square->col_position;
    }

    return;
}

void copy_white_noise_background (unsigned char * source,
                                  unsigned char * destination,
                                  unsigned int size
                                  )
{
    memcpy (destination, source, size);

    return;
}

void move_square_horizontally_vertically (FILE * file_output,
                                          unsigned char * grid,
                                          square_t * square,
                                          unsigned int step,
                                          unsigned int grid_height,
                                          unsigned int grid_width,
                                          unsigned char * back_up_buffer
                                          )
{
   unsigned int colorY = rand () % BASIC_COLOR_CNT;
   unsigned int colorU = rand () % BASIC_COLOR_CNT;
   unsigned int colorV = rand () % BASIC_COLOR_CNT;


    while(1) {

        for (; square->col_position + step + square->size <= grid_width;
             square->col_position += step) {

            set_square_Y (grid, square, grid_height, grid_width,
                          main_colors[colorY]);
            set_square_U (grid, square, grid_height, grid_width,
                          main_colors[colorU]);
            set_square_V (grid, square, grid_height, grid_width,
                          main_colors[colorV]);

            saveFrame (file_output, grid, grid_height, grid_width);

            memcpy (grid, back_up_buffer, grid_height * grid_width * uv_ratio);
        }

        colorY = rand () % BASIC_COLOR_CNT;
        colorU = rand () % BASIC_COLOR_CNT;
        colorV = rand () % BASIC_COLOR_CNT;

        if (square->row_poisiton + step +  square->size < grid_height) {
            square->row_poisiton += step;
        } else {
            break;
        }

        for(;square->col_position > 0; square->col_position -= step) {

            set_square_Y (grid, square, grid_height, grid_width,
                          main_colors[colorY]);
            set_square_U (grid, square, grid_height, grid_width,
                          main_colors[colorU]);
            set_square_V (grid, square, grid_height, grid_width,
                          main_colors[colorV]);

            saveFrame (file_output, grid, grid_height, grid_width);

            memcpy (grid, back_up_buffer, grid_height * grid_width * uv_ratio);
        }

        colorY = rand () % BASIC_COLOR_CNT;
        colorU = rand () % BASIC_COLOR_CNT;
        colorV = rand () % BASIC_COLOR_CNT;

        if(square->row_poisiton + step +  square->size < grid_height) {
            square->row_poisiton += step;
        } else {
            break;
        }
    }

    return;
}

void move_square_diagonally (FILE * file_output,
                             unsigned char * grid,
                             square_t * square,
                             unsigned int step,
                             unsigned int grid_height,
                             unsigned int grid_width,
                             unsigned char * back_up_buffer)
{
    unsigned int colorY = rand () % BASIC_COLOR_CNT;
    unsigned int colorU = rand () % BASIC_COLOR_CNT;
    unsigned int colorV = rand () % BASIC_COLOR_CNT;

    int left = FALSE;
    int right = TRUE;
    int up = FALSE;
    int down = TRUE;

    int cnt = 0;

    while (cnt < LOOP_TESTING_NUMBER) {

        if (right == TRUE && down == TRUE) {

            for (; square->col_position + step + square->size <= grid_width &&
                   square->row_poisiton+ step + square->size <= grid_height;
                   square->col_position += step) {

                set_square_Y (grid, square, grid_height, grid_width,
                              main_colors[colorY]);
                set_square_U (grid, square, grid_height, grid_width,
                              main_colors[colorU]);
                set_square_V (grid, square, grid_height, grid_width,
                              main_colors[colorV]);

                saveFrame (file_output, grid, grid_height, grid_width);

                memcpy (grid, back_up_buffer, grid_height * grid_width * uv_ratio);

                square->row_poisiton += step;
            }

            if(grid_width - square->col_position <
               grid_height - square->row_poisiton) {
                right = FALSE;
                left = TRUE;
            } else {
                up = TRUE;
                down = FALSE;
            }
        }

        colorY = rand () % BASIC_COLOR_CNT;
        colorU = rand () % BASIC_COLOR_CNT;
        colorV = rand () % BASIC_COLOR_CNT;

        if (right == TRUE && up == TRUE) {

            for( ; square->col_position + step + square->size <= grid_width &&
                   square->row_poisiton  > 0; square->col_position += step) {

                set_square_Y (grid, square, grid_height, grid_width,
                              main_colors[colorY]);
                set_square_U (grid, square, grid_height, grid_width,
                              main_colors[colorU]);
                set_square_V (grid, square, grid_height, grid_width,
                              main_colors[colorV]);


                saveFrame (file_output, grid, grid_height, grid_width);

                memcpy (grid, back_up_buffer, grid_height * grid_width * uv_ratio);

                square->row_poisiton -= step;
            }

            if (grid_width - square->col_position < square->row_poisiton) {
                right = FALSE;
                left = TRUE;
            } else {
                up = FALSE;
                down = TRUE;
            }
        }

        colorY = rand () % BASIC_COLOR_CNT;
        colorU = rand () % BASIC_COLOR_CNT;
        colorV = rand () % BASIC_COLOR_CNT;

        if (left == TRUE && up == TRUE) {

            for (; square->col_position > 0  &&
                   square->row_poisiton >  0; square->col_position -= step) {

                set_square_Y (grid, square, grid_height, grid_width,
                              main_colors[colorY]);
                set_square_U (grid, square, grid_height, grid_width,
                              main_colors[colorU]);
                set_square_V (grid, square, grid_height, grid_width,
                              main_colors[colorV]);

                saveFrame (file_output, grid, grid_height, grid_width);

                memcpy (grid, back_up_buffer, grid_height * grid_width * uv_ratio);

                square->row_poisiton -= step;
            }

            if (square->col_position < square->row_poisiton) {
                left = FALSE;
                right = TRUE;
            } else {
                up = FALSE;
                down = TRUE;
            }

        }


        colorY = rand () % BASIC_COLOR_CNT;
        colorU = rand () % BASIC_COLOR_CNT;
        colorV = rand () % BASIC_COLOR_CNT;


        if (left == TRUE && down == TRUE) {

                for (; square->col_position > 0 &&
                       square->row_poisiton + square->size <= grid_height;
                       square->col_position -= step) {

                    set_square_Y (grid, square, grid_height, grid_width,
                                  main_colors[colorY]);
                    set_square_U (grid, square, grid_height, grid_width,
                                  main_colors[colorU]);
                    set_square_V (grid, square, grid_height, grid_width,
                                  main_colors[colorV]);

                    saveFrame (file_output, grid, grid_height, grid_width);

                    memcpy (grid, back_up_buffer,
                            grid_height * grid_width * uv_ratio);

                    square->row_poisiton += step;
                }

                if (square->col_position < grid_height - square->row_poisiton) {
                    right = TRUE;
                    left = FALSE;
                } else {
                    down = FALSE;
                    up = TRUE;
                }
        }

        colorY = rand () % BASIC_COLOR_CNT;
        colorU = rand () % BASIC_COLOR_CNT;
        colorV = rand () % BASIC_COLOR_CNT;

        cnt++;
    }

    return;
}


void move_square_horizontally_thread (unsigned char * buffer,
                                      unsigned int height,
                                      unsigned int width,
                                      square_t * square,
                                      unsigned int step,
                                      unsigned char * background,
                                      unsigned int * colorY,
                                      unsigned int * colorU,
                                      unsigned int * colorV
                                     )
{

    memcpy(buffer, background, height * width * uv_ratio);

    set_square_Y (buffer, square, height, width, main_colors[*colorY]);
    set_square_U (buffer, square, height, width, main_colors[*colorU]);
    set_square_V (buffer, square, height, width, main_colors[*colorV]);

    if (right == TRUE) {
        if (square->col_position + step + square->size <= width) {
            square->col_position += step;
        } else {
            right = FALSE;
            left = TRUE;
            if (down == TRUE) {
                if (square->row_poisiton + step + square->size <= height) {
                    square->row_poisiton += step;
                } else {
                    down = FALSE;
                    up = TRUE;
                }
            } else {
                if (square->row_poisiton - step > 0) {
                    square->row_poisiton -= step;
                } else {
                    up = FALSE;
                    down = TRUE;
                }
            }

            *colorY = rand () % BASIC_COLOR_CNT;
            *colorU = rand () % BASIC_COLOR_CNT;
            *colorV = rand () % BASIC_COLOR_CNT;
        }
    } else if (left == TRUE) {
        if (square->col_position - step > 0) {
            square->col_position -= step;
        } else {
            left = FALSE;
            right = TRUE;
            if (down == TRUE) {
                if (square->row_poisiton + step + square->size <= height) {
                    square->row_poisiton += step;
                } else {
                    down = FALSE;
                    up = TRUE;
                }
            } else {
                if (square->row_poisiton - step > 0) {
                    square->row_poisiton -= step;
                } else {
                    up = FALSE;
                    down = TRUE;
                }
            }
            *colorY = rand () % BASIC_COLOR_CNT;
            *colorU = rand () % BASIC_COLOR_CNT;
            *colorV = rand () % BASIC_COLOR_CNT;
        }
    }


    return;
}

void move_square_diagonally_thread (unsigned char * buffer,
                                    unsigned int height,
                                    unsigned int width,
                                    square_t * square,
                                    unsigned int step,
                                    unsigned char * background,
                                    unsigned int * colorY,
                                    unsigned int * colorU,
                                    unsigned int * colorV
                                    )
{

    memcpy (buffer, background, height * width * uv_ratio);

    set_square_Y (buffer, square, height, width, main_colors[*colorY]);
    set_square_U (buffer, square, height, width, main_colors[*colorU]);
    set_square_V (buffer, square, height, width, main_colors[*colorV]);

    if (right == TRUE && down == TRUE) {
         if(square->col_position + step + square->size <= width &&
            square->row_poisiton + step + square->size <= height) {
            square->col_position += step;
            square->row_poisiton += step;
        } else {
            if (width - square->col_position < height - square->row_poisiton) {
                right = FALSE;
                left = TRUE;
            } else {
                up = TRUE;
                down = FALSE;
            }

            *colorY = rand () % BASIC_COLOR_CNT;
            *colorU = rand () % BASIC_COLOR_CNT;
            *colorV = rand () % BASIC_COLOR_CNT;
        }
    } else if (right == TRUE && up == TRUE) {
        if (square->col_position + step + square->size <= width &&
            square->row_poisiton > 0) {
            square->col_position += step;
            square->row_poisiton -= step;
        } else {
            if (width - square->col_position < square->row_poisiton) {
                right = FALSE;
                left = TRUE;
            } else {
                up = FALSE;
                down = TRUE;
            }

            *colorY = rand () % BASIC_COLOR_CNT;
            *colorU = rand () % BASIC_COLOR_CNT;
            *colorV = rand () % BASIC_COLOR_CNT;
        }
    } else if (left == TRUE && down == TRUE) {
        if (square->col_position > 0 &&
            square->row_poisiton + step + square->size <= height) {
            square->col_position -= step;
            square->row_poisiton += step;
        } else {
            if (square->col_position < height - square->row_poisiton) {
                right = TRUE;
                left = FALSE;
            } else {
                down = FALSE;
                up = TRUE;
            }

            *colorY = rand () % BASIC_COLOR_CNT;
            *colorU = rand () % BASIC_COLOR_CNT;
            *colorV = rand () % BASIC_COLOR_CNT;
        }
    } else if (left == TRUE && up == TRUE) {
        if (square->col_position > 0  && square->row_poisiton >  0) {
            square->col_position -= step;
            square->row_poisiton -= step;
        } else {
            if (square->col_position < square->row_poisiton) {
                left = FALSE;
                right = TRUE;
            } else {
                up = FALSE;
                down = TRUE;
            }

            *colorY = rand () % BASIC_COLOR_CNT;
            *colorU = rand () % BASIC_COLOR_CNT;
            *colorV = rand () % BASIC_COLOR_CNT;
        }
    }

    return;
}
