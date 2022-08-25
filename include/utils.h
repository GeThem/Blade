#pragma once

#include <string.h>
#include <malloc.h>
#include <random>
#include <atlfile.h>

void StrReplace(char* text, const char* substring, const char* replaceWith);

float RandFloat(float min, float max);

int RandInt(int min, int max);

//Returns pointer to the start of the second value in the same string, \0 is put instead of splitBy symbol;
char* StrSplitInTwo(char* string, char splitBy);