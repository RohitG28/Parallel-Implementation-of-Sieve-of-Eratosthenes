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
	long int n = 1000000000;

	long int sqrtN = ceil((double)sqrt(n));

	long int blockSize;
	long int low,high,remainder;

	marked1 = (char*)malloc((sqrtN+1)*sizeof(char));

	//Size of range given to each process
	blockSize = (n-(sqrtN))/(noOfProcesses);
	low = sqrtN + processId*blockSize + 1;
	high = low + blockSize - 1;
	remainder = (n-(sqrtN))%(noOfProcesses);

	if(processId == noOfProcesses-1)
	{
		high += remainder;
	}

	if(low%2 == 0)
		low++;

	if(high%2 == 0)
		high--;

	long int highIndex = (high-low)/2;
	long int sieveStart;
	long int j = 0; 

	marked2 = (char*)malloc((((high-low)/2)+1)*sizeof(char));

	int rootProcess = 0;

	memset(marked1, '0', (sqrtN+1));
	memset(marked2, '0', (((high-low)/2)+1));

	MPI_Barrier(MPI_COMM_WORLD);

	for(long int k=4;k<=sqrtN;k+=2)
	{
		marked1[k] = '1';
	}

	for(long int i=3; i<=sqrtN; i++)
	{
		if(marked1[i] == '0')
		{
			for(long int k=i+i;k<=sqrtN;k+=i)
			{
				marked1[k] = '1';
			}
			
			sieveStart = (low/i)*i;
			
			if(sieveStart<low)
				sieveStart = sieveStart+i;

			if(sieveStart%2 == 0)
				sieveStart += i;

			if(sieveStart <= high)
			{
				j = (sieveStart - low)/2;

				for(j;j<=highIndex;j+=i)
				{
					marked2[j] = '1';
					// if(processId == 3)
					// 	cout << low+j*2 << " " << i << endl;	
				}	
			}
		}
	}

	// MPI_Barrier(MPI_COMM_WORLD);
	elapsedTime += MPI_Wtime();
	
	// if(processId == rootProcess)
	// {
	// 	for(long int i=2; i<=sqrtN; i++)
	// 	{
	// 		if(marked1[i] == '0')
	// 		{
	// 			cout << processId << ": " << i << endl;
	// 		}
	// 	}
	// }

	MPI_Barrier(MPI_COMM_WORLD);

	// for(long int i=0; i<=highIndex; i++)
	// {
	// 	if(marked2[i] == '0')
	// 	{
	// 		printf("%ld : %ld\n",processId,low+i+i);
	// 	}
	// }

	free(marked1);
	free(marked2);

	printf("total: %lf  fraction: %lf\n",elapsedTime,elapsedTime1); 

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}