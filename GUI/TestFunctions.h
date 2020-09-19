#pragma once
#include "stdafx.h"

void s(LPCSTR a)
{
	MessageBoxA(NULL, a, "", NULL);
}

void s(LPCWSTR a)
{
	MessageBox(NULL, a, L"", NULL);
}
void s(int a)
{
	wchar_t aa[34];
	_itow_s(a, aa, 10);
	MessageBox(NULL, aa, L"", NULL);
}

void s()
{
	MessageBox(0,0,0,0);
}

void charTowchar(const char* chr, wchar_t* wchar, int size)
{
	MultiByteToWideChar(CP_ACP,
		0, chr, strlen(chr) + 1,
		wchar,
		size / sizeof(wchar[0]));
}

unsigned int Hash(const wchar_t* str)
{
	unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

unsigned int Hash(const char* str)
{
	unsigned int seed = 131;
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}