/*
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 *September 2016 Maria Gabriela Sandoval Esquivel
 */

//#include <unistd.h>
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h> 

void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

int x, y;

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

void read_png_file(char* file_name)
{
        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp((png_const_bytep)header, 0, 8))
                abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[read_png_file] png_create_read_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during init_io");

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during read_image");

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);

        fclose(fp);
}


void write_png_file(char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (y=0; y<height; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);
}


void process_file(void)
{
       
	int **imgmat;
	imgmat = new int*[height];
	for(int i=0; i<height;i++)
	{
		imgmat[i] = new int[width];
	}//end generar matriz imgmat
	
	int **imgmat1;
	imgmat1 = new int*[height];
	for(int i=0; i<height;i++)
	{
		imgmat1[i] = new int[width];
	}//end generar matriz imgmat1

	int xsobel[3][3]= {	{-1,0,1},
						{-2,0,2},
						{-1,0,1} };
	int ysobel[3][3]= {	{-1,-2,-1},
						{0,0,0},
						{1,2,1} };
	//recorrer vector de imagen
        for (y=1; y<(height-1); y++) {

                png_byte* row0 = row_pointers[y-1];
				png_byte* row = row_pointers[y];
				png_byte* row2 = row_pointers[y+1];
				
                for (x=1; x<(width-1); x++) {
					//los 9 vecinos del pixel a escala de grises
                        png_byte* ptr00 = &(row0[(x-1)*4]); ptr00[0]=(ptr00[0]+ptr00[1]+ptr00[2])/3;
						png_byte* ptr01 = &(row0[x*4]);		ptr01[0]=(ptr01[0]+ptr01[1]+ptr01[2])/3;
						png_byte* ptr02 = &(row0[(x+1)*4]);	ptr02[0]=(ptr02[0]+ptr02[1]+ptr02[2])/3;
						png_byte* ptr10 = &(row[(x-1)*4]);	ptr10[0]=(ptr10[0]+ptr10[1]+ptr10[2])/3;
						png_byte* ptr = &(row[x*4]);		ptr[0]=(ptr[0]+ptr[1]+ptr[2])/3;
						png_byte* ptr12 = &(row[(x+1)*4]);	ptr12[0]=(ptr12[0]+ptr12[1]+ptr12[2])/3;
						png_byte* ptr20 = &(row2[(x-1)*4]);	ptr20[0]=(ptr20[0]+ptr20[1]+ptr20[2])/3;
						png_byte* ptr21 = &(row2[x*4]);		ptr21[0]=(ptr21[0]+ptr21[1]+ptr21[2])/3;
						png_byte* ptr22 = &(row2[(x+1)*4]);	ptr22[0]=(ptr22[0]+ptr22[1]+ptr22[2])/3;
						
						int p[3][3]={ {ptr00[0],ptr01[0],ptr01[0]},{ptr10[0],ptr[0],ptr12[0]},{ptr20[0],ptr21[0],ptr22[0]}};

						//sobel mask
						int sumx=0; 
						int sumy=0;
						for(int i=0;i<3;i++){
							for(int j=0;j<3;j++){
							sumx = sumx + xsobel[i][j]*p[2-i][2-j];
							sumy = sumy + ysobel[i][j]*p[2-i][2-j];
							}
						}
						imgmat[y][x]= (abs(sumx)+ abs(sumy))/2;
						imgmat1[y][x] = 0;
						
						//binarizar (debería de ser Gauss)
						if(imgmat[y][x]<127)
							imgmat[y][x]=0;
						else
							imgmat[y][x]=1;

                }
        }//end for y_pointer
		

		//sumar renglones y columnas en máscaras de 5x5
		for(int i=2; i<height-2;i++){
			for(int j=2; j<width-2;j++){
				//int A=imgmat[i-2][j-2]+imgmat[i-1][j-2]+imgmat[i][j-2]+imgmat[i+1][j-2]+imgmat[i+2][j-2];
				//int B=imgmat[i-2][j-1]+imgmat[i-1][j-1]+imgmat[i][j-1]+imgmat[i+1][j-1]+imgmat[i+2][j-1];
				int C=imgmat[i-2][j]+imgmat[i-1][j]+imgmat[i][j]+imgmat[i+1][j]+imgmat[i+2][j];
				//int D=imgmat[i-2][j+1]+imgmat[i-1][j+1]+imgmat[i][j+1]+imgmat[i+1][j+1]+imgmat[i+2][j+1];
				//int E=imgmat[i-2][j+2]+imgmat[i-1][j+2]+imgmat[i][j+2]+imgmat[i+1][j+2]+imgmat[i+2][j+2];

				//int F=imgmat[i-2][j-2]+imgmat[i-2][j-1]+imgmat[i-2][j]+imgmat[i-2][j+1]+imgmat[i-2][j+2];
				//int G=imgmat[i-1][j-2]+imgmat[i-1][j-1]+imgmat[i-1][j]+imgmat[i-1][j+1]+imgmat[i-1][j+2];
				int H=imgmat[i][j-2]+imgmat[i][j-1]+imgmat[i][j]+imgmat[i][j+1]+imgmat[i][j+2];
				//int I=imgmat[i-1][j-2]+imgmat[i+1][j-1]+imgmat[i+1][j]+imgmat[i+1][j+1]+imgmat[i+1][j+2];
				//int J=imgmat[i-2][j-2]+imgmat[i+2][j-1]+imgmat[i+2][j]+imgmat[i+2][j+1]+imgmat[i+2][j+2];

				if(8<C+H){
					imgmat1[i][j] = 255;
					imgmat1[i+1][j] = 255;
					imgmat1[i-1][j] = 255;
					imgmat1[i][j+1] = 255;
					imgmat1[i][j-1] = 255;
				}//end if 

			}//end for j
		}//end for i sumas


		//modificar el vector de la imagen: 
		for(y=0; y<height; y++){
			for(x=0;x<width;x++){
				png_byte* ptr = &(row_pointers[y][x*4]);
				ptr[0]=imgmat1[y][x];
				ptr[1]=ptr[0];
				ptr[2]=ptr[0];
			}
		}
}



int main(int argc, char **argv)
{
        if (argc != 3)
                abort_("Usage: program_name <file_in> <file_out>");

        read_png_file(argv[1]);
        process_file();
        write_png_file(argv[2]);

        return 0;
}