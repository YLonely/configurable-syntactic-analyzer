#include "stdafx.h"
//Recording the productions that are created.
production *pro_list = NULL;

//Recording the elements that are created.
element *token_list = NULL;
int token_num = 0;


int transfer_index = 0;

char current_str[64];

void token_analyze();
void pro_analyze();
int match(char *s);
void add_t(element *e);
void add_p(production *p);
int is_contain(char *key);
void* find_by_key(char *key);
item* items_analyze();
int advance();
void space_skip();
element* elements_analyze();

void file_load(char *buff, FILE *fp)
{
	int i = fread(buff, sizeof(char), FILE_LOAD_MAX, fp);
	if (i == FILE_LOAD_MAX)
		exception("File load out of range error", NULL);

}

/*
	Load all the grammar info into a buff,then create the
	structs group to describe the grammar.
*/
int file_ptr = 0;
char *file_buff = NULL;
void grammar_load(FILE *fp)
{
	file_buff = (char*)calloc(FILE_LOAD_MAX, sizeof(char));
	file_load(file_buff, fp);
	space_skip();
	if (match("%token"))
		token_analyze();
	space_skip();
	if (match("{"))
		pro_analyze();
	else
		exception("Illegal grammar form error", "grammar_load");
}

void token_analyze()
{
	space_skip();
	element *e = NULL;
	/*
		Add STRING_END and EMPTY token first
	*/
	e = (element*)calloc(1, sizeof(element));
	e->is_terminator = TRUE;
	e->type.t_index = transfer_index;
	strcpy(e->terminator_name, "EMPTY");
	add_t(e);

	e = (element*)calloc(1, sizeof(element));
	e->is_terminator = TRUE;
	e->type.t_index = transfer_index;
	strcpy(e->terminator_name, "STRING_END");
	add_t(e);

	while (advance())
	{
		e = (element*)calloc(1, sizeof(element));
		e->is_terminator = TRUE;
		//temp->next = NULL;
		e->type.t_index = transfer_index;
		strcpy(e->terminator_name, current_str);
		add_t(e);
		space_skip();
	}
	if (!match(";"))
		exception("Illegal grammar form error", "token_analyze");

}

void pro_analyze()
{
	//int ptr_temp;
	int contain;
	//ptr_temp = file_ptr;
	production *p = (production*)calloc(1, sizeof(production));
	production *temp_p = NULL;
	space_skip();
	while (advance())
	{
		contain = is_contain(current_str);
		if (!contain)
		{
			strcpy(p->head, current_str);
			p->p_index = transfer_index;
			if (!match(":"))
				exception("Illegal grammar form error", "pro_analyze");
			add_p(p);
			p->items = items_analyze();
			p = (production*)calloc(1, sizeof(production));
		} else
		{
			temp_p = (production*)find_by_key(current_str);
			if (!match(":"))
				exception("Illegal grammar form error", "pro_analyze");

			temp_p->items = items_analyze();
		}
		space_skip();
	}
	match("}");
}

int match(char *s)
{
	int length = strlen(s);
	int temp = file_ptr;
	for (int i = 0; i < length; i++, file_ptr++)
	{
		if (file_buff[file_ptr] != s[i])
		{
			//exception("Match error", s);
			file_ptr = temp;
			return 0;
		}
	}
	return 1;
}


/*
	Add a token to the end of the token list.
*/
void add_t(element *e)
{
	element *temp = token_list;
	if (!token_list)
		token_list = e;
	else
	{
		for (; temp->next != NULL; temp = temp->next);
		temp->next = e;
	}
	transfer_index++;
	token_num++;
}

/*
	Add a production to the end of the production list.
*/
void add_p(production *p)
{
	production *temp = pro_list;
	if (!pro_list)
		pro_list = p;
	else
	{
		for (; temp->next != NULL; temp = temp->next);
		temp->next = p;
	}
	transfer_index++;
}

void* find_by_key(char *key)
{
	production *p = pro_list;
	element *e = token_list;
	element *temp = NULL;
	for (; p; p = p->next)
	{
		if (!strcmp(p->head, key))
			return p;
	}

	for (; e; e = e->next)
	{
		if (!strcmp(e->terminator_name, key) && e->is_terminator)
		{
			temp = (element*)calloc(1, sizeof(element));
			temp->is_terminator = TRUE;
			//temp->next = NULL;
			temp->type.t_index = e->type.t_index;
			strcpy(temp->terminator_name, e->terminator_name);
			return temp;
		}
	}
	return NULL;
}



