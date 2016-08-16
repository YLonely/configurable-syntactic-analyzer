#include "stdafx.h"
//Recording the productions that are created.
production *pro_list = NULL;

//Recording the elements that are created.
element *token_list = NULL;
int transfer_index = 0;

char current_str[64];

void token_analyze();
void pro_analyze();
int match(char *s);
void add_t(element *e);
void add_p(production *p);
int is_contain(char *key);
void* find(char *key);
item* items_analyze();
int advance();
void space_skip();
element* elements_analyze();

int file_load(char *buff, FILE *fp)
{
	int i = fread(buff, sizeof(char), FILE_LOAD_MAX, fp);
	if (i == FILE_LOAD_MAX)
	{
		exception("File load out of range error", NULL);
		return 0;
	}
	return 1;
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
	if (!file_load(file_buff, fp))
		return;

	space_skip();
	if (match("%token"))
		token_analyze();
	space_skip();
	if (match("{"))
		pro_analyze();
	else
	{
		exception("Illegal grammar form error", "grammar_load");
		return;
	}
}

void token_analyze()
{
	space_skip();
	element *temp = NULL;

	while (TRUE)
	{
		advance();
		temp = (element*)calloc(1, sizeof(element));
		temp->is_terminator = TRUE;
		temp->next = NULL;
		temp->type.t_index = transfer_index;
		strcpy(temp->terminator_name, current_str);
		add_t(temp);
		if (file_buff[file_ptr] == ';')
			break;
		else
			file_ptr++;
	}
	match(";");
}

void pro_analyze()
{
	//int ptr_temp;
	int contain;
	//ptr_temp = file_ptr;
	production *temp = (production*)calloc(1, sizeof(production));
	production *temp_p = NULL;
	while (TRUE)
	{
		space_skip();
		if (file_buff[file_ptr] == '}')
			break;
		advance();
		contain = is_contain(temp->head);
		if (!contain)
		{
			strcpy(temp->head, current_str);
			temp->p_index = transfer_index;
			if (!match(":"))
			{
				getchar();
				exit(0);
			}
			temp->items = items_analyze();
			add_p(temp);
			temp = (production*)calloc(1, sizeof(production));
		} else
		{
			temp_p = (production*)find(temp->head);
			if (!match(":"))
			{
				getchar();
				exit(0);
			}
			temp_p->items = items_analyze();
		}
	}
	match("}");
}

int match(char *s)
{
	int length = strlen(s);
	for (int i = 0; i < length; i++, file_ptr++)
	{
		if (file_buff[file_ptr] != s[i])
		{
			exception("Match error", s);
			return 0;
		}
	}
	file_ptr++;
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

void* find(char *key)
{
	production *temp_p = pro_list;
	element *temp_e = token_list;
	element *temp = NULL;
	for (; temp_p; temp_p = temp_p->next)
	{
		if (!strcmp(temp_p->head, key))
			return temp_p;
	}

	for (; temp_e; temp_e = temp_e->next)
	{
		if (!strcmp(temp_e->terminator_name, key))
		{
			temp = (element*)calloc(1, sizeof(element));
			temp->is_terminator = TRUE;
			temp->next = NULL;
			temp->type.t_index = temp_e->type.t_index;
			strcpy(temp->terminator_name, temp_e->terminator_name);
			return temp;
		}
	}
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

item* items_analyze()
{
	item* first_item = (item*)calloc(1, sizeof(item));
	item* temp = first_item;
	while (TRUE)
	{
		temp->ele = elements_analyze();
		if (file_buff[file_ptr] == '|')
		{
			temp->next = (item*)calloc(1, sizeof(item));
			temp = temp->next;
		} else if (file_buff[file_ptr] == ';')
			return first_item;
		else
		{
			exception("Illegal grammar form error", "items_analyze");
			getchar();
			exit(0);
		}
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
				first_element = (element*)find(current_str);
				temp_e = first_element;
			} else
			{
				temp_e->next = (element*)find(current_str);
				temp_e = temp_e->next;
			}

		} else if (contain == PRODUCTION)
		{
			if (first_element == NULL)
			{
				first_element = (element*)calloc(1, sizeof(element));
				first_element->is_terminator = FALSE;
				first_element->type.pro = (production*)find(current_str);
				temp_e = first_element;
			} else
			{
				temp_e->next = (element*)calloc(1, sizeof(element));
				temp_e->next->is_terminator = FALSE;
				temp_e->next->type.pro = (production*)find(current_str);
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
