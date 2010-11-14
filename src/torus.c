//
// C++-programma voor project TVA-HEMA  cluster.cc 
// Walter Kosters, UL Informatica
// 071-5277059   kosters@liacs.nl
//
// 27 juni 2001
//
// Gebruik afstandenmatrix uit uitdis.txt
// om inbedding in [0,1] x [0,1] te vinden
//
// invoerfile:   uitdis.txt  --- de afstanden
//


#include <stdio.h>  // fopen(), fclose(), fcsanf(), fprintf(), ...
#include <stdlib.h> // srand(), rand(), RAND_MAX
#include <GL/glut.h>
#include <sys/timeb.h>

#define MAXFIL 1000 // maximaal aantal filiaal/datum combinaties

#define WINDOW_X_SIZE 500
#define WINDOW_Y_SIZE 500

int Filiaalnummers[MAXFIL],
    nrofprods,
    shownames = 0,
    target = 1,
    cycle = 1,
    //cyclemax = 1000,       // aantal keer
    ondergrens = 0, // minimum aantal correlaties
    randomseed;  // startwaarde randomgenerator
char Filiaalinfo[MAXFIL][10],
     popName[MAXFIL][30];
double correl[MAXFIL][MAXFIL], // correlatiematrix
       totaal[MAXFIL],         // totaal aantal correlaties
       X[MAXFIL],              // huidige x-ccordinaten
       Y[MAXFIL],              // huidige y-coordinaten
       aantrekking = 8.70001, // aantrekkingskracht
       maxcorr = 0,     // maximale correlatie
       magnif = 1,
       shiftx = 1,
       shifty = 1;
//float test[2] = {0.0, 0.0};

void duwentrek(void);
  int i, 
      j;          // tellertjes
      //cycle;      // index cykel
  double aantr,// = (double)aantrekking / 1000, // aantrekkingskracht
         fout = 0.0,                         // gemaakte fout
         dist,                               // huidige afstand
         eta = 0.25,                         // trekkracht
         xi, 
         xj, 
         yi, 
         yj,                                 // coordinaten
         minx, 
         miny, 
         maxx, 
         maxy;                               // maxi/minimale coordinaten
  float point[2];

// Geef kwadraat van Euclidische afstand.
double afstand(double x1, double y1, double x2, double y2) {
  return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}//afstand

double min(double a, double b) {
  if (a < b)
    return a;
  return b; 
}//min

double min4(double a, double b, double c, double d) {
  return min(a, min(b, min(c, d)));
}//min4

double afstand2(double x1, double y1, double x2, double y2) {
  return min4(afstand(x1, y1, x2, y2), 
              afstand(x1 + 1.0, y1, x2, y2),
              afstand(x1 + 1.0, y1 + 1.0, x2, y2), 
              afstand(x1, y1 + 1.0, x2, y2));
}//afstand2

// Geef absolute waarde van x.
double abso(double x) {
  if (x < 0)
    return -x;
  return x;
}//fabs

// Zorg ervoor dat 0.0 <= coordinaat <= 1.0
double corrigeer(double coordinaat) {
  if (coordinaat > 1.0)
    return coordinaat - 2.0;
  if (coordinaat < -1.0)
    return coordinaat + 2.0;
  return coordinaat;
}//corrigeer

void DrawText(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b) {
  int lines;
  char* p;

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH), 
      0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glColor3f(r,g,b);
  glRasterPos2i(x, y);
  for(p = s, lines = 0; *p; p++) {
    if (*p == '\n') {
      lines++;
      glRasterPos2i(x, y-(lines*18));
    }//if
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *p);
  }//for
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}//DrawText

