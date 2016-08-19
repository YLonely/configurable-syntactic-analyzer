#include "stdafx.h"

extern production *pro_list;
extern element *token_list;
extern int transfer_index;

int first(int first_index, int sec_index, int *first_arr);
/*
	Patch two elements together.
	Patch e1 to the end of the e2.
*/
void add_e2e(element *e1, element *e2)
{
	element *temp = NULL;
	element *temp_ptr = e2;
	if (e1 == NULL)
	{
		exception("Empty e1 param error", NULL);
		return;
	}
	if (e2 == NULL)
	{
		e2 = (element*)calloc(1, sizeof(element));
		e2->is_terminator = e1->is_terminator;
		e2->type = e1->type;
		//e2->next = NULL;
	} else
	{
		temp = (element*)calloc(1, sizeof(element));
		temp->is_terminator = e1->is_terminator;
		temp->type = e1->type;
		//temp->next = NULL;
		for (; temp_ptr->next != NULL; temp_ptr = temp_ptr->next);
		temp_ptr->next = temp;
	}
}


/*
	Counting the terminator of FIRST(element)
	record it in first_elements
*/
void count_first(element *e, element *first_elements)
{
	item *temp;
	if (!e)
		return;
	if (e->is_terminator)
	{
		add_e2e(e, first_elements);
	} else
	{
		temp = e->type.pro->items;
		while (temp)
		{
			if (temp->ele->is_terminator&&temp->ele->type.t_index == EMPTY)
				count_first(e->next, first_elements);
			else
				count_first(temp->ele, first_elements);
			temp = temp->next;
		}
	}
}


void* find_by_index(int index, int *type)
{
	production *p = pro_list;
	element *e = token_list;
	element *temp = NULL;
	for (; p; p = p->next)
	{
		if (p->p_index == index)
		{
			*type = PRODUCTION;
			return p;
		}
	}
	for (; e; e = e->next)
	{
		if (e->type.t_index == index&&e->is_terminator)
		{
			*type = TOKEN;
			return temp;
		}
	}
	return NULL;
}


/*
	The closure of one or more kernel productions
*/
set* closure(n_pro *pro)
{
	set *s = (set*)calloc(1, sizeof(set));
	n_pro *first_p = pro;
	n_pro *last_p = pro;
	int type, first_arr_len;
	int *first_arr = (int*)malloc(2 * transfer_index*sizeof(int));
	void *pvoid = NULL;
	item *t = NULL;
	for (; last_p->next; last_p = last_p->next);
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
	return s;
}


element* find_used_by_first(int index)
{
	element *temp = NULL;
	void* pvoid = NULL;
	int type;
	pvoid = find_by_index(index, &type);
	if (type == PRODUCTION)
	{
		temp = (element*)calloc(1, sizeof(element));
		//temp->next = NULL;
		temp->type.pro = (production*)pvoid;
		return temp;
	} else if (type == TOKEN)
	{

		temp = (element*)calloc(1, sizeof(element));
		temp->is_terminator = TRUE;
		//temp->next = NULL;
		temp->type = ((element*)pvoid)->type;
		strcpy(temp->terminator_name, ((element*)pvoid)->terminator_name);
		return temp;
	}
	return NULL;
}



/*
	Return the first elements of the element
*/
int first(int first_index, int sec_index, int *first_arr)
{
	element *first_elements = NULL, *e = NULL;
	element *temp = NULL;
	int num;
	if (!first_index)
		e = find_used_by_first(sec_index);
	else if (!sec_index)
		e = find_used_by_first(first_index);
	else
	{
		e = find_used_by_first(first_index);
		e->next = find_used_by_first(sec_index);
	}
	count_first(e, first_elements);
	for (num = 0; temp; temp = temp->next, num++)
		first_arr[num] = temp->type.t_index;

	while (first_elements)
	{
		temp = first_elements;
		first_elements = first_elements->next;
		free(temp);
	}


	free(e);
	return num;
}


