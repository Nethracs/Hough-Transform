
#ifndef __SOBELHELPERS_H__
#define __SOBELHELPERS_H__

#include<SImage.h>
#include<float.h>

// Sobel opeartion from Gx and Gy
// Gx - output of SobelX and image, Gy - output of SobelY and image
SDoublePlane sobel(SDoublePlane& Gx,SDoublePlane& Gy)
{
	SDoublePlane edge_image(Gx.rows(),Gx.cols());
	
	for(int i = 0;i<Gx.rows();++i)
	{
		for(int j = 0;j<Gx.cols();++j)
		{
			edge_image[i][j] = sqrt((Gx[i][j]*Gx[i][j]) + (Gy[i][j]*Gy[i][j]));
			if(edge_image[i][j] > 100)
				edge_image[i][j] = 255;
			else
				edge_image[i][j] = 0;
		}
	}
	return edge_image;
}

/*
	This function finds the distance using Distance tarnsform
*/ 
SDoublePlane edgeDistance(SDoublePlane& input_image,SDoublePlane& templ)
{
	// Edge image converted to 1 and 0.
	SDoublePlane I(input_image.rows(),input_image.cols());
	for(int i = 0;i<input_image.rows();++i)
	{
		for(int j =0;j<input_image.cols();++j)
		{
			if(input_image[i][j] != 0)
				I[i][j] = 1;
	
		}
	}
	
	//Edge template converted to 1 and 0
	SDoublePlane T(templ.rows(),templ.cols());
	for(int i = 0;i<templ.rows();++i)
	{
		for(int j =0;j<templ.cols();++j)
		{
			if(templ[i][j] != 0)
				T[i][j] = 1;
	
		}
	}
	
	// L-Infinity distance transform
	// Acknowledge : http://www.cs.cornell.edu/courses/cs664/2008sp/handouts/cs664-7-dtrans.pdf - Slides 12 - 14
	SDoublePlane D(input_image.rows(),input_image.cols());
	SDoublePlane dis_filter1(2, 2);
    SDoublePlane dis_filter2(2, 2);
    dis_filter1[0][0] = dis_filter1[0][1] = dis_filter1[1][0] = 1;
    dis_filter1[1][1] = 0;
    dis_filter2[0][1] = dis_filter2[1][0] = dis_filter2[1][1] = 1;
    dis_filter2[0][0] = 0;
	
	
	for(int i = 0;i<input_image.rows();++i)
	{
		for(int j =0;j<input_image.cols();++j)
		{
			if(I[i][j] == 1)
				D[i][j] = 0;
			else
				D[i][j] = DBL_MAX;
		}
	}
	

	for(int i = 0; i< input_image.rows();++i)
	{
		for(int j = 0;j<input_image.cols();++j)
		{
			D[i][j] = min(D[i][j],min(D[i-1<0?0:i-1][j]+1,min(D[i][j-1<0?0:j-1]+1,D[i-1<0?0:i-1][j-1<0?0:j-1]+1)));
		}
	}
	
	
	int r = input_image.rows();
	int c = input_image.cols();
	for(int i = r-1; i>=0;i--)
	{
		for(int j = c-1;j>=0;j--)
		{
			D[i][j] = min(D[i][j],min(D[i+1==r?r-1:i+1][j]+1,min(D[i][j+1==c?c-1:j+1]+1,D[i+1==r?r-1:i+1][j+1==c?c-1:j+1]+1)));
		}
	}
	
	
	//Function to find the distances of template from image
	SDoublePlane output(input_image.rows(),input_image.cols());
	for(int i = 0;i<I.rows() - T.rows();++i)
	{
		for(int j =0;j<I.cols()-T.cols();++j)
		{
			for(int k = 0;k<T.rows();++k)
			{
				for(int l =0;l<T.cols();++l)
				{
					output[i][j] += T[k][l] * D[i+k][j+l];
				}
			}
		}
	}
	
	//Padding for boundary cases
	for(int i = 0;i<I.rows();++i)
	{
		for(int j =I.cols()-T.cols();j<I.cols();++j)
		{
			output[i][j] = 1000;
		}
	}
	for(int i = I.rows() - T.rows();i<I.rows();++i)
	{
		for(int j =0;j<I.cols();++j)
		{
			output[i][j] = 1000;
		}
	}
	
	return output;
}

// Once the note is detected , it is replaced by higher vales so that it wont be detected multiple times
void blockDetectedNote(SDoublePlane& input_image,int fromx,int fromy,int width,int height)
{

	for(int i = fromx;i<fromx+height;++i)
	{
		for(int j = fromy-4;j<fromy+width-4;++j)
		{
			if(j >= 0)
				input_image[i][j] = 1000000;
		}
	}
}

/*
Image rescaling 
*/
SDoublePlane scale(SDoublePlane& input_image,double scale)
{
	SDoublePlane outputImage((int)input_image.rows()*scale,(int)input_image.cols()*scale);
	
	double reScale = 1.0/scale;
	for(int i = 0;i <outputImage.rows();++i)
	{
		for(int j = 0;j<outputImage.cols();++j)
		{
			double value1 = input_image[(int)(ceil(i*reScale))][(int)(ceil(j*reScale))];
			double value2 = input_image[(int)(ceil(i*reScale))][(int)(floor(j*reScale))];
			double value3 = input_image[(int)(floor(i*reScale))][(int)(ceil(j*reScale))];
			double value4 = input_image[(int)(floor(i*reScale))][(int)(floor(j*reScale))];
			outputImage[i][j] = ((value1+value2+value3+value4)/4);
		}
	}

	return outputImage;	
}


#endif