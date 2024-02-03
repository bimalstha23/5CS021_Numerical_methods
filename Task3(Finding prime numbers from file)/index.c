// TASK 3: Finding prime numbers from multiple text files using multithreading (15% - 100 marks)
// You will be given files containing a list of numbers. The amount and numbers themselves will be
// random. You will create a C program which counts the number of prime numbers there are within
// the files and output to a file the amount of prime numbers found, along with the prime numbers
// themselves. The aim of this task is to use POSIX threads to parallelise the task to take advantage of
// the multicore processor within your machine to speed up the task. The threads you spawn within the
// program must compute an equal or close to an equal amount of computations to make the program
// more efficient in relation to speed. For this section, you will be reading the data from three files and
// splitting it across many threads, you should load in the data from files and split the data into equal
// parts, then process each slice within your threads. This task also tests your knowledge of dynamic
// memory allocation. NOTE – this program should work with any amount of threads.
// Creating an algorithm to detect prime numbers (10 marks)
// Using dynamic memory – “malloc” (20 marks)
// Using multithreading with equal computations (50 marks)
// Outputting correct output to a file (20 marks)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

typedef struct
{
	int rank;
	long long start;
	long long end;
	long primeCount;
	long *primeNumbers;
} ThreadInfo;

ThreadInfo *threadInfos;
long *numbers;

int isPrime(int num)
{
	if (num <= 1)
	{
		return 0;
	}

	for (int i = 2; i <= sqrt(num); ++i)
	{
		if (num % i == 0)
		{
			return 0;
		}
	}
	return 1;
}
// // This function threadRunner is responsible for finding prime numbers within a specific range assigned to each thread. It iterates through the numbers within the range, checks if each number is prime using the isPrime function, and stores the prime numbers in the primeNumbers array of the corresponding thread. It also writes the count and the prime numbers found by each thread to a separate file.

/**
 * This function is the entry point for each thread in the program.
 * It finds prime numbers within a given range and stores them in an array.
 * The prime numbers are then written to a file named "Thread_<rank>_Primes.txt".
 *
 * @param rank The rank of the thread.
 * @return void
 */
void *threadRunner(void *rank)
{
	int threadRank = *(int *)rank;
	long primeCount = 0;
	long start = threadInfos[threadRank].start;
	long end = threadInfos[threadRank].end;

	for (; start <= end; ++start)
	{
		if (isPrime(numbers[start]))
		{
			threadInfos[threadRank].primeNumbers[primeCount] = numbers[start];
			primeCount++;
		}
	}

	threadInfos[threadRank].primeCount = primeCount;

	char filename[20];
	sprintf(filename, "Thread_%d_Primes.txt", threadInfos[threadRank].rank);
	FILE *primeFile = fopen(filename, "w");
	fprintf(primeFile, "Number of primes found by Thread %d: %ld\n\n", threadInfos[threadRank].rank, primeCount);
	for (long i = 0; i < primeCount; ++i)
	{
		fprintf(primeFile, "%ld\n", threadInfos[threadRank].primeNumbers[i]);
	}
	fclose(primeFile);

	return NULL;
}

/**
 * @brief Main function that reads numbers from a file, divides the work among multiple threads,
 *        and finds prime numbers using concurrency.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 *             The second argument should be the number of threads to use.
 *
 * @return 0 on success, non-zero value on failure.
 */
int main(int argc, char const *argv[])
{
	// Check if the correct number of command-line arguments is provided
	if (argc != 2)
	{
		fprintf(stderr, "%s Number of Threads is Required\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Convert the number of threads from string to integer
	int numThreads = atoi(argv[1]);

	// Get the filename from the user
	char filename[100];
	printf("Enter File name to import: ");
	scanf("%s", filename);

	// Open the file for reading
	FILE *numFile = fopen(filename, "r");
	if (numFile == NULL)
	{
		fprintf(stderr, "Error importing File. Try Again... \n");
		exit(EXIT_FAILURE);
	}

	// Count the number of numbers in the file
	long numbersCount = 0;
	char ch;
	while ((ch = fgetc(numFile)) != EOF)
	{
		if (ch == '\n')
		{
			numbersCount++;
		}
	}
	rewind(numFile);

	// Allocate memory for thread information and numbers array
	threadInfos = malloc(numThreads * sizeof(ThreadInfo));
	numbers = malloc(numbersCount * sizeof(long));
	pthread_t threadIds[numThreads];

	// Read the numbers from the file into the numbers array
	for (long i = 0; i < numbersCount; i++)
	{
		fscanf(numFile, "%ld", &numbers[i]);
	}
	fclose(numFile);

	// Divide the work among the threads
	long chunkSize = numbersCount / numThreads;
	for (int i = 0; i < numThreads; ++i)
	{
		if (i == 0)
		{
			threadInfos[i].start = i;
			threadInfos[i].end = i + chunkSize - 1;
		}
		else if (i == numThreads - 1)
		{
			threadInfos[i].start = threadInfos[i - 1].end + 1;
			threadInfos[i].end = numbersCount - 1;
		}
		else
		{
			threadInfos[i].start = threadInfos[i - 1].end + 1;
			threadInfos[i].end = threadInfos[i].start + chunkSize;
		}

		threadInfos[i].rank = i;
		threadInfos[i].primeNumbers = malloc((threadInfos[i].end - threadInfos[i].start + 1) * sizeof(long));
	}

	// Create threads and run the threadRunner function
	for (int i = 0; i < numThreads; ++i)
	{
		int returnValue = pthread_create(&threadIds[i], NULL, threadRunner, (void *)&threadInfos[i].rank);
		if (returnValue != 0)
		{
			fprintf(stderr, "Error occurred while creating threads. Value=%d\n", returnValue);
			exit(EXIT_FAILURE);
		}
	}

	// Wait for all threads to finish
	for (int i = 0; i < numThreads; ++i)
	{
		pthread_join(threadIds[i], NULL);
	}

	// Print the results
	printf("Total numbers: %ld\n", numbersCount);
	long totalPrimes = 0;
	for (int i = 0; i < numThreads; ++i)
	{
		printf("\nThread ID: %d\n", i);
		printf("Start index: %lld, End index: %lld\n", threadInfos[i].start, threadInfos[i].end);
		printf("Number of primes found by the thread: %ld\n", threadInfos[i].primeCount);
		totalPrimes += threadInfos[i].primeCount;
	}

	// Write the prime numbers to an output file
	FILE *outputFile = fopen("All_Primes.txt", "w");
	fprintf(outputFile, "There are %ld prime numbers\n", totalPrimes);

	for (int i = 0; i < numThreads; ++i)
	{
		for (long j = 0; j < threadInfos[i].primeCount; ++j)
		{
			fprintf(outputFile, "%ld\n", threadInfos[i].primeNumbers[j]);
		}
		free(threadInfos[i].primeNumbers);
	}
	fclose(outputFile);

	printf("\nResults exported to Outputfile.txt\n");

	// Free allocated memory
	free(threadInfos);
	free(numbers);

	return 0;
}
