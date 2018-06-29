/*
    Author: Peter Berta
    Date: 29.11.2016
    Subj.: Parallel programming
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/freeglut.h>
  #include <GL/freeglut_ext.h>
#endif

typedef struct {
    GLbyte r;
    GLbyte g;
    GLbyte b;
} pixel;

#define TEX_SIZE 600
#define MSG_LIMIT 1080000
#define THREAD_NUM 16
pixel image[TEX_SIZE][TEX_SIZE];

GLuint texture;
int ntasks;

int start = 0;
int end = TEX_SIZE-1;
float max = 30000;
float zoom = 4.0;
float movex = 0.0;
float movey = 0.0;

float msgf[6];
char msgc[MSG_LIMIT];

/*
    clock_t t_start = clock(), diff;

    diff = clock() - t_start;

    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
*/

void fill_image(int x, int y, int r, int g, int b) {
    image[y][x].r = r;
    image[y][x].g = g;
    image[y][x].b = b;
}

void fill_msg(int col, int row, int r, int g, int b){
    // (riadok * velkost_riadku + stlpec) * 3
    msgc[row*TEX_SIZE*3 + col*3] = r;
    msgc[row*TEX_SIZE*3 + col*3 + 1] = g;
    msgc[row*TEX_SIZE*3 + col*3 + 2] = b;
}

void serv_count(int id) {

    // code of fractal from http://jonisalonen.com/2013/lets-draw-the-mandelbrot-set/
    int height = TEX_SIZE, width = TEX_SIZE;
    int row, col;

    #pragma omp parallel for collapse (2) private(row, col) shared(image) num_threads(THREAD_NUM)
    for (row = start; row < end+1; row++) {
        for (col = 0; col < width; col++) {

            double c_re = (col - width/2.0)*zoom/width + movex;
            double c_im = (row - height/2.0)*zoom/width + movey;
            double x = 0, y = 0;
            int iteration = 0;

            while (x*x+y*y <= 4 && iteration < max) {
                double x_new = x*x - y*y + c_re;
                y = 2*x*y + c_im;
                x = x_new;
                iteration++;
            }
            if (iteration < max) { //color
                fill_msg(col,row-start,255 - iteration*10,0,0);
            }
            else { // black
                fill_msg(col,row-start,0,0,0);
            }
        }
    }
}

// Initialize OpenGL state
void init() {
	// Texture setup
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Other
    glClearColor(0,0,0,0);
    gluOrtho2D(-1,1,-1,1);
    glLoadIdentity();
    glColor3f(1,1,1);
}


void host_send(){
    int i,j,k,err,tag = 42;
    MPI_Status status;

    msgf[2] = max;
    msgf[3] = zoom;
    msgf[4] = movex;
    msgf[5] = movey;


    for (i=1;i<ntasks;i++){
        msgf[0] =  (TEX_SIZE*(i-1))/(ntasks-1);
        msgf[1] =  TEX_SIZE*(i)/(ntasks-1) -1;

        err = MPI_Send(msgf, 6, MPI_FLOAT, i, tag, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS) {
            printf("Process %i: Error in MPI_Send!\n", i);
            exit(1);
        }
    }

    // vycistit obraz
    for(j=0;j<TEX_SIZE;j++)
        for(k=0;k<TEX_SIZE;k++)
            fill_image(k,j,255,255,255);


    for (i=1;i<ntasks;i++){

        err = MPI_Recv(msgc, MSG_LIMIT, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD,&status);
        if (err != MPI_SUCCESS) {
            printf("Error in MPI_Recv!\n");
            exit(1);
        }

        int source_id = status.MPI_SOURCE;
        int pos = 0;

        // 200 - 399
        for(j=(TEX_SIZE/(ntasks-1))*(source_id-1);j<((TEX_SIZE/(ntasks-1))*source_id);j++){
            for(k=0;k<TEX_SIZE;k++){
                fill_image(k,j,msgc[pos],msgc[pos+1],msgc[pos+2]);
                pos += 3;
            }
        }
    }
}

