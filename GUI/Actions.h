#pragma once
#include "stdafx.h"
#include "resource.h"
ATOM MyRegisterClass(HINSTANCE h, WNDPROC proc, LPCWSTR ClassName)
{//��װ����ע��Class����
	WNDCLASSEXW wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DROPSHADOW;
	wcex.lpfnWndProc = proc;
	wcex.hInstance = h;
	wcex.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_GUI));//�����Զ��崰��ͼ��
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GUI);
	wcex.lpszClassName = ClassName;//�Զ���ClassName��WndProc
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUI));//Сͼ��
	return RegisterClassExW(&wcex);
}