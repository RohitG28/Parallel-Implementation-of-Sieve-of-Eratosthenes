#include "mpi.h"
#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv)
{	
	clock_t start, end;
	double elapsedTime;
	 
	// start = clock();
	elapsedTime = -MPI_Wtime();
	
	//Per process array of marked numbers
	char* marked;

	//prime initialized to 2
	long long int prime = 2;	

	//variable N
	long long int n = 10000000;

	long long int sqrtN = ceil((double)sqrt(n));

	long long int blockSize; 

	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	//Let every process come execute MPI_Init
	err = MPI_Barrier(MPI_COMM_WORLD);

	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);

	//Size of range given to each process
	blockSize = ceil((double)n/noOfProcesses);
	

	int rootProcess = 0;

	//Root process receives primes from each process
	long long int* numbersReceived = (long long int*)malloc((noOfProcesses)*sizeof(long long int));


	//Every number is unmarked initially
	marked = (char*)malloc(blockSize*sizeof(char));
	
	for(long long int i=0;i<blockSize;i++)
	{
		marked[i] = '0';
	}
	
	memset(numbersReceived, 0, sizeof(numbersReceived));

	
	int q =0;
	long long int lastUnmarked=-1;
	long long int lastUnmarkedIndex = 0;


	while(prime != -1 && prime<=sqrtN)
	{	

		//Condition imposed to ignore initial case
		if(q!=0)
		{
			//Gather First unmarked number in each block from all processes 
			err = MPI_Gather(&(lastUnmarked), 1, MPI_LONG_LONG, numbersReceived, 1, MPI_LONG_LONG, rootProcess, MPI_COMM_WORLD);

			//Root process will broadcast the minimum to every process 
			if (processId == rootProcess)
			{
				int allNull = 1;
				for(int i=0 ;i <noOfProcesses;i++)
				{
					if(numbersReceived[i]!=-1 && allNull==1)
					{	
						prime = numbersReceived[i];
						allNull = 0;	
					}
					else if(numbersReceived[i]!=-1)
					{
						prime = min(numbersReceived[i],prime);
					}
				}

				//If -1 is received from all processes, broadcast -1
				if(allNull==1)
				{
					prime = -1;
				}

				// Printing the numbers received from all processes
				// for(int i=0;i<(noOfProcesses);i++)
				// {
				// 	cout << numbersReceived[i] << " ";
				// }
				// cout << endl;
			}
			
			//Broadcast the prime number to all the processes
			err =  MPI_Bcast(&prime, 1, MPI_LONG_LONG, rootProcess, MPI_COMM_WORLD);
		}
		
		// cout << "Entered " << processId << endl;
		if(prime != (-1) && prime<=sqrtN)
		{
			//If the prime number is less than the block's end, then only proceed --- otherwise no need to process the 
			//blocks with upper limit less than the prime number, since it won't have any multiples of the prime number
			if(prime <= (processId*blockSize+2+blockSize-1))
			{

				//proceed only after lastunmarked index
				for(long long int i=lastUnmarkedIndex;i<blockSize;i++)
				{

					//If the current number goes beyond N, stop the marking phase
					if((i+processId*blockSize+2) > n)
					{
						break;
					}

					//If the current number is a multiple of prime, then mark it
					if(((i+processId*blockSize+2) % prime) == 0)
						marked[i] = '1';

					//Unmark the prime number, which got marked in the previous step
					if((i+processId*blockSize+2) == prime)
					{
						marked[i] = '0';
						lastUnmarkedIndex++;
					}
				}
			}

			//Again initialize lastUnmarked to -1
			lastUnmarked = -1;	


			if((prime <= (processId*blockSize+2+blockSize-1)) && ((processId*blockSize+2) <= sqrtN))
			{
				for(long long int i=lastUnmarkedIndex;i<blockSize;i++)
				{

					//If the current number goes beyond N, stop finding the last unmarked
					if((i+processId*blockSize+2) > n)
					{
						break;
					}

					//If unmarked is found
					if(marked[i] == '0') 
					{
						lastUnmarked = i+processId*blockSize+2;
						lastUnmarkedIndex = i;
						break;
					}
				}
			}
			else
			{
				lastUnmarked = -1;
			}		
		}
		// cout << "Exit " << processId << endl;		
		q=1;	
	}

	MPI_Barrier(MPI_COMM_WORLD);
	// end = clock();
	// elapsedTime = ((double) (end - start)) / CLOCKS_PER_SEC;
	elapsedTime += MPI_Wtime();

	// //Total size of gathered array = sum of sizes of marked arrays from all the processes
	// char* isPrime = (char*)malloc(sizeof(char)*noOfProcesses*blockSize);


	// //Gather Data from Everywhere
	// err = MPI_Gather(marked, blockSize, MPI_CHAR, isPrime, blockSize, MPI_CHAR, rootProcess, MPI_COMM_WORLD);


	// //Printing the Prime Numbers
	if(1)
	{
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
	}

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}