#include "extractStrip.h"

FHeader readFileHeader(FILE* userGivenName) {
    FHeader image;
    fread(&image.signature,sizeof(short),1,userGivenName);
    if(image.signature !=0x4D42){
    	printf("Error, the file format is not in BMP.\n"); 
	exit(1);
    }
    fread(&image.fileSize,sizeof(int),1,userGivenName);
    fread(&image.reserved1,sizeof(int),1,userGivenName);
    fread(&image.offset,sizeof(int),1,userGivenName);
    fread(&image.sizeofDibHeader,sizeof(int),1,userGivenName);
    fread(&image.width,sizeof(int),1,userGivenName);
    fread(&image.height,sizeof(int),1,userGivenName);
    fread(&image.ncp,sizeof(short),1,userGivenName);
    fread(&image.npp,sizeof(short),1,userGivenName);

    // WARNING !!!!!!!
    image.height = image.height*(-1);

    image.restDibHeader = calloc(image.offset-30,sizeof(int)); // allocating 
    unsigned int k=0;
    unsigned int j=0;
    while(k<((image.offset-30)/4)){
        fread(&(image.restDibHeader[j]),sizeof(int),1,userGivenName);
        k++;
        j++;
    }

    // WRITING ONLY PIXELS TO THE ARRAY WITH SIZE image.width*image.height
    image.imageArray = calloc(image.width*image.height*(image.npp/8),sizeof(unsigned char));
/*    for(int i=0;i<image.height;i++){
        // printf("i:%d\n",i );
        for(int l=0;l<image.width*(image.npp/8);l++){
            fread(&(image.imageArray[i*image.width*(image.npp/8)+l]),sizeof(unsigned char),1,userGivenName);
            // printf("red-arraySizeofPixels[%d]:%u\n",i*image.width*(image.npp/8)+l,image.imageArray[i*image.width*(image.npp/8)+l] );
        }

    }*/

    //qisa zaman
    fread(image.imageArray,image.fileSize-image.sizeofDibHeader,1,userGivenName);

    // SIZE HESABLAMAGHA EHTIYAC YOXDUR ONSUZDA HEIGHT * WIDTH DIR!!!
    // size of array of pixels
    // image.arraySize = (i-1)*image.width+l;
    // printf("arraySizeofPixels:%lu\n",image.arraySize );
    // printf("i:%d\nj:%d\n",i,j );
    // printf("arrayImage[0]:%u\n",image.imageArray[0] );
    fclose(userGivenName); 
    return image; // is a table of different characters that we have iside of file
}

unsigned char *imageStrip(FHeader image,unsigned int newWidth,unsigned char *arrayTarget){
    unsigned char *pt = calloc((newWidth*image.height*(image.npp/8)),sizeof(unsigned char));
    unsigned int stripPoint = (image.width-newWidth)/2;
    for(int i=0,c=0;i<image.height;i++){
        for(int j=0;j<image.width*(image.npp/8);j++){
            if(j==stripPoint*(image.npp/8)){
                for(;j<(newWidth+stripPoint)*(image.npp/8);c++,j++){
                    pt[c] = arrayTarget[i*image.width*(image.npp/8)+j];
                }
            }
        }
    }
    return pt;
}

void printContent(FHeader image,char *fileName){
    printf("original file name:%s\n",fileName );
    printf("original fileSize:%u\n",image.fileSize );
    printf("reserved1:%u\n",image.reserved1 );
    printf("offset:%u\n",image.offset );
    printf("sizeofDibHeader:%u\n",image.sizeofDibHeader );
    printf("width:%u\n",image.width );
    printf("height:%u\n",image.height );
    printf("ncp:%u\n",image.ncp );
    printf("npp:%u\n",image.npp );
}

void printImageArray(unsigned int *array,unsigned int size){
    for(int i=0;i<size;i++) printf("%u-", array[i]);
}


void printImageArrayWithStrip(FHeader image,unsigned int newWidth){
    unsigned int a = (image.width-newWidth)/2*(image.npp/8);
    for(int i=0,j=0;i<image.width*image.height*image.npp/8;i++){
        if(i>=a+(2*a+newWidth)*j){
            printf("\n\n\t\tSTRIP PART\t\t\n\n");
            for(;i<(newWidth+a)+(2*a+newWidth)*j;i++){
                printf("%u-", image.imageArray[i]);
            }
            printf("\n\n");
            j++;
        }
    }
}

