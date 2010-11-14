#include <stdio.h>  // printf()
#include <stdlib.h> // exit()
#include <unistd.h> // sleep(), getopt()
#include <math.h>   // pow(), sqrt()
#include <GL/glut.h>
#include "input.h"
#include "output.h"

// Definitions.
#define PI 3.141592653589793

//#define test 1

// Global variables.
int WINDOW_X_SIZE = 500,
    WINDOW_Y_SIZE = 500,
    shownames = 0,
    calculate = 1,
    showerr = 1,
    variant = 0,
    mincorr = 0,
    autofollow = 0,
    disp = 0,
    S_C = 0,
    knn = 1,
    npairs;
float magnif = 1.0,
      minerr,
      maxerr,
      //oldmaxerr = 10.0,
      globalerror,
      oldmeanerr = 10.0,
      sd,
      oldsd = 10.0,
      dyn,
      field_dropoff = 2,
      error_dropoff = 0.5,
      scale = 1.0,
      sstep = 0.1,
      attr = 1.0,
      stop = 0.000002,
      s_t = 20,
      astep = 0.1;
char text[NAME_LENGTH];

void distances(void),
     nmdistances(void),
     pushpull(int, int),
     nmpushpull(int, int, int, int);
float norm(float);
float min(float, float);
void drawline(float [2], float [2]);
//float dist(float [2], float [2]);
void drawpoint(float [2]);
void display(void);
void neighdump(int);

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
// Returns: float : The minimum of a and b.
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

float correct(float x, float y) {
  if (y == 0.25)
    return cos(PI * (x * 2)) * fabs(y - x);
  return cos(PI * (log(((pow((1 / (y * 2.0)) - 1, 2) - 1) * ((x * 2))) + 1) /
                   (log(pow((1 / (y * 2.0)) - 1, 2))))) * fabs(y - x);
}//correct

float correct2(float x, float y) {
  return fabs(x - y);
}//correct

float square(float f) {
  return f * f;
}//square

void ploterr(float mu, float sigma) {
  float f,
       x[2] = {0.0, 0.0},
       y[2];


  glColor3f(1.0, 0.0, 0.0);
  for (f = 0.0; f < 0.5; f += 0.0001) {
    y[0] = f * 2.0;
    y[1] = pow(2.71828183, -(square(f - mu) / (2 * square(sigma)))) / 
           (sigma * 2.5066282731);
    y[1] /= 200;
    //drawpoint(x);
    drawline(x, y);
    x[0] = y[0];
    x[1] = y[1];
  }//for
  glColor3f(1.0, 1.0, 1.0);
}//ploterr

float dist(float x[2], float y[2]) {
  float v[2] = {y[0], y[1]};

  if (x[0] - y[0] > 0.5)
    v[0] += 1.0;
  if (x[0] - y[0] < -0.5)
    v[0] -= 1.0;
  if (x[1] - y[1] > 0.5)
    v[1] += 1.0;
  if (x[1] - y[1] < -0.5)
    v[1] -= 1.0;

  return square(x[0] - v[0]) + square(x[1] - v[1]);
}//dist

