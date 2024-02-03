// This task will test your knowledge of file input and mathematical formulas. Basic linear regression
// (LR) is used to find a relationship between two types of data. For example, you could use LR to find
// the relationship between glucose intake and heart rate. These initially would be plotted on a graph
// where the x axis represents glucose and the y axis represents heart rate. Using the LR formula, you
// can find y=bx+a (equation of a straight line (more commonly known as y=mx+c) between “n” number
// of points on a graph. Below are the formulas to find “a” and “b.”

// You will be given multiple text files containing “n” number of coordinates. Your program will read in
// the coordinates (x,y) and use the LR formula to produce the gradient (a) and the constant (b), and
// being able to print out y=bx+a. For example, if a = 0.5 and b=-2, your program will print out
// y=-2x+0.5. This equation now represents the trend in the data you will be given. Finally, your
// program will ask the user to type in a value of x which will then calculate y.

#include <stdio.h>
#include <stdlib.h>

int main()
{
	int n = 0;										 // Variable to store the number of coordinates
	double X, Y;									 // Variables to store the input and calculated values of X and Y
	double sumX = 0, sumX2 = 0, sumY = 0, sumXY = 0; // Variables to store the sum of X, X^2, Y, and XY
	double a, b;									 // Variables to store the calculated values of a and b
	char filename[100];								 // Array to store the filename

	printf("Enter File name to import: ");
	scanf("%s", filename);				  // Prompting the user to enter the filename to import
	FILE *numfile = fopen(filename, "r"); // Opening the file in read mode

	if (numfile == NULL)
	{
		printf("Error importing File. Try Again... \n"); // Displaying an error message if the file import fails
		exit(EXIT_FAILURE);								 // Exiting the program with failure status
	}

	while (fscanf(numfile, "%*lf,%*lf") == 0)
	{
		n++; // Counting the number of coordinates in the file
	}

	rewind(numfile); // Resetting the file pointer to the beginning of the file

	double x[n], y[n]; // Arrays to store the x and y coordinates

	for (int i = 0; i < n; i++)
	{
		if (fscanf(numfile, "%lf,%lf", &x[i], &y[i]) != 2)
		{
			printf("Error reading coordinates from file. Exiting...\n"); // Displaying an error message if reading coordinates from the file fails
			exit(EXIT_FAILURE);											 // Exiting the program with failure status
		}

		sumX += x[i];		  // Calculating the sum of X
		sumX2 += x[i] * x[i]; // Calculating the sum of X^2
		sumY += y[i];		  // Calculating the sum of Y
		sumXY += x[i] * y[i]; // Calculating the sum of XY
	}
	fclose(numfile); // Closing the file

	b = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX); // Calculating the value of b
	a = (sumY - b * sumX) / n;								   // Calculating the value of a

	printf("Values : a=%lf and b = %lf", a, b);		 // Printing the calculated values of a and b
	printf("\nbest fit is: y = %lf + %lfx\n", a, b); // Printing the equation of the best fit line

	printf("Enter X value to calculate Y: ");
	scanf("%lf", &X);				// Prompting the user to enter the value of X
	Y = a + b * X;					// Calculating the value of Y
	printf("Y value is %lf \n", Y); // Printing the calculated value of Y

	return 0;
}