void redisplay(void) {
  int i;
  float point[2];
  char text[50];

  glClear(GL_COLOR_BUFFER_BIT);
  for (i = 1; i <= nrofprods; i++) {
    point[0] = (X[i] * 2 * magnif) - (shiftx * magnif);
    point[1] = (Y[i] * 2 * magnif) - (shifty * magnif);
    if (shownames) {
      //sprintf(text, "%s_%f", popName[i - 1], totaal[i]);
      sprintf(text, "%s", popName[i - 1]);
      DrawText((point[0] * 250) + 250, (point[1] * 250) + 250, 
               text, 1, 1, 1);
    }//if
    else {
      glBegin(GL_POINTS);
        glVertex2fv(point);
      glEnd();
    }//else
  }//for
  if (target) {
    glBegin(GL_LINES);
      glVertex2d(-0.025, -0.025);
      glVertex2d(-0.025, 0.025);
      glVertex2d(-0.025, 0.025);
      glVertex2d(0.025, 0.025);
      glVertex2d(0.025, 0.025);
      glVertex2d(0.025, -0.025);
      glVertex2d(0.025, -0.025);
      glVertex2d(-0.025, -0.025);
    glEnd();
  }//if

  glutSwapBuffers();
  glFlush();
}//redisplay

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'w':
      shifty += 0.02 / magnif;
      break;
    case 's':
      shifty -= 0.02 / magnif;
      break;
    case 'a':
      shiftx -= 0.02 / magnif;
      break;
    case 'd':
      shiftx += 0.02 / magnif;
      break;
    case 'z':
      magnif *= 1.1;
      break;
    case 'x':
      magnif /= 1.1;
      break;
    case 'r':
      magnif = 1;
      shiftx = 1;
      shifty = 1;
      for (i = 1; i <= nrofprods; i++) {
        X[i] = (double)rand() / (double)(RAND_MAX);
        Y[i] = (double)rand() / (double)(RAND_MAX);
      }//for
      break;
    case 'n':
      shownames = !shownames;
      break;
    case 't':
      target = !target;
      break;
    case 'o':
      aantrekking *= 0.99;
      printf("aantrekking: %lf\n", aantrekking);
      break;
    case 'p':
      aantrekking /= 0.99;
      printf("aantrekking: %lf\n", aantrekking);
      break;
    case '[':
      eta *= 0.99;
      printf("eta: %lf\n", eta);
      break;
    case ']':
      eta /= 0.99;
      printf("eta: %lf\n", eta);
      break;
    case ' ':
      cycle = !cycle;
      break;
    case 0x1b:
      exit(0);
    default:
      printf("%x\n", key);
  }//switch
  glutPostRedisplay();
}//keyboard

//void duwentrek(int nrofprods, double correl[MAXFIL][MAXFIL],
//               double totaal[MAXFIL], double maxcorr, int cyclemax,
//               nt randomseed, int ondergrens,
//               double aantrekking) {
void duwentrek() {
  char text[50];
/*
  int i, 
      j;          // tellertjes
      //cycle;      // index cykel
  double aantr = (double)aantrekking / 1000, // aantrekkingskracht
         fout = 0.0,                         // gemaakte fout
         dist,                               // huidige afstand
         eta = 0.25,                         // trekkracht
         xi, 
         xj, 
         yi, 
         yj,                                 // coordinaten
         minx, 
         miny, 
         maxx, 
         maxy;                               // maxi/minimale coordinaten
  float point[2];
*/

  //maxcorr = maxcorr / 2; // eventueel / 2 erbij
/*
  srand(randomseed);    // initialiseer randomgenerator
  for (i = 1; i <= nrofprods; i++) {
    X[i] = (double)rand() / (double)(RAND_MAX);
    Y[i] = (double)rand() / (double)(RAND_MAX);
  }//for
*/

  aantr = (double)aantrekking / 1000; // aantrekkingskracht
  //for (cycle = 1; cycle <= cyclemax; cycle++) {
  if (cycle) { // <= cyclemax) {
    // OpenGL zut.
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);
    glPointSize(1.0);
    // --
//    if (cycle % 50 == 0)
    //eta *= 0.99;
    fout = 0.0;
/*
    if (cycle % 100 == 0 || cycle == 1) {
      for (i = 1; i <= nrofprods; i++) // Druk alleen fout af.
        if (totaal[i] >= ondergrens)
          for (j = i + 1; j <= nrofprods; j++)
            if (totaal[j] >= ondergrens)
              fout += abso(afstand(X[i], Y[i], X[j], Y[j]) - 
                                    ((double)correl[i][j]) /
                                     (aantr * ((double)totaal[i] +
                                      totaal[j] + 1)));
      printf("Cykel %i : %f\n", cycle, fout);
    }//if
*/
    for (i = 1; i <= nrofprods; i++)
      if (totaal[i] >= 0)
        for (j = i+1; j <= nrofprods; j++) {
          if (totaal[j] >= 0) {
            dist = eta * (afstand2(X[i], Y[i], X[j], Y[j]) -
                                  ((double)correl[i][j]) /
                                   (aantr * ((double) totaal[i] + 
                                    totaal[j] + 1)));
            xi = X[i] - dist * (X[i] - X[j]);
            yi = Y[i] - dist * (Y[i] - Y[j]);
            xj = X[j] + dist * (X[i] - X[j]);
            yj = Y[j] + dist * (Y[i] - Y[j]);
            X[i] = corrigeer(xi);
            Y[i] = corrigeer(yi);
            X[j] = corrigeer(xj);
            Y[j] = corrigeer(yj);
          }//if
        }//for
/*
    if (cycle % 1600 == 0) {
      maxx = -1.0;
      minx = 2.0;
      maxy = -1.0;
      miny = 2.0;
      for (i = 1; i <= nrofprods; i++)
        if (totaal[i] > 0) {
          if (X[i] > maxx)
            maxx = X[i];
          if (X[i] < minx)
            minx = X[i];
          if (Y[i] > maxy)
            maxy = Y[i];
          if (Y[i] < miny)
            miny = Y[i];
        }//if
      for (i = 1; i <= nrofprods; i++)
        if (totaal[i] > 0) {
          X[i] = (X[i] - minx) / (maxx - minx);
          Y[i] = (Y[i] - miny) / (maxy - miny);
        }//if
    }//if
*/
    // OpenGL zut.
    for (i = 1; i <= nrofprods; i++) {
    //  point[0] = (X[i] * 2) - 1;
    //  point[1] = (Y[i] * 2) - 1;
      point[0] = (X[i] * 2 * magnif) - (shiftx * magnif);
      point[1] = (Y[i] * 2 * magnif) - (shifty * magnif);
      if (shownames) {
        //sprintf(text, "%s_%f", popName[i - 1], totaal[i]);
        sprintf(text, "%s", popName[i - 1]);
        DrawText((point[0] * 250) + 250, (point[1] * 250) + 250, 
                 text, 1, 1, 1);
      }//if
      else {
        glBegin(GL_POINTS);
          glVertex2fv(point);
//          glVertex2fv(test);
        glEnd();
      }//else
    }//for
    glutSwapBuffers();
    glFlush();
    // --
    //cycle++;
  }//if
  if (target) {
    glBegin(GL_LINES);
      glVertex2d(-0.025, -0.025);
      glVertex2d(-0.025, 0.025);
      glVertex2d(-0.025, 0.025);
      glVertex2d(0.025, 0.025);
      glVertex2d(0.025, 0.025);
      glVertex2d(0.025, -0.025);
      glVertex2d(0.025, -0.025);
      glVertex2d(-0.025, -0.025);
    glEnd();
  }//if
