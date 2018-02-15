#include "mpi.h"
#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;


void setBit(long int* marked, long int k)
{
	marked[k/sizeof(long int)] |= 1 << (k%sizeof(long int));
}

void clearBit(long int* marked, long int k){
	marked[k/sizeof(long int)] &= ~(1 << (k%sizeof(long int)));
}

int testBit(long int* marked,  long int k)
{
	return ((marked[k/sizeof(long int)] & (1 << (k%sizeof(long int)) )) != 0 ); 
}



int main(int argc, char** argv)
{	
	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	/**
	// clock_t start, end;
	double elapsedTime;
	 
	// start = clock();
	elapsedTime = -MPI_Wtime();
	**/

	double tbeg = MPI_Wtime();


	//Let every process come execute MPI_Init
	//err = MPI_Barrier(MPI_COMM_WORLD);

	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);
	
	//Per process array of marked numbers
	long int* marked1;
	long int* marked2;

	//prime initialized to 2
	// long int prime = 2;	

	//variable N
	long int n = 10000000;

	long int sqrtN = (long int)ceil((double)sqrt(n));

	long int blockSize;
	long int low,high,remainder;

	// we use (sizeof(long)-1+size_needed)/sizeof(long) to to round up
	marked1 = (long int*)malloc((((sqrtN+1)*sizeof(long int))+sizeof(long int)-1)/sizeof(long int));
	memset(marked1, 0, sizeof(marked1));

	//Size of range given to each process
	blockSize = (n-(sqrtN+1))/noOfProcesses;
	low = sqrtN + processId*blockSize + 1;
	high = low + blockSize - 1;
	remainder = (n-(sqrtN+1))%noOfProcesses;

	if(processId == noOfProcesses-1)
	{
		high += remainder;
	}

	//we use (sizeof(long)-1+size_needed)/sizeof(long) to to round up
	marked2 = (long int*)malloc((((high-low+1)*sizeof(long int))+sizeof(long int)-1)/sizeof(long int));
	memset(marked2, 0, sizeof(marked2));

	int rootProcess = 0;

	int flag = 0;
	long int lastUnmarked = low;

	//Every number is unmarked initially
	/**
	for(long int i=2;i<(sqrtN+1);i++)
	{
		//marked1[i] = '0';
	}
	for(long int i=0;i<(high-low+1);i++)
	{
		//marked2[i] = '0';
	}
	**/
	for(long int i=2; i<=sqrtN; i++)
	{
		//if(marked1[i] == '0')
		if(!testBit(marked1,i))
		{
			// if(processId == rootProcess)
			// cout << i << endl;
			for(long int j=i+1;j<=sqrtN;j++)
			{
				if(j%i == 0)
				{
					//marked1[j] = '1';
					setBit(marked1,j);
				}
			}

			flag = 0;
			for(long int j=lastUnmarked;j<=high;j++)
			{
				if(j%i == 0)
				{
					//marked2[j-low] = '1';
					setBit(marked2,j-low);
				}

				//if((flag != 1) && marked2[j-low] == '0')
				if((flag != 1) && !testBit(marked2,j-low))
				{
					lastUnmarked = j;
					flag = 1;
				}	
			}	
		}
	}

	//MPI_Barrier(MPI_COMM_WORLD);
	// end = clock();
	// elapsedTime = ((double) (end - start)) / CLOCKS_PER_SEC;
	//elapsedTime += MPI_Wtime();

	double elapsedTime = MPI_Wtime() - tbeg;

	// //Total size of gathered array = sum of sizes of marked arrays from all the processes
	// char* isPrime = (char*)malloc(sizeof(char)*noOfProcesses*blockSize);


	// //Gather Data from Everywhere
	// err = MPI_Gather(marked, blockSize, MPI_CHAR, isPrime, blockSize, MPI_CHAR, rootProcess, MPI_COMM_WORLD);


	// //Printing the Prime Numbers
	free(marked1);
	free(marked2);

		// int x = 1;

		// cout<<"\n-----------Printing Prime Numbers-----------\n\n";

		// for(int i=blockSize; i<((noOfProcesses)*blockSize);i++)
		// {
			
		// 	//Avoid going beyond N
		// 	if(i-blockSize+2>n)
		// 		break;


		// 	//Only print the left unmarked symbols -- these are primes
		// 	if(isPrime[i] == '0')
		// 		cout<< x++ <<") " << i-blockSize+2 << endl;
		// }

		// cout<<"\n----------------Woah----------------\n";
	cout << elapsedTime << endl;

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}
