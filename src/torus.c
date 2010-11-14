#include <stdio.h>  // fopen(), fclose(), fcsanf(), fprintf(), ...
#include <stdlib.h> // srand(), rand(), RAND_MAX
#include <math.h>   // pow(), sqrt()
#include <GL/glut.h>

#define WINDOW_X_SIZE 500
#define WINDOW_Y_SIZE 500

float point1[2] = {0, 0},
      point2[2] = {0.1, 0.1};

// Draw a normalized point in the opengl window.
//
// Arguments: float x : The x-coordintate of the point (0 <= x < 1).
//                  y : The y-coordintate of the point (0 <= y < 1).
//
void drawpoint(float x, float y) {
  float t[2] = {(x * 2) - 1, (y * 2) - 1};

  glBegin(GL_POINTS);
    glVertex2fv(t);
  glEnd();
}//drawpoint

// Normalize an number between 0 and 1 by throwing out the integer part.
//
// Arguments: float f : A positive number.
//
//
float norm(float f) {
  return f - floor(f);
}//norm

float min(float a, float b) {
  if (a < b)
    return a;
  return b;
}//min

float dist(float a[2], float b[2]) {
  float d0 = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2)), // normal
        d1 = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - (b[1] + 1), 2)),
        d2 = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - (b[1] - 1), 2)),

        d3 = sqrt(pow(a[0] - (b[0] + 1), 2) + pow(a[1] - b[1], 2)),
        d4 = sqrt(pow(a[0] - (b[0] + 1), 2) + pow(a[1] - (b[1] + 1), 2)),
        d5 = sqrt(pow(a[0] - (b[0] + 1), 2) + pow(a[1] - (b[1] - 1), 2)),

        d6 = sqrt(pow(a[0] - (b[0] - 1), 2) + pow(a[1] - b[1], 2)),
        d7 = sqrt(pow(a[0] - (b[0] - 1), 2) + pow(a[1] - (b[1] + 1), 2)),
        d8 = sqrt(pow(a[0] - (b[0] - 1), 2) + pow(a[1] - (b[1] - 1), 2));

  return min(min(d0, min(d1, min(d2, min(d3, d4)))),
             min(d5, min(d6, min(d7, d8))));
}//dist

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  drawpoint(point1[0], point1[1]);
  drawpoint(point2[0], point2[1]);
  printf("%f\n", dist(point1, point2));
  /*
  glBegin(GL_POINTS);
    glVertex2fv(point);
  glEnd();
  */
  glutSwapBuffers();
  glFlush();
  point1[0] = norm(point1[0] + .002);
  point1[1] = norm(point1[1] + .005);
}//display

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case ' ':
      display();
      break;
  }//switch
}//keyboard

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_X_SIZE, WINDOW_Y_SIZE);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Visualization.");


  //glPointSize(10.0);

  glutDisplayFunc(display);
  //glutIdleFunc(display);
  glutKeyboardFunc(keyboard);

  glutMainLoop();

  return 0;
}//main
