#pragma once
#include "stdafx.h"
#include "resource.h"
ATOM MyRegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName)
{//封装过的注册Class函数
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DROPSHADOW;
	wcex.lpfnWndProc = proc;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//不能自定义窗体图标
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	wcex.lpszClassName = ClassName;//自定义ClassName和WndProc
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//小图标
	return RegisterClassExW(&wcex);
}