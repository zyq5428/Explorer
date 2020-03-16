/*
 * Generate random numbers based on runtime
 * int N:	Number of data
 * int min;	Data minimum
 * int max;	Data maximum
 * return 	Data array pointer
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(void)
{
	int N,i;
	int min, max;
	int rand_data[N];
	int data;

	N = 10;
	min = 2;
	max = 10;

	srand((unsigned)time(NULL));
	
	for (i = 0; i < N; i++) {
		//rand_data[i] = rand() % (max - min + 1) + min;
		//printf("%d\n", rand_data[i]);
		data = (rand()) % (max - min + 1) + min;
		printf("%d\n", data);
	}

//	printf("%d\n", rand() % (max - min + 1) + min);
	return 0;
}
