#pragma once

#include <string.h>
#include <malloc.h>
#include <random>

void StrReplace(char* text, const char* substring, const char* replaceWith);

float RandFloat(float min, float max);

int RandInt(int min, int max);