/*
	Whether the element or production with the key is in the list.
*/
int is_contain(char *key)
{
	production *temp_p = pro_list;
	element *temp_e = token_list;
	for (; temp_p; temp_p = temp_p->next)
	{
		if (!strcmp(temp_p->head, key))
			return PRODUCTION;
	}
	for (; temp_e; temp_e = temp_e->next)
	{
		if (!strcmp(temp_e->terminator_name, key))
			return TOKEN;
	}
	return 0;
}

void item_body_count(item *t)
{
	int body_len;
	element *e = t->ele;
	for (body_len = 0; e; e = e->next, body_len++)
	{
		if (e->is_terminator)
			t->body[body_len] = e->type.t_index;
		else
			t->body[body_len] = e->type.pro->p_index;
	}
	t->body_len = body_len;
}

item* items_analyze()
{
	item* first_item = (item*)calloc(1, sizeof(item));
	item* temp = first_item;
	while (TRUE)
	{
		temp->ele = elements_analyze();
		temp->body = (int*)calloc(20, sizeof(int));
		item_body_count(temp);
		if (match("|"))
		{
			temp->next = (item*)calloc(1, sizeof(item));
			temp = temp->next;
		} else if (match(";"))
			return first_item;
		else
			exception("Illegal grammar form error", "items_analyze");
	}

}


element* elements_analyze()
{
	element* first_element = NULL;
	element* temp_e = NULL;
	production* temp_p = NULL;
	int contain;
	while (advance())
	{
		contain = is_contain(current_str);
		if (contain == TOKEN)
		{
			if (first_element == NULL)
			{
				first_element = (element*)find_by_key(current_str);
				temp_e = first_element;
			} else
			{
				temp_e->next = (element*)find_by_key(current_str);
				temp_e = temp_e->next;
			}

		} else if (contain == PRODUCTION)
		{
			if (first_element == NULL)
			{
				first_element = (element*)calloc(1, sizeof(element));
				//first_element->is_terminator = FALSE;
				first_element->type.pro = (production*)find_by_key(current_str);
				temp_e = first_element;
			} else
			{
				temp_e->next = (element*)calloc(1, sizeof(element));
				//temp_e->next->is_terminator = FALSE;
				temp_e->next->type.pro = (production*)find_by_key(current_str);
				temp_e = temp_e->next;
			}
		} else
		{
			temp_p = (production*)calloc(1, sizeof(production));
			temp_p->p_index = transfer_index;
			strcpy(temp_p->head, current_str);
			add_p(temp_p);
			if (first_element == NULL)
			{
				first_element = (element*)calloc(1, sizeof(element));
				first_element->type.pro = temp_p;
				temp_e = first_element;
			} else
			{
				temp_e->next = (element*)calloc(1, sizeof(element));
				temp_e->next->type.pro = temp_p;
				temp_e = temp_e->next;
			}
		}
		space_skip();
	}
	return first_element;
}





/*
	Find next legal token name or production head.
*/
int advance()
{
	int i = 0;
	for (; (file_buff[file_ptr] >= '0' && file_buff[file_ptr] <= '9') || (file_buff[file_ptr] >= 'a' && file_buff[file_ptr] <= 'z') || (file_buff[file_ptr] >= 'A' && file_buff[file_ptr] <= 'Z') || file_buff[file_ptr] == '_'; i++, file_ptr++)
	{
		current_str[i] = file_buff[file_ptr];
	}
	current_str[i] = 0;
	return i;
}

void space_skip()
{
	for (; file_buff[file_ptr] == ' ' || file_buff[file_ptr] == '\n' || file_buff[file_ptr] == '\t' || file_buff[file_ptr] == '\r'; file_ptr++);
}



/*
	Release the mem of list.
*/
void mem_release()
{
	production *p = pro_list;
	element *e = token_list;
	production *temp_p = NULL;
	element *temp_e = NULL;

	while (p)
	{
		temp_p = p;
		p = p->next;
		free(temp_p);
	}

	while (e)
	{
		temp_e = e;
		e = e->next;
		free(temp_e);
	}
}
