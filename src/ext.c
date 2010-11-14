#include <stdlib.h>
#include <stdio.h>

#define C_M 2000
#define knn 10

int crims[C_M];
int weight[3] = {1, 4, 16};

int main(int argc, char **argv) {
  FILE *input = fopen(argv[1], "r");
  char *str = (char *)malloc(256 * sizeof(char)),
       *no = (char *)malloc(256 * sizeof(char)),
       *d = (char *)malloc(256 * sizeof(char));
  int i,
      j = 0,
      w = -1;

  for (i = 0; i < C_M; i++) 
    crims[i] = 0.0;
  
  while (!feof(input)) {
    //fscanf(input, "%[^\n]\n", str);
    fscanf(input, "%s", no);
    fscanf(input, "%s", d);
    if (!(j % ((knn + 1) * 4)))
      w++;
    if (j % (knn + 1)) {
      //printf("%i, %i, %0.10f\n", atoi(str), weight[w], 1/atof(d));
      crims[atoi(no)] += weight[w] /** 10000000 * (1 - atof(d))*/;
      //printf("%f\n", 1000000 * atof(d));
    }//if
    else
      fscanf(input, "%s", no);
    //  printf("%s\n", str);
    j++;
  }//while

  for (i = 0; i < C_M; i++) 
    printf("%i %i\n", i, crims[i]);

  free(no);
  free(str);
  fclose(input);

  return 0;
}//main
