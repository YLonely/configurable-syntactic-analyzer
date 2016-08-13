#pragma once
#include "constant.h"
struct _production
{
	char head[20];
	struct _item *items;
	struct _element *look_ahead;
};

struct _item
{
	struct _element *ele;
	int dot_pos;
	struct _item *next;
};

struct _element
{
	union
	{
		int terminator;
		struct _production *pro;
	}type;
	boolean is_terminator;
	struct _element *next;
};


/*
	The struct that represent the set of items
*/
struct _set
{

	struct _production *pro_arr;
	//A table that records the transfer path when the set read a terminator 
	int *transfer_table;
	//The totle number of productions in the set
	int num;
};


typedef struct _element element;
typedef struct _item item;
typedef struct _production production;
typedef struct _set set;
