#include "utils.h"

void StrReplace(char* text, const char* substring, const char* replaceWith)
{
	int substringLen = strlen(substring);
	int replaceWithLen = strlen(replaceWith);
	char secondPart[50] = "";
	while ((text = strstr(text, substring)) != NULL)
	{
		strcpy_s(secondPart, text + substringLen);
		*text = '\0';
		strcat_s(text, 50, replaceWith);
		strcat_s(text, 50, secondPart);
		text += replaceWithLen;
	}
}

float RandFloat(float min, float max)
{
	return min + float(rand()) / (RAND_MAX / (max - min));
}

int RandInt(int min, int max)
{
	return min + rand() % (max - min);
}