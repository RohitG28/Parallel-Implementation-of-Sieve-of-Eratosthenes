#include "mpi.h"
#include <iostream>

//For using ceil function
#include <math.h>
#include <stdio.h>

//For File I/O
#include <fstream>
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char** argv)
{	
	double elapsedTime;
	 
	int err, processId, noOfProcesses;	
	
	//Parallelization starts
	err = MPI_Init(&argc, &argv);

	//Time calculation starts
	elapsedTime = -MPI_Wtime();
	
	//Get Rank and Total no of processes
	err = MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	err = MPI_Comm_size(MPI_COMM_WORLD, &noOfProcesses);
	
	//Per process array of marked numbers
	//marked1 stores numbers from 2 to sqrt(N)
	//marked2 stores numbers from low to high assigned to each process
	char* marked1;
	char* marked2;

	//variable N
	long int n = atol(argv[1]);

	//Vector stores primes computed by the current process
	vector<long int> primes;
	
	//Square root of N
	long int sqrtN = ceil((double)sqrt(n));

	//If square root of N is even, make it odd by subtracting 1
	if(sqrtN%2 == 0)
	{
		sqrtN = sqrtN - 1;
	}

	//Number of elements to be assigned to each process
	long int blockSize;

	//Lowest, highest element assigned to a process and the remaining elements after division
	long int low,high,remainder;

	//Allocate required space
	marked1 = (char*)malloc((((sqrtN-3)/2)+1)*sizeof(char));

	blockSize = (n-(sqrtN))/(noOfProcesses);
	low = sqrtN + processId*blockSize + 1;
	high = low + blockSize - 1;
	remainder = (n-(sqrtN))%(noOfProcesses);

	//Put the remaining elements in the last process's block
	if(processId == noOfProcesses-1)
	{
		high += remainder;
	}

	//If first number assigned to a process is even then move to next odd number
	if(low%2 == 0)
		low++;

	//If last number assigned to a process is odd then move one number back
	if(high%2 == 0)
		high--;

	//Divide the marked2 array by 2 so as to remove all even numbers
	long int highIndex = (high-low)/2;
	long int sieveStart;
	long int j = 0; 

	marked2 = (char*)malloc((((high-low)/2)+1)*sizeof(char));

	int rootProcess = 0;

	//Initialize all numbers as prime
	memset(marked1, '0', (((sqrtN-3)/2)+1));
	memset(marked2, '0', (((high-low)/2)+1));

	//Files to store prime numbers
	ofstream primesFile, statFile;
	char processIdStr[3];
	sprintf(processIdStr,"%d",processId);
	
	//Let all processes reach this point 
	MPI_Barrier(MPI_COMM_WORLD);
	long int primeNo;

	if(processId == rootProcess)
	{
		//First push 2 into the primes array of root process
		primes.push_back(2);
	}

	//For each unmarked number in marked1, sieve marked1 as well as marked2 arrays thus marking non primes
	for(long int i=0; i<=(sqrtN-3)/2; i++)
	{
		if(marked1[i] == '0')
		{
			primeNo = i*2 + 3;

			if(processId == rootProcess)
			{
				primes.push_back(primeNo);
			}

			for(long int k=i+primeNo;k<=(sqrtN-3)/2;k+=primeNo)
			{
				marked1[k] = '1';
			}
			
			//Find the first number(obviously odd) divisible by the current prime in the process range
			sieveStart = (low/primeNo)*primeNo;
			
			if(sieveStart<low)
				sieveStart = sieveStart+primeNo;

			//If the number is even, again add the prime number to get its first odd multiple in the range
			if(sieveStart%2 == 0)
				sieveStart += primeNo;

			//The number from which sieve starts must be less than the last number in the range
			if(sieveStart <= high)
			{
				j = (sieveStart - low)/2;

				for(j;j<=highIndex;j+=primeNo)
				{
					marked2[j] = '1';
				}	
			}
		}
	}

	//Now push the prime numbers into primes array of each process
	for(long int i=0; i<=highIndex; i++)
	{
		if(marked2[i] == '0')
		{
			primes.push_back(low + i*2);
		}
	}

	//Time calculation done
	elapsedTime += MPI_Wtime();

	//Free the arrays
	free(marked1);
	free(marked2);

	MPI_Barrier(MPI_COMM_WORLD);

	//Write the primes into files
	primesFile.open (processIdStr, std::ofstream::out | std::ofstream::trunc);

	for(long int i=0; i<primes.size(); i++)
	{
		primesFile << primes[i] << "\n";
	}
	
	primesFile.close();

	printf("total time take by %d : %lf\n",processId,elapsedTime); 

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}
