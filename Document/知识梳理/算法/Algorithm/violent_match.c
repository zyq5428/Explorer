#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int ViolentMatch(char* s, char* p)  
{  
    int sLen = strlen(s);  
    int pLen = strlen(p);  
  
    int i = 0;  
    int j = 0;  
    while (i < sLen && j < pLen)  
    {  
        if (s[i] == p[j])  
        {  
            //①如果当前字符匹配成功（即S[i] == P[j]），则i++，j++      
            i++;  
            j++;  
        }  
        else  
        {  
            //②如果失配（即S[i]! = P[j]），令i = i - (j - 1)，j = 0      
            i = i - j + 1;  
            j = 0;  
        }  
    }  
    //匹配成功，返回模式串p在文本串s中的位置，否则返回-1  
    if (j == pLen)  
        return i - j;  
    else  
        return -1;  
}  

int main(int argc, int *argv[])
{
	char *dest_str = "absdbsdafasdbssf";
	char key[30] = "sdbs";
	int position = 0;
	printf("%s\n", dest_str);
	printf("%s\n", key);
	
	//printf("Please enter the string your want to find:\n");
	
	//fgets(key, 30, stdin);
	
	position = ViolentMatch(dest_str, key);
	
	
	if (position == -1)
		printf("no string\n");
	else
		printf("The string appears in the following position: %d\n", position);
	
	return 0;
}