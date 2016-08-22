#include "stdafx.h"
extern production *pro_list;
extern element *token_list;
extern int transfer_index;
extern int token_num;

set* sets_arr[100];
int sets_num = 0;
int ending_set = 0;
/*
The closure of one or more kernel productions
*/
set* closure(set *se)
{
	se->transfer_table = (int*)calloc(transfer_index, sizeof(int));
	n_pro *first_p = se->pro_list;
	n_pro *last_p = se->pro_list;
	int type, first_arr_len;
	//int kernel_num = 1;
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
				last_p->next->body = (int*)calloc(BODY_LENGTH, sizeof(int));
				memcpy(last_p->next->body, t->body, BODY_LENGTH);
				last_p->next->body_len = t->body_len;
				last_p->next->dot_pos = 0;
				last_p->next->look_ahead = first_arr[i];
				last_p->next->next = NULL;

				last_p = last_p->next;
			}
		}
	}
	free(first_arr);
	return se;
}

/*
	The set that the program will go in when read a grammar symbol.
	More specifically, function _goto will figure out the kernel productions when read a symbol.
*/
set* _goto(set *s, int symbol_index)
{
	n_pro* temp = s->pro_list;
	n_pro *first = NULL, *last = NULL;
	set* se = NULL;
	int kernel_num = 0;
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

				kernel_num++;
			} else
			{
				last->next = (n_pro*)malloc(sizeof(n_pro));
				memcpy(last->next, temp, sizeof(n_pro));
				last->next->dot_pos++;
				last->next->next = NULL;
				last = last->next;

				kernel_num++;
			}
		}
	}
	if (kernel_num)
	{
		se = (set*)malloc(sizeof(set));
		se->kernel_num = kernel_num;
		se->pro_list = first;
		se->transfer_table = NULL;
	}
	return se;
}


boolean setcmp(set *s1, set *s2)
{
	n_pro *temp1 = s1->pro_list, *temp2 = s2->pro_list;

	if (s1->kernel_num != s2->kernel_num)
		return FALSE;
	for (int i = 0; i < s1->kernel_num; i++, temp1 = temp1->next, temp2 = temp2->next)
	{
		if (temp1->body_len != temp2->body_len || temp1->dot_pos != temp2->dot_pos || temp1->head != temp2->head || temp1->look_ahead != temp2->look_ahead)
			return FALSE;
		for (int j = 0; j < temp1->body_len; j++)
		{
			if (temp1->body[j] != temp2->body[j])
				return FALSE;
		}
	}
	return TRUE;
}

int is_contain(set *s)
{
	for (int i = 0; i < sets_num; i++)
	{
		if (setcmp(s, sets_arr[i]))
			return i;
	}
	return -1;
}

int add_set(set *s)
{
	int num;
	if ((num = is_contain(s)) != -1)
		return num;
	sets_arr[sets_num++] = s;
	if (sets_num > sizeof(sets_arr) / sizeof(set*))
		exception("Array out range error", "add_set");

	return sets_num - 1;
}




/*
	This function figure out all the set for a specific grammar.
*/
void sets()
{
	int goto_index;
	set* closure_s = NULL;
	set* goto_s = NULL;
	/*
		Add [S'-->¡¤S,$] into the sets_arr.
	*/
	set* begin = (set*)malloc(sizeof(set));
	begin->kernel_num = 1;
	begin->pro_list = (n_pro*)malloc(sizeof(n_pro));
	begin->pro_list->body = (int*)calloc(BODY_LENGTH, sizeof(int));
	begin->pro_list->body[0] = token_num + 1;
	begin->pro_list->body_len = 1;
	begin->pro_list->dot_pos = 0;
	begin->pro_list->head = token_num;
	begin->pro_list->look_ahead = STRING_END;
	begin->pro_list->next = NULL;
	add_set(begin);

	for (int i = 0; i < sets_num; i++)
	{
		closure_s = closure(sets_arr[i]);
		for (int j = 1; j < transfer_index; j++)
		{
			goto_s = _goto(closure_s, j);
			if (!goto_s)
				continue;
			goto_index = add_set(goto_s);
			if (i == 0 && j == token_num + 1)
				ending_set = goto_index;
			if (sets_arr[i]->transfer_table[j])
				exception("This grammar is not LR(1).", NULL);
			sets_arr[i]->transfer_table[j] = goto_index + 2;
			/*
				Because in the transfer table, 0 means error and 1 means match successfully.
			*/

		}
	}
	sets_arr[ending_set]->transfer_table[STRING_END] = 1;

}


void yacc_init(char *file_path)
{
	FILE *fp = NULL;
	if (!(fp = fopen(file_path, "r")))
		exception("Failed to open the file.", NULL);
	grammar_load(fp);
	fclose(fp);
	sets();
}