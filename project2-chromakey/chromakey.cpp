#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);

int main(int argc, char *argv[])
{
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }
  
  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  threshold = atof(argv[3]);

  // Call Method 1 Function
method1(inputImage, chromaMask, threshold);

  // Produce the output by calling replace()
replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[4]
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }	

  // Call Method 2 Function
  method2(inputImage, chromaMask);

  // Produce the output by calling replace()
  replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method1(unsigned char inImage[][SIZE][RGB], bool mask[][SIZE], double threshold)
{
  //For method1, I used the top row and the left-most column
  // to calculate the chromakey
  int redRow = 0; //red of RGB of first row 
  int redColumn = 0; //red of RGB of first column
  int greenRow = 0; //pattern follows froma bove
  int greenColumn = 0;
  int blueRow = 0;
  int blueColumn = 0;
  int avgRed = 0;
  int avgGreen = 0;
  int avgBlue = 0;
  int redInput = 0; //used to simplify distance formula
  int greenInput = 0;
  int blueInput = 0;
  double distance; // calculate distance

//This for loop loops through the entire image, calculates the
// red green and blue from top row, left most column
  for (int i = 0; i <= SIZE; i++ ) 
  {
    redRow += inImage[0][i][0];
    greenRow += inImage[0][i][1];
    blueRow += inImage[0][i][2];
    redColumn += inImage[i][0][0];
    greenColumn += inImage[i][0][1];
    blueColumn += inImage[i][0][2];
  }
  //Calculates the average of the row, column
  avgRed = ((redRow+redColumn)/(SIZE*2));
  avgGreen = ((greenRow+greenColumn)/(SIZE*2));
  avgBlue = ((blueRow+blueColumn)/(SIZE*2));

//Nested for loop used to loop through the entire image,
//compare our RGB to the given image RGB
  for (int i = 0; i <= SIZE; i++) 
  {
    for (int j = 0; j < SIZE; j++) 
    {
      redInput = inImage[i][j][0];
      greenInput = inImage[i][j][1];
      blueInput = inImage[i][j][2];
      distance = sqrt((pow(avgRed-redInput,2))+(pow(avgGreen-greenInput,2))+(pow(avgBlue-blueInput,2)));

//If the distance is less than the threshold, then it is part of the background
// else, it is part of the foreground. it = pixel.
      if (distance < threshold) {
        mask[i][j] = 0;
      } else if (distance > threshold) {
        mask[i][j] = 1;
      }
    }
  }



}

// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
  int redRow = 0;
  int redColumn = 0;
  int greenRow = 0;
  int greenColumn = 0;
  int blueRow = 0;
  int blueColumn = 0;
  int avgRed = 0;
  int avgGreen = 0;
  int avgBlue = 0;
  int redInput = 0;
  int greenInput = 0;
  int blueInput = 0;
  int chromaSumRed = 0; // new chroma sum variable
  int chromaSumGreen = 0; 
  int chromaSumBlue = 0;
  int avgChromaRed = 0;
  int avgChromaGreen = 0;
  int avgChromaBlue = 0;
  double distance;
  double sumDistance;
  double avgDistance;

//This is the same as method 1
  for (int i = 0; i <= SIZE; i++ ) 
  {
    redRow += inImage[0][i][0];
    greenRow += inImage[0][i][1];
    blueRow += inImage[0][i][2];
    redColumn += inImage[i][0][0];
    greenColumn += inImage[i][0][1];
    blueColumn += inImage[i][0][2];
  }
  avgRed = ((redRow+redColumn)/(SIZE*2));
  avgGreen = ((greenRow+greenColumn)/(SIZE*2));
  avgBlue = ((blueRow+blueColumn)/(SIZE*2));

//find the average chromakey
//Nested for loop loops through the enitre top left chunk
//We used the top left chunk as it is empty and the chromakey for
//every single image
for (int i = 0; i <= 60; i++) {
  for (int j = 0; j <= 60; j++) {
    chromaSumRed += inImage[i][j][0];
    chromaSumGreen += inImage[i][j][1];
    chromaSumBlue += inImage[i][j][2];
  }
}
//Divide by 3600 to get the average for every RGB in the chunk
avgChromaRed = chromaSumRed/3600;
avgChromaGreen = chromaSumGreen/3600;
avgChromaBlue = chromaSumBlue/3600;

//this nested for loop loops through the entire image and
//gets the red, blue, and green in the image
for (int i = 0; i <= SIZE; i++) {
  for (int j = 0; j <= SIZE; j++) {
    redInput = inImage[i][j][0];
    greenInput = inImage[i][j][1];
    blueInput = inImage[i][j][2];
    //this part here will become the new threshold
    sumDistance += sqrt((pow(avgChromaRed-redInput,2))+(pow(avgChromaGreen-greenInput,2))+(pow(avgChromaBlue-blueInput,2)));
  }
}
//this will get the avgDistance by dividing the threshold
//to the size^2
avgDistance = sumDistance/(SIZE*SIZE);

//this nested for loop is pretty similar to method1, except at the end,
//we compare distance to avgDistance as it is the new threshold
for (int i = 0; i <= SIZE; i++) {
  for (int j = 0; j <= SIZE; j++) {
    redInput = inImage[i][j][0];
    greenInput = inImage[i][j][1];
    blueInput = inImage[i][j][2];
    distance = sqrt((pow(avgRed-redInput,2))+(pow(avgGreen-greenInput,2))+(pow(avgBlue-blueInput,2)));

    if (distance < avgDistance) {
      mask[i][j] = 0;
    } else if (distance >= avgDistance) {
      mask[i][j] = 1;
    }
  }
}


}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
  // Create the output image using the mask to determine
  //  whether to use the pixel from the Input or Background image

  //3 dimensional array nested for loop to output the image
  //with the respective RGB value.
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      for (int k = 0; k < RGB; k++) {
      if(mask[i][j] == 1){
        outImage[i][j][k] = inImage[i][j][k]; 
      }
      else{
        outImage[i][j][k] = bgImage[i][j][k];
      }
   }
  }
  }
}
