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

static void print_result(int n, int array[])
{
	int i;
	for (i = 0; i < n; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

/*
 * use merge sorting algorithm, and print results.
 * int n:	Number of data
 * int min;	Data minimum
 * int max;	Data maximum
 * int array[] 	Data array pointer
 * int direction	sort direction
 */

#define L2H	0
#define H2L	1

void merge(int start, int mid, int end, int array[])
{
	int n1 = mid - start + 1;
    int n2 = end - mid;
    int left[n1], right[n2];
    int i, j, k;
	
	for (i = 0; i < n1; i++)	/* left holds a[start..mid] */
		left[i] = array[start + i];
		
	for (j = 0; j < n2; j++)	/* right holds a[mid+1..end] */
		right[j] = array[mid + 1 + j];
		
	i = j = 0;
	
	for (k = start; i < n1 && j < n2; k++) {
		if (left[i] < right[j]) {
			array[k] = left[i];
			i++;
		} else {
			array[k] = right[j];
			j++;
		}
    }
	
    if (i < n1)			/* left[] is not exhausted */
	for (; i < n1; i++) {
	    array[k] = left[i];
	    k++;
	}
	
    if (j < n2)			/* right[] is not exhausted */
	for (; j < n2; j++) {
	    array[k] = right[j];
	    k++;
	}
}

int sort(int n, int start, int end, int array[])
{
	int mid;
	int ret;
	int i;
	
	if (start < end) {
		mid = (start + end) / 2;
		printf("sort (%d-%d, %d-%d)\n", start, mid, mid+1, end);
		print_result(n, array);
		sort(n,start,mid,array);
		sort(n,mid+1,end,array);
		merge(start,mid,end,array);
		printf("merge (%d-%d, %d-%d)\n", start, mid, mid+1, end);
		print_result(n, array);
	}
}

void merge_sort(int n, int min, int max, int array[], int direction)
{
	sort(n, 0, n-1, array);
}

#define N	10
#define MIN	30
#define MAX	100
#define SORT_DIRCT	H2L

int main(int argc, char **argv)
{
    int rand_data[N];

    get_rand(N,MIN,MAX,rand_data);

    merge_sort(N,MIN,MAX,rand_data,SORT_DIRCT);

    return 0;
}
