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
 * use select sorting algorithm, and print results.
 * int n:	Number of data
 * int min;	Data minimum
 * int max;	Data maximum
 * int array[] 	Data array pointer
 * int direction	sort direction
 */

#define L2H	0
#define H2L	1

int find_min(int n, int array[])
{
	int now_min = 0;
	int compare = 0;
	int ret;
	int i;
	
	for (compare = 0; compare < n; compare++) {
		if (array[now_min] > array[compare])
			now_min = compare;
	}

	ret = array[now_min];

	/*rearrange*/
	for(i = now_min; i < (n-1); i++) {
		array[i] = array[i+1];
	}

	return ret;
}

static void print_result(int n, int array[])
{
	int i;
	for (i = 0; i < (n+1); i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

void select_sort(int n, int min, int max, int array[], int direction)
{
	int sorted[n];
	int unsorted[n];
	int *p;
	int i, num;

	p = array;
	num = n;

	for (i = 0; i < n; i++) {
		unsorted[i] = array[i];
	}

	/*into the select*/
	for (i = 0; i < n; i++) {
		sorted[i] = find_min(num,unsorted);
		printf("%dst sort:\n", i+1);
		printf("Sorted:\t");
		print_result(i,sorted);
		printf("unSorted:\t");
		print_result(n-2-i,unsorted);
		printf("\n");
		num--;
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

    select_sort(N,MIN,MAX,rand_data,SORT_DIRCT);

    return 0;
}
