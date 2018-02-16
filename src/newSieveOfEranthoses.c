#include "mpi.h"
#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv)
{	
	clock_t start, end;
	double elapsedTime;
	 
	start = clock();
	// elapsedTime = -MPI_Wtime();

	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	//Let every process come execute MPI_Init
	err = MPI_Barrier(MPI_COMM_WORLD);

	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);
	
	//Per process array of marked numbers
	char* marked1;
	char* marked2;

	//prime initialized to 2
	// long int prime = 2;	

	//variable N
	long int n = 10000000000;

	long int sqrtN = ceil((double)sqrt(n));

	long int blockSize;
	long int low,high,remainder;

	marked1 = (char*)malloc((sqrtN+1)*sizeof(char));

	//Size of range given to each process
	blockSize = (n-(sqrtN+1))/noOfProcesses;
	low = sqrtN + processId*blockSize + 1;
	high = low + blockSize - 1;
	remainder = (n-(sqrtN+1))%noOfProcesses;

	if(processId == noOfProcesses-1)
	{
		high += remainder;
	}

	marked2 = (char*)malloc((high-low+1)*sizeof(char));

	int rootProcess = 0;

	int flag = 0;
	long int lastUnmarked = low;

	//Every number is unmarked initially
	for(long int i=2;i<(sqrtN+1);i++)
	{
		marked1[i] = '0';
	}
	for(long int i=0;i<(high-low+1);i++)
	{
		marked2[i] = '0';
	}

	for(long int i=2; i<=sqrtN; i++)
	{
		if(marked1[i] == '0')
		{
			if(processId == rootProcess)
			cout << i << endl;
			for(long int j=i+1;j<=sqrtN;j++)
			{
				if(j%i == 0)
				{
					marked1[j] = '1';
				}
			}

			flag = 0;
			for(long int j=lastUnmarked;j<=high;j++)
			{
				if(j%i == 0)
				{
					marked2[j-low] = '1';
				}

				if((flag != 1) && marked2[j-low] == '0')
				{
					lastUnmarked = j;
					flag = 1;
				}	
			}	
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	end = clock();
	elapsedTime = ((double) (end - start)) / CLOCKS_PER_SEC;
	// elapsedTime += MPI_Wtime();

	free(marked1);
	free(marked2);

	cout << elapsedTime << endl;

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}