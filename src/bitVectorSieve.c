#include "mpi.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv)
{	
	double elapsedTime,elapsedTime1=0;
	 
	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	//Let every process come execute MPI_Init
	// err = MPI_Barrier(MPI_COMM_WORLD);

	elapsedTime = -MPI_Wtime();
	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);
	
	//Per process array of marked numbers
	char* marked1;
	char* marked2;

	//variable N
	long int n = 100000000;

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
	long int j = 0; 

	memset(marked1, '0', (sqrtN+1));
	memset(marked2, '0', (high-low+1));

	for(long int i=2; i<=sqrtN; i++)
	{
		if(marked1[i] == '0')
		{
			for(long int k=i+i;k<=sqrtN;k+=i)
			{
				marked1[k] = '1';
			}
			
			j = (low/i)*i;
			
			if(j<low)
				j = j+i;

			elapsedTime1 -= MPI_Wtime();
			for(j;j<=high;j+=i)
			{
				marked2[j-low] = '1';	
			}	
			elapsedTime1 += MPI_Wtime();
		}
	}

	// MPI_Barrier(MPI_COMM_WORLD);
	
	free(marked1);
	free(marked2);

	elapsedTime += MPI_Wtime();
	printf("total: %lf  fraction: %lf\n",elapsedTime,elapsedTime1); 

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}