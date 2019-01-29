#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct bmpFileHeader{
    unsigned long arraySize;
    unsigned char *imageArray;   // only colour pixels
    unsigned short signature; // bmp,png or ...
    unsigned int fileSize;
    unsigned int reserved1; // do not needed
    unsigned int offset; // which tells at which byte the image starts
    unsigned int sizeofDibHeader; // size of dibheader
    unsigned int width; // width of image
    unsigned int height; // height of image
    unsigned short ncp; // number of color planes
    unsigned short npp; // number per pixel
    unsigned int *restDibHeader; // rest part of header 
} FHeader;

typedef struct pixel{
    unsigned char *red;
    unsigned char *green;    
    unsigned char *blue;
    unsigned char *alpha;
}PIXEL;

void printImageArrayWithStrip(FHeader image,unsigned int newWidth); // PRINTING STRIPPED (NEW)ARRAY FOR MORE DETAILS ABOUT IMAGE
FHeader readFileHeader(FILE* userGivenName); // reading all the contents of file to a STRUCTURE
unsigned char *imageStrip(FHeader image,unsigned int newWidth,unsigned char *arrayTarget); // CUTTING SOME PART OF PIXEL ARRAY  AND STORING IT IN NEW PIXEL ARRAY
void printContent(FHeader image,char* fileName); // PRINTING CONTENT OF AN ARRAY
void printImageArray(unsigned int *array,unsigned int size); // PRINTING ONLY CONTENTS OF AN ARRAY
void writeToFile(char *fileName,FHeader image,unsigned int newWidth,unsigned char *arrayStrip); // WRITING ALL THE IMPORTANT DATA(WITHOUT PIXEL DATA) + NEW PIXEL ARRAY TO A NEW FILE


float* createPSF(float *arrayPSF,float max,int size);
float maxValue(float *arrayPSF,int size);
void createBMP(char* fileName,FHeader image,unsigned int newWidth,unsigned char *arrayTarget);
unsigned char* createElongatedArray(FHeader image,unsigned int newWidth,float *arrayPSF);
void merge_images(int n_images, unsigned int w, unsigned int h, FHeader *image_arrays, char *filename);
