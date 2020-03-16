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

static void print_result(int n, int start, int end, int array[])
{
	int i;
	for (i = (start+0); i < (start+n); i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

#define NUM	10
void fast(int n, int start, int end, int array[])
{	
	printf("N :%d \t Start :%d \t End :%d\n", n, start, end);
	print_result(n, start, end, array);
	
	if (start < end) {		
		int less[n];
		int more[n];
		int less_n = 0;
		int more_n = 0;
		int i;
		/*
		*The position at which the reference value starts is determined as the array[less_n]
		*The number smaller than the reference value is less_n(0~(less_n-1))
		*The position after the replacement is the array [start ~ (start + less_n-1)]
		*The number larger than the reference value is more_n(0~(more-1))
		*The position after the replacement is the array[end-(more-1) ~ end]
		*/
		for (i = (start+1); i < (start + n); i++) {
			if (array[i] < array[start]) {
				less[less_n] = array[i];
				less_n++;
			} else {
				more[more_n] = array[i];
				more_n++;
			}
		}
		
		printf("less_n :%d ; more_n :%d \n", less_n, more_n);
		printf("less data\t:");
		print_result(less_n, 0, 0, less);
		printf("more data\t:");
		print_result(more_n, 0, 0, more);
		
		array[start+less_n] = array[start];
		
		for (i = 0; i < less_n; i++) {
			array[start+i] = less[i];
		}
		
		for (i = 0; i < more_n; i++) {
			array[start+less_n+1+i] = more[i];
		}
		
		//printf("sort:");
		print_result(n, start, end, array);
		print_result(10, 0, 0, array);
		
		fast(less_n,start,start+less_n-1,array);
		fast(more_n,start+less_n+1,end,array);
	}
}

void fast_sort(int n, int min, int max, int array[], int direction)
{	
	fast(n, 0, n-1, array);
	print_result(n, 0, 0, array);
}

#define N	10
#define MIN	30
#define MAX	100
#define SORT_DIRCT	H2L

int main(int argc, char **argv)
{
    int rand_data[N];

    get_rand(N,MIN,MAX,rand_data);

    fast_sort(N,MIN,MAX,rand_data,SORT_DIRCT);

    return 0;
}
