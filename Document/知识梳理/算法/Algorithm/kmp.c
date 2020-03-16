/*
*用KMP算法实现字符串匹配搜索方法
*该程序实现的功能是搜索本目录下的所有文件的内容是否与给定的
*字符串匹配，如果匹配，则输出文件名：包含该字符串的行
*待搜索的目标串搜索指针移动位数 = 已匹配的字符数 - 对应部分匹配值
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void make_next(const char p[], int next[])
{
	int q, k;	//q:模版字符串下标；k:最大前后缀长度
	int m = strlen(p);	//模版字符串长度
	next[0] = 0;	//模版字符串的第一个字符的最大前后缀长度为0
 
	//for循环，从第二个字符开始，依次计算每一个字符对应的next值
	for(q = 1, k = 0; q < m; ++q) {	
		//递归的求出P[0]···P[q]的最大的相同的前后缀长度k
		while(k > 0 && p[q] != p[k]) {
			k = next[k-1];
		}
		//如果相等，那么最大相同前后缀长度加1
		if (p[q] == p[k]) {
			k++;
		}	
		next[q] = k;
	}
}

int kmp(const char T[],const char P[],int next[])
{
    int n,m;
    int i,q;
    n = strlen(T);
    m = strlen(P);
    make_next(P,next);
    for (i = 0,q = 0; i < n; ++i)
    {
        while(q > 0 && P[q] != T[i])
            q = next[q-1];
        if (P[q] == T[i])
        {
            q++;
        }
        if (q == m)
        {
            printf("The string appears in the %dth character of the target string\n",(i-m+1));
        }
    }    
}

int main(int argc, int *argv[])
{
     int i;
     int next[20]={0};
     char T[] = "ababxbababababcadababfdsss";
     char P[] = "abab";
     printf("%s\n",T);
     printf("%s\n",P );
     // makeNext(P,next);
     kmp(T,P,next);
     for (i = 0; i < strlen(P); ++i)
     {
         printf("%d ",next[i]);
     }
     printf("\n");
 
     return 0;
}






