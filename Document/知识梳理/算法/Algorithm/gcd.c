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

/*
*find return 1.
*ubable to find return 0.
*/

int gcd(int x, int y)
{
	//printf("Start :%d \t End :%d\n", start, end);
	//print_result(end-start+1, start, end, array); 
	int r = x % y;
	
	while(r) {
		x = y;
		y = r;
		r = x % y;
	} 
	
	return y;		
}


//#define NUM	10
void fast(int n, int start, int end, int array[])
{	
	//printf("N :%d \t Start :%d \t End :%d\n", n, start, end);
	//print_result(n, start, end, array);
	
	if (start < end) {		
		int ref = array[start];
		int ref_pos = start;
		int temp;
		int i;
		int finish = 0;
		/*
		*The position at which the reference value starts is determined as the array[less_n]
		*The number smaller than the reference value is less_n(0~(less_n-1))
		*The position after the replacement is the array [start ~ (start + less_n-1)]
		*The number larger than the reference value is more_n(0~(more-1))
		*The position after the replacement is the array[end-(more-1) ~ end]
		*/
		while(!finish) {
		for (i = end; i >= start; i--) {
			if (array[i] < ref) {
				if (i < ref_pos) {
					finish = 1;
					//printf("finish!\n");
					break;
				}
				temp = array[i];
				array[i] = array[ref_pos];
				array[ref_pos] = temp;
				ref_pos = i;
				break;
			}
		}
		
		for (i = start; i < start+n; i++) {
			if (array[i] > ref) {
				if (i > ref_pos) {
					finish = 1;
					//printf("finish!\n");
					break;
				}
				temp = array[i];
				array[i] = array[ref_pos];
				array[ref_pos] = temp;
				ref_pos = i;

				break;
			}
		}
		
		if (array[start] == array[end])
			finish = 1;
		}
		
		//print_result(n, start, end, array);
		//print_result(10, 0, 0, array);
		
		fast(ref_pos-start,start,ref_pos-1,array);
		fast(end-ref_pos,ref_pos+1,end,array);
	}
}

void fast_sort(int n, int min, int max, int array[], int direction)
{	
	fast(n, 0, n-1, array);
	print_result(n, 0, 0, array);
}

#define N	2
#define MIN	0
#define MAX	100
#define SORT_DIRCT	H2L

int test[5] = { 4, 7, 3, 9, 6};

int main(int argc, char **argv)
{
    int rand_data[N];
	
	int GCD;

    get_rand(N,MIN,MAX,rand_data);

    //fast_sort(N,MIN,MAX,rand_data,SORT_DIRCT);
	fast_sort(N,MIN,MAX,rand_data,SORT_DIRCT);
	
	//printf("Please enter the value you want to find:");
	//scanf("%d", &input);
	
	GCD = gcd(rand_data[1],rand_data[0]);
	printf("The greatest common divisors of %d and %d are: %d\n", rand_data[1],rand_data[0],GCD);

    return 0;
}
