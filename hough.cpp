/***
Hough transform is a feature extraction technique to indetify desired shapes within an image
This implementation is to find the staff lines in music notes based few template images
To find the hough transform we get the binary image and detect the edges.
Hough transform takes the input image as edge detected image and return the resultant image with edges detected on them.
Edge detection is done with the help of sobel gradient filter

This project was done as part of B657- computer vision course work at Indiana University, Bloomington 
by Nethra C Sashikar, Suhas Gulur and Mingze Xu

**/




#include "SImage.h"
#include "SImageIO.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include "DrawText.h"

using namespace std;

// Convert to a binary image with value only 0 and 1
SDoublePlane binary_image(const SDoublePlane &input){
    SDoublePlane output(input.rows(), input.cols());
    for(int i = 0; i < input.rows(); i++){
        for(int j = 0; j < input.cols(); j++){
            if(input[i][j] <= 127){
                output[i][j] = 0;
            }else{
                output[i][j] = 1;
            }
        }
    }

    return output;
}


// Step 3: Convolve an image with a separable convolution kernel
SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
    SDoublePlane output(input.rows(), input.cols());
    SDoublePlane temp(input.rows(), input.cols());

    if(row_filter.cols() > 1){
        cout << "Invalid row filter size: row filter should be a n*1 vector" << endl;
        return output;
    }
    if(col_filter.rows() > 1){
        cout << "Invalid col filter size: col filter should be a 1*n vector" << endl;
        return output;
    }

    int frows = row_filter.rows();
    int fcols = col_filter.cols();

    if(frows < 1 || frows % 2 == 0 || fcols < 1 || fcols % 2 == 0){
        cout << "Invalid filter size: filter size should be a odd number larger than 0" << endl;
        return output;
    }

    int fr = (frows - 1) / 2;
    int fc = (fcols - 1) / 2;

    for(int i = 0; i < input.rows(); i++){
        for (int j = 0; j < input.cols(); j++){
            for(int u = -fr; u <= fr; u++){
                int idx = i - u;
                if(idx < 0)
                    temp[i][j] += row_filter[u+fr][0] * input[0][j];
                else if(idx >= input.rows())
                    temp[i][j] += row_filter[u+fr][0] * input[input.rows()-1][j];
                else
                    temp[i][j] += row_filter[u+fr][0] * input[idx][j];
            }
        }
    }

    for(int i = 0; i < input.rows(); i++){
        for (int j = 0; j < input.cols(); j++){
            for(int v = -fc; v <= fc; v++){
                int idy = j - v;
                if(idy < 0)
                    output[i][j] += col_filter[0][v+fc] * temp[i][0];
                else if(idy >= input.cols())
                    output[i][j] += col_filter[0][v+fc] * temp[i][input.cols()-1];
                else
                    output[i][j] += col_filter[0][v+fc] * temp[i][idy];
            }
        }
    }

    return output;
}




// Sobel gradient filter
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool _gx)
{
    SDoublePlane output(input.rows(), input.cols());
    SDoublePlane row_filter(3, 1);
    SDoublePlane col_filter(1, 3);

    if(_gx){
        row_filter[0][0] = 1;
        row_filter[1][0] = 2;
        row_filter[2][0] = 1;
        col_filter[0][0] = -1;
        col_filter[0][1] = 0;
        col_filter[0][2] = 1;
    }else{
        row_filter[0][0] = 1;
        row_filter[1][0] = 0;
        row_filter[2][0] = -1;
        col_filter[0][0] = 1;
        col_filter[0][1] = 2;
        col_filter[0][2] = 1;
    }

    output = convolve_separable(input, row_filter, col_filter);
    return output;
}

