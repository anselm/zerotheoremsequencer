
#define CUBE_START 1
#define CUBE_MANCOM 96
#define CUBE_ERROR 1000
#define CUBE_WHITE 1488
#define CUBE_END   1600

// I use a factory like pattern so that I don't have to expose the cube class and all its properties to the entire universe
Group* NewCubeGroup();

