#include "stdafx.h"

void exception(char *e, char *detail)
{
	if (e == NULL)
		exception("Empty exception param error", NULL);
	else
	{
		if (detail == NULL)
		{
			printf("%s", e);
		} else
			printf("%s Info:%s", e, detail);
	}
}