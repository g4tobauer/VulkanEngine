#pragma once
#ifndef ENGINE_BASE
#define ENGINE_BASE

#ifndef ENGINE_COMMON_HEADERS
#include "CommonHeaders.h"
#endif // !ENGINE_COMMON_HEADERS

class Core;
class BaseEngine
{
public:
	Core* pCore;
};
#endif // !ENGINE_BASE

