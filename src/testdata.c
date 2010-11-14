#include <stdio.h>
#include <math.h>

#define PI 3.141592653589793

// x in [0.0, 0.5] actual distance.
// y in [0.0, 0.5] desired distance.
//
// returns [0.0, 0.5] correction..
//
float dist(float x, float y) {
  if (y == 0.25)
    return cos(PI * (x * 2)) * fabs(x - y);
  return cos(PI * (log(((pow((1 / (y * 2.0)) - 1, 2) - 1) * 
                       ((x * 2))) + 1) /
                   (log(pow((1 / (y * 2.0)) - 1, 2))))) * fabs(x - y);

}//dist

int main(void) {
  float x;

  for (x = 0.0; x < 0.5; x += 0.001)
    printf("%f %f\n", x, dist(x, 0.35));
  return 0;
}//main
