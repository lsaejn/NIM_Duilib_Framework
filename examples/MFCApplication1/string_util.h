#pragma once


template<typename Ty, int nSize>
int ArraySize(Ty(&Arg)[nSize])
{
	return nSize;
}

bool startWith(const char* src, const char* des);
bool endWith(const char* src, const char* des);