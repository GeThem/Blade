#include "utils.h"

void StrReplace(char* text, const char* substring, const char* replaceWith)
{
	int textLen = strlen(text);
	int secondPartLen = textLen;
	int substringLen = strlen(substring);
	int replaceWithLen = strlen(replaceWith);
	char* secondPart = (char*)malloc(sizeof(char) * secondPartLen);
	while ((text = strstr(text, substring)) != NULL)
	{
		textLen += replaceWithLen - substringLen + 1;
		strcpy_s(secondPart, secondPartLen, text + substringLen);
		*text = '\0';
		strcat_s(text, textLen, replaceWith);
		strcat_s(text, textLen, secondPart);
		text += replaceWithLen;
	}
	free(secondPart);
}

float RandFloat(float min, float max)
{
	return min + float(rand()) / (RAND_MAX / (max - min + 1));
}

int RandInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}

char* StrSplitInTwo(char*& stringPointer, char splitBy)
{
	char* val = stringPointer;
	stringPointer = strchr(stringPointer, splitBy);
	if (stringPointer)
		*stringPointer++ = '\0';
	return val;
}