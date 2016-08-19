#pragma once
#include "constant.h"
struct _production
{
	char head[20];
	int p_index;
	struct _item *items;
	struct _production *next;
};

struct _item
{
	struct _element *ele;
	struct _item *next;
	int *body;
	int body_len;
};

struct _element
{
	union
	{
		int t_index;
		struct _production *pro;
	}type;
	boolean is_terminator;
	char terminator_name[20];
	struct _element *next;
};


typedef struct _n_pro
{
	int head;
	int *body;
	int body_len;
	int look_ahead;
	int dot_pos;
	struct _n_pro *next;
}n_pro;
//new production


/*
	The struct that represent the set of items
*/
struct _set
{

	n_pro *pro_list;

	//A table that records the transfer path when the set read a terminator 
	int *transfer_table;

	//The total number of productions in the set
	int list_len;

	struct _set *next;
};


typedef struct _element element;
typedef struct _item item;
typedef struct _production production;
typedef struct _set set;


set* closure(n_pro *pro);