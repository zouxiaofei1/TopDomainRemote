//为了减短GUI.cpp长度
//多数和Class不相关的函数都被放到这里

#pragma once

#include "resource.h"


#define Delta 10 //按钮渐变色速度
#define MY_MAX_PATH	300
#define KEY_DOWN(VK_NNAME) ((GetKeyState(VK_NNAME) & 0x8000) ? 1:0)

//定义颜色
#define COLOR_WHITE			RGB(255,255,255)
#define COLOR_BLACK			RGB(0,0,0)
#define COLOR_RED			RGB(255,0,0)
#define COLOR_NORMAL_BLUE	RGB(40, 130, 240)
#define COLOR_LIGHTER_BLUE	RGB(230, 255, 255)
#define COLOR_LIGHT_BLUE	RGB(210, 255, 255)
#define COLOR_LIGHTER_GREY	RGB(243, 243, 243)
#define COLOR_LIGHT_GREY	RGB(220, 220, 220)
#define COLOR_DARKER_GREY	RGB(150, 150, 150)
#define COLOR_NORMAL_GREY	RGB(135, 135, 135)
#define COLOR_DARKEST_GREY	RGB(50,50,50)
#define COLOR_TITLE_1		RGB(0x66, 0xcc, 0xff)
#define COLOR_TITLE_2		RGB(0x66, 0xcc, 0xff)
#define COLOR_GREEN			RGB(20, 200, 80)
#define COLOR_GREENEST		RGB(0, 255, 0)
#define COLOR_ORANGE		RGB(255, 100, 0)
#define COLOR_LIGHT_ORANGE	RGB(255, 180, 10)
#define COLOR_DARKER_BLUE	0xDA8B01
#define COLOR_CLOSE_LEAVE	RGB(255, 109, 109)
#define COLOR_CLOSE_HOVER	RGB(250, 100, 100)
#define COLOR_CLOSE_PRESS	RGB(232, 95, 95)
#define COLOR_OK			RGB(5, 200, 135)
#define COLOR_RECOMMENDED	RGB(10, 255, 10)
#define COLOR_NOTREC		RGB(0x63, 0xB8, 0xFF)

//定义最大项目数
#define MAX_BUTTON		13
#define MAX_CHECK		3
#define MAX_FRAME		5
#define MAX_TEXT		10
#define MAX_EDIT		10
#define MAX_LINE		4
#define MAX_STRING		15
#define MAX_EDIT_WIDTH	200
#define MAX_EDIT_HEIGHT	80

#define REDRAW_FRAME	1
#define REDRAW_BUTTON	2
#define REDRAW_CHECK	3
#define REDRAW_TEXT		4
#define REDRAW_EDIT		5

#define InitBrushes WhiteBrush = CreateSolidBrush(COLOR_WHITE);\
DBlueBrush = CreateSolidBrush(COLOR_LIGHT_BLUE);\
LBlueBrush = CreateSolidBrush(COLOR_LIGHTER_BLUE);\
NormalBlueBrush = CreateSolidBrush(COLOR_NORMAL_BLUE);\
LGreyBrush = CreateSolidBrush(COLOR_LIGHTER_GREY);\
DGreyBrush = CreateSolidBrush(COLOR_LIGHT_GREY);\
TitleBrush = CreateSolidBrush(COLOR_TITLE_1);\
BlackPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);\
WhitePen = CreatePen(PS_SOLID, 1, COLOR_WHITE);\
TitlePen = CreatePen(PS_SOLID, 2, COLOR_TITLE_2);\
CheckGreenPen = CreatePen(PS_SOLID, 2, COLOR_GREEN);\
NormalGreyPen = CreatePen(PS_SOLID, 1, COLOR_NORMAL_GREY);\
NormalBluePen = CreatePen(PS_SOLID, 1, COLOR_NORMAL_BLUE);\
BlackBrush = CreateSolidBrush(COLOR_BLACK)