/*
  test[1] += 0.001;
  test[1] = corrigeer(test[1]);
  printf("%f\n", test[1]);
*/
  usleep(10000);
}//duwentrek

int main(int argc, char* argv[]) {
  struct timeb T;
  FILE *invoer;   // invoerfile
  int i,               // tellertje
      j,               // tellertje
      p,               // tellertje
      q;               // tellertje

  invoer = fopen("uitdis.txt", "r");

  // Open GL display.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_X_SIZE, WINDOW_Y_SIZE);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Visualization.");
  glutDisplayFunc(redisplay);
  glutKeyboardFunc(keyboard);
  // -- Open GL display.

  ftime(&T);
  randomseed = T.millitm;
  srand(randomseed);    // initialiseer randomgenerator

  fscanf(invoer, "%i", &nrofprods);
  for (i = 0; i < nrofprods; i++)
    fscanf(invoer, "%s\n", (char *)&popName[i]);
  for (i = 1; i <= nrofprods; i++) {
    X[i] = (double)rand() / (double)(RAND_MAX);
    Y[i] = (double)rand() / (double)(RAND_MAX);
  }//for

  for (p = 0; p < MAXFIL; p++) {
    totaal[p] = 0;
    for (q = 0; q < MAXFIL; q++)
      correl[p][q] = 0;
  }//for
  i = 1;

  while (!feof(invoer) && (i <= nrofprods)) {
    for (j = 1; j <= nrofprods; j++) {
      fscanf(invoer, "%lf", &correl[i][j]);
      correl[i][j] /= 10000.0;
      totaal[i] += correl[i][j];
    }//for
    Filiaalnummers[i] = i;
    i++;
  }//while

  printf("Number of sets of samples:   %i\n", nrofprods);

  fclose(invoer);

  for (p = 1; p <= nrofprods; p++)
    for (q = 1; q <= nrofprods; q++)
      if (correl[p][q] > maxcorr)
        maxcorr = correl[p][q];

  glutIdleFunc(duwentrek);

  //printf("End.\n");
  glutMainLoop();

  return 0;
}//main
