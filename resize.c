// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames and variables
    int n = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // check that n is between 1 and 100 inclusive
    if (n > 100 || n < 0)
    {
        fprintf(stderr, "Incompatible resize value\n");
        return 2;
    }

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }

    // infile padding calculation
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // update image height and width in bitmapinfoheader
    bi.biWidth *= n;
    bi.biHeight *= n;

    // determine padding for scanlines
    int newPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // update image size in both headers
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + newPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight / n); i < biHeight; i++)
    {
        // iterate over rows n - 1 times
        for (int j = 0; j < (n - 1); j++)
        {
            // iterate over pixels in scanline
            for (int k = 0, biWidth = bi.biWidth / n; k < biWidth; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                for (int l = 0; l < n; l++)
                {
                    // write RGB triple to outfile n times
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // add newPadding
            for (int m = 0; m < newPadding; m++)
            {
                fputc(0x00, outptr);
            }

            // return infile cursor back to start of line
            fseek(inptr, -(bi.biWidth / n) * sizeof(RGBTRIPLE), SEEK_CUR);
        }

        // iterate over pixels in scanline
        for (int k = 0, biWidth = bi.biWidth / n; k < biWidth; k++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            for (int l = 0; l < n; l++)
            {
                // write RGB triple to outfile n times
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
        }

        // add newPadding
        for (int m = 0; m < newPadding; m++)
        {
            fputc(0x00, outptr);
        }

        // skip over original padding, if any
        fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}