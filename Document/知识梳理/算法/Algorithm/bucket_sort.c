#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*
 * Generate random numbers based on runtime, and print.
 * int n:	Number of data
 * int min;	Data minimum
 * int max;	Data maximum
 * int array[] 	Data array pointer
 */

void get_rand(int n, int min, int max, int array[])
{
    int i;

    srand((unsigned int) time(NULL));

    for (i = 0; i < n; i++) {
	array[i] = rand()%(max-min+1)+min;

	printf("%d ", array[i]);
    }
    printf("\n");
}

/*
 * use bucket sorting algorithm, and print results.
 * int n:	Number of data
 * int min;	Data minimum
 * int max;	Data maximum
 * int array[] 	Data array pointer
 * int direction	sort direction
 */

#define L2H	0
#define H2L	1

void bucket_sort(int n, int min, int max, int array[], int direction)
{
	int bucket[max];
	int value;
	int i;

	/*init bucket*/
	for (i = 0; i <= max; i++) {
		bucket[i] = 0;
	}

	/*into the bucket*/
	for (i = 0; i < n; i++) {
		value = array[i];
		bucket[value]++;
	}

	/*print result*/
	printf("bucket sort result: \n");
	if (direction == L2H) {
		for (i = 0; i <= max; i++) {
			while(bucket[i]) {
				printf("%d ", i);
				bucket[i]--;
			}
		}
	} else {
		for (i = max; i >= 0; i--) {
			while(bucket[i]) {
				printf("%d ", i);
				bucket[i]--;
			}
		}
	}
	printf("\n");
}

#define N	10
#define MIN	30
#define MAX	100
#define SORT_DIRCT	H2L

int main(int argc, char **argv)
{
    int rand_data[N];

    get_rand(N,MIN,MAX,rand_data);

    bucket_sort(N,MIN,MAX,rand_data,SORT_DIRCT);

    return 0;
}
