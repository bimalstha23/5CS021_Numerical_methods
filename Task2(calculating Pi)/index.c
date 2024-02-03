// TASK 2: Calculating Pi using Leibniz formula and multithreading (15% - 100 marks)
// The Leibniz formula is an infinite series method of calculating Pi. The formula is a very simple way of
// calculating Pi, however, it takes a large amount of iterations to produce a low precision value of Pi.
// This task requires a large amount of computation and therefore it is vital that you use multithreading
// to speed up the program. Below is the Leibniz formula:

// As the series can be iterated infinite number of times, your program should allow the user to take in
// 2 inputs; the first is the number of iterations and the second is the number of threads the user would
// like to use. This means that the slicing of workload needs to be dynamic.
// Calculating Pi using Leibniz formula (20 marks)
// Using multithreading with appropriate slicing (60 marks)
// Correct value of Pi printed out depending on iteration count (20 marks)

// -------------------------NOTE-------------------------
// include - pthread and -lm when compiling

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

typedef struct
{
	int rank;		 // Rank of the thread
	long long start; // Starting index of iterations for the thread
	long long end;	 // Ending index of iterations for the thread
	double sum;		 // Partial sum calculated by the thread
} ThreadInfo;

ThreadInfo *threadInfos; // Array of ThreadInfo structures to store information for each thread

/**
 * Calculates the value of Pi using a series approximation.
 *
 * @param threadRank The rank of the thread calling the function.
 * @return The partial sum of the series calculated by the thread.
 */
double calculatePi(int threadRank)
{
	double localSum = 0;
	for (long long i = threadInfos[threadRank].start; i <= threadInfos[threadRank].end; ++i)
	{
		double numerator = pow(-1, i);		 // Numerator of the series
		double denominator = (2 * i) + 1;	 // Denominator of the series
		localSum += numerator / denominator; // Calculate partial sum for the thread
	}
	return localSum;
}

/**
 * @brief This function is the entry point for each thread in the program.
 *
 * @param rank The rank of the thread.
 * @return void* Returns NULL.
 */
void *threadRunner(void *rank)
{
	int threadRank = *(int *)rank;
	threadInfos[threadRank].sum = calculatePi(threadRank); // Calculate partial sum for the thread
	return NULL;
}

/**
 * @brief The main function of the program.
 *
 * This function is the entry point of the program. It takes command line arguments
 * for the number of iterations and the number of threads. It then calculates the value
 * of Pi using the Monte Carlo method by distributing the iterations among multiple threads.
 * The program prints the thread information, performs the calculations, and prints the final
 * value of Pi.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @return int The exit status of the program.
 */
int main(int argc, char const *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <iterations> <numThreads>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	long long totalIterations = atoll(argv[1]);			// Total number of iterations
	int numThreads = atoi(argv[2]);						// Number of threads
	long long chunkSize = totalIterations / numThreads; // Number of iterations per thread

	threadInfos = calloc(numThreads, sizeof(ThreadInfo)); // Allocate memory for threadInfos array
	pthread_t threadIds[numThreads];					  // Array to store thread IDs

	printf("Number of iterations: %lld\n", totalIterations);
	printf("Number of threads: %d\n", numThreads);
	printf("-----------------------------------\n");

	for (int i = 0; i < numThreads; ++i)
	{
		threadInfos[i].rank = i; // Assign rank to each thread
		if (i == numThreads - 1)
		{
			threadInfos[i].start = i * chunkSize + 1; // Calculate starting index for the last thread
			threadInfos[i].end = totalIterations;	  // Set ending index for the last thread
		}
		else
		{
			threadInfos[i].start = i * chunkSize;				   // Calculate starting index for the thread
			threadInfos[i].end = threadInfos[i].start + chunkSize; // Calculate ending index for the thread
		}
	}

	printf("Thread Information:\n");
	for (int i = 0; i < numThreads; ++i)
	{
		printf("Thread ID: %d\nIterations Start: %lld\nIterations End: %lld\n\n",
			   threadInfos[i].rank + 1, threadInfos[i].start, threadInfos[i].end);
	}
	printf("-----------------------------------\n");

	for (int i = 0; i < numThreads; ++i)
	{
		int returnValue = pthread_create(&threadIds[i], NULL, threadRunner, (void *)&threadInfos[i].rank); // Create threads
		if (returnValue != 0)
		{
			fprintf(stderr, "Error occurred while creating threads. Value=%d\n", returnValue);
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < numThreads; ++i)
	{
		pthread_join(threadIds[i], NULL); // Wait for threads to finish
	}

	double pi = 0.0;
	for (int i = 0; i < numThreads; ++i)
	{
		pi += threadInfos[i].sum; // Accumulate partial sums from each thread
	}

	printf("Calculation Summary:\n");
	printf("Total Iterations: %lld\n", totalIterations);
	printf("Number of Threads: %d\n", numThreads);
	printf("Pi Value: %.15lf\n", pi * 4); // Calculate and print the final value of Pi

	free(threadInfos); // Free memory allocated for threadInfos array

	exit(EXIT_SUCCESS);
}