void pushpull(int x, int y) {
  float v[2] = {point[y][0], point[y][1]},
        correction,
        temp;

  if (point[x][0] - point[y][0] > 0.5)
    v[0] += 1.0;
  if (point[x][0] - point[y][0] < -0.5)
    v[0] -= 1.0;
  if (point[x][1] - point[y][1] > 0.5)
    v[1] += 1.0;
  if (point[x][1] - point[y][1] < -0.5)
    v[1] -= 1.0;

  correction = correct(square(point[x][0] - v[0]) + square(point[x][1] - v[1]),
                       scale * readdist(x, y)) / attr;
  temp = fabs((square(point[x][0] - v[0]) + square(point[x][1] - v[1])) -
                (scale * readdist(x, y)));
                /*
  printf("%f %f %f\n", square(point[x][0] - v[0]) + square(point[x][1] - v[1]),
                       scale * readdist(x, y),
                       temp / (scale * readdist(x, y)));
                       */
  globalerror += temp / scale;
  sd += square(temp / scale);
  if (temp > maxerr)
    maxerr = temp;
  if (temp < minerr)
    minerr = temp;

#ifdef test
  if (correction > 0) {
  correction = correct2(square(point[x][0] - v[0]) + square(point[x][1] - v[1]),
                       scale * readdist(x, y)) / attr;
#endif
  v[0] = correction * (v[0] - point[x][0]);
  v[1] = correction * (v[1] - point[x][1]);

  point[x][0] = norm(point[x][0] - v[0]);
  point[x][1] = norm(point[x][1] - v[1]);
  point[y][0] = norm(point[y][0] + v[0]);
  point[y][1] = norm(point[y][1] + v[1]);
#ifdef test
  }
#endif
}//pushpull

void nmpushpull(int x1, int y1, int x2, int y2) {
  float v1[2] = {point[y1][0], point[y1][1]},
        v2[2] = {point[y2][0], point[y2][1]},
        correction = 0.0;

  if (point[x1][0] - point[y1][0] > 0.5)
    v1[0] += 1.0;
  if (point[x1][0] - point[y1][0] < -0.5)
    v1[0] -= 1.0;
  if (point[x1][1] - point[y1][1] > 0.5)
    v1[1] += 1.0;
  if (point[x1][1] - point[y1][1] < -0.5)
    v1[1] -= 1.0;

  if (point[x2][0] - point[y2][0] > 0.5)
    v2[0] += 1.0;
  if (point[x2][0] - point[y2][0] < -0.5)
    v2[0] -= 1.0;
  if (point[x2][1] - point[y2][1] > 0.5)
    v2[1] += 1.0;
  if (point[x2][1] - point[y2][1] < -0.5)
    v2[1] -= 1.0;

  if ((square(point[x1][0] - v1[0]) + square(point[x1][1] - v1[1])) >
      (square(point[x2][0] - v2[0]) + square(point[x2][1] - v2[1]))) {
    if (readdist(x1, y1) < readdist(x2, y2))
      correction = -0.05 / attr;
  }//if
  else
    if (readdist(x1, y1) > readdist(x2, y2))
      correction = 0.05 / attr;

/*
  correction = correct(square(point[x][0] - v[0]) + square(point[x][1] - v[1]),
                       scale * readdist(x, y)) / attr;
  temp = fabs((square(point[x][0] - v[0]) + square(point[x][1] - v[1])) -
                (scale * readdist(x, y)));
  globalerror += temp / scale;
  sd += square(temp / scale);
  if (temp > maxerr)
    maxerr = temp;
  if (temp < minerr)
    minerr = temp;
    */

  v1[0] = correction * (v1[0] - point[x1][0]);
  v1[1] = correction * (v1[1] - point[x1][1]);
  v2[0] = -correction * (v2[0] - point[x2][0]);
  v2[1] = -correction * (v2[1] - point[x2][1]);

  point[x1][0] = norm(point[x1][0] - v1[0]);
  point[x1][1] = norm(point[x1][1] - v1[1]);
  point[y1][0] = norm(point[y1][0] + v1[0]);
  point[y1][1] = norm(point[y1][1] + v1[1]);

  point[x2][0] = norm(point[x2][0] - v2[0]);
  point[x2][1] = norm(point[x2][1] - v2[1]);
  point[y2][0] = norm(point[y2][0] + v2[0]);
  point[y2][1] = norm(point[y2][1] + v2[1]);
}//nmpushpull