//Sobel edges detected
SDoublePlane find_edges(const SDoublePlane &input, double thresh=127)
{
    SDoublePlane output(input.rows(), input.cols());
    SDoublePlane show(input.rows(), input.cols());
    SDoublePlane gx = sobel_gradient_filter(input, true);
    SDoublePlane gy = sobel_gradient_filter(input, false);

    for(int i = 0; i < input.rows(); i++){
        for(int j = 0; j < input.cols(); j++){
            output[i][j] = sqrt((gx[i][j] * gx[i][j])+ (gy[i][j]*gy[i][j]));
        }
    }

    for(int i = 0; i < input.rows(); i++){
        for(int j = 0; j < input.cols(); j++){
            if(output[i][j] > thresh){
                output[i][j] = 1;
                show[i][j] = 255;
            }else{
                output[i][j] = 0;
                show[i][j] = 0;
            }
        }
    }
    SImageIO::write_png_file("edges.png", show, show, show);

    return output;
}


// Hough transform
int hough_transform(const SDoublePlane &input,const SDoublePlane &original_image)
{
    int irows = input.rows();
    int icols = input.cols();
    SDoublePlane output = original_image;
    SDoublePlane output1 = original_image;
    
    SDoublePlane voting_space(irows, irows);


    for(int i = 0; i < irows; i++){
        for(int j = 0; j < icols; j++){
            if(input[i][j] == 1){
                // For every line above this point
                for(int b = 1; b < i-1; b++){
                    // This point is in the second line of staff
                    if((i-b) <= (irows-i)/3)
                        voting_space[i-b][b] += 1;
                    // This point is in the third line of staff
                    if((i-b)/2 <= (irows-i)/2 && (i-b)%2 == 0)
                        voting_space[(i-b)/2][b] += 1;
                    // This point is in the fourth line of staff
                    if((i-b)/3 <= (irows-i) && (i-b)%3 == 0)
                        voting_space[(i-b)/3][b] += 1;
                    // This point is in the fifth line of staff
                    if((i-b)%4 == 0)
                        voting_space[(i-b)/4][b] += 1;
                }
                // This point is in the first line of staff
                for(int k = 2; k < (irows-i)/4; k++){
                    voting_space[k][i] += 1;
                }
            } 
        }
    }

    //Extracting the max - Inorder to find the first line
    double max = -1;
    int row = -1;
    for(int i = 0; i < irows; i++){
        for(int j = 0; j < irows; j++){
            if(voting_space[i][j] > max){
                max = voting_space[i][j];
                row = i;
            }
        }
    }

    //Extracting the linesfrom the max values
    vector<int> result;
    for(int j = 0; j < irows; j++){
        if(voting_space[row][j] > 0.8*max && voting_space[row][j] > voting_space[row][j+row]){
            result.push_back(j);
            result.push_back(j+row);
            result.push_back(j+2*row);
            result.push_back(j+3*row);
            result.push_back(j+4*row);
            j = j + 2*row;
        }
    }

    //Marking the B component in the grey scale to 1. To get the staves in Blue color
    for(vector<int>::iterator it = result.begin(); it != result.end(); ++it){
        for(int j = 0; j < icols; j++){
            output[*it][j] = 255;
            output1[*it][j] = 0;
        }
    }

    //R - G :Normal Image B: NormalImage + Blues staves
    SImageIO::write_png_file("staves.png", output1, output1, output);

    return row;
}

// Main function
int main(int argc, char *argv[])
{
    if(!(argc == 2)){
        cerr << "usage: " << argv[0] << " input_image" << endl;
        return 1;
    }


	// Read files
    string input_filename(argv[1]);
	//Read Image
    SDoublePlane input_image = SImageIO::read_png_file(input_filename.c_str());
	//Read template	
    SDoublePlane template_image1 = SImageIO::read_png_file("template1.png");
    SDoublePlane template_image2 = SImageIO::read_png_file("template2.png");
    SDoublePlane template_image3 = SImageIO::read_png_file("template3.png");
    
    SDoublePlane binary_input = binary_image(input_image);
    SDoublePlane edges_image = find_edges(binary_input, 0);


    //6. Distance between two lines.
    int space = hough_transform(edges_image,input_image);

   }
