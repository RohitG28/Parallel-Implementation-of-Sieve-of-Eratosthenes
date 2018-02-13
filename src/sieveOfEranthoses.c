#include "mpi.h"
#include <iostream>
// #include <math.h>

using namespace std;

int main(int argc, char** argv)
{
	int err, processId, noOfProcesses;
	int n;
	cin >> n;

	err = MPI_Init(&argc, &argv);

	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);

	rootProcess = 0;
	if(processId == rootProcess)
	{
		int notPrime[n];
		int numOfElePerProcess = n / noOfProcesses;
		int startElement;
		for(int i=1;i<noOfProcesses;i++)
		{
			startElement = i*numOfElePerProcess;
			MPI_Send(&numOfElePerProcess,1,MPI_INT,i,1,MPI_COMM_WORLD);
			MPI_Send(&startElement,1,MPI_INT,i,1,MPI_COMM_WORLD);
		}
		
	}
	else
	{
		int numOfElePerProcess,startElement;
		MPI_Recv(&numOfElePerProcess,1,MPI_INT,rootProcess,1,MPI_COMM_WORLD,NULL);
		
		int array[numOfElePerProcess];

		MPI_Recv(&startElement,1,MPI_INT,rootProcess,1,MPI_COMM_WORLD,NULL);

		

		int numOfEleReturn = numOfElePerProcess;
		MPI_Send(&numOfEleReturn,1,MPI_INT,rootProcess,1,MPI_COMM_WORLD);
		MPI_Send(&array[0],numOfEleReturn,MPI_INT,rootProcess,1,MPI_COMM_WORLD);
	}

	err = MPI_Finalize();

	return 0;
}