void single_process(){
    int height = TEX_SIZE, width = TEX_SIZE;
    int row, col;

    #pragma omp parallel for collapse (2) private(row, col) shared(image) num_threads(THREAD_NUM)
    for (row = start; row < end+1; row++) {
        for (col = 0; col < width; col++) {

            double c_re = (col - width/2.0)*zoom/width + movex;
            double c_im = (row - height/2.0)*zoom/width + movey;
            double x = 0, y = 0;
            int iteration = 0;

            while (x*x+y*y <= 4 && iteration < max) {
                double x_new = x*x - y*y + c_re;
                y = 2*x*y + c_im;
                x = x_new;
                iteration++;
            }

            if (iteration < max) { //color
                fill_image(col,row-start,255 - iteration*10,0,0);
            }
            else { // black
                fill_image(col,row-start,0,0,0);
            }
        }
    }
}
// Generate and display the image.
void display() {
    glutSetWindowTitle("Waiting...");
    clock_t t_start = clock(), diff;

    if (ntasks == 1) single_process();
        else host_send(); // send + receive

    diff = clock() - t_start;
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    glutSetWindowTitle("Done!");

    if (ntasks > 2) printf("Time taken with %d processes %d s %d ms\n", ntasks-1, msec/1000, msec%1000);
        else printf("Time taken with 1(%d) process %d s %d ms\n",ntasks, msec/1000, msec%1000);

    // Copy image to texture memory
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    // Clear screen buffer
    glClear(GL_COLOR_BUFFER_BIT);
    // Render a quad
    glBegin(GL_QUADS);
        glTexCoord2f(1,0); glVertex2f(1,-1);
        glTexCoord2f(1,1); glVertex2f(1,1);
        glTexCoord2f(0,1); glVertex2f(-1,1);
        glTexCoord2f(0,0); glVertex2f(-1,-1);
    glEnd();
    // Display result
    glFlush();
    glutPostRedisplay();
    glutSwapBuffers();
}


void keyboard ( unsigned char key, int x, int y )
{
    int i,err,tag=42;

    switch ( key ) {
        case 27:
            /*
            msgf[0]=-1;
            for (i=1;i<=ntasks;i++){
                err = MPI_Send(msgf, 6, MPI_FLOAT, i, tag, MPI_COMM_WORLD);
                if (err != MPI_SUCCESS) {
                    printf("Process %i: Error in MPI_Send!\n", i);
                    exit(1);
                }
            }
            */
            exit ( 0 );
            break;
        case 'm': zoom *= 0.98;
            break;
        case 'n': zoom *= 1.02;
            break;
        case 'd': movex += 0.05*zoom;
            break;
        case 'a': movex -= 0.05*zoom;
            break;
        case 'w': movey += 0.05*zoom;
            break;
        case 's': movey -= 0.05*zoom;
            break;
        case 'q': max += 1;
            break;
        case 'e': max -= 1;
            break;
        default:
            break;
    }
    display();
}

// Main entry function
int main(int argc, char ** argv) {
    const int tag = 42;	        /* Message tag */
    int   id, source_id, err, i,j,k;
    MPI_Status status;

    err = MPI_Init(&argc, &argv); /* Initialize MPI */
    if (err != MPI_SUCCESS) {
        printf("MPI_init failed!\n");
        exit(1);
    }

    err = MPI_Comm_size(MPI_COMM_WORLD, &ntasks);	/* Get nr of tasks */
    if (err != MPI_SUCCESS) {
        printf("MPI_Comm_size failed!\n");
        exit(1);
    }

    err = MPI_Comm_rank(MPI_COMM_WORLD, &id);    /* Get id of this process */
    if (err != MPI_SUCCESS) {
        printf("MPI_Comm_rank failed!\n");
        exit(1);
    }

    /* Check that we run on at least two processors
    if (ntasks < 2) {
        printf("You have to use at least 2 processors to run this program\n");
        MPI_Finalize();
        exit(0);
    }
    */

    // Koniec initu MPI

    if (id == 0){
        // Init GLUT
        glutInit(&argc, argv);
        glutInitWindowSize(TEX_SIZE, TEX_SIZE);
        glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
        glutCreateWindow("Mandelbrot - OpenMP & MPI");
        // Set up OpenGL state
        init();
        // Run the control loop
        display();
        glutKeyboardFunc( keyboard );
        glutMainLoop();
    }

    else {
        while(1){
            // prijmem detaily o pocitani
            err = MPI_Recv(msgf, 6, MPI_FLOAT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            if (err != MPI_SUCCESS) {
                printf("Error in MPI_Recv!\n");
                exit(1);
            }

            start = (int)msgf[0];
            end = (int)msgf[1];
            max = msgf[2];
            zoom = msgf[3];
            movex = msgf[4];
            movey = msgf[5];


            // pocitanie fraktalu
            serv_count(id);

            err = MPI_Send(msgc, MSG_LIMIT, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
            if (err != MPI_SUCCESS) {
                printf("Process %i: Error in MPI_Send!\n", id);
                exit(1);
            }
        }
    }


    err = MPI_Finalize();	         /* Terminate MPI */
    if (err != MPI_SUCCESS) {
        printf("Error in MPI_Finalize!\n");
        exit(1);
    }
    return EXIT_SUCCESS;
}
