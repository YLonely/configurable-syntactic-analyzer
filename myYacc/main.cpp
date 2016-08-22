// myYacc.cpp : 定义控制台应用程序的入口点。
// A simple Parsing based on LR(1).

#include "stdafx.h"
extern set* sets_arr[100];
extern int sets_num;

int main()
{
	yacc_init("D:/Personal/Desktop/yacc.txt");
	//printf("\t0 $ c d S1 S C\n");
	for (int i = 0; i < sets_num; i++)
	{
		printf("I%d\t", i);
		for (int j = 0; j <= 6; j++)
		{
			printf("%5d", sets_arr[i]->transfer_table[j]);
		}
		putchar(10);
	}
	getchar();
	return 0;
}

