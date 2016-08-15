#include "stdafx.h"
//Recording the productions that are created.
production *pro_list = NULL;
int pro_index = -1;

//Recording the elements that are created.
element *token_list = NULL;
int token_index = 0;


void token_analyze();
void pro_analyze();
void match(char *s);
void skip();
void add_t(element *e);
void add_p(production *p);


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
	//Skip all the useless charaters.
	skip();

	if (file_buff[file_ptr] == '%')
		token_analyze();

	skip();
	if (file_buff[file_ptr] == '{')
		pro_analyze();
	else
	{
		exception("Illegal grammar form error", NULL);
		return;
	}
}

void token_analyze()
{
	match("%token");
	skip();
	element *temp = NULL;
	temp = (element*)calloc(1, sizeof(element));
	for (int i = 0; file_buff[file_ptr] != ';'; i++, file_ptr++)
	{

		if (file_buff[file_ptr] == ',')
		{
			file_ptr++;
			i = -1;
			temp->is_terminator = TRUE;
			temp->next = NULL;
			temp->type.t_index = token_index;
			add_t(temp);
			temp = (element*)calloc(1, sizeof(element));
		}

		//We can record the name of tokens when necessary.
		/*
		else
			temp->terminator_name[i] = file_buff[file_ptr];
		*/
	}
	file_ptr++;
	temp->is_terminator = TRUE;
	temp->next = NULL;
	temp->type.t_index = token_index;
	add_t(temp);

	match(";");
}

void pro_analyze()
{
	match("{");

	match("}");
}

void match(char *s)
{
	int length = strlen(s);
	for (int i = 0; i < length; i++, file_ptr++)
	{
		if (file_buff[file_ptr] != s[i])
		{
			exception("Match error", s);
			getchar();
			exit(0);
		}
	}
	file_ptr++;
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
	token_index++;
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
	pro_index--;
}


void skip()
{
	for (; file_buff[file_ptr] == ' ' || file_buff[file_ptr] == '\n' || file_buff[file_ptr] == '\t' || file_buff[file_ptr] == '\r'; file_ptr++);
}
