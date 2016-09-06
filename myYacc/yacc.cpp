#include "stdafx.h"
#include "lex.h"

extern production *pro_list;
extern element *token_list;
extern int transfer_index;
extern int token_num;

extern int lex_lines;

set* sets_arr[1000];
int sets_num = 0;
int ending_set = 0;

char **name_record_arr = NULL;

boolean procmp(n_pro *p1, n_pro *p2)
{
	if (p1->body_len != p2->body_len || p1->dot_pos != p2->dot_pos || p1->head != p2->head || p1->look_ahead != p2->look_ahead)
		return FALSE;
	for (int i = 0; i < p1->body_len; i++)
	{
		if (p1->body[i] != p2->body[i])
			return FALSE;
	}
	return TRUE;
}


boolean pro_is_contain(n_pro *pro_list, n_pro *p)
{
	n_pro *temp = NULL;
	for (temp = pro_list; temp; temp = temp->next)
		if (procmp(temp, p))
			return TRUE;
	return FALSE;
}

//char change(int index)
//{
//	switch (index)
//	{
//	case 1:
//		return '$';
//	case 2:
//		return 'a';
//	case  3:
//		return 'b';
//	case  4:
//		return 'c';
//	case 5:
//		return 'd';
//	case 6:
//		return '@';
//	case 7:
//		return 'S';
//	case 8:
//		return 'B';
//	case 9:
//		return 'A';
//	default:
//		break;
//	}
//}



//void test_print(n_pro *temp)
//{
//	int i, j;
//	char str[30];
//	str[0] = change(temp->head);
//	str[1] = '-';
//	str[2] = '>';
//	str[3 + temp->dot_pos] = '.';
//	for (i = 3, j = 0; j < temp->body_len; i++, j++)
//	{
//		if (str[i] == '.')
//		{
//			j--;
//			continue;
//		} else
//			str[i] = change(temp->body[j]);
//	}
//	str[i++] = ',';
//	str[i++] = change(temp->look_ahead);
//	str[i] = 0;
//	printf("%s\n", str);
//}

/*
The closure of one or more kernel productions
*/
set* closure(set *se)
{
	//int test = 0;

	//se->transfer_table = (int*)calloc(transfer_index, sizeof(int));
	n_pro *first_p = se->pro_list;
	n_pro *last_p = se->pro_list;
	n_pro *temp = NULL;

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
		first_arr_len = first(first_p->body[first_p->dot_pos + 1], first_p->look_ahead, first_arr);
		for (t = ((production*)pvoid)->items; t; t = t->next)
		{
			for (int i = 0; i < first_arr_len; i++)
			{
				temp = (n_pro*)malloc(sizeof(n_pro));
				temp->head = first_p->body[first_p->dot_pos];
				temp->body = (int*)malloc(BODY_LENGTH*sizeof(int));
				memcpy(temp->body, t->body, BODY_LENGTH);
				temp->body_len = t->body_len;
				temp->dot_pos = 0;
				temp->look_ahead = first_arr[i];
				temp->next = NULL;
				if (pro_is_contain(se->pro_list, temp))
				{
					free(temp->body);
					free(temp);
				} else
				{
					last_p->next = temp;
					last_p = last_p->next;
				}

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
		if (temp->body[temp->dot_pos] == symbol_index&&temp->dot_pos != temp->body_len)
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
		se->transfer_table = (int*)calloc(transfer_index, sizeof(int));

		temp = first;
		for (int i = 0; i < kernel_num; i++, temp = temp->next)
		{
			if (temp->body_len == temp->dot_pos&&temp->head != token_num)
				se->transfer_table[temp->look_ahead] = -(i + 1);
		}
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
	{
		free(s->transfer_table);
		free(s);
		return num;
	}
	sets_arr[sets_num++] = s;
	if (sets_num > sizeof(sets_arr) / sizeof(set*))
		exception("Array out of range error", "add_set");

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
	begin->transfer_table = (int*)calloc(transfer_index, sizeof(int));
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
		for (int j = 0; j < transfer_index; j++)
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


void name_record()
{
	int type;
	void *p = NULL;
	name_record_arr = (char**)malloc(transfer_index*sizeof(char*));
	for (int i = 0; i < transfer_index; i++)
	{
		*(name_record_arr + i) = (char*)malloc(20 * sizeof(char));
		p = find_by_index(i, &type);
		if (type == TOKEN)
			strcpy(*(name_record_arr + i), ((element*)p)->terminator_name);
		else if (type == PRODUCTION)
			strcpy(*(name_record_arr + i), ((production*)p)->head);
	}
	//mem_release();
}


void yacc_init(char *grammar_file_path, char *regex_file_path, char *code_file_path)
{
	FILE *fp = NULL;
	if (!(fp = fopen(grammar_file_path, "r")))
		exception("Failed to open the file.", NULL);
	grammar_load(fp);
	fclose(fp);
	sets();
	name_record();
	lex_ini(regex_file_path, code_file_path);
}

int get_index(char *token_name)
{
	for (int i = 0; i < transfer_index; i++)
	{
		if (!strcmp(token_name, *(name_record_arr + i)))
			return i;
	}
	return -1;
}



struct state
{
	int arr[100];
	int top;
}state_stack;
lexeme lex;
void yacc_analyze()
{
	n_pro *p = NULL;
	int yacc_state = 0;
	int token_index;
	//Used when temp==0
	int a[20];
	int i = 0;

	state_stack.top = 1;
	int temp;

	int load = 1;

	state_stack.arr[0] = yacc_state;
	while (1)
	{
		if (load)
		{
			get_next_token(&lex);
			printf("Match:<%s,%s>\n", lex.lexeme_name, lex.token_name);
		} else
			load = 1;
		token_index = get_index(lex.token_name);
		yacc_state = state_stack.arr[state_stack.top - 1];
		temp = sets_arr[yacc_state]->transfer_table[token_index];
	L:
		if (temp == 0)
		{
			if (temp = sets_arr[yacc_state]->transfer_table[0])
			{
				printf("Try empty string.\n");
				load = 0;
				goto L;
			}
			printf("line:%d,lexme:%s\n", lex_lines, lex.lexeme_name);
			printf("Suppose to be a(an) ");
			for (int j = 0; j < transfer_index; j++)
			{
				temp = sets_arr[yacc_state]->transfer_table[j];
				if (temp != 0)
				{
					a[i] = j;
					i++;
				}
			}
			for (int j = 0; j < i; j++)
			{
				if (j == i - 1)
					printf("%s", name_record_arr[a[j]]);
				else
					printf("%s or ", name_record_arr[a[j]]);
			}
			putchar(10);
			exception("Analysis error.", "yacc_analyze");
		} else if (temp == 1)
		{
			printf("Analysis completed.The code fits the grammar.\n");
			return;
		} else if (temp < 0)
		{
			temp = -temp;
			p = sets_arr[yacc_state]->pro_list;
			for (int i = 0; i < temp - 1; i++, p = p->next);
			printf("Reduce to %s.\n", name_record_arr[p->head]);
			state_stack.top = state_stack.top - p->body_len;
			yacc_state = sets_arr[state_stack.arr[state_stack.top - 1]]->transfer_table[p->head] - 2;
			state_stack.arr[state_stack.top++] = yacc_state;
			load = 0;
		} else
		{
			printf("Shift in.\n");
			state_stack.arr[state_stack.top++] = temp - 2;
		}
	}
}