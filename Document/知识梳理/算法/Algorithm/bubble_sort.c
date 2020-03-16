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
 * use bubble sorting algorithm, and print results.
 * int n:	Number of data
 * int min;	Data minimum
 * int max;	Data maximum
 * int array[] 	Data array pointer
 * int direction	sort direction
 */

#define L2H	0
#define H2L	1

int find_max(int n, int array[])
{
	int temp;
	int ret;
	int i;
	
	for (i = 0; i < (n-1); i++) {
		if (array[i] > array[i+1]) {
			temp = array[i];
			array[i] = array[i+1];
			array[i+1] = temp;
		}
	}

	ret = array[n-1];

	return ret;
}

static void print_result(int n, int array[])
{
	int i;
	for (i = 0; i < n; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

void bubble_sort(int n, int min, int max, int array[], int direction)
{
	int sorted[n];
	int unsorted[n];
	int *p;
	int i, num;
	int sorted_num = 0;
	int unsorted_num = n;

	p = array;
	num = n;

	for (i = 0; i < n; i++) {
		unsorted[i] = array[i];
	}


	/*bubble*/
	for (i = 0; i < n; i++) {
		find_max(unsorted_num,unsorted);
		printf("%dst sort:\n", i+1);
		print_result(n,unsorted);
		printf("\n");
	}

}

#define N	10
#define MIN	30
#define MAX	100
#define SORT_DIRCT	H2L

int main(int argc, char **argv)
{
    int rand_data[N];

    get_rand(N,MIN,MAX,rand_data);

    bubble_sort(N,MIN,MAX,rand_data,SORT_DIRCT);

    return 0;
}
