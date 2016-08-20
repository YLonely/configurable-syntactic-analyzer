#include "stdafx.h"
extern production *pro_list;
extern element *token_list;
extern int transfer_index;


/*
The closure of one or more kernel productions
*/
set* closure(n_pro *pro)
{
	set *s = (set*)calloc(1, sizeof(set));
	s->transfer_table = (int*)calloc(transfer_index, sizeof(int));
	n_pro *first_p = pro;
	n_pro *last_p = pro;
	int type, first_arr_len;
	int kernel_num = 1;
	int *first_arr = (int*)malloc(2 * transfer_index*sizeof(int));
	void *pvoid = NULL;
	item *t = NULL;
	for (; last_p->next; last_p = last_p->next, kernel_num++);
	for (; first_p; first_p = first_p->next)
	{
		if (first_p->body_len == first_p->dot_pos)
			continue;
		pvoid = find_by_index(first_p->body[first_p->dot_pos], &type);
		if (type == TOKEN)
			continue;
		for (t = ((production*)pvoid)->items; t; t = t->next)
		{
			first_arr_len = first(first_p->body[first_p->dot_pos + 1], first_p->look_ahead, first_arr);
			for (int i = 0; i < first_arr_len; i++)
			{
				last_p->next = (n_pro*)malloc(sizeof(n_pro));
				last_p->next->head = first_p->body[first_p->dot_pos];
				memcpy(last_p->next->body, t->body, BODY_LENGTH);
				last_p->next->body_len = t->body_len;
				last_p->next->dot_pos = 0;
				last_p->next->look_ahead = first_arr[i];
				last_p->next->next = NULL;

				last_p = last_p->next;
			}
		}
	}
	s->pro_list = pro;
	s->kernel_num = kernel_num;
	return s;
}

/*
	The set that the program will go in when read a grammar symbol.
	More specifically, function _goto will figure out the kernel productions when read a symbol.
*/
n_pro* _goto(set *s, int symbol_index)
{
	n_pro* temp = s->pro_list;
	n_pro *first = NULL, *last = NULL;
	for (; temp; temp = temp->next)
	{
		if (temp->body[temp->dot_pos] == symbol_index)
		{
			if (!first)
			{
				first = (n_pro*)malloc(sizeof(n_pro));
				last = first;
				memcpy(first, temp, sizeof(n_pro));
				first->dot_pos++;
				first->next = NULL;
			} else
			{
				last->next = (n_pro*)malloc(sizeof(n_pro));
				memcpy(last->next, temp, sizeof(n_pro));
				last->next->dot_pos++;
				last->next->next = NULL;
				last = last->next;
			}
		}
	}
	return first;
}