void writeToFile(char *fileName,FHeader image,unsigned int newWidth,unsigned char *arrayStrip){
    int padding =((4-(newWidth*(image.npp/8))%4)%4);
    FILE *fakeFile;
    fakeFile = fopen(fileName,"wb");
    unsigned int newFileSize = image.offset+(newWidth*(image.npp/8)+padding)*image.height;
    // printf("new file size:%d\n",newFileSize );
    fwrite(&image.signature,sizeof(short),1,fakeFile);
    fwrite(&newFileSize,sizeof(int),1,fakeFile);
    fwrite(&image.reserved1,sizeof(int),1,fakeFile);
    fwrite(&image.offset,sizeof(int),1,fakeFile);
    fwrite(&image.sizeofDibHeader,sizeof(int),1,fakeFile);
    fwrite(&newWidth,sizeof(int),1,fakeFile);
    // WARNING!!!
    image.height = image.height*(-1);
    fwrite(&image.height,sizeof(int),1,fakeFile);
    // WARNING!!!
    image.height = image.height*(-1);
    fwrite(&image.ncp,sizeof(short),1,fakeFile);
    fwrite(&image.npp,sizeof(short),1,fakeFile);
    int j=0; // counter1 
    int i=0; // counter2 
    while(i<((image.offset-30)/4)){
        fwrite(&(image.restDibHeader[j]),sizeof(int),1,fakeFile);
        i++;
        j++;
    }
/*    int ch=0;
    for(int i=0;i<image.height;i++){
        for(int j=0;j<newWidth*(image.npp/8);j++){
            fwrite(&(arrayStrip[i*newWidth*(image.npp/8)+j]),sizeof(unsigned char),1,fakeFile);
        }
        for(int c=0;c<padding;c++)
            fputc(ch,fakeFile); //fputc() is used to write a single character at a time to a given file.
    }*/

    fwrite(arrayStrip,image.fileSize-image.sizeofDibHeader,1,fakeFile);
}


float* createPSF(float *arrayPSF,float max,int size){
    for(int i=0;i<size;i++){
        arrayPSF[i]=(float)max/(float)arrayPSF[i];
    }
    return arrayPSF;
}

float maxValue(float *arrayPSF,int size){
    float max = 0;
    for(int i=0;i<size;i++)
        if(max < arrayPSF[i])
            max = (float)arrayPSF[i];
    return max;
}

void createBMP(char* fileName,FHeader image,unsigned int newWidth,unsigned char *arrayTarget){
    int padding =((4-(newWidth*(image.npp/8))%4)%4);
    FILE *fakeFile;
    fakeFile = fopen(fileName,"wb");
    unsigned int newFileSize = image.offset+(newWidth*(image.npp/8)+padding)*image.height;
    // printf("new file size:%d\n",newFileSize );
    fwrite(&image.signature,sizeof(short),1,fakeFile);
    fwrite(&newFileSize,sizeof(int),1,fakeFile);
    fwrite(&image.reserved1,sizeof(int),1,fakeFile);
    fwrite(&image.offset,sizeof(int),1,fakeFile);
    fwrite(&image.sizeofDibHeader,sizeof(int),1,fakeFile);
    fwrite(&newWidth,sizeof(int),1,fakeFile);
    // WARNING!!!
    image.height = image.height*(-1);
    fwrite(&image.height,sizeof(int),1,fakeFile);
    // WARNING!!!
    image.height = image.height*(-1);
    fwrite(&image.ncp,sizeof(short),1,fakeFile);
    fwrite(&image.npp,sizeof(short),1,fakeFile);
    int j=0; // counter1 
    int i=0; // counter2 
    while(i<((image.offset-30)/4)){
        fwrite(&(image.restDibHeader[j]),sizeof(int),1,fakeFile);
        i++;
        j++;
    }

/*    int ch=0;
    for(int i=0;i<image.height;i++){
        for(int j=0;j<newWidth*(image.npp/8);j++){
            fwrite(&(arrayTarget[i*newWidth*(image.npp/8)+j]),sizeof(unsigned char),1,fakeFile);
        }
        for(int c=0;c<padding;c++)
            fputc(ch,fakeFile); //fputc() is used to write a single character at a time to a given file.
    }*/    

    fwrite(arrayTarget,image.height*newWidth*image.npp/8-image.sizeofDibHeader,1,fakeFile);

}


