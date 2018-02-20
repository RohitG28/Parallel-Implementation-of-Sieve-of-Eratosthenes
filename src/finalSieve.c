#include "mpi.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
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

	vector<long int> primes;
	
	long int sqrtN = ceil((double)sqrt(n));

	if(sqrtN%2 == 0)
	{
		sqrtN = sqrtN - 1;
	}

	long int blockSize;
	long int low,high,remainder;

	marked1 = (char*)malloc((((sqrtN-3)/2)+1)*sizeof(char));

	//Size of range given to each process
	blockSize = (n-(sqrtN))/(noOfProcesses);
	low = sqrtN + processId*blockSize + 1;
	high = low + blockSize - 1;
	remainder = (n-(sqrtN))%(noOfProcesses);

	if(processId == noOfProcesses-1)
	{
		high += remainder;
	}

	//if first number assigned to a process is even then move to next odd number
	if(low%2 == 0)
		low++;

	//if last number assigned to a process is odd then move one number back
	if(high%2 == 0)
		high--;

	//divide the marked2 array by 2 so as to remove all even numbers
	long int highIndex = (high-low)/2;
	long int sieveStart;
	long int j = 0; 

	marked2 = (char*)malloc((((high-low)/2)+1)*sizeof(char));

	int rootProcess = 0;

	//initialize all numbers to be prime
	memset(marked1, '0', (((sqrtN-3)/2)+1));
	memset(marked2, '0', (((high-low)/2)+1));

	ofstream primesFile, statFile;
	char processIdStr[3];
	sprintf(processIdStr,"%d",processId);
	
	//let all processes reach this point 
	MPI_Barrier(MPI_COMM_WORLD);
	long int primeNo;

	//mark all even numbers except 2 in marked1 as non prime 
	// for(long int k=4;k<=sqrtN;k+=2)
	// {
	// 	marked1[k] = '1';
	// }

	if(processId == rootProcess)
	{
		primes.push_back(2);
	}

	//for each unmarked number in marked1 sieve marked1 as well as marked2 arrays thus marking non primes
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
			
			//find the first number(obviously odd) divisible by the current prime in the process range
			sieveStart = (low/primeNo)*primeNo;
			
			if(sieveStart<low)
				sieveStart = sieveStart+primeNo;

			//if the number is even, again add the prime number to get its first odd multiple in the range
			if(sieveStart%2 == 0)
				sieveStart += primeNo;

			//the number from which sieve starts must be less than the last number in the range
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

	for(long int i=0; i<=highIndex; i++)
	{
		if(marked2[i] == '0')
		{
			primes.push_back(low + i*2);
		}
	}
	elapsedTime += MPI_Wtime();

	free(marked1);
	free(marked2);

	MPI_Barrier(MPI_COMM_WORLD);

	primesFile.open (processIdStr, std::ofstream::out | std::ofstream::trunc);

	for(long int i=0; i<primes.size(); i++)
	{
		primesFile << primes[i] << "\n";
	}
	
	primesFile.close();

	if(processId == rootProcess)
	{
		statFile.open ("statfile", std::ofstream::out | std::ofstream::trunc);
		statFile << n << "\n";
		statFile << noOfProcesses << "\n";
		statFile << elapsedTime << "\n";
		statFile.close();
	}

	printf("total time take by %d : %lf\n",processId,elapsedTime); 

	//Parallel Code over
	err = MPI_Finalize();

	return 0;
}
