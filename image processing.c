#include <stdio.h>
#include "bitmap.h"

typedef uint8_t  BYTE;
typedef struct // structure for the RGB values of pixels
{
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;

int smooth(); // second part of the homework
int downscale(); // first part of the homework

int main(){

    downscale();
    smooth();

    return;
}
int downscale(){
    FILE *imageFile = fopen("itu.bmp", "rb");
    FILE *output = fopen("itu-downscaled.bmp","wb");

    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, imageFile); // reading the header files

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, imageFile);

    RGBTRIPLE triple,*triples;
    triples = (RGBTRIPLE *) malloc(bf.bfSize-54); // memory allocation for the RGB values of the pixels

    int oldwidth = bi.biWidth; // creating a weight and height variable to make coding easy
    int oldheight = bi.biHeight;

    bi.biWidth = bi.biWidth/2; // for downscaling operations setting the new sizes
    bi.biHeight = bi.biHeight/2;
    bf.bfSize = (bf.bfSize-54)/4 + 54;
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, output); //writing the new header informations to the output file
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, output);
    int newwidth = bi.biWidth;
    int newheight = bi.biHeight;

    int i;
    for (i =1;i<=oldheight*oldwidth;i++){ // reading the RGB values from the itu.bmp
        fread(&triple , sizeof(RGBTRIPLE),1,imageFile);
        triples[i] = triple; // triples have all the pixel information that we will need
    }

    fclose(imageFile);

    int first=1,second=2,third =oldwidth+1,fourth=oldwidth+2; // first 4 pixels we need for downscale operation

    for(i=1;i<=newheight*newwidth;i++){
        int red,green,blue;
        red = (triples[first].rgbtRed+triples[second].rgbtRed+ triples[third].rgbtRed + triples[fourth].rgbtRed)/4;
        green = (triples[first].rgbtGreen+triples[second].rgbtGreen+ triples[third].rgbtGreen + triples[fourth].rgbtGreen)/4;
        blue = (triples[first].rgbtBlue+triples[second].rgbtBlue+ triples[third].rgbtBlue + triples[fourth].rgbtBlue)/4;

        //above the red, green, blue variables have the new values of them

        RGBTRIPLE triple; // create a temporary structure triple and set the new components of the colors
        triple.rgbtRed=red;
        triple.rgbtGreen=green;
        triple.rgbtBlue=blue;

        fwrite(&triple, sizeof(RGBTRIPLE), 1, output); // writing the new pixel to the output image
        if(first%(oldwidth+1)==0){
            // this part is for after I do an operation with a row it skips the next line bcs I already add that pixels in calculating the new components
            // skips means it makes the first, second, third and fourth variables 722 higher
            first+=oldwidth+2;
            second+=oldwidth+2;
            third+=oldwidth+2;
            fourth+=oldwidth+2;
        }
        else{
            // if it is not the end of the line it adds two every time to go the next four pixel that I need
            first+=2;
            second+=2;
            third+=2;
            fourth+=2;
        }
    }

    free(triples);  // free the triples
    fclose(output);
}

int smooth(){

    FILE *imageFile = fopen("itu.bmp", "rb");
    FILE *output = fopen("itu-smoothed.bmp","wb");

    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, imageFile);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, imageFile);

    RGBTRIPLE *triples;
    triples = (RGBTRIPLE *) malloc(sizeof(RGBTRIPLE)*(bi.biWidth+2)*(bi.biHeight+2)); // memory allocation for the pixels and a padding for applying the kernel

    int oldwidth = bi.biWidth;
    int oldheight = bi.biHeight;
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, output);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, output);

    int b=721;
    for(int i = 0;i<oldheight*oldwidth+2404;i++){
        // this for loop for putting all the pixels into the triples
        if((i<721) || (i% 722 ==0) || (i == b) || ( i>=347283)){
            //this if statement checks the padding, if the position is in the corners it sets it to zero for all color components
            RGBTRIPLE triple;
            triple.rgbtBlue=0;
            triple.rgbtGreen=0;
            triple.rgbtRed=0;
            triples[i] = triple;
            if(i == b){
                b+=722;
            }
        }
        else{
            // if its not padding then it write the pixel exactly the same as itu.bmp
            RGBTRIPLE triple;
            fread(&triple,sizeof(RGBTRIPLE),1,imageFile);
            triples[i] = triple;
        }
    }
    fclose(imageFile);

    int first=0,sec=1,third=2,fourth=oldwidth+2,fifth=oldwidth+3,sixth=oldwidth+4,seventh=oldwidth*2+4,eighth=oldwidth*2+5,nineth=oldwidth*2+6;
    // this variables are basically works the same as in the downscale
    // these nine variables represents the first 9 pixels that i need for the output file
    for(int i =1;i<=(oldheight)*(oldwidth);i++){
        // in this part kernel applies
        RGBTRIPLE triple;
        triple.rgbtBlue = triples[first].rgbtBlue * 0.0625 + triples[sec].rgbtBlue * 0.125 + triples[third].rgbtBlue * 0.0625 + triples[fourth].rgbtBlue * 0.125 +
            triples[fifth].rgbtBlue *0.25 + triples[sixth].rgbtBlue * 0.125 + triples[seventh].rgbtBlue * 0.0625 +
                triples[eighth].rgbtBlue * 0.125 +triples[nineth].rgbtBlue* 0.0625;
        triple.rgbtGreen = triples[first].rgbtGreen * 0.0625 + triples[sec].rgbtGreen * 0.125 + triples[third].rgbtGreen * 0.0625 + triples[fourth].rgbtGreen * 0.125 +
            triples[fifth].rgbtGreen *0.25 + triples[sixth].rgbtGreen * 0.125 + triples[seventh].rgbtGreen * 0.0625 +
                triples[eighth].rgbtGreen * 0.125 +triples[nineth].rgbtGreen* 0.0625;
        triple.rgbtRed = triples[first].rgbtRed * 0.0625 + triples[sec].rgbtRed * 0.125 + triples[third].rgbtRed * 0.0625 + triples[fourth].rgbtRed * 0.125 +
            triples[fifth].rgbtRed *0.25 + triples[sixth].rgbtRed * 0.125 + triples[seventh].rgbtRed * 0.0625 +
                triples[eighth].rgbtRed * 0.125 +triples[nineth].rgbtRed* 0.0625;

        fwrite(&triple,sizeof(RGBTRIPLE),1,output);

        if((nineth+1)%722==0){
            // when end of a row it adds three to skip the next line
            // if we don't do this operation it will also calculate the last two component of a row and the first component of the new row
            // this causes a shift in the image
            first+=3;
            sec+=3;
            third+=3;
            fourth+=3;
            fifth+=3;
            sixth+=3;
            seventh+=3;
            eighth+=3;
            nineth+=3;
        }
        else{
            // if it is not in the at the end of the file it only adds one to the variables to shift one right
            first+=1;
            sec+=1;
            third+=1;
            fourth+=1;
            fifth+=1;
            sixth+=1;
            seventh+=1;
            eighth+=1;
            nineth+=1;
        }
    }
    free(triples);
    fclose(output);

}