void neighdump(int follow) {
  FILE *neigh;
  char *str = (char *)malloc(256 * sizeof(char));
  float *dnn = (float *)malloc(knn * sizeof(float)),
        v[2],
        d,
        dm;
  int *nn = (int *)malloc(knn * sizeof(int)),
      i,
      j,
      m = -1;


  sprintf(str, "/tmp/neigh.%i.txt", follow);
  neigh = fopen(str, "w");

  for (i = 0; i < knn; i++)
    dnn[i] = 100;

  for (i = 0; i < number; i++) {
    if (i != follow) {
      v[0] = point[i][0];
      v[1] = point[i][1];
      if (point[follow][0] - point[i][0] > 0.5)
        v[0] += 1.0;
      if (point[follow][0] - point[i][0] < -0.5)
        v[0] -= 1.0;
      if (point[follow][1] - point[i][1] > 0.5)
        v[1] += 1.0;
      if (point[follow][1] - point[i][1] < -0.5)
        v[1] -= 1.0;
      d = square(point[follow][0] - v[0]) + square(point[follow][1] - v[1]);

      dm = 0.0;
      for (j = 0; j < knn; j++) 
        if (dnn[j] > dm) {
          dm = dnn[j];
          m = j;
        }//if

      if (d < dnn[m]) {
        dnn[m] = d;
        nn[m] = i;
      }//if
    }//if
  }//for
  for (i = 0; i < knn; i++)
    fprintf(neigh, "%i %1.10f\n", nn[i], dnn[i]);

  free(nn);
  free(dnn);
  free(str);
  fclose(neigh);
}//neighdump

// Draw a normalized point in the opengl window.
//
// Arguments: float p[2] : The coordinates of the point (p[0] = x, p[1] = y)
//                         with (0 <= x, y < 1).
//
void drawpoint(float p[2]) {
  float t[2] = {((p[0] * 2.0) - 1.0) * magnif, 
                ((p[1] * 2.0) - 1.0) * magnif};

  glBegin(GL_POINTS);
    glVertex2fv(t);
  glEnd();
}//drawpoint

void dtext(float p[2], int i) {
  float t[2] = {(((p[0] * WINDOW_X_SIZE) - (WINDOW_X_SIZE / 2)) * magnif) + 
                 (WINDOW_X_SIZE / 2), 
                (((p[1] * WINDOW_Y_SIZE) - (WINDOW_Y_SIZE / 2)) * magnif) + 
                 (WINDOW_Y_SIZE / 2)};

  sprintf(text, "%s", name[i]);
  DrawText(t[0], t[1], text, 1, 1, 1);
  //drawpoint(p);
}//text

void display(void) {
  int i;

  glClear(GL_COLOR_BUFFER_BIT);
  for (i = 0; i < number; i++) {
    if (shownames)
      dtext(point[i], i);
    else
      drawpoint(point[i]);
  }//for
  if (calculate) {
    globalerror = 0.0;
    sd = 0.0;
    maxerr = 0.0;
    minerr = 10.0;
    if (variant)
      nmdistances();
    else
      distances();
    globalerror /= npairs;
    if (showerr) {
      dyn = fabs((oldmeanerr - globalerror) + (oldsd - sd));
      printf("max: %f, mean:%f, sd:%f, g:%1.10f\r", 
             maxerr, globalerror,
             sqrt((sd - (npairs * square(globalerror))) / (npairs - 1)), dyn);
      //oldmaxerr = maxerr;
      oldmeanerr = globalerror;
      oldsd = sd;
      if (dyn < stop) {
        S_C++;
        if ((S_C > s_t) && autofollow) {
          for (i = 0; i < number; i++)
            neighdump(i);
          cleanup();
          exit(0);
        }//if
      }//if
      else
        S_C = 0;
      fflush(stdout);
    }//if
    ploterr(globalerror, sqrt((sd - (npairs * square(globalerror))) / 
                              (npairs - 1)));
  }//if
  else
    usleep(50000);
  ploterr(globalerror, sqrt((sd - (npairs * square(globalerror))) / 
                            (npairs - 1)));
  glutSwapBuffers();
  glFlush();
  WINDOW_X_SIZE = glutGet(GLUT_WINDOW_WIDTH);
  WINDOW_Y_SIZE = glutGet(GLUT_WINDOW_HEIGHT);
}//display

