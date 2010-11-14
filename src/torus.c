#include <stdio.h>  // printf()
#include <stdlib.h> // exit()
#include <math.h>   // pow(), sqrt()
#include <GL/glut.h>
#include "input.h"

// Definitions.
#define WINDOW_X_SIZE 500
#define WINDOW_Y_SIZE 500

// Global variables.
float quadrant[9][2] = {
  { 0.0,  0.0},
  { 0.0,  1.0},
  { 0.0, -1.0},
  { 1.0,  0.0},
  { 1.0,  1.0},
  { 1.0, -1.0},
  {-1.0,  0.0},
  {-1.0,  1.0},
  {-1.0, -1.0}
};

// Normalize an number between 0 and 1 by throwing out the integer part.
//
// Arguments: float f : A positive number.
//
// Returns: float : Everything after the decimal point.
//
float norm(float f) {
  return f - floor(f);
}//norm

// Calculate the minimum of two numbers.
//
// Arguments: float a : A number.
//                  b : Another number.
//
// Returns: float : The mininum of a and b.
//
float min(float a, float b) {
  if (a < b)
    return a;
  return b;
}//min

void drawline(float p[2], float q[2]) {
  float t[2] = {(p[0] * 2.0) - 1.0, (p[1] * 2.0) - 1.0};
  float u[2] = {(q[0] * 2.0) - 1.0, (q[1] * 2.0) - 1.0};

  glBegin(GL_LINES);
    glVertex2d(t[0], t[1]);
    glVertex2d(u[0], u[1]);
  glEnd();
}//drawpoint

// Calculate the squared distance between two points on a torus.
//
// Arguments: float a[2] : Point one (a[0] = x, a[1] = y).
//                  b[2] : Point two (b[0] = x, b[1] = y).
//
// Returns: float : The minimum distance between a and b.
//
float dist(float a[2], float b[2]) {
  int minindex,
      i;
  float min = 1.0, // 0.5 is the maximum.
        temp;

  for (i = 0; i < 9; i++) {
    temp = pow(a[0] - (b[0] + quadrant[i][0]), 2) + 
           pow(a[1] - (b[1] + quadrant[i][1]), 2);
    if (temp < min) {
      min = temp;
      minindex = i;
    }
  }//for
  printf("(%f, %f); (%f, %f): (%f, %f)\n", a[0], a[1], b[0], b[1],
         (b[0] + quadrant[minindex][0]) - a[0],
         (b[1] + quadrant[minindex][1]) - a[1]);
  float v[2] = {(b[0] + quadrant[minindex][0]) - a[0],
                (b[1] + quadrant[minindex][1]) - a[1]};
  float c[2] = {a[0] + .4 * v[0], a[1] + .4 * v[1]};
  float d[2] = {b[0] - .4 * v[0], b[1] - .4 * v[1]};
  /*
  drawline(a, b);
  glutSwapBuffers();
  glFlush();
  */
  drawline(a, c);
  drawline(b, d);

  return min;
}//dist

// Draw a normalized point in the opengl window.
//
// Arguments: float p[2] : The coordintates of the point (p[0] = x, p[1] = y)
//                         with (0 <= x, y < 1).
//
void drawpoint(float p[2]) {
  float t[2] = {(p[0] * 2.0) - 1.0, (p[1] * 2.0) - 1.0};

  glBegin(GL_POINTS);
    glVertex2fv(t);
  glEnd();
}//drawpoint

void display(void) {
  int i;

  glClear(GL_COLOR_BUFFER_BIT);
  /*
  drawpoint(point1[0], point1[1]);
  drawpoint(point2[0], point2[1]);
  printf("%f\n", dist(point1, point2));
  */
  /*
  glBegin(GL_POINTS);
    glVertex2fv(point);
  glEnd();
  */
  for (i = 0; i < number; i++)
    drawpoint(point[i]);
  distances();
  glutSwapBuffers();
  glFlush();
  /*
  point1[0] = norm(point1[0] + .002);
  point1[1] = norm(point1[1] + .005);
  */
}//display

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case ' ':
      display();
      break;
    case 0x1b:
      cleanup();
      exit(0);
    default:
      printf("%x\n", key);
  }//switch
}//keyboard

void distances(void) {
  int i,
      j;

  for (i = 0; i < number; i++)
    for (j = i + 1; j < number; j++)
      printf("%i %i: %f %f\n", i, j, readdist(i, j), dist(point[i], point[j]));
}//distances

int main(int argc, char* argv[]) {

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_X_SIZE, WINDOW_Y_SIZE);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Visualization.");
  glPointSize(10.0);

  initrand(); // Initialize the random generator.
  readfile(); // Read the input.
  printinfo();
  printf("\n");
  printmatrix();
  printf("\n");
  distances();



  glutDisplayFunc(display);
  //glutIdleFunc(display);
  glutKeyboardFunc(keyboard);

  glutMainLoop();

  cleanup(); // Free the allocated memory.

  return 0;
}//main
