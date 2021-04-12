#include <queue>
#include <math.h>

#define GREEN 1
#define BLUE 0
unsigned char* RGBToHSV(char*, int, int);
void dilation(unsigned char*, int, int, int, int);
void erosion(unsigned char*, int width, int height, int, int);
int* make_mask(unsigned char* data, long width, long height);
int* judgeBlue(unsigned char r, unsigned char g, unsigned char b, int ss, int vv);
void OTSU(unsigned char* img, int r, int c);
int Otsu(unsigned char* img, int r, int c);
void dilation_for_float(float** data, int width, int height, int kernel_w, int kernel_h);
void erosion_for_float(float** data, int width, int height, int kernel_w, int kernel_h);
using namespace std;
extern int ImageColor;
extern int Avg_s, Avg_v;
