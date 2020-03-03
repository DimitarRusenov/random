#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

/****************************************************************
* INCLUDE FILES
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


/****************************************************************
* DEFINES AND MACROS
****************************************************************/
#define BASE10_TEN 10

#define YUV_MAGIC "YUV4MPEG2 "
#define YUV_MAGIC_LEN 10

#define FRAME_RATE "F300:1"
#define FRAME_RATE_LEN 5

#define INTERLACING "Ip"
#define INTERLACING_LEN 2

#define PIXEL_ASPECT_RAT "A1:1"
#define PIXEL_ASPECT_RAT_LEN 4

#define FRAME_WORD "FRAME"
#define FRAME_WORD_LEN 5

#define MAX_LEN_RESOLUTION 4

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

#define TRUE 1
#define FALSE 0

#define LOOP_TESTING_NUMBER 3
#define BASIC_COLOR_CNT 3

#define MAX_NUMBER_FRAMES 5
#define FULL_CNT 0
#define EMPTY_CNT 5


/****************************************************************
* CONSTANTS
****************************************************************/

static const char _720_res[] = "720p";
static const char _1080_res[] = "1080p";
static const char _1632x1224_res[] = "1632x1224";

static const unsigned int _720p_width = 1280;
static const unsigned int _720p_height = 720;

static const unsigned int _1080p_width = 1920;
static const unsigned int _1080p_height = 1080;

static const unsigned int _1632x1224_width = 1632;
static const unsigned int _1632x1224_height = 1224;

static const unsigned int square_len_720p = 16;
static const unsigned int square_len_1080p = 32;
static const unsigned int square_len_1632p = 64;

static const unsigned char main_colors[3] = {0x00, 0x80, 0xFF};

static const float uv_ratio = 1.5;

typedef struct
{
    unsigned int size;
    unsigned int row_poisiton;
    unsigned int col_position;
}square_t;


/*
* Reads the resolution from the input.
*/

unsigned int read_resolution (char * input);

/*
* Reads the moving step of the square from the input.
*/

unsigned int read_step (char * input, unsigned int height, unsigned int width);

/*
* Sets the square parameters  which will be moving in the .yuv file
*/

void set_initial_square_parameters (square_t * square,
                                    const unsigned int resolution);

/* 
* Sets the width and height of the grid to be allocated later by allocate_grid()
*/

void set_grid_parameters (unsigned int * width,
                          unsigned int * height,
                          const unsigned int resolution);

/* 
* Generates the header of the .yuv file.
*/

void write_header (FILE * file_output,
                  const unsigned int width,
                  const unsigned int height
                  );

/* 
* Writes the width and the height in a file header.
*/

void write_width_height_in_header (FILE * file_output,
                                  unsigned int width,
                                  unsigned int height
                                  );

/* 
* Writes the frame rate in a file header.
*/

void write_frame_rate (FILE * file_output);

/*
* Writes the interlacing in a file header.
*/

void write_interlacing (FILE * file_output);

/* 
* Writes the pixel aspect ratio in a file header.
*/

void write_pixel_aspect_ratio (FILE * file_output);

/*
* Initially fills the grid with Y(luma) pixels
*/

void fill_grid_Y (unsigned char * buffer,
                  unsigned int height,
                  unsigned int width,
                  unsigned char color
                  );

/*
* Initially fills the grid with U(chroma - Cr) pixels
*/

void fill_grid_U (unsigned char * buffer,
                  unsigned int height,
                  unsigned int width,
                  unsigned char color
                  );


/* 
* Initially fills the grid with V(chroma - Cb) pixels
*/

void fill_grid_V (unsigned char * buffer,
                  unsigned int height,
                  unsigned int width,
                  unsigned char color
                  );

/*
* SaveFrame() saves a frame in the .yuv file.
*/

int saveFrame (FILE * file_output,
               unsigned char * grid,
               unsigned int height,
               unsigned int width
               );

/*
* Draws a square in a grid.
*/

void draw_square (unsigned char * grid,
                  const square_t * square,
                  const unsigned int width,
                  unsigned char color
                  );


/* 
* Saves the .yuv video file in which
* the square is moving in horizontal and vertical lines.
*/

void move_square_horizontally_vertically (FILE * file_output,
                                          unsigned char * grid,
                                          square_t * square,
                                          unsigned int step,
                                          unsigned int grid_height,
                                          unsigned int grid_width,
                                          unsigned char * back_up_buffer
                                          );


/*
* Saves the .yuv video file in which
* the square is moving in diagonal lines.
*/

void move_square_diagonally (FILE * file_output,
                             unsigned char * grid,
                             square_t * square,
                             unsigned int step,
                             unsigned int grid_height,
                             unsigned int grid_width,
                             unsigned char * back_up_buffer
                             );



/*
* Sets the background of the .yuv file to white
*/

void set_background_white (unsigned char * buffer,
                           unsigned int height,
                           unsigned int width
                           );


/*
* Sets the Y(luma) of the square.
*/

void set_square_Y (unsigned char * buffer,
                   const square_t * square,
                   unsigned int height,
                   unsigned int width,
                   unsigned char color
                   );


/* 
* Sets the U(Cr) of the square.
*/

void set_square_U (unsigned char * buffer,
                   const square_t * square,
                   unsigned int height,
                   unsigned int width,
                   unsigned char color
                   );


/*
* Sets the V(Cb) of the square.
*/

void set_square_V (unsigned char * buffer,
                   const square_t * square,
                   unsigned int height,
                   unsigned int width,
                   unsigned char color
                   );

/*
* Generates white noise background to the .yuv video.
*/

void generate_white_noise (unsigned char * buffer,
                           unsigned int height,
                           unsigned int width
                           );


/*
* Keeps the original white noise background.
*/

void copy_white_noise_background (unsigned char * source,
                                  unsigned char * destination,
                                  unsigned int size
                                  );

/*
* Moves the square horizontally.
* USED BY THREADS!
*/
void move_square_horizontally_thread (unsigned char * buffer,
                                      unsigned int height,
                                      unsigned int width,
                                      square_t * square,
                                      unsigned int step,
                                      unsigned char * background,
                                      unsigned int * colorY,
                                      unsigned int * colorU,
                                      unsigned int * colorV
                                     );


/*
* Moves the square diagonally.
* USED BY THREADS!
*/
void move_square_diagonally_thread (unsigned char * buffer,
                                    unsigned int height,
                                    unsigned int width,
                                    square_t * square,
                                    unsigned int step,
                                    unsigned char * background,
                                    unsigned int * colorY,
                                    unsigned int * colorU,
                                    unsigned int * colorV
                                    );

#endif /* FUNCTIONS_H_ */