/*unsigned char* createElongatedArray(FHeader image,unsigned int newWidth,float *arrayPSF){
    unsigned char *arrayTarget = calloc(image.height*newWidth*(image.npp/8),sizeof(unsigned char));
    int maxPSF = arrayPSF[0];
    int base = 0;
    int c2 = 1;
    int c1 = 0;
    int c1Temp = 0;
    for(int i=0;i<image.height;i++){
        for(int j=0,a=0;j<(newWidth*(image.npp/8));j++){
            base = i/200;
            int psfTemp = maxPSF;
            if(base<12){
                if( (newWidth- (int)(image.width*arrayPSF[base]))%2 == 0){
                    if(j<((int)((newWidth - (int)(image.width*arrayPSF[base]))/2*(image.npp/8))) || j>((int)(((int)((newWidth - (int)(image.width*arrayPSF[base]))/2)+(int)(image.width*arrayPSF[base]))*(image.npp/8)))){
                        continue;
                    }
                }
                else{
                    if(j<((int)((newWidth - (int)(image.width*arrayPSF[base]))/2*(image.npp/8))) || j>((int)(((int)((newWidth - (int)(image.width*arrayPSF[base]))/2)+(int)(image.width*arrayPSF[base]))*(image.npp/8))+1)){
                        continue;                    
                    }
                }
                for(int l=(image.npp/8);l>0;l--){
                    arrayTarget[i*newWidth*(image.npp/8)+j] = image.imageArray[i*image.width*(image.npp/8)+a];
                    // printf("arrayTarget[%d][%d] = image.imageArray[%d] = %u\n", i,j,i*image.width*(image.npp/8)+a,arrayTarget[i*newWidth*(image.npp/8)+j]);
                    j = j + 1;
                    a = a + 1;
                }
                for(int l=(image.npp/8);l>0;l--){
                    arrayTarget[i*newWidth*(image.npp/8)+j] = (unsigned int)((double)(arrayPSF[base]-1)*image.imageArray[i*image.width*(image.npp/8)+a-l]+(double)(2-arrayPSF[base])*image.imageArray[i*image.width*(image.npp/8)+a+(image.npp/8)-l]);
                    // printf("arrayTarget[%d][%d] = %f*image.imageArray[%d]+%f*image.imageArray[%d] = %u\n", i,j,(double)(arrayPSF[base]-1),i*image.width*(image.npp/8)+a-l,(double)(2-arrayPSF[base]),i*image.width*(image.npp/8)+a+(image.npp/8)-l,arrayTarget[i*newWidth*(image.npp/8)+j]);                    
                    j = j + 1;
                }
                j=j-1;
            }   
        }
    }
    return arrayTarget;
}*/

unsigned char* createElongatedArray(FHeader image,unsigned int newWidth,float *arrayPSF){
    unsigned char *arrayTarget = calloc(image.height*newWidth*(image.npp/8),sizeof(unsigned char));
    int base = 0;
    float maxPSF = arrayPSF[0];
    float psfTemp = arrayPSF[0];
    float c1 = 1;
    float c2 = 0;
    float r = maxPSF;
    for(int i=0;i<image.height;i++){
        for(int j=0,a=0;j<newWidth*image.npp/8;j++){
            base = i/200;
            maxPSF = arrayPSF[base];
            if(base<12){               
                if( (newWidth- (int)(image.width*arrayPSF[base]))%2 == 0){
                    if(j<((int)((newWidth - (int)(image.width*arrayPSF[base]))/2*(image.npp/8))) || j>((int)(((int)((newWidth - (int)(image.width*arrayPSF[base]))/2)+(int)(image.width*arrayPSF[base]))*(image.npp/8)))){
                        continue;
                    }
                }
                else{
                    if(j<((int)((newWidth - (int)(image.width*arrayPSF[base]))/2*(image.npp/8))) || j>((int)(((int)((newWidth - (int)(image.width*arrayPSF[base]))/2)+(int)(image.width*arrayPSF[base]))*(image.npp/8))+1)){
                        continue;                    
                    }
                }
                if(psfTemp > 1){
                    c1 = 1;
                    c2 = 0;
                    psfTemp = psfTemp - 1;
                    r = r - 1;
                }
                else{
                    c1 = psfTemp;
                    c2 = 1 - c1;
                    psfTemp = maxPSF - c2;
                    if((r-c1)==0){
                        r = maxPSF;
                        a = a+4;
                        r = r - c2;
                    }
                    else{
                        r = r - c1;
                    }
                }

/*                printf("c1:%f\t",c1 );
                printf("c2:%f\t",c2 );
                printf("r:%f\t",r );
                printf("psfTemp:%f\n",psfTemp ); */               
                for(int l=0;l<(image.npp/8);l++){
                    arrayTarget[i*newWidth*image.npp/8+j] = (int)((double)c1*(double)image.imageArray[i*image.width*image.npp/8+a+l]) + (int)((double)c2*(double)image.imageArray[i*image.width*image.npp/8+a+4+l]);
                    j = j+1;
                }
                j = j - 1;
            }   
        }
    }
    return arrayTarget;
}


void merge_images(int n_images, unsigned int w, unsigned int h, FHeader *image_arrays, char *filename){
    unsigned char *new = malloc(n_images * w * h * image_arrays[0].npp/8);

    for(int n=0; n<n_images; ++n){
        for(int j=0; j<h; ++j){
            for(int i=0; i<w*image_arrays[0].npp/8; ++i){
                new[j*n_images*w*image_arrays[0].npp/8 + i+n*w*image_arrays[0].npp/8] = image_arrays[n].imageArray[j*w*image_arrays[0].npp/8 + i];
            }
        }
    }

    createBMP(filename, image_arrays[0], n_images*w, new);
}