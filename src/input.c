#include <stdio.h>     // scanf(), printf(), ...
#include <stdlib.h>    // srand(), rand(), RAND_MAX
#include <sys/timeb.h> // ftime(), timeb

#include "input.h"

#define EPSILON 0.000000001
#define DIMENSION 2

// Global variables.
int number;     // The number of entities.
char **name;    // The names of the entities.
float **matrix, // The distance matrix.
      **point,  // The points to be displayed.
      *error,   // The error of each point.
      minel = 1.0,
      maxel = 0.0;

// Initialize the random genenerator on the time.
void initrand(void) {
  struct timeb T;

  ftime(&T);
  srand(T.millitm);
}//initrand

// Initialize the points randomly around the center.
void initpoints(void) {
  int i,
      j;

  for (i = 0; i < number; i++)
    for (j = 0; j < DIMENSION; j++)
      point[i][j] = 0.5 + ((float)rand() / RAND_MAX) / 5;
}//initpoints

// Read the number of entities, their names and distances from stdin and
// initialize the points to be displayed randomly.
//
// Affected global variables: int number     : The number of entities.
//                            char **name    : The names of the entities.
//                            float **matrix : The distance matrix.
//                                  **points : The points to be displayed.
//
void readfile(void) {
  int i,
      j;

  scanf("%i", &number); // Set the number of entities.

  name = (char **)malloc(number * sizeof(char *));     // Fill the name list.
  for (i = 0; i < number; i++) {
    name[i] = (char *)malloc(NAME_LENGTH * sizeof(char));
    scanf("%s\n", name[i]);
  }//for

  matrix = (float **)malloc(number * sizeof(float *)); // Fill the matrix.
  matrix[0] = NULL;
  for (i = 1; i < number; i++) {
    matrix[i] = (float *)malloc(i * sizeof(float));
    for (j = 0; j < i; j++) {
      scanf("%f", &matrix[i][j]);
      matrix[i][j] /= 2.0;
      matrix[i][j] *= matrix[i][j];                    // Square the element.
      if (matrix[i][j] == 0.0)
        matrix[i][j] = EPSILON;
      if (matrix[i][j] < minel)
        minel = matrix[i][j];
      if (matrix[i][j] > maxel)
        maxel = matrix[i][j];
    }//for
  }//for
  //printf("%f %f\n", minel, maxel);

  error = (float *)malloc(number * sizeof(float));     // Make the errors.
  point = (float **)malloc(number * sizeof(float *));  // Make the points.
  for (i = 0; i < number; i++)
    point[i] = (float *)malloc(DIMENSION * sizeof(float));
  initpoints();
}//readfile

// Free the allocated memory.
//
// Affected global variables: char **name    : The names list.
//                            float **matrix : The distance matrix.
//                                  **point  : The display points.
//
void cleanup(void) {
  int i;

  for (i = 0; i < number; i++) {
    free(point[i]);
    free(matrix[i]);
    free(name[i]);
  }//for
  free(point);
  free(error);
  free(matrix);
  free(name);
}//cleanup

// Give the distance between two entities.
//
// Arguments: int i : Entity one (0 <= i < number).
//                j : Entity two (0 <= j < number).
//
// Returns: int : The distance between i and j.
//
float readdist(int i, int j) {
  if (i == j)
    return 0.0;
  if (i < j)
    return matrix[j][i];
  return matrix[i][j];
}//readdist

// Print debugging information.
//
// Print the names,
// print the raw distance matrix,
// print the points.
//
void printinfo(void) {
  int i,
      j;

  for (i = 0; i < number; i++)
    printf("%s\n", name[i]);

  for (i = 1; i < number; i++) {
    for (j = 0; j < i; j++)
      printf("%f ", matrix[i][j]);
    printf("\n");
  }//for

  for (i = 0; i < number; i++) {
    for (j = 0; j < DIMENSION; j++)
      printf("%f ", point[i][j]);
    printf("\n");
  }//for
}//printinfo

// Print the full distance matrix.
//
void printmatrix(void) {
  int i, 
      j;

  for (i = 0; i < number; i++) {
    for (j = 0; j < number; j++)
      printf("%f ", readdist(i, j));
    printf("\n");
  }//for
}//printmatrix
