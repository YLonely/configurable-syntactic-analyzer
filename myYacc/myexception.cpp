#include "myexception.h"
#include "stdio.h"

void exception(char *e)
{
	if (e == NULL)
		exception("Empty exception param error");
	else
	{
		printf("%s", e);
	}
}