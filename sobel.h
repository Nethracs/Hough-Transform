SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
  SDoublePlane output(input.rows(), input.cols());
  SDoublePlane temp(input.rows(), input.cols());
  int k=row_filter.cols()/2;
  int l=col_filter.rows()/2;
  double sum=0.0;
  //Applying row_filter//
  for(int row=0;row<input.rows();row++)
  {
    for(int col=k;col<input.cols()-k;col++)
    {
      sum=0.0;
      for(int filrow=-k;filrow<=k;filrow++)
      {
        sum+=row_filter[filrow+k][0]*input[row][col+filrow];
      }
      temp[row][col]=sum;
    }
  }
  //Applying column filter
   for(int row=l;row<input.rows()-l;row++)
  {
    for(int col=0;col<input.cols();col++)
    {
      sum=0.0;
      for(int filcol=-l;filcol<=l;filcol++)
      {
        sum+=col_filter[filcol+l][0]*temp[row+filcol][col];
      }
       output[row][col]=sum;
     }
   }

  return output;
}
SDoublePlane sobel_gradient_filter(const SDoublePlane &input,bool _gx)
{
  SDoublePlane output(input.rows(), input.cols());
  double x;
  double y;
  double val;

  // Implement a sobel gradient estimation filter with 1-d filters
   SDoublePlane filx(3,1);
   SDoublePlane fily(3,1);
   if(_gx){
        filx[0][0] =1;
        filx[1][0] = 2;
        filx[2][0] = 1;
        fily[0][0] = -1;
        fily[1][0] = 0;
        fily[2][0] = 1;
    }else{
        filx[0][0] = -1;
        filx[2][0] = 0;
        filx[1][0] = 1;
        fily[0][0] = 1;
        fily[1][0] = 2;
        fily[2][0] = 1;
    }

    output = convolve_separable(input, filx, fily);
    return output;

  
}
SDoublePlane find_edges(const SDoublePlane &input, double thresh=100)
{
  // Implement an edge detector of your choice, e.g.
  // use your sobel gradient operator to compute the gradient magnitude and threshold
  SDoublePlane output(input.rows(), input.cols());
  output=sobel_gradient_filter(input,1);
  output=sobel_gradient_filter(output,0);
  for(int row=0;row<output.rows();row++)
  {
    for(int col=0;col<output.cols();col++)
    {
      output[row][col]=output[row][col]>thresh?255:0;
    }
  } 
  return output;
}
#endif
