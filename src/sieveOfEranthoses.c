#include "mpi.h"
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char** argv)
{

	char* marked;
	int prime = 2;	

	int n;
	cin >> n;

	int sqrtN = ceil((double)sqrt(n));

	int blockSize; 

	int err, processId;	
	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);

	blockSize = ceil((double)n / noOfProcesses);

	rootProcess = 0;

	int* primesReceived = (int*)malloc(noOfProcesses*sizeof(int));
	memset(primes_received, '0', sizeof(primesReceived));
	marked = (char*)malloc(blockSize*sizeof(char));
	int q =0;
	int lastUnmarked;
	while(prime != -1)
	{
		if(q!=0)
		{

			err = MPI_Gather(&(lastUnmarked), 1, MPI_INT,(void *)primes_received, 1, MPI_INT, rootProcess, MPI_COMM_WORLD);

			if (processId == rootProcess)
			{
				
			}
			
			err =  MPI_Bcast( &prime, 1, MPI_INT, rootProcess, MPI_COMM_WORLD);
		}
		
		if(processId!=rootProcess)
		{
			for(int i=0;i<blockSize;i++)
				marked[i] = '0';
			
			if(prime <= ((processId-1)*blockSize+2+blockSize-1))
			{
				for(int i=0;i<blockSize;i++)
				{
					if(((i+(processId-1)*blockSize+2) % prime) == 0)
						marked[i] = '1';
				}
			}

			lastUnmarked = -1;	

			if((prime <= ((processId-1)*blockSize+2+blockSize-1)) && (((processId-1)*blockSize+2) <= sqrtN))
			{
				for(int i=0;i<blockSize;i++)
				{
					if((marked[i] == '0'))
					{
						lastUnmarked = i+(processId-1)*blockSize+2;
					}
				}
			}
			else
			{
				lastUnmarked = -1;
			}
		}	

		q++;	
	}

	err = MPI_Finalize();

	return 0;
}