// flcjr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//

#include <stdio.h>
#include <stdlib.h>

#include "flc_file.h"

//------------------------------------------------------------------------------
void helpText()
{
	printf("flcjr - v0.0\n");
	printf("------------\n");
	printf("FLC to Jr Sprite Conversion tool\n");
	printf("\nflcjr [options] <FLC_File> <Sprite_File>\n");
	printf("  -v  verbose\n\n");

	exit(-1);
}

//------------------------------------------------------------------------------
void hline_bounds(unsigned char* pixels, int width, int height, int x0, int y0, int x1)
{
	pixels += y0*width;

	for (int idx = x0; idx <= x1; ++idx)
	{
		pixels[ idx ] |= 0xC0;
	}

}

//------------------------------------------------------------------------------
void vline_bounds(unsigned char* pixels, int width, int height, int x0, int y0, int y1)
{
	pixels += y0*width;
	pixels += x0;

	for (int idx = y0; idx <= y1; ++idx)
	{
		pixels[0] |= 0xC0;
		pixels += width;
	}

}

//------------------------------------------------------------------------------

void make_objs(int width, int height, unsigned char *pixels)
{
	int minx = width;
	int miny = height;
	int maxx = 0;
	int maxy = 0;

	// get the bounds
	for (int y = 0; y < height; ++y)
	{
		int idx = y * width;

		for (int x = 0; x < width; ++x)
		{
			if (pixels[ idx + x ])
			{
				if (x < minx) minx = x;
				if (x > maxx) maxx = x;
				if (y < miny) miny = y;
				if (y > maxy) maxy = y;
			}
		}
	}

	// draw bounds
	hline_bounds(pixels,width,height,minx,miny,maxx);
	hline_bounds(pixels,width,height,minx,maxy,maxx);
	vline_bounds(pixels,width,height,minx,miny,maxy);
	vline_bounds(pixels,width,height,maxx,miny,maxy);

}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	char* pInfilePath  = nullptr;
	char* pOutfilePath = nullptr;
	bool bVerbose = false;

	for (int idx = 1; idx < argc; ++idx )
	{
		char* arg = argv[ idx ];

		if ('-' == arg[0])
		{
			// Parse as an option
			if ('v'==arg[1])
			{
				bVerbose = true;
			}
		}
		else if (nullptr == pInfilePath)
		{
			// Assume the first non-option is an input file path
			pInfilePath = argv[ idx ];
		}
		else if (nullptr == pOutfilePath)
		{
			// Assume second non-option is an output file path
			pOutfilePath = argv[ idx ];
		}
		else
		{
			// Oh Crap, we have a non-option, but we don't know what to do with
			// it
			printf("ERROR: Invalid option, Arg %d = %s\n\n", idx, argv[ idx ]);
			helpText();
		}
	}

	if (pInfilePath)
	{
		FlcFile import_file( pInfilePath );

		if (import_file.GetFrameCount() > 0)
		{
			printf("\nLoaded %s - success!\n", pInfilePath);

			printf("Width x Height = %d x %d\n", import_file.GetWidth(), import_file.GetHeight() );

			printf("FrameCount = %d\n", import_file.GetFrameCount());

		}
		else
		{
			printf("\nERROR: Unable to load %s\n", pInfilePath);
			helpText();
		}

		if (pOutfilePath)
		{
			int width  = import_file.GetWidth();
			int height = import_file.GetHeight();
			int num_frames = import_file.GetFrameCount();

			const std::vector<unsigned char *>& frames = import_file.GetPixelMaps();

			for (int frame_no = 0; frame_no < num_frames; ++frame_no)
			{
				make_objs(width, height, frames[ frame_no ]);
			}

			import_file.SaveToFile(pOutfilePath);
		}

	}
	else
	{
		helpText();
	}


	return 0;
}

