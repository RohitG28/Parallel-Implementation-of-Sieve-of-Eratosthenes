#include "mpi.h"
#include <iostream>
#include <math.h>

#define FILE_BLOCK_SIZE 4096

using namespace std;

int main(int argc, char** argv)
{	
	//Per process array of marked numbers
	char marked[FILE_BLOCK_SIZE];

	//prime initialized to 2
	long long prime = 2;

	double elapsedTime = -MPI_Wtime();	

	//variable N
	long long n = 100;

	long long sqrtN = ceil((double)sqrt(n));

	long long blockSize; 

	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	//Let every process come execute MPI_Init
	err = MPI_Barrier(MPI_COMM_WORLD);

	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);

	//Size of range given to each process
	blockSize = ceil((double)n/(noOfProcesses-1));
	
	int rootProcess = 0;

	//Root process receives primes from each process
	long long* numbersReceived = (long long*)malloc((noOfProcesses)*sizeof(long long));

	// Every number is unmarked initially
	for(int i=0;i<FILE_BLOCK_SIZE;i++)
	{
		marked[i] = '0';
	}
	
	maxProcessIDLength = 10;
	if(processId != rootProcess)
	{
		char s[maxProcessIDLength]; 
		sprintf(s, "%d", processId);
		FILE* fp = fopen(s,"ab+");
		long long noOfIterations = blockSize/FILE_BLOCK_SIZE;
		for(long long i=0;i<noOfIterations;i++)
		{
			fwrite(marked,sizeof(char),FILE_BLOCK_SIZE,fp);
		}
		long long remainingCharsToWrite = (blockSize - FILE_BLOCK_SIZE*noOfIterations);
		if(remainingCharsToWrite > 0)
			fwrite(marked,sizeof(char),remainingCharsToWrite,fp);
	}

	memset(numbersReceived, 0, sizeof(numbersReceived));
	
	long long q =0;
	long long lastUnmarked=-1;
	long long lastUnmarkedIndex = 0;
	int nread;
	long long j;

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
				for(int i=1 ;i <noOfProcesses;i++)
				{
					if(numbersReceived[i]!=-1 && allNull==1)
					{
						
						prime = numbersReceived[i];
						allNull = 0;
						
					}
					else if(numbersReceived[i]!=-1){
						prime = min(numbersReceived[i],prime);
					}
				}

				//If -1 is received from all processes, broadcast -1
				if(allNull==1)
				{
					prime = -1;
				}

				//Printing the numbers received from all processes
				for(int i=1;i<(noOfProcesses);i++)
				{
					cout << numbersReceived[i] << " ";
				}
				cout << endl;
			}
			
			//Broadcast the prime number to all the processes
			err =  MPI_Bcast(&prime, 1, MPI_LONG_LONG, rootProcess, MPI_COMM_WORLD);
		}
		
		if(prime != (-1) && prime<=sqrtN)
		{

			//All processes except the root process do the following job
			if(processId!=rootProcess)
			{

				//If the prime number is less than the block's end, then only proceed --- otherwise no need to process the 
				//blocks with upper limit less than the prime number, since it won't have any multiples of the prime number
				if(prime <= ((processId-1)*blockSize+2+blockSize-1))
				{
					j = 0;
					//proceed only after lastunmarked index
					fseek(fp,lastUnmarkedIndex,SEEK_SET);
					while(1)
					{
						j++;
						nread = fread(marked,sizeof(char),FILE_BLOCK_SIZE,fp);
						for(long long i=0;i<nread;i++)
						{

							//If the current number goes beyond N, stop the marking phase
							if((i*j+(processId-1)*blockSize+2) > n)
							{
								break;
							}

							//If the current number is a multiple of prime, then mark it
							if(((i*j+(processId-1)*blockSize+2) % prime) == 0)
								marked[i] = '1';

							//Unmark the prime number, which got marked in the previous step
							if((i*j+(processId-1)*blockSize+2) == prime)
							{
								marked[i] = '0';
								lastUnmarkedIndex++;
							}
						}
						fseek(fp,(-nread),SEEK_CUR);
						fwrite(marked,sizeof(char),nread,fp);
						if(nread < FILE_BLOCK_SIZE)
							break;
					}
					fseek(fp,0,SEEK_SET);
				}

				//Again initialize lastUnmarked to -1
				lastUnmarked = -1;	


				if((prime <= ((processId-1)*blockSize+2+blockSize-1)) && (((processId-1)*blockSize+2) <= sqrtN))
				{
					j = 0;
					//proceed only after lastunmarked index
					fseek(fp,lastUnmarkedIndex,SEEK_SET);
					while(1)
					{
						j++;
						nread = fread(marked,sizeof(char),FILE_BLOCK_SIZE,fp);
						for(long long i=0;i<nread;i++)
						{

							//If the current number goes beyond N, stop finding the last unmarked
							if((i*j+(processId-1)*blockSize+2) > n)
							{
								break;
							}

							//If unmarked is found
							if(marked[i] == '0') 
							{
								lastUnmarked = i*j+(processId-1)*blockSize+2;
								lastUnmarkedIndex = i*j;
								break;
							}
						}
						if(nread < FILE_BLOCK_SIZE)
							break;
					}
					fseek(fp,0,SEEK_SET);
				}
				else
				{
					lastUnmarked = -1;
				}
			}	
		}		
		q++;	
	}
	
	//Total size of gathered array = sum of sizes of marked arrays from all the processes
	char* isPrime = (char*)malloc(sizeof(char)*noOfProcesses*blockSize);


	//Gather Data from Everywhere
	err = MPI_Gather(marked, blockSize, MPI_CHAR, isPrime, blockSize, MPI_CHAR, rootProcess, MPI_COMM_WORLD);

	elapsedTime += MPI_Wtime();

	//Printing the Prime Numbers
	if(processId==rootProcess)
	{
		int x = 1;

		cout<<"\n-----------Printing Prime Numbers-----------\n\n";

		for(int i=blockSize; i<((noOfProcesses)*blockSize);i++)
		{
			
			//Avoid going beyond N
			if(i-blockSize+2>n)
				break;


			//Only print the left unmarked symbols -- these are primes
			if(isPrime[i] == '0')
				cout<< x++ <<") " << i-blockSize+2 << endl;
		}

		cout<<"\n----------------Woah----------------\n";
	}

	if(processId == rootProcess)
		cout << "Elapsed Time:" << elapsedTime << endl;

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}