void translate(float x, float y) {
  int i;

  for (i = 0; i < number; i++) {
    point[i][0] = norm(point[i][0] + x);
    point[i][1] = norm(point[i][1] + y);
  }//for
}//translate

void filedump(void) {
  FILE *out,
       *out2;
  int i;

  out = fopen("/tmp/points.out", "w");
  out2 = fopen("/tmp/points.lbl", "w");
  for (i = 0; i < number; i++) {
    fprintf(out, "%f, %f\n", point[i][0], point[i][1]);
    fprintf(out2, "set label \"%s\" at %f,%f font \"Symbol,3\"\n", name[i], 
            point[i][0], point[i][1]);
  }//for

  fclose(out2);
  fclose(out);
}//filedump

void errorgraphl(void) {
  const char *errout = "/tmp/outl.pgm";
  FILE *out = fopen(errout, "w");
  float temp[2],
        mintemp = 11.0,
        colour;
  int i,
      j,
      k;

  printf("\nPlotting local error field to file %s... ", errout);
  fflush(stdout);
  fprintf(out, "P2\n# CREATOR: Torus Version 0.0\n%i %i\n255\n", WINDOW_X_SIZE, 
                                                                 WINDOW_Y_SIZE);
  printf("\n");
  for (i = 0; i < number; i++) { // Calculate the mean error of point[i].
    error[i] = 0.0;
    for (j = 0; j < number; j++)
      //error[i] += pow(1 + fabs(dist(point[i], point[j]) - (scale * readdist(i, j))), error_dropoff);
      //error[i] += fabs(dist(point[i], point[j]) - (scale * readdist(i, j)));
      error[i] += fabs(dist(point[i], point[j]) - (scale * readdist(i, j))) /
                  pow(1 + (4 * dist(point[i], point[j])), field_dropoff);
    error[i] /= number;
    error[i] /= 2;
    //error[i] /= 5;
    if (error[i] < mintemp)
      mintemp = error[i];
    printf("%i %f\n", i, error[i]);
  }//for

  if (mincorr) {
    // minerr correction.
    for (i = 0; i < number; i++) {
      error[i] -= mintemp;
      printf("%i %f\n", i, error[i]);
    }
  }
  
  for (i = 0; i < WINDOW_X_SIZE; i++)
    for (j = 0; j < WINDOW_Y_SIZE; j++) {
      colour = 0.0;
      temp[0] = (float)j / WINDOW_Y_SIZE;
      temp[1] = 1 - (float)i / WINDOW_X_SIZE;
      for (k = 0 ; k < number; k++) {
        //printf("%f\n", dist(temp, point[k]));
        //colour += ((20 * 255) * error[k]) * pow(2.718281828, 
        //                                  -pow(100 * dist(temp, point[k]), 2));
        //colour += (2000 * error[k]) / (pow(200 * dist(temp, point[k]), field_dropoff) + 1);
        //colour += (255 * error[k]) / pow(1 + dist(temp, point[k]), 100);
        colour += (50000 * error[k]) / (pow(200 * dist(temp, point[k]), field_dropoff) + 1);
      }
      fprintf(out, "%i\n", 255 - (int)(colour / number));  
    }//for

  fclose(out);
  printf("done.\n");
}//errorgraphl

