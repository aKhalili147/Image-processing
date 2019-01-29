#include "extractStrip.h"
#include <string.h>

int main(int argc,char* argv[]){
    
    
    // STEP 1
    
    // float arrayScalingFactor[12] = {43.6,47.462,55.1,61.726,63.04,69.489,75.133,77,78.133,82.75,84.466,85.55};
    float arrayScalingFactor[12] = {43.6,47.462,50.234,55.1,58.5,61.726,63.04,66.894,69.489,72.3,75.133,77};
    // float arrayScalingFactor[12] = {36.47,42.6,46.99,50.8,54.91,58.75,62.17,64.99,67.49,69.61,70.83,72};
    float maxPSF = maxValue(arrayScalingFactor,12);
    // float maxPSF = 110;
    // printf("maxValue: %f\n",maxPSF );

    float *arrayPSF = calloc(12,sizeof(float));
    arrayPSF = createPSF(arrayScalingFactor,maxPSF,12);
/*    for (int i = 0; i < 12; ++i)
        printf("arrayPSF[%d]:%f\n", i,arrayPSF[i]);*/

    if(argv[1][0] == '-' && argv[1][1] == 'r'){
        // STEP 2 and STEP3
        
        FILE* fSource;   // to read the file name which will be copied onto fTarget file
        char* fileName = malloc(sizeof(char));
        int i=0;

        for(;argv[2][i] != '.' && argv[2][i+1] != 'b';i++) fileName[i] = argv[2][i];
        
        if((fSource = fopen(argv[2],"rb")) == NULL)
            printf("Error, the file doesn't exist.\n");
        
        FHeader image = readFileHeader(fSource); //Attention!!! You should check that the pot folder and codes are in the same folder 
        // printContent(image,argv[1]);  // printing the content of file
        int bytesPerPixel = image.npp/8;  // NOT PIXELS ,"BYTES" PER PIXELS

        int newWidth = image.width*maxValue(arrayPSF,12);
        // printf("image New Width:%d\n",newWidth );


        // STEP4

        unsigned char *arrayTarget = calloc(newWidth*image.height*(image.npp/8),sizeof(unsigned char));
        arrayTarget = createElongatedArray(image,newWidth,arrayPSF);
        char *nameResized = malloc(strlen(argv[2]) + 8 + 1);  // "_resized" is 8 chars added to the nameResized
        // finding . from the end
        int dot_index = -1;
        for(int i=strlen(argv[2]); i>=0; --i){
            if(argv[2][i] == '.'){
                dot_index = i;
                break;
            }
        }
        for(int i=0; i<dot_index; ++i) nameResized[i] = argv[2][i];
        strcat(nameResized, "_resized.bmp");
        nameResized[strlen(argv[2]) + 8 + 1] = '\0';
        // printf("==== nameResized: %s\n", nameResized);
        createBMP(nameResized, image, newWidth, arrayTarget);

        image.width = newWidth;
        int strip = (int) 110;
        unsigned char *temp = calloc(strip*image.height*bytesPerPixel,sizeof(unsigned char));  // ALLOCATING STRIPPED ARRAY
        temp = imageStrip(image,strip,arrayTarget); // WRITING ONLY STRIPPED PIXELS TO AN ARRAY
        
        char *name = malloc(strlen(argv[2]) + 6 + 1);
        for(int i=0; i<dot_index; ++i) name[i] = argv[2][i];
        strcat(name, "_strip.bmp");
        // printf("==== name: %s\n", name);
        writeToFile(name, image, strip, temp); // WRITING IT TO NEW FILE
    }


// -m (MERGE)
    else if(argv[1][0] == '-' && argv[1][1] == 'm'){
        
        // printf("argc = %d\n", argc);
        // for(int i=0; i<argc; ++i){
        //     printf("argv[%d] = <%s>\n", i, argv[i]);
        // }

        FHeader *images = malloc((argc-1)*sizeof(FHeader));

        int n_images = argc - 2;
        for(int i=2; i<argc; ++i){
            images[i-2] = readFileHeader(fopen(argv[i], "rb"));
        }

        merge_images(n_images, 110, 2230, images, "res.bmp");
    }



/*    printf("Array Image!!!\n");

    for(int i=0;i<1;i++){
            for(int j=0;j<10;j++){
            printf("arrayImage[%d]:%u\n", i*image.width+j,image.imageArray[i*image.width+j]);
        }
    }*/
    
/*    printf("Array Target!!!\n");

    for(int i=0;i<image.height;i++){
        for(int j=0;j<newWidth;j++){
            printf("arrayTarget[%d]:%u\n", i*newWidth+j,arrayTarget[i*newWidth+j]);
        }
    }*/

    // artiqHisse(image,newWidth,arrayPSF);

/*    clock_t begin = clock();
    int strip = atoi(&(argv[1][1])); // take the only number part as string and convert it to decimal
    FILE* fSource;        // to read the 4th argument as a file name
    char* fileName = malloc(sizeof(char));
    int i=0;

    for(;argv[2][i] != '.' && argv[2][i+1] != 'b';i++) fileName[i] = argv[2][i];
    
    if((fSource = fopen(argv[2],"rb")) == NULL)
        printf("Error, the file doesn't exist.\n");
    
    FHeader image = readFileHeader(fSource); //Attention!!! You should check that the pot folder and codes are in the same folder 
    printContent(image,argv[2]);  // printing the content of file
    int bytesPerPixel = image.npp/8;  // NOT PIXELS ,"BYTES" PER PIXEL
    
    if(strip < 0){
        printf("Error, undefined width:\n");
        exit(1);
    }
    else if(strip > image.width){
        printf("Error, given width is greater than the original picture width\n");
        exit(1);
    }
    else{
        unsigned int *temp = calloc(strip*image.height,bytesPerPixel);  // ALLOCATING STRIPPED ARRAY
        temp = imageStrip(image,strip); // WRITING ONLY STRIPPED PIXELS TO AN ARRAY
        char* tempName = strcat(fileName,"_S"); // MAKING NAME OF FILE
        tempName = strcat(tempName,&(argv[1][1])); // MAKING NAME OF FILE
        tempName = strcat(tempName,&(argv[2][i]));
        writeToFile("strippedPicture.bmp",image,strip,temp); // WRITING IT TO NEW FILE
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("%lf\n",time_spent );
        //printStripImageArray(temp,strip*image.height-image.width);
        //printImageArrayWithStrip(image,strip);
        //printImageArray(image.imageArray,image.height*image.width);
    }*/
}