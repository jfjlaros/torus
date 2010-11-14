void errorgraphl(void) {
  const char *errout = "/tmp/out/outl.pgm";
  FILE *out = fopen(errout, "w");
  float temp[2],
        colour;
  int i,
      j,
      k;

  printf("\nPlotting local error field to file %s... ", errout);
  fflush(stdout);
  fprintf(out, "P2\n# CREATOR: Torus Version 0.0\n%i %i\n255\n", WINDOW_X_SIZE, 
                                                                 WINDOW_Y_SIZE);
  for (i = 0; i < number; i++) { // Calculate the mean error of point[i].
    error[i] = 0.0;
    for (j = 0; j < number; j++)
      error[i] += pow(fabs(dist(point[i], point[j]) - (scale * readdist(i, j))), error_dropoff);
    error[i] /= number;
    //printf("%i %f\n", i, error[i]);
  }//for
  
  for (i = 0; i < WINDOW_X_SIZE; i++)
    for (j = 0; j < WINDOW_Y_SIZE; j++) {
      colour = 0.0;
      temp[0] = (float)j / WINDOW_Y_SIZE;
      temp[1] = 1 - (float)i / WINDOW_X_SIZE;
      for (k = 0 ; k < number; k++) {
        //printf("%f\n", dist(temp, point[k]));
        //colour += ((20 * 255) * error[k]) * pow(2.718281828, 
        //                                  -pow(100 * dist(temp, point[k]), 2));
        colour += (2000 * error[k]) / (pow(200 * dist(temp, point[k]), field_dropoff) + 1);
      }
      fprintf(out, "%i\n", 255 - (int)(colour / number));  
    }//for
        

  fclose(out);
  printf("done.\n");
}//errorgraphl