void errorgraph(void) {
  const char *errout = "/tmp/out.pgm";
  FILE *out = fopen(errout, "w");
  float temp[2],
        mintemp = 10.0,
        colour;
  int i,
      j,
      k;

  printf("\nPlotting global error field to file %s... ", errout);
  fflush(stdout);
  fprintf(out, "P2\n# CREATOR: Torus Version 0.0\n%i %i\n255\n", WINDOW_X_SIZE, 
                                                                 WINDOW_Y_SIZE);
  printf("\n");
  for (i = 0; i < number; i++) { // Calculate the mean error of point[i].
    error[i] = 0.0;
    for (j = 0; j < number; j++)
      error[i] += fabs(dist(point[i], point[j]) - (scale * readdist(i, j)));
    error[i] /= number;
    error[i] /= 2;
    //error[i] *= 10;
    if (error[i] < mintemp)
      mintemp = error[i];
    printf("%i %f\n", i, error[i]);
  }//for

  if (mincorr) {
    // minerr correction.
    for (i = 0; i < number; i++) {
      error[i] -= mintemp;
      printf("%i %f\n", i, error[i]);
    }
  }
  
  for (i = 0; i < WINDOW_X_SIZE; i++)
    for (j = 0; j < WINDOW_Y_SIZE; j++) {
      colour = 0.0;
      temp[0] = (float)j / WINDOW_Y_SIZE;
      temp[1] = 1 - (float)i / WINDOW_X_SIZE;
      for (k = 0 ; k < number; k++) {
        //printf("%f\n", dist(temp, point[k]));
        //colour += ((20 * 255) * error[k]) * pow(2.718281828, 
        //                                  -pow(100 * dist(temp, point[k]), 2));
        colour += (50000 * error[k]) / (pow(200 * dist(temp, point[k]), field_dropoff) + 1);
       // colour += (50 * 255 * error[k]) / pow(1 + dist(temp, point[k]), 100);
      }
      fprintf(out, "%i\n", 255 - (int)(colour / number));  
    }//for
        

  fclose(out);
  printf("done.\n");
}//errorgraph

void keyboard(unsigned char key, int x, int y) {
  int i;

  switch (key) {
    case ' ':
      calculate = !calculate;
      break;
    case 'e':
      errorgraph();
      break;
    case 'l':
      errorgraphl();
      break;
    case 'g':
      showerr = !showerr;
      break;
    case 's':
      translate(0.0, 0.01 / magnif);
      break;
    case 'w':
      translate(0.0, -0.01 / magnif);
      break;
    case 'd':
      translate(-0.01 / magnif, 0.0);
      break;
    case 'a':
      translate(0.01 / magnif, 0.0);
      break;
    case 'n':
      shownames = !shownames;
      break;
    case 'm':
      if (mincorr)
        printf("Turning off minimum correction for the error charts.\n");
      else
        printf("Turning on minimum correction for the error charts.\n");
      mincorr = !mincorr;
      break;
    case 'N':
      variant = !variant;
      break;
    case 'z':
      magnif *= 1.1;
      break;
    case 'x':
      magnif /= 1.1;
      if (magnif < 1.0)
        magnif = 1.0;
      break;
    case '[':
      attr += astep;
      printf("\npushpull divisor: %f\n", attr);
      break;
    case ']':
      attr -= astep;
      printf("\npushpull divisor: %f\n", attr);
      break;
    case 'r':
      initpoints();
      break;
    case 'f':
      filedump();
      printf("\nPoints dumped to file.\n");
      break;
    case 'F':
      for (i = 0; i < number; i++) 
        neighdump(i);
      printf("\nDumping neighbourhood of all points\n");
      break;
    case '=':
      scale += sstep;
      printf("\nscale: %f\n", scale);
      break;
    case '-':
      scale -= sstep;
      printf("\nscale: %f\n", scale);
      break;
    case ';':
      field_dropoff -= 0.1;
      printf("\nfield drop off: %f\n", field_dropoff);
      break;
    case '\'':
      field_dropoff += 0.1;
      printf("\nfield drop off: %f\n", field_dropoff);
      break;
    case '.':
      error_dropoff -= 0.1;
      printf("\nerror drop off: %f\n", error_dropoff);
      break;
    case '/':
      error_dropoff += 0.1;
      printf("\nerror drop off: %f\n", error_dropoff);
      break;
    case 0x1b:
    case 'q':
      cleanup();
      exit(0);
    default:
      printf("\nUnknown key: %x\n", key);
  }//switch
}//keyboard

