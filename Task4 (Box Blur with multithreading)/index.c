// TASK 4: Gaussian Blur with multithreading (15% - 100 marks)
// Your program will decode a PNG file into an array and apply the gaussian blur filter. Blurring an image
// reduces noise by taking the average RGB values around a specific pixel and setting it’s RGB to the
// mean values you’ve just calculated. This smoothens the colour across a matrix of pixels. For this
// assessment, you will use a 3x3 matrix. For example, if you have a 5x5 image such as the following (be
// aware that the coordinate values will depend on how you format your 2D array):
// 0,4 1,4 2,4 3,4 4,4
// 0,3 1,3 2,3 3,3 4,3
// 0,2 1,2 2,2 3,2 4,2
// 0,1 1,1 2,1 3,1 4,1
// 0,0 1,0 2,0 3,0 4,0
// The shaded region above represents the pixel we want to blur, in this case, we are focusing on pixel
// 1,2 (x,y) (Centre of the matrix). to apply the blur for this pixel, you would sum all the Red values from
// the surrounding coordinates including 1,2 (total of 9 R values) and find the average (divide by 9). This
// is now the new Red value for coordinate 1,2. You must then repeat this for Green and Blue values.
// This must be repeated throughout the image. If you are working on a pixel which is not fully
// surrounded by pixels (8 pixels), you must take the average of however many neighbouring pixels
// there are.
// Reading in an image file into a single or 2D array (10 marks)
// Applying Gaussian filter on image (20 marks)
// Using multithreading appropriately to apply Gaussian filter (40 marks)
// Using dynamic memory – malloc (10 marks)
// Outputting the correct image with Gaussian Blur applied (20 marks)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lodepng.h"

typedef struct
{
	int rank;
	long start;
	long end;
	unsigned char *blurredPixels;
} ThreadInfo;

ThreadInfo *threadInfos;
unsigned char **image2D;
unsigned int width, height;

/**
 * Applies a box blur effect to a pixel in the image.
 *
 * @param rank The rank of the thread.
 * @param col The column index of the pixel.
 * @param row The row index of the pixel.
 * @return The blurred pixel value.
 */
int imageBlur(int rank, int col, int row)
{
	if (row == 0 || row == height - 1 || col == 0 || col == width * 4 - 1)
	{
		return image2D[row][col];
	}

	int sumR = 0, sumG = 0, sumB = 0;
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			sumR += image2D[row + i][(col + j) * 4];
			sumG += image2D[row + i][(col + j) * 4 + 1];
			sumB += image2D[row + i][(col + j) * 4 + 2];
		}
	}
	return (sumR / 9) | ((sumG / 9) << 8) | ((sumB / 9) << 16) | 255;
}

/**
 * The function executed by each thread.
 *
 * @param rank The rank of the thread.
 * @return NULL.
 */
void *threadRunner(void *rank)
{
	int threadRank = *(int *)rank;
	long start = threadInfos[threadRank].start;
	long end = threadInfos[threadRank].end;
	int pixelIndex = 0;

	for (long row = start; row <= end; ++row)
	{
		for (int col = 0; col < width * 4; ++col)
		{
			threadInfos[threadRank].blurredPixels[pixelIndex++] = imageBlur(threadRank, col, row);
		}
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	if (argc < 2)
	{
		printf(" %s Number of Threads are Required\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	char *filename = malloc(1000 * sizeof(char));
	printf("Enter filename:");
	scanf("%s", filename);

	unsigned char *image;
	unsigned int error = lodepng_decode32_file(&image, &width, &height, filename);
	if (error)
	{
		printf("Error %u: %s\n", error, lodepng_error_text(error));
		exit(EXIT_FAILURE);
	}

	threadInfos = malloc(atoi(argv[1]) * sizeof(ThreadInfo));
	image2D = malloc(sizeof(unsigned char *) * height);
	for (int i = 0; i < height; ++i)
	{
		image2D[i] = malloc(sizeof(unsigned char) * width * 4);
	}

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width * 4; ++j)
		{
			image2D[i][j] = image[i * width * 4 + j];
		}
	}

	pthread_t threadIDs[atoi(argv[1])];
	int chunkSize = height / atoi(argv[1]);
	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		if (i == 0)
		{
			threadInfos[i].start = 0;
			threadInfos[i].end = chunkSize;
		}
		else if (i == atoi(argv[1]) - 1)
		{
			threadInfos[i].start = i * chunkSize + 1;
			threadInfos[i].end = height - 1;
		}
		else
		{
			threadInfos[i].start = i * chunkSize + 1;
			threadInfos[i].end = (i + 1) * chunkSize;
		}
		threadInfos[i].rank = i;
		threadInfos[i].blurredPixels = malloc((threadInfos[i].end - threadInfos[i].start + 1) * width * 4 * sizeof(unsigned char));
		pthread_create(&threadIDs[i], NULL, threadRunner, &threadInfos[i].rank);
	}

	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		pthread_join(threadIDs[i], NULL);
	}

	unsigned char *blurImage = malloc(height * width * 4 * sizeof(unsigned char));
	int index = 0;
	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		int range = (threadInfos[i].end - threadInfos[i].start + 1) * width * 4;
		for (int j = 0; j < range; ++j)
		{
			blurImage[index++] = threadInfos[i].blurredPixels[j];
		}
	}

	char *outputPNG = "output.png";
	lodepng_encode32_file(outputPNG, blurImage, width, height);
	printf("New image created as output.png\n");

	free(image);
	free(filename);
	free(threadInfos);
	free(blurImage);
	for (int i = 0; i < height; ++i)
	{
		free(image2D[i]);
	}
	free(image2D);

	return 0;
}
