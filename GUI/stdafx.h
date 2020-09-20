// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define WINVER          _WIN32_WINNT_WINXP
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
//#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 在此处引用程序需要的其他头文件
#include <map>

#include <winsock2.h>
#include <WS2tcpip.h>
#include <WinBase.h>
#include <stack>
#include <Shlobj.h>
#include <commdlg.h>//包括"选择文件"对话框的头文件
#include <iphlpapi.h>