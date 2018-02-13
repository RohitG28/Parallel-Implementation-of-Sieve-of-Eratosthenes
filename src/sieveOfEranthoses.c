#include "mpi.h"
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char** argv)
{
	char* marked;
	int prime = 2;	

	int n = 13;

	int sqrtN = ceil((double)sqrt(n));

	int blockSize; 

	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);
	err = MPI_Barrier(MPI_COMM_WORLD);

	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);

	blockSize = ceil((double)n/(noOfProcesses-1));
	cout << "I am blockSize " <<  blockSize << endl;
	MPI_Barrier(MPI_COMM_WORLD);

	int rootProcess = 0;

	int* primesReceived = (int*)malloc((noOfProcesses-1)*sizeof(int));

	marked = (char*)malloc(blockSize*sizeof(char));
	memset(marked, '0', sizeof(marked));
	memset(primesReceived, 0, sizeof(primesReceived));

	int* displacement = (int*)malloc((noOfProcesses-1)*sizeof(int));
	int* rcvCount = (int*)malloc((noOfProcesses-1)*sizeof(int)); 
	for(int i=0;i<(noOfProcesses-1);i++)
	{
		displacement[i] = i;
		rcvCount[i] = 1;
	}

	int q =0;
	int lastUnmarked = -1;
	while(prime != -1)
	{
		cout << prime << endl;
		if(q!=0)
		{
			err = MPI_Gatherv(&(lastUnmarked), 1, MPI_INT, primesReceived, rcvCount, displacement, MPI_INT, rootProcess, MPI_COMM_WORLD);

			if (processId == rootProcess)
			{
				int allNull = 1;
				for(int i=0;i<noOfProcesses-1;i++)
				{
					if(primesReceived[i]!=-1 && allNull==1){
						
						prime = primesReceived[i];
						allNull = 0;
						
					}
					else if(primesReceived[i]!=-1){
						prime = min(primesReceived[i],prime);
					}
				}

				if(allNull==1)
				{
					prime = -1;
				}

				for(int i=0;i<(noOfProcesses-1);i++)
				{
					cout << primesReceived[i] << " ";
				}
				cout << endl;
			}
			
			err =  MPI_Bcast( &prime, 1, MPI_INT, rootProcess, MPI_COMM_WORLD);
		}
		
		if(prime != (-1))
		{
			if(processId!=rootProcess)
			{
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
						if((marked[i] == '0')) ///////if not marked
						{
							lastUnmarked = i+(processId-1)*blockSize+2;
							break;
						}
					}
				}
				else
				{
					lastUnmarked = -1;
				}
			}	
		}
		err = MPI_Barrier(MPI_COMM_WORLD);
		q++;	
	}

	err = MPI_Finalize();

	return 0;
}