void distances(void) {
  int i,
      j;

  for (i = 0; i < number; i++) 
    for (j = i + 1; j < number; j++)
      pushpull(i, j);
}//distances

void nmdistances(void) {
  int i,
      j,
      k,
      l;

  for (i = 0; i < number - 2; i++)
    for (j = i + 1; j < number; j++) {
      for (k = 0; k < number - 1; k++)
        for (l = k + 1; l < number; l++)
          if ((i != k) || (j != l))
            nmpushpull(i, j, k, l);
    }//for
}//nmdistances

int main(int argc, char* argv[]) {
  int i;

  while ((i = getopt(argc, argv, "a:hd:m:D:M:f:k:")) != -1)
    switch (i) {
      case 'a':
        autofollow = 1;
        stop = atof(optarg);
        break;
      case 'd':
        astep = atof(optarg);
        break;
      case 'm':
        sstep = atof(optarg);
        break;
      case 'D':
        attr = atof(optarg);
        break;
      case 'M':
        scale = atof(optarg);
        break;
      case 'k':
        knn = atoi(optarg);
        break;
      case 'h':
        printf("Usage: %s [-h] [-d divider_step_size] [-D divider]"
               " [-m factor_step_size]\n [-M factor] < <input>\n\n", argv[0]);
        printf("  -d <divider_step_size> set the push/pull step size.\n");
        printf("  -D <divider> set the initial push/pull divider.\n");
        printf("  -m <factor_step_size> set the multiplication factor step"
               " size\n");
        printf("  -M <factor> set the initial multiplication factor of the"
               " distances.\n");
        printf("  -h This help.\n\n");
        printf("Defaults are: divider_step_size %.1f, divider %.1f"
               " factor_step_size %.1f,\n factor %.1f\n\n", astep, attr, sstep, 
               scale);
        printf("When running the program, the following keystrokes are"
               " available:\n");
        printf("  SPACE - Start / resume calculations.\n");
        printf("  g     - Toggle global error display.\n");
        printf("  s     - Navigate down.\n");
        printf("  w     - Navigate up.\n");
        printf("  d     - Navigate left.\n");
        printf("  a     - Navigate right.\n");
        printf("  n     - Toggle names display.\n");
        printf("  z     - Zoom in.\n");
        printf("  x     - Zoom out.\n");
        printf("  [     - Increase the push/pull divisor (less aggressive).\n");
        printf("  ]     - Decrease the push/pull divisor (more aggressive).\n");
        printf("  r     - Restart.\n");
        printf("  f     - Dump data to /tmp/points.txt and /tmp/points.lbl\n");
        printf("  =     - Increase the multiplication factor of the" 
               " distances.\n");
        printf("  -     - Decrease the multiplication factor of the" 
               " distances.\n");
        printf("  '     - Increase field drop off constant.\n");
        printf("  ;     - Decrease field drop off constant.\n");
        printf("  .     - Increase error drop off constant.\n");
        printf("  /     - Decrease error drop off constant.\n");
        printf("  ESC   - Quit.\n\n");
        printf("  q     - Quit.\n\n");
        exit(0);
        break;
    }//switch

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_X_SIZE, WINDOW_Y_SIZE);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Visualization.");
  //glPointSize(10.0);

  initrand(); // Initialize the random generator.
  readfile(); // Read the input.
  npairs = (square(number) - number) / 2;
  //printinfo();
  //printf("\n");
  //printmatrix();
  //printf("\n");



  //glutDisplayFunc(display);
  glutIdleFunc(display);
  glutKeyboardFunc(keyboard);

  glutMainLoop();

  cleanup(); // Free the allocated memory.

  return 0;
}//main
