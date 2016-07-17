# Hough-Transform

Hough transform is a feature extraction technique used to isolate a particular image within an image.
This implementation of hough transform is used to find the staff lines in the music notes.

#Implementation

Hough transform takes an edge detected image as input and find the desired shape in an image data.

Edge detection is done with sobel filter and hough transform  is applied to the image.

#Steps to run and compile the program

1. make
2. ./hough train Deep (Train phase)
3. ./hough test Deep (Test Phase)

#Output

![alt tag](https://github.com/Nethracs/Hough-Transform/blob/master/staves.png)

