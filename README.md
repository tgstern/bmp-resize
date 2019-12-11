# .BMP Resize

Program to resize bitmap images by Unix command line, written in C. Resizes image by multiplying pixels by
a user given scale (1-100) and saves the resulting .bmp image. Requires <cs50.h> header file.

Once compiled, excute with syntax:
```
./resize 10 smiley.bmp bigsmiley.bmp
```
Where the first argument is the scale, second is the file to resize, and third is the scaled file's destination.

Designed for HarvardX's CS50: Introduction to Computer Science Fall 2019
