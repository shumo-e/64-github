#pragma once
#include "AllocMemory.h"
#include "CString.h"
#include "FileOperation.h"
#include "PE.h"

#ifdef _WIN64
#pragma comment(lib,"64/Common.lib")
#else
#pragma comment(lib,"32/Common.lib")
#endif // _WIN64

using namespace std;





