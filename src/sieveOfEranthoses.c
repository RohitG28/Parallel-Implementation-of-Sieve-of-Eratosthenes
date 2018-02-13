#include "mpi.h"
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char** argv)
{

	char* marked;
	int prime;	

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

	if (processId == rootProcess)
	{
			
		
	}
	else
	{
		
	}

	err = MPI_Finalize();

	return 0;
}