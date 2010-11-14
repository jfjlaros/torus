// Defines.
#define NAME_LENGTH 30000

// Exported global variables.
extern int number;     // The number of entities.
extern char **name;    // The names of the entities.
extern float **matrix, // The distance matrix.
             **point,  // The points to be displayed.
             *error;   // The error of each point.

// Public prototypes.
void initrand(void),    // Initialize the random genenerator on the time.
     initpoints(void),  // Initialize the points randomly around the center.
     readfile(void),    // Read the input and initialize the global variables.
     cleanup(void),     // Free the allocated memory.
     printinfo(void),   // Print debugging information.
     printmatrix(void); // Print the full distance matrix.
float readdist(int, int); // Give the distance between two entities.
