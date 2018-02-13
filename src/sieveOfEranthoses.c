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

	int q =0;
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
			int lastUnmarked = 0;
			marked = (char*)malloc(blockSize*sizeof(char));	
			for(int i=0;i<blockSize;i++)
				marked[i] = '0';
			
			
				for(int i=0;i<blockSize;i++)
				{
					if(((i+(processId-1)*blockSize+2) % prime) == 0)
				}
				
		}	

		q++;	
	}

	err = MPI_Finalize();

	return 0;
}