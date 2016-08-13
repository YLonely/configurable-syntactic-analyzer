#include "stdafx.h"

/*
	Patch two elements together.
	Patch e1 to the end of the e2.
*/
void element_add(element *e1, element *e2)
{
	element *temp = NULL;
	if (e1 == NULL)
	{
		exception("Empty e1 param error");
		return;
	}
	if (e2 == NULL)
	{
		e2 = (element*)malloc(sizeof(element*));
		e2->is_terminator = e1->is_terminator;
		e2->type = e1->type;
		e2->next = NULL;
	} else
	{
		temp = (element*)malloc(sizeof(element*));
		temp->is_terminator = e1->is_terminator;
		temp->type = e1->type;
		temp->next = NULL;
		e2->next = temp;
	}
}


/*
	Counting the terminator of FIRST(element)
	record it in first_elements
*/
void count_first(element *e, element *first_elements)
{
	item *temp;
	if (e->is_terminator)
	{
		element_add(e, first_elements);
	} else
	{
		temp = e->type.pro->items;
		while (temp)
		{
			if (temp->ele->is_terminator&&temp->ele->type.terminator == EMPTY)
				count_first(e->next, first_elements);
			else
				count_first(temp->ele, first_elements);
			temp = temp->next;
		}
	}
}

/*
	Return the first elements of  elements
*/
element* first(element *e)
{
	element *first_elements = NULL;
	count_first(e, first_elements);
	return first_elements;
}