//这是TopDomainRemote工程源代码的主文件
//by zouxiaofei1 2015 - 2020

//头文件
#include "stdafx.h"
#include "GUI.h"
#include "Actions.h"
#include "TestFunctions.h"
#include "Hotkey.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Imm32.lib")//自定义输入法位置用的Lib
#pragma warning(disable:4996)
#pragma warning(disable:4838)
#pragma warning(disable:28159)
#pragma warning(disable:4309)//禁用一些烦人的警告

//部分(重要)函数的前向声明
BOOL				InitInstance(HINSTANCE);//初始化
LRESULT	CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);//主窗口


//和绘图有关的全局变量
const wchar_t szWindowClass[] = L"TopDomainRemote";
HBRUSH DBlueBrush, LBlueBrush, WhiteBrush, NormalBlueBrush, TitleBrush, LGreyBrush, DGreyBrush, BlackBrush;//各色笔刷
HPEN BlackPen, WhitePen, TitlePen, CheckGreenPen, NormalGreyPen, NormalBluePen, LightestGreyPen, LightGreyPen;//各色笔
HDC hdc, rdc;//主窗口缓冲hdc + 贴图hdc
HBITMAP hBmp, lBmp;//主窗口hbmp
HWND List;
HINSTANCE hInst;//当前实例备份变量

//其他全局变量
char ip[30];//自己的主ip
char Allips[20][30];//自己的所有ip
int numofips, curips;//ip数量 & 当前显示的ip
struct SearchThreadStruct
{
	int ipBegin;
	int ipEnd;
	char* ip123;
	int ii;
};
struct IPandi { char* ip; int i; };
char IPsearched[30][256];//判断ip是否被寻找过 0->未寻找 1->只寻找过ip 2->完全寻找

class CathyClass//从TopDomainTools那里抄过来的Class
{
public:
	void InitClass(HINSTANCE HInstance)//新Class使用之前最好Init一下
	{								//  （不Init也行）
		hInstance = HInstance;//设置hInst
		CurButton = CurFrame = CurCheck = CurLine = CurText = 0;
		CurWnd = 1;//清"零"
		CoverButton = -1;//ps:可以在InitClass之前设定DPI
		CoverCheck = 0;
		//默认宋体
		DefFont = CreateFontW((int)(16 * DPI), (int)(8 * DPI), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
	}
	__forceinline wchar_t* GetStr(LPCWSTR ID) { return str[Hash(ID)]; }//通过Hash + map 来快速索引字符串的数据结构		ID(索引字符串) -> Hash -(map)> 字符串地址

	void SetStr(LPCWSTR Str, LPCWSTR ID)//通过ID设置字符串
	{
		unsigned int tmp = Hash(ID);
		if (str[tmp] != 0)str[tmp] = 0;//删除当前ID中原有的字符串
		str[tmp] = new wchar_t[wcslen(Str) + 20];//申请相应的内存空间
		wcscpy(str[tmp], Str);//复制新的
	}

	void CreateString(LPCWSTR Str, LPCWSTR ID)//创建新字符串
	{//注意:应仅在使用常量初始化时使用本函数,用变量初始化是建议CurString++再用SetStr
		++CurString;
		if (Str != NULL)
		{
			str[Hash(ID)] = new wchar_t[wcslen(Str) + 1];
			wcscpy(str[Hash(ID)], Str);
		}
		string[CurString].ID = str[Hash(ID)];
		string[CurString].str = (LPWSTR)Str;
	}
	void CreateEditEx(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID, HFONT Font, BOOL Ostr)
	{//创建自绘输入框
		++CurEdit;
		Edit[CurEdit].Left = Left; Edit[CurEdit].Top = Top;
		Edit[CurEdit].Width = Wid; Edit[CurEdit].Height = Hei;
		Edit[CurEdit].Page = Page;//设置长宽高等信息

		if (!Ostr)
		{//设置灰色标识的提示字符串
			wcscpy_s(Edit[CurEdit].OStr, name);
			Edit[CurEdit].font = Font;//有提示字符串时不能有正常字符串
			Edit[CurEdit].str = new wchar_t[21];
			*Edit[CurEdit].str = 0;
		}
		else//没有提示字符串:
			SetEditStrOrFont(name, Font, CurEdit);
		wcscpy_s(Edit[CurEdit].ID, ID);
	}

	void CreateButtonEx(int Number, int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, HBRUSH Leave, \
		HBRUSH Hover, HBRUSH press, HPEN Leave2, HPEN Hover2, HPEN Press2, HFONT Font, BOOL Enabled, BOOL Dshadow, COLORREF FontRGB, LPCWSTR ID)
	{//创建按钮的"高级"函数
		Button[Number].Left = Left; Button[Number].Top = Top;//设置长宽位置等信息
		Button[Number].Width = Wid; Button[Number].Height = Hei;
		Button[Number].Page = Page; Button[Number].Leave = Leave;
		Button[Number].Hover = Hover; Button[Number].Press = press;//设置离开 & 悬浮 & 点击 三种状态时的 HBRUSH & HPEN
		Button[Number].Leave2 = Leave2; Button[Number].Hover2 = Hover2;
		Button[Number].Press2 = Press2; Button[Number].Font = Font;
		Button[Number].Enabled = Enabled; Button[Number].FontRGB = FontRGB;
		wcscpy(Button[Number].Name, name); wcscpy(Button[Number].ID, ID); but[Hash(ID)] = Number;
		Button[Number].Shadow = Dshadow;

		LOGBRUSH LogBrush;//从HBRUSH中提取出RGB颜色
		LOGPEN LogPen;//	(渐变色需要)
		GetObject(Leave, sizeof(LogBrush), &LogBrush); Button[Number].b1[0] = GetRValue(LogBrush.lbColor);
		Button[Number].b1[1] = GetGValue(LogBrush.lbColor); Button[Number].b1[2] = GetBValue(LogBrush.lbColor);
		GetObject(Hover, sizeof(LogBrush), &LogBrush); Button[Number].b2[0] = GetRValue(LogBrush.lbColor);
		Button[Number].b2[1] = GetGValue(LogBrush.lbColor); Button[Number].b2[2] = GetBValue(LogBrush.lbColor);
		GetObject(Leave2, sizeof(LogPen), &LogPen); Button[Number].p1[0] = GetRValue(LogPen.lopnColor);
		Button[Number].p1[1] = GetGValue(LogPen.lopnColor); Button[Number].p1[2] = GetBValue(LogPen.lopnColor);
		GetObject(Hover2, sizeof(LogPen), &LogPen); Button[Number].p2[0] = GetRValue(LogPen.lopnColor);
		Button[Number].p2[1] = GetGValue(LogPen.lopnColor); Button[Number].p2[2] = GetBValue(LogPen.lopnColor);
	}
	void CreateButton(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name, LPCWSTR ID)//创建按钮（简化版）
	{
		++CurButton;//这里的name Wid Hei 不用全名是因为警告"隐藏了全局声明"
		Button[CurButton].Left = Left; Button[CurButton].Top = Top;//上下左右
		Button[CurButton].Width = Wid; Button[CurButton].Height = Hei;
		Button[CurButton].Page = Page; wcscpy_s(Button[CurButton].Name, name);
		wcscpy_s(Button[CurButton].ID, ID); but[Hash(ID)] = CurButton;
		Button[CurButton].Leave2 = Button[CurButton].Hover2 = Button[CurButton].Press2 = BlackPen;
		Button[CurButton].Leave = WhiteBrush; Button[CurButton].Hover = DBlueBrush; Button[CurButton].Press = LBlueBrush;
		Button[CurButton].b1[0] = Button[CurButton].b1[1] = Button[CurButton].b1[2] = 255;
		Button[CurButton].b2[0] = 210; Button[CurButton].b2[1] = Button[CurButton].b2[2] = 255; Button[CurButton].Shadow = true;
	}
	void CreateFrame(int Left, int Top, int Wid, int Hei, int Page, LPCWSTR name)//创建内容框
	{
		++CurFrame;//															--- Example -----
		Frame[CurFrame].Left = Left; Frame[CurFrame].Page = Page;//			    |				|
		Frame[CurFrame].Height = Hei; Frame[CurFrame].Top = Top;//			    |				|
		Frame[CurFrame].Width = Wid;//										    |				|
		wcscpy_s(Frame[CurFrame].Name, name);//								    -----------------
	}
	void CreateCheck(int Left, int Top, int Page, int Wid, LPCWSTR name)//创建"√"
	{
		++CurCheck;//设置长宽高等信息
		Check[CurCheck].Left = Left; Check[CurCheck].Top = Top;
		Check[CurCheck].Page = Page; Check[CurCheck].Width = Wid;
		wcscpy_s(Check[CurCheck].Name, name);
	}
	void CreateText(int Left, int Top, int Page, LPCWSTR name, COLORREF rgb)//创建注释文字
	{
		++CurText;
		Text[CurText].Left = Left; Text[CurText].Top = Top;
		Text[CurText].Page = Page; Text[CurText].rgb = rgb;
		wcscpy_s(Text[CurText].Name, name);
	}
	void CreateLine(int StartX, int StartY, int EndX, int EndY, int Page, COLORREF rgb)//创建线段
	{//start & end 传入的都是未缩放坐标
		++CurLine;
		Line[CurLine].StartX = StartX; Line[CurLine].StartY = StartY;
		Line[CurLine].EndX = EndX; Line[CurLine].EndY = EndY;
		Line[CurLine].Page = Page; Line[CurLine].Color = rgb;
	}
	BOOL InsideButton(int cur, POINT& point)//根据传入的point判断鼠标指针是否在按钮内
	{//cur:按钮的编号，不是ID
		return (Button[cur].Left * DPI <= point.x && Button[cur].Top * DPI <= point.y && (long)((Button[cur].Left + Button[cur].Width) * DPI) >= point.x && (long)((Button[cur].Top + Button[cur].Height) * DPI) >= point.y);
	}
	int InsideCheck(int cur, POINT& point)//同理 判断鼠标指针是否在check内
	{
		if (Check[cur].Left * DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + 15 * DPI + 1 >= point.x
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 1;//在check的方框内

		if (Check[cur].Left * DPI <= point.x && Check[cur].Top * DPI <= point.y && Check[cur].Left * DPI + Check[cur].Width * DPI >= point.x
			&& Check[cur].Top * DPI + 15 * DPI + 1 >= point.y)return 2;//在check方框右侧一定距离内
		return 0;//哪边都不在
	}
	void DrawFrames(int cur)//绘制Frames
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Frame
		for (i = 1; i <= CurFrame; ++i)
		{
		begin:
			if (Frame[i].Page == CurWnd || Frame[i].Page == 0)
			{
				SelectObject(hdc, BlackPen);//绘制方框
				SelectObject(hdc, DefFont);
				MoveToEx(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI), NULL);//多加点(int)xx*DPI 减少警告
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI + Frame[i].Height * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI + Frame[i].Width * DPI), (int)(Frame[i].Top * DPI));
				LineTo(hdc, (int)(Frame[i].Left * DPI), (int)(Frame[i].Top * DPI));
				SetTextColor(hdc, Frame[i].rgb);//自定义文字颜色
				RECT rc = GetRECTf(i);
				SetBkMode(hdc, OPAQUE);//打印上方文字
				DrawTextW(hdc, Frame[i].Name, (int)wcslen(Frame[i].Name), &rc, NULL);
				SetTextColor(hdc, NULL);
			}
			if (cur != 0)return;
		}
	}
	void DrawButtons(int cur)//绘制按钮
	{
		int i;//如果使用ObjectRedraw则跳过其他Button
		if (cur != 0) { i = cur; goto begin; }//结构示意：选择颜色(渐变 or 禁用 or 默认) -> 选择字体
		for (i = 1; i <= CurButton; ++i)//				-> 绘制方框 -> 绘制下载进度条 -> 绘制文字 -> 清理
		{
		begin:
			if (Button[i].Page == CurWnd || Button[i].Page == 0)
			{
				HPEN TmpPen = 0; HBRUSH TmpBrush = 0;
				if (Button[i].Enabled == false)//禁用则显示灰色
				{
					SelectObject(hdc, DGreyBrush);
					SelectObject(hdc, Button[i].Leave2);
					SetTextColor(hdc, COLOR_NORMAL_GREY);
					goto colorok;//直接跳过渐变色
				}
				SetTextColor(hdc, Button[i].FontRGB);
				if (Button[i].Percent != 0 && Button[i].Percent != 100 && Button[i].DownTot == 0)//渐变色绘制
				{
					TmpPen = CreatePen(PS_SOLID, 1, RGB((Button[i].p2[0] - Button[i].p1[0]) * Button[i].Percent / 100 + Button[i].p1[0], \
						(Button[i].p2[1] - Button[i].p1[1]) * Button[i].Percent / 100 + Button[i].p1[1], (Button[i].p2[2] - Button[i].p1[2]) * Button[i].Percent / 100 + Button[i].p1[2]));
					//临时创建画笔和画刷
					SelectObject(hdc, TmpPen);
					TmpBrush = CreateSolidBrush(RGB((Button[i].b2[0] - Button[i].b1[0]) * Button[i].Percent / 100 + Button[i].b1[0], \
						(Button[i].b2[1] - Button[i].b1[1]) * Button[i].Percent / 100 + Button[i].b1[1], (Button[i].b2[2] - Button[i].b1[2]) * Button[i].Percent / 100 + Button[i].b1[2]));
					SelectObject(hdc, TmpBrush);
					goto colorok;
				}
				if (CoverButton == i && Button[i].DownTot == 0)//没有禁用 也没有渐变色 -> 默认颜色
					if (Press == 1) {
						SelectObject(hdc, Button[i].Press);//按下按钮
						SelectObject(hdc, Button[i].Press2);
					}
					else {
						SelectObject(hdc, Button[i].Hover);//悬浮
						SelectObject(hdc, Button[i].Hover2);
					}
				else
				{
					SelectObject(hdc, Button[i].Leave);//离开
					SelectObject(hdc, Button[i].Leave2);
				}
			colorok:
				
				if (Button[i].Font == NULL)SelectObject(hdc, DefFont); else SelectObject(hdc, Button[i].Font);//字体

				Rectangle(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + 0.5),
					(int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + Button[i].Height * DPI));//绘制方框
				if (Button[i].Shadow && ButtonEffect)//在按钮的右下方画一圈阴影
				{
					SelectObject(hdc, NormalGreyPen);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + 0.5), 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI), (int)(Button[i].Top * DPI + Button[i].Height * DPI));
					LineTo(hdc, (int)(Button[i].Left * DPI + 0.5) - 1, (int)(Button[i].Top * DPI + Button[i].Height * DPI));

					SelectObject(hdc, LightGreyPen);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 1, (int)(Button[i].Top * DPI + 0.5), 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 1, (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 1);
					LineTo(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 1);

					SelectObject(hdc, LightestGreyPen);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 2, (int)(Button[i].Top * DPI + 0.5), 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 2, (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 1);
					MoveToEx(hdc, (int)(Button[i].Left * DPI + Button[i].Width * DPI) + 1, (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 2, 0);
					LineTo(hdc, (int)(Button[i].Left * DPI + 0.5), (int)(Button[i].Top * DPI + Button[i].Height * DPI) + 2);
				}
				RECT rc = GetRECT(i);

				SetBkMode(hdc, TRANSPARENT);//去掉文字背景

				//打印文字(默认)
				DrawTextW(hdc, Button[i].Name, (int)wcslen(Button[i].Name), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				if (TmpPen != NULL)DeleteObject(TmpPen);//回收句柄
				if (TmpBrush != NULL)DeleteObject(TmpBrush);
			}
			if (cur != 0)return;//使用ObjectRedraw时直接结束
		}
		SetTextColor(hdc, COLOR_BLACK);
	}
	void DrawChecks(int cur)//绘制Checks
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Check
		for (i = 1; i <= CurCheck; ++i)
		{
		begin:
			if (Check[i].Page == 0 || Check[i].Page == CurWnd)
			{
				SetTextColor(hdc, COLOR_BLACK);
				if (i == CoverCheck)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, NormalGreyPen);

				SelectObject(hdc, LGreyBrush);
				SelectObject(hdc, DefFont);//check默认边长为15，并不能调整
				Rectangle(hdc, (int)(Check[i].Left * DPI), (int)(Check[i].Top * DPI), (int)(Check[i].Left * DPI + 15 * DPI), (int)(Check[i].Top * DPI + 15 * DPI));
				TextOut(hdc, (int)(Check[i].Left * DPI + 20 * DPI), (int)(Check[i].Top * DPI), Check[i].Name, (int)wcslen(Check[i].Name));
				if (Check[i].Value == 1)//打勾
				{						//比较难看
					SelectObject(hdc, CheckGreenPen);//试过了直接贴图，更难看
					MoveToEx(hdc, (int)(Check[i].Left * DPI + 2 * DPI), (int)(Check[i].Top * DPI + 7 * DPI), NULL);//所以只能这样了 = =
					LineTo(hdc, (int)(Check[i].Left * DPI + 6 * DPI), (int)(Check[i].Top * DPI + 12 * DPI));
					LineTo(hdc, (int)(Check[i].Left * DPI + 12 * DPI), (int)(Check[i].Top * DPI + 3 * DPI));
				}
			}
			if (cur != 0)return;
		}
	}
	void DrawLines()//绘制线段
	{//线段一般不需要重绘
		for (int i = 1; i <= CurLine; ++i)//因此没有加ObjectRedraw
			if (Line[i].Page == 0 || Line[i].Page == CurWnd)
			{
				HPEN tmpPen = CreatePen(0, 1, Line[i].Color);
				SelectObject(hdc, tmpPen);//直接用LineTo
				MoveToEx(hdc, (int)(Line[i].StartX * DPI), (int)(Line[i].StartY * DPI), NULL);
				LineTo(hdc, (int)(Line[i].EndX * DPI), (int)(Line[i].EndY * DPI));
				DeleteObject(tmpPen);
			}
	}
	void DrawTexts(int cur)//绘制文字
	{
		int i;
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Texts
		for (i = 1; i <= CurText; ++i)
		{
		begin:
			if (Text[i].Page == 0 || Text[i].Page == CurWnd)
			{
				SetTextColor(hdc, Text[i].rgb);
				SelectObject(hdc, DefFont);//文字的字体缩放效果不太理想
				wchar_t* tmp = str[Hash(Text[i].Name)];
				if (tmp == 0)break;
				TextOutW(hdc, (int)(Text[i].Left * DPI), (int)(Text[i].Top * DPI), tmp, (int)wcslen(tmp));
			}
			if (cur != 0)return;
		}
	}
	void DrawEdits(int cur)//绘制输入框
	{
		int i, MOffset = 0, showBegin = 0, showEnd = 0;//结构示意:	创建缓存dc -> 绘制边框(蓝色 or 灰色) -> 打印文字(提示文字)
		HDC mdc;//													->打印文字(未选中) ->打印文字(选中) -> 从缓存dc贴图 -> 清理
		mdc = CreateCompatibleDC(hdc);

		SelectObject(mdc, EditBitmap);//Edit专用“三缓冲”bitmap和dc
		SetBkMode(mdc, TRANSPARENT);
		if (cur != 0) { i = cur; goto begin; }//如果使用ObjectRedraw则跳过其他Edits

		for (i = 1; i <= CurEdit; ++i)
		{
		begin:
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
			{
				SelectObject(mdc, WhitePen);//清空缓存dc
				SelectObject(mdc, WhiteBrush);
				Rectangle(mdc, 0, 0, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);

				SelectObject(hdc, WhiteBrush);
				if (i == CoverEdit)SelectObject(hdc, NormalBluePen); else SelectObject(hdc, BlackPen);//如果当前Edit被选中则用蓝色绘制边框
				Rectangle(hdc, (int)((Edit[i].Left - 5) * DPI), (int)(Edit[i].Top * DPI),//否则仅用默认的黑色
					(int)((Edit[i].Left + Edit[i].Width + 5) * DPI), (int)((Edit[i].Top + Edit[i].Height) * DPI));

				if (*Edit[i].OStr != 0)//如果当前Edit显示的是Ostr(仅用于提示的灰色文字)
				{//在Hdc上直接打印完走人
					SetTextColor(hdc, COLOR_DARKER_GREY);
					RECT rc = { (long)((Edit[i].Left - 5) * DPI) , (long)(Edit[i].Top * DPI),(long)((Edit[i].Left + Edit[i].Width + 5) * DPI),(long)((Edit[i].Top + Edit[i].Height) * DPI) };
					DrawTextW(hdc, Edit[i].OStr, (int)wcslen(Edit[i].OStr), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					continue;
				}

				SIZE sel, ser;//pos1 / pos2:确定打印时“选中部分”真正的左右两端点
				int pos1, pos2;//因为有时候从左到右移动鼠标选中文字，有时从右向左
				if (Edit[i].Pos1 > Edit[i].Pos2 && Edit[i].Pos2 != -1)pos1 = Edit[i].Pos2, pos2 = Edit[i].Pos1; else pos1 = Edit[i].Pos1, pos2 = Edit[i].Pos2;
				//pos1&pos2里记录的数值只表示选中的先后顺序，不代表左右，因此这里要特殊处理

				if (Edit[i].font != NULL)SelectObject(mdc, Edit[i].font); else SelectObject(mdc, DefFont);//字体
				if (Edit[i].XOffset != 0)
				{//当XOffset不为0时，可能大部分文字都不会显示在屏幕上，这时候它们无需被打印
					showBegin = max(MyGetTextExtentPoint32Binary(i, Edit[i].XOffset) - 1, 0);
					SIZE se;//获取被显示在屏幕上的文字开始处和结尾处
					MyGetTextExtentPoint32(i, -1, showBegin - 1, se);
					MOffset = se.cx;//获取MOffset(相对于XOffset后向左的偏移量)
					//当没有这个优化时，缓存上打印时总是打印整个字符串，贴图时从XOffset开始贴
					//有这一优化时，若XOffset不为0，在mdc上打印时从横坐标0开始打印，较原来向左偏移了(字符串前端未被打印的字符的长度)个单位，所以贴图时也应向左移MOffset单位.
					showEnd = min(MyGetTextExtentPoint32Binary(i, (int)(Edit[i].XOffset + Edit[i].Width * DPI)), Edit[i].strLength - 1);
				}
				if (pos2 == -1)
				{//如果没有选中，直接打印+贴图 -> 走人
					if (Edit[i].XOffset != 0)
						TextOutW(mdc, 0, 4, Edit[i].str + showBegin, showEnd - showBegin + 1);
					else
						TextOutW(mdc, 0, 4, Edit[i].str, Edit[i].strLength);
					goto next;
				}
				//如果选中:较为复杂的情况
				MyGetTextExtentPoint32(i, -1, pos1 - 1, sel);//选中条左边字符总长度
				MyGetTextExtentPoint32(i, -1, pos2 - 1, ser);//选中条长度+左边字符总长度
				if (Edit[i].XOffset != 0)
				{//XOffset不为0，使用MOffset优化的情况
					if (pos1 >= showBegin)
					{//注意XOffset和Moffset都是乘上过DPI的真实坐标
						SetTextColor(mdc, COLOR_BLACK);
						TextOutW(mdc, 0, 4, Edit[i].str + showBegin, min(pos1, showEnd) - showBegin);//黑色打印选中条左边文字
					}//注:此优化为最近添加，可能不稳定
					if (pos2 >= showBegin)
					{
						SelectObject(mdc, NormalBluePen);
						SelectObject(mdc, NormalBlueBrush);//用蓝色绘制选中条背景
						Rectangle(mdc, max(sel.cx - MOffset, 0), 0, min(ser.cx - MOffset, int(Edit[i].XOffset + Edit[i].Width * DPI)), (int)(ser.cy + 5 * DPI));
						SetTextColor(mdc, COLOR_WHITE);
						TextOutW(mdc, max(sel.cx - MOffset, 0), 4, max(Edit[i].str + pos1, Edit[i].str + showBegin), min(pos2, showEnd) - max(pos1, showBegin) + 1);//白色打印选中条中间文字
					}
					if (pos2 <= showEnd)
					{
						SetTextColor(mdc, COLOR_BLACK);
						TextOutW(mdc, max(ser.cx - MOffset, 0), 4, max(Edit[i].str + pos2, Edit[i].str + showBegin), showEnd - max(pos2, showBegin) + 1);//黑色打印选中条右边文字
					}
				}
				else
				{
					SelectObject(mdc, NormalBlueBrush);
					SelectObject(mdc, NormalBluePen);//用蓝色绘制选中条背景
					Rectangle(mdc, sel.cx, 0, ser.cx, (int)(ser.cy + 5 * DPI));
					SetTextColor(mdc, COLOR_BLACK);
					TextOutW(mdc, 0, 4, Edit[i].str, pos1);//黑色打印选中条左边文字
					SetTextColor(mdc, COLOR_WHITE);
					TextOutW(mdc, sel.cx, 4, &Edit[i].str[pos1], pos2 - pos1);//白色打印选中条中间文字
					SetTextColor(mdc, COLOR_BLACK);
					TextOutW(mdc, ser.cx, 4, &Edit[i].str[pos2], Edit[i].strLength - pos2);//黑色打印选中条右边文字
				}
			next:
				int yMax = (int)((Edit[i].Top + Edit[i].Height / 2) * DPI - 4 - Edit[i].strHeight / 2);
				if (yMax < Edit[i].Top * DPI + 1)yMax = (int)(Edit[i].Top * DPI + 1);//贴图
				if (Edit[i].XOffset == 0)
					BitBlt(hdc, (int)((Edit[i].Left + Edit[i].Width / 2) * DPI) - Edit[i].strWidth / 2, yMax//Xoffset为0时居中
						, Edit[i].strWidth
						, Edit[i].strHeight + 4, mdc, 0, 0, SRCCOPY);
				else
					BitBlt(hdc, (int)(Edit[i].Left * DPI), yMax//有Xoffset时直接根据Xoffset-MOffset贴
						, (int)(Edit[i].Width * DPI)
						, Edit[i].strHeight + 4, mdc, Edit[i].XOffset - MOffset, 0, SRCCOPY);
			}
			if (cur != 0)goto end;
		}
	end:
		DeleteDC(mdc);
	}
	void RedrawObject(int type, int cur)//1=Frame,2=Button,3=Check,4=Text,5=Edit
	{//ObjectRedraw技术的分派函数
		switch (type) {
		case REDRAW_FRAME:DrawFrames(cur); break;
		case REDRAW_BUTTON:DrawButtons(cur); break;
		case REDRAW_CHECK:DrawChecks(cur); break;
		case REDRAW_TEXT:DrawTexts(cur); break;
		case REDRAW_EDIT:DrawEdits(cur); break;
		}
	}
	//自动绘制所有控件的函数，效率低，不应经常使用
	void DrawEVERYTHING() { DrawFrames(0); DrawButtons(0); DrawChecks(0); DrawLines(); DrawTexts(0); DrawEdits(0); }
	RECT GetRECT(int cur)//更新Buttons的rc 自带DPI缩放
	{
		RECT rc = { (long)(Button[cur].Left * DPI), (long)(Button[cur].Top * DPI),
			(long)(Button[cur].Left * DPI + Button[cur].Width * DPI),(long)(Button[cur].Top * DPI + Button[cur].Height * DPI) };
		return rc;
	}
	RECT GetRECTf(int cur)//更新Frames的rc 自带DPI缩放
	{
		RECT rc = { (long)(Frame[cur].Left * DPI + 10 * DPI) ,(long)(Frame[cur].Top * DPI - 7 * DPI) ,
		(long)(Frame[cur].Left * DPI + Frame[cur].Width * DPI),(long)(Frame[cur].Top * DPI + 30 * DPI) };
		return rc;
	}
	RECT GetRECTe(int cur)//更新Edit的rc 自带DPI缩放
	{
		RECT rc{ (long)((Edit[cur].Left - 5) * DPI) ,(long)(Edit[cur].Top * DPI),
		(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI),(long)((Edit[cur].Top + Edit[cur].Height) * DPI) };
		return rc;
	}
	RECT GetRECTc(int cur)//更新Check的rc 自带DPI缩放
	{
		RECT rc{ (long)(Check[cur].Left * DPI) ,(long)(Check[cur].Top * DPI) ,
			(long)(Check[cur].Left * DPI + 15 * DPI) ,(long)(Check[cur].Top * DPI + 15 * DPI) };
		return rc;
	}
	void RefreshEditSize(int cur)
	{//当更改Edit的内容后，刷新Edit字符的宽度
		HDC mdc;
		HBITMAP bmp;//创建一个空bmp用来GetTextExtentPoint32(获取字符宽度)
		mdc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(mdc, bmp);//设置字体
		if (Edit[cur].font != NULL)SelectObject(mdc, Edit[cur].font); else SelectObject(mdc, DefFont);
		delete[]Edit[cur].strW;//删除旧的宽度
		Edit[cur].strW = new int[Edit[cur].strLength + 1];
		SIZE se;
		GetTextExtentPoint32(mdc, Edit[cur].str, 1, &se);
		Edit[cur].strW[0] = se.cx;
		Edit[cur].strHeight = se.cy;
		for (int i = 1; i < Edit[cur].strLength; ++i)
		{//依次获取每一位的宽度
			GetTextExtentPoint32(mdc, Edit[cur].str + i, 1, &se);
			Edit[cur].strW[i] = Edit[cur].strW[i - 1] + se.cx;//strW的第i位指的是第0位到第i位的宽度之和
			Edit[cur].strHeight = max(Edit[cur].strHeight, se.cy);//strHeight是高度的最大值
		}
		DeleteDC(mdc);
		DeleteObject(bmp);
	}
	void SetEditStrOrFont(LPCWSTR Newstr, HFONT font, int cur)//改变指定Edit的字体或文字
	{
		if (font != NULL) Edit[cur].font = font;//先设置font，因为文字宽度和字体有关

		if (Newstr != NULL)//改变文字
		{
			Edit[cur].OStr[0] = 0;
			if (Edit[cur].str != NULL)if (*Edit[cur].str != NULL)delete[] Edit[cur].str;
			Edit[cur].strLength = (int)wcslen(Newstr);
			Edit[cur].str = new wchar_t[Edit[cur].strLength + 1];
			wcscpy(Edit[cur].str, Newstr);
		}
		RefreshEditSize(cur);//刷新字符串宽度
		Edit[cur].strWidth = Edit[cur].strW[Edit[cur].strLength - 1];
		if ((int)(Edit[cur].Width * DPI) < Edit[cur].strWidth) {
			if (Edit[cur].XOffset == 0)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI) / 2;
		}
		else Edit[cur].XOffset = 0;
	}
	int GetNearestChar(int cur, POINT Point)//试着获取输入框中离光标最近的字符
	{//Point为窗体中真实坐标
		POINT point;
		if (Edit[cur].strWidth <= Edit[cur].Width * DPI)//如果字符较少没有填满
		{//将窗体中真实坐标转化为Edit中的坐标
			if (Point.x < (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI - Edit[cur].strWidth / 2))return 0;
			if (Point.x > (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI + Edit[cur].strWidth / 2))return Edit[cur].strLength;
			point.x = Point.x - (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) + Edit[cur].strWidth / 2;
		}
		else//反之直接用XOffset计算
			point.x = (long)(Point.x - Edit[cur].Left * DPI + Edit[cur].XOffset);
		int pos = MyGetTextExtentPoint32Binary(cur, point.x);//获取大于point.x的第一个字符
		SIZE sel, ser;
		MyGetTextExtentPoint32(cur, -1, pos - 1, sel);//计算前一个字符宽度
		MyGetTextExtentPoint32(cur, -1, pos, ser);//计算后一个
		if (point.x < (sel.cx + ser.cx) / 2)return pos; else return pos + 1;//判断是选中光标左边还是右边的字符
	}
	void EditPaste(int cur)//将剪贴板中文字粘贴到Edit中的函数
	{
		if (cur == 0 || CoverEdit == 0)return;//如果没有选中Edit则退出 （一般不会出现这种情况的......吧）
		char* buffer = NULL;
		if (OpenClipboard(hWnd))
		{//打开剪切板
			HANDLE hData = GetClipboardData(CF_TEXT);
			if (hData == 0)return;//如果剪切板中格式不是CF_TEXT，则打开失败并退出
			buffer = (char*)GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
		}
		if (buffer == 0)return;
		size_t len = strlen(buffer), len2 = Edit[cur].strLength + 1;//这段代码只能粘贴文字
		wchar_t* ClipBoardtmp = new wchar_t[len + 1], * Edittmp = new wchar_t[len + len2];
		ZeroMemory(ClipBoardtmp, sizeof(wchar_t) * len);
		ZeroMemory(Edittmp, sizeof(wchar_t) * (len + len2));
		if (buffer != NULL)//读取
			MultiByteToWideChar(CP_ACP, 0, buffer, -1, ClipBoardtmp, (int)(sizeof(wchar_t) * len));
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2), pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		if (pos1 == -1)
		{//如果只有单光标选中
			wchar_t t = Edit[cur].str[pos2];
			Edit[cur].str[pos2] = '\0';
			wcscpy(Edittmp, Edit[cur].str);
			wcscat(Edittmp, ClipBoardtmp);
			Edit[cur].str[pos2] = t;//在光标后面加入剪切板中字符并拼接
			wcscat(Edittmp, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += (int)wcslen(ClipBoardtmp);
			SetEditStrOrFont(Edittmp, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		else//选中区段
		{
			Edit[cur].str[pos1] = '\0';
			wcscpy(Edittmp, Edit[cur].str);
			wcscat(Edittmp, ClipBoardtmp);//将选择部分替换成剪切板中字符并拼接
			wcscat(Edittmp, &Edit[cur].str[pos2]);
			Edit[cur].Pos1 += (int)wcslen(ClipBoardtmp);
			Edit[cur].Pos2 = -1;
			SetEditStrOrFont(Edittmp, 0, cur);
			RefreshXOffset(cur);
			RefreshCaretByPos(cur);
		}
		delete[] Edittmp;//清理内存
		delete[] ClipBoardtmp;
		EditRedraw(cur);//重绘控件
	}
	void EditHotKey(int wParam)//Edit框按下热键时的分派函数
	{
		if (CoverEdit == 0)return;
		switch (wParam)
		{
		case 34:EditMove(CoverEdit, -1); break;//<-键
		case 35:EditMove(CoverEdit, 1); break;//->键
		case 36:EditPaste(CoverEdit); break;//粘贴
		case 37:EditCopy(CoverEdit); break;//复制
		case 38://剪切
			if (Edit[CoverEdit].Pos2 != -1)
				EditCopy(CoverEdit),//剪切其实是先复制再删除
				EditDelete(CoverEdit, min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2),
					max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2));
			break;
		case 39:EditAll(CoverEdit); break;//全选
		case 40://Delete键（不是Backspace!）
			if (Edit[CoverEdit].strLength == Edit[CoverEdit].Pos1)break;
			int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//Backspace键直接算在WM_CHAR里面了
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else EditDelete(CoverEdit, pos1, pos1 + 1);
			break;
		}
		return;
	}
	void EditUnHotKey()//取消注册Edit的热键
	{//在点击一个Edit外部时自动执行
		for (int i = 34; i < 41; ++i)AutoUnregisterHotKey(hWnd, i);
		HideCaret(hWnd);//隐藏闪烁的光标
	}
	void EditRegHotKey()//注册Edit的热键
	{//在点击一个Edit时自动执行
		AutoRegisterHotKey(hWnd, 34, NULL, VK_LEFT);//<-
		AutoRegisterHotKey(hWnd, 35, NULL, VK_RIGHT);//-?
		AutoRegisterHotKey(hWnd, 36, MOD_CONTROL, 'V');//粘贴
		AutoRegisterHotKey(hWnd, 37, MOD_CONTROL, 'C');//复制
		AutoRegisterHotKey(hWnd, 38, MOD_CONTROL, 'X');//剪切
		AutoRegisterHotKey(hWnd, 39, MOD_CONTROL, 'A');//全选
		AutoRegisterHotKey(hWnd, 40, NULL, VK_DELETE);//Delete键
		DestroyCaret();//在点击的地方创建闪烁的光标
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
	}
	void EditCHAR(wchar_t wParam)//输入
	{
		if (!Edit[CoverEdit].Press && CoverEdit == 0)return;//没有选择Edit时退出
		if (wParam >= 0x20 && wParam != 0x7F)//当按下的是正常按键时:
		{
			int pos = min(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			int pos2 = max(Edit[CoverEdit].Pos1, Edit[CoverEdit].Pos2);
			if (pos == -1)//没有选中文字(增加)
				EditAdd(CoverEdit, pos2, pos2, wParam);
			else//选中了一段文字(替换)
				EditAdd(CoverEdit, pos, pos2, wParam);
		}
		else if (wParam == VK_BACK)//Backspace键
		{
			int pos1 = Edit[CoverEdit].Pos1, pos2 = Edit[CoverEdit].Pos2;
			if (pos2 != -1)//删除光标的前一个文字
				EditDelete(CoverEdit, min(pos1, pos2), max(pos1, pos2));
			else//删除选中的一段文字
				EditDelete(CoverEdit, pos1 - 1, pos1);
		}
		return;
	}
	void EditAdd(int cur, int Left, int Right, wchar_t Char)//向某个Edit中添加一个字符 或 把一段字符替换成一个字符
	{//这里的Left&Right是字符位置而不是坐标
		int len = Edit[cur].strLength + 5;//计算原Edit中文字的长度
		wchar_t* Tempstr = new wchar_t[len], t = 0;//申请对应长度的缓存空间
		if (Left == Right)t = Edit[cur].str[Left];//如果只是添加字符，则先备份光标右侧的字符
		Edit[cur].str[Left] = '\0';
		wcscpy(Tempstr, Edit[cur].str);//将光标左侧的所有字符复制到缓存中
		Tempstr[Left] = Char;
		Tempstr[Left + 1] = 0;
		Edit[cur].str[Left] = t;//添加新的字符
		wcscat(Tempstr, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left + 1;//将右侧的字符再接上去
		Edit[cur].Pos2 = -1;
		ZeroMemory(Edit[cur].str, sizeof(Edit[cur].str));
		SetEditStrOrFont(Tempstr, 0, cur);//刷新
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void EditDelete(int cur, int Left, int Right)//删除一个或一段字符
	{
		if (Left == -1)return;//在最左边还按下BackSpace ?
		wchar_t* Tempstr = new wchar_t[Edit[cur].strLength];
		Edit[cur].str[Left] = '\0';
		wcscpy(Tempstr, Edit[cur].str);
		wcscat(Tempstr, Edit[cur].str + Right);
		Edit[cur].Pos1 = Left;
		Edit[cur].Pos2 = -1;
		SetEditStrOrFont(Tempstr, 0, cur);
		RefreshXOffset(cur);//刷新
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}
	void EditAll(int cur)//选中一个Edit中所有字符
	{
		if (cur == 0)return;//未选中任何Edit
		Edit[cur].Pos1 = 0;
		Edit[cur].Pos2 = Edit[cur].strLength;
		RefreshXOffset(CoverEdit);
		RefreshCaretByPos(CoverEdit);
		EditRedraw(cur);//刷新
	}

	void EditMove(int cur, int offset)//移动选中的Edit中光标的位置
	{//offset可以为负
		int xback;//offset也可以不是1
		xback = Edit[cur].XOffset;
		if (Edit[cur].Pos2 != -1)return;
		Edit[cur].Pos1 += offset;
		if (Edit[cur].Pos1 < 0)Edit[cur].Pos1 = 0;
		if (Edit[cur].Pos1 > Edit[cur].strLength)Edit[cur].Pos1 = Edit[cur].strLength;
		RefreshXOffset(cur);
		RefreshCaretByPos(cur);
	}

	void EditCopy(int cur)//复制一个Edit中已选中的内容
	{
		if (cur == 0)return;
		wchar_t* EditStr, t;
		char* ClipBoardStr;
		if (Edit[cur].Pos2 == -1)return;
		int pos1 = min(Edit[cur].Pos1, Edit[cur].Pos2);
		int pos2 = max(Edit[cur].Pos1, Edit[cur].Pos2);
		EditStr = new wchar_t[pos2 - pos1 + 1];
		ClipBoardStr = new char[(pos2 - pos1 + 1) * 2];
		t = Edit[cur].str[pos2];
		Edit[cur].str[pos2] = '\0';
		wcscpy(EditStr, Edit[cur].str + pos1);
		Edit[cur].str[pos2] = t;
		WideCharToMultiByte(CP_ACP, 0, EditStr, -1, ClipBoardStr, 0xffff, NULL, NULL);

		if (OpenClipboard(hWnd))
		{
			HGLOBAL clipbuffer;
			char* buffer;
			EmptyClipboard();
			clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(ClipBoardStr) + 1);
			buffer = (char*)GlobalLock(clipbuffer);
			if (buffer != NULL)strcpy(buffer, LPCSTR(ClipBoardStr));
			GlobalUnlock(clipbuffer);
			SetClipboardData(CF_TEXT, clipbuffer);
			CloseClipboard();
		}
	}
	void MyGetTextExtentPoint32(int cur, int start, int end, SIZE& se)
	{//自制获取字符串宽度的函数
		if (*Edit[cur].OStr != 0 || Edit[cur].strW == 0)return;//若还有提示字符串则退出
		if (start == -1)//记录数组里0位时第1个字符的宽度
		{//若想从第1个字符左边开始获取，则start应为-1
			if (end == -1)se.cx = 0; else se.cx = Edit[cur].strW[end];
		}//然而数组[]不能为-1，这里要特殊处理
		else se.cx = Edit[cur].strW[end] - Edit[cur].strW[start];
		se.cy = Edit[cur].strHeight;
	}

	int MyGetTextExtentPoint32Binary(int cur, int point)
	{//通过二分查找，获取指定Edit中宽度大于search的第一个字符位置
		int left = 0, right = Edit[cur].strLength - 1, mid;
		while (right >= left)//循环直至左右指针交叉(找不到)
		{
			mid = (left + right) >> 1;//中=(左+右)/2
			if (Edit[cur].strW[mid] < point)left = mid + 1;//当在point左边时，向右二分
			if (Edit[cur].strW[mid] >= point)//若在point右边
			{//且mid-1在point左边->找到了->退出
				if (Edit[cur].strW[mid - 1] < point)return mid; else
					right = mid - 1;//太过右边->向左二分
			}
		}
		if (left == 0)return 0; else return Edit[cur].strLength - 1;//找不到->返回最左 or 最右
	}

	void RefreshCaretByPos(int cur)//刷新选中的Edit中光标的位置
	{
		if (Edit[cur].Pos1 == -1)return;//指定Edit未被选中->退出
		if (Edit[cur].strLength == 0) {
			CaretPos.x = (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI), CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - 6 * DPI);
			goto finish;
		}
		SIZE se;//通过这个Edit的Pos1的字符来计算字符长度
		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);
		if (Edit[cur].XOffset == 0)//减去Xoffset，再加上Edit的坐标就是光标位置了
			CaretPos.x = se.cx + (long)((Edit[cur].Left + Edit[cur].Width / 2) * DPI) - Edit[cur].strWidth / 2, CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2);
		else
			CaretPos.x = (int)(se.cx + Edit[cur].Left * DPI - Edit[cur].XOffset), CaretPos.y = (int)((Edit[cur].Top + Edit[cur].Height / 2 - 4) * DPI - Edit[cur].strHeight / 2);
	finish:
		SetCaretPos(CaretPos.x, CaretPos.y);
		ShowCaret(hWnd); ShowCrt = true;
	}
	void EditDown(int cur)//鼠标左键在某个Edit上按下
	{
		EditRegHotKey();//先注册下热键再说
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CoverEdit != cur)Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1, EditRedraw(CoverEdit);//如果直接从一个Edit点到
		CoverEdit = cur;//														另一个Edit，那么先把之前的Pos和蓝框问题解决好
		if (*Edit[cur].OStr != 0)//去掉灰色的提示文字
		{
			*Edit[cur].OStr = 0;
			ZeroMemory(Edit[cur].str, sizeof(Edit[cur].str));
		}
		Edit[cur].Pos1 = Edit[cur].Pos2 = -1;
		Edit[cur].Press = true;
		Edit[cur].Pos1 = GetNearestChar(cur, point);//计算Pos1

		RefreshCaretByPos(cur);//计算 闪烁的光标 的位置
		EditRedraw(cur);//重绘这个Edit
	}


	BOOL InsideEdit(int cur, POINT& point)//通过POINT判断是否在指定Edit内
	{//POINT为鼠标真实坐标
		return ((Edit[cur].Left - 5) * DPI <= point.x && Edit[cur].Top * DPI <= point.y &&
			(long)((Edit[cur].Left + Edit[cur].Width + 5) * DPI) >= point.x && (Edit[cur].Top + Edit[cur].Height) * DPI >= point.y);
	}

	void LeftButtonDown()//鼠标左键按下
	{
		POINT point;
		GetCursorPos(&point);//获取真实坐标
		ScreenToClient(hWnd, &point);
		if (CoverButton != -1)//当按钮按下 & 停留在按钮上时
		{
			Press = 1;//重绘这个按钮
			RECT rc = GetRECT(CoverButton);
			if (Obredraw)Readd(REDRAW_BUTTON, CoverButton);
			Redraw(rc);
		}
		if (!InsideEdit(CoverEdit, point) && Edit[CoverEdit].Pos2 == -1 && CoverEdit != 0)
		{
			int tmp0 = CoverEdit;//原来一个Edit被激活
			CoverEdit = 0;//现在鼠标点在那个Edit外面时
			if (Obredraw)Readd(REDRAW_EDIT, tmp0);//重绘原来的Edit
			RECT rc = GetRECTe(tmp0);
			Redraw(rc);
			EditUnHotKey();//取消热键
		}
		if (EditPrv != 0)
		{//鼠标点在另一个Edit上时同样重绘Edit(好吧有点啰嗦= =)
			if (Obredraw)Readd(REDRAW_EDIT, EditPrv);
			RECT rc = GetRECTe(EditPrv);
			Redraw(rc);
			EditPrv = 0;
		}
		if (CoverEdit != 0)//停留在Edit上时
			EditDown(CoverEdit);
		else
			EditUnHotKey();
	}

	void CheckGetNewInside(POINT& point)//检查鼠标是否在任何Check内
	{//POINT为鼠标真实坐标
		for (int i = 1; i <= CurCheck; ++i)//遍历所有check
		{
			if (Check[i].Page == CurWnd || Check[i].Page == 0)//不在同一页面 -> 直接跳过
				if (InsideCheck(i, point) != 0)//在check的文字中或方框内
				{
					CoverCheck = i;//设置covercheck
					if (Obredraw)Readd(REDRAW_CHECK, i);
					RECT rc = GetRECTc(i);//重绘
					Redraw(rc);
					return;
				}
		}
	}
	void ButtonGetNewInside(POINT& point)//检查鼠标是否在任何Button内
	{//POINT为鼠标真实坐标
		for (int i = 0; i <= CurButton; ++i)//历史原因，Button编号是从0开始的
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point))//在按钮中
				{
					CoverButton = i;//设置CoverButton
					if (ButtonEffect)//特效开启
					{//设定渐变色
						Button[i].Percent += 40;//先给40%的颜色 （太淡了看不出来）
						if (Button[i].Percent > 100)Button[i].Percent = 100;
					}
					if (Obredraw)Readd(REDRAW_BUTTON, i);
					RECT rc = GetRECT(i);//重绘
					Redraw(rc);
					return;
				}
	}
	void EditGetNewInside(POINT& point)//检查鼠标是否在任何Edit内
	{//POINT为鼠标真实坐标
		for (int i = 1; i <= CurEdit; ++i)
			if (Edit[i].Page == CurWnd || Edit[i].Page == 0)
				if (InsideEdit(i, point))
				{
					EditPrv = CoverEdit;//记录好之前的Edit编号
					CoverEdit = i;
					return;
				}
	}
	void MouseMove()//鼠标移动
	{
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hWnd, &point);
		if (CoverButton == -1)ButtonGetNewInside(point);//原来不在按钮内 -> 看看现在是否移进按钮
		else//原来在
		{
			if (!Button[CoverButton].Enabled) { CoverButton = -1; goto disabled; }//这个按钮被禁用了  直接跳到下面
			if ((Button[CoverButton].Page != CurWnd && Button[CoverButton].Page != 0) || !InsideButton(CoverButton, point))
			{//现在不在
				if (Obredraw)Readd(REDRAW_BUTTON, CoverButton);
				if (ButtonEffect)
				{//CoverButton设为-1 , 重绘
					Button[CoverButton].Percent -= Delta;
					if (Button[CoverButton].Percent < 0)Button[CoverButton].Percent = 0;
				}
				RECT rc = GetRECT(CoverButton);
				CoverButton = -1;
				Redraw(rc);
				ButtonGetNewInside(point);//有可能从一个按钮直接移进另一个按钮内
			}
		}
	disabled:
		if (CoverCheck == 0)CheckGetNewInside(point);//在外面 -> 寻找新check
		else
		{//同理(真的)
			if ((Check[CoverCheck].Page != CurWnd && Check[CoverCheck].Page != 0) || InsideCheck(CoverCheck, point) == 0)
			{
				if (Obredraw)Readd(REDRAW_CHECK, CoverCheck);
				RECT rc = GetRECTc(CoverCheck);
				CoverCheck = 0;
				Redraw(rc);
				CheckGetNewInside(point);
			}
		}
		if (CoverEdit != 0 && Edit[CoverEdit].Press == true)
		{
			//如果Edit被按下 (同时在拖动选择条)
			int t = Edit[CoverEdit].Pos2;
			Edit[CoverEdit].Pos2 = GetNearestChar(CoverEdit, point);//寻找和鼠标指针最近的字符
			RefreshCaretByPos(CoverEdit);//移动Caret(闪烁的光标)
			if (Edit[CoverEdit].Pos2 == Edit[CoverEdit].Pos1) { Edit[CoverEdit].Pos2 = -1; goto end; }//只选择了一个字符
			if (Edit[CoverEdit].Pos2 != t && Edit[CoverEdit].Width < Edit[CoverEdit].strWidth && !InsideEdit(CoverEdit, point))RefreshXOffset(CoverEdit);//Edit中文本过长，移动到了框外面
			if (Edit[CoverEdit].Pos2 != t)EditRedraw(CoverEdit);//只要和原来有任何不同就重绘
		}
	end:

		if (Msv == 0)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);//检测鼠标移进移出的函数
			tme.hwndTrack = hWnd;//在鼠标移出窗体时会触发一个WM_LEAVE消息，根据这个可以改变按钮颜色
			tme.dwFlags = TME_LEAVE;//缺点是当焦点直接被另一个窗口夺取时(比如按下windows键)
			TrackMouseEvent(&tme);//什么反应都没有
			Msv = 1;//移出
		}
		else Msv = 0;//移进
	}
	void EditRedraw(int cur)//重绘Edit的外壳函数
	{
		if (Obredraw)Readd(REDRAW_EDIT, cur);
		Redraw(GetRECTe(cur));//标准ObjectRedraw写法
	}
	void ButtonRedraw(int cur)//重绘Edit的外壳函数
	{
		if (Obredraw)Readd(REDRAW_BUTTON, cur);
		Redraw(GetRECT(cur));//标准ObjectRedraw写法
	}

	void RefreshXOffset(int cur)//重新计算Edit的Xoffset
	{
		if (Edit[cur].strWidth < Edit[cur].Width * DPI) {
			Edit[cur].XOffset = 0; return;//如果Edit中内容太少，XOffset直接为0，退出
		}
		SIZE se;

		if (Edit[cur].Pos2 != -1)
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos2 - 1, se);//计算长度
		else
			MyGetTextExtentPoint32(cur, -1, Edit[cur].Pos1 - 1, se);//计算长度//过程比较复杂，但原理简单
		if (se.cx < Edit[cur].XOffset)Edit[cur].XOffset = se.cx - 1;
		if (se.cx > (long)(Edit[cur].XOffset + Edit[cur].Width * DPI))Edit[cur].XOffset += (int)(se.cx - (Edit[cur].XOffset + Edit[cur].Width * DPI) - 1);
		if (Edit[cur].strWidth - Edit[cur].XOffset < Edit[cur].Width * DPI)Edit[cur].XOffset = (int)(Edit[cur].strWidth - Edit[cur].Width * DPI);
		RefreshCaretByPos(cur);
		EditRedraw(cur);
	}

	void SetPage(int newPage)//设置窗口的页数
	{
		if (newPage == CurWnd)return;//点了当前页的按钮，直接退出
		HideCaret(hWnd);//换页时自动隐藏闪烁的光标
		ShowCrt = false;
		Edit[CoverEdit].Press = false;
		Edit[CoverEdit].Pos1 = Edit[CoverEdit].Pos2 = -1;
		CoverEdit = 0;//取消Edit的选中
		CurWnd = newPage;
		while (!es.empty())es.pop();
		while (!rs.empty())rs.pop();
		Redraw();//切换页面时当然需要全部重绘啦
	}
	void SetDPI(DOUBLE NewDPI)//改变窗口的缩放大小
	{//							(由于某历史原因，缩放大小的变量被我命名成了DPI)
		if (DPI == NewDPI)return;
		DPI = NewDPI;//创建新大小的字体
		DeleteObject(DefFont);
		DefFont = CreateFontW((int)(16 * DPI), (int)(8 * DPI), 0, 0, FW_THIN, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("宋体"));
		for (int i = 1; i <= CurEdit; ++i)SetEditStrOrFont(0, DefFont, i), RefreshXOffset(i);//设置字体
		RefreshCaretByPos(CoverEdit);
		if (Width != 0 && Height != 0)SetWindowPos(hWnd, NULL, 0, 0, (int)(Width * DPI), (int)(Height * DPI), SWP_NOMOVE | SWP_NOREDRAW);
		DestroyCaret();//设置闪烁的光标
		CreateCaret(hWnd, NULL, 1, (int)(20 * DPI));
		Redraw();//全部重绘
	}

	LPWSTR GetCurInsideID(POINT& point)//获取当前鼠标处于的按钮的ID
	{
		for (int i = 0; i <= CurButton; ++i)
			if ((Button[i].Page == CurWnd || Button[i].Page == 0) && Button[i].Enabled)
				if (InsideButton(i, point) && Button[i].DownTot == 0)
					return Button[i].ID;//返回ID
		return Button[0].ID;//返回一个空值
	}
	inline int GetNumbyID(LPCWSTR ID) { return but[Hash(ID)]; }//通过按钮的ID获取其编号

	void EnableButton(int cur, bool enable)
	{
		Button[cur].Enabled = enable;
		if (Button[cur].Page == CurWnd || Button[cur].Page == 0)
		{
			Readd(REDRAW_BUTTON, cur);
			Redraw(GetRECT(cur));
		}
	}
	void SetHDC(HDC HDc)//给要绘制的窗口设置一个新的hdc
	{
		hdc = HDc;
		if (EditBitmap != NULL)DeleteObject(EditBitmap);//给Edit创建一个Bitmap
		EditBitmap = CreateCompatibleBitmap(hdc, MAX_EDIT_WIDTH, MAX_EDIT_HEIGHT);
	}
	FORCEINLINE void Erase(const RECT& rc) { es.push(rc); }//设置要擦除的区域
	void Redraw(const RECT& rc) { InvalidateRect(hWnd, &rc, FALSE), UpdateWindow(hWnd); }//自动重绘 & 刷新指定区域
	void Redraw()
	{//(remove when CC is for another program)
		while (!es.empty())es.pop();
		while (!rs.empty())rs.pop();
		InvalidateRect(hWnd, nullptr, false),
			UpdateWindow(hWnd);
	}//添加要刷新的控件-、
	void Readd(int type, int cur) { rs.push({ type,cur }); }//1=Frame,2=Button,3=Check,4=Text,5=Edit

	//下面是Class的变量

	struct ButtonEx//按钮
	{
		long Left, Top, Width, Height, Page, Download, Percent, DownCur, DownTot;
		BOOL Enabled=TRUE, Shadow;
		HBRUSH Leave, Hover, Press;//离开 and 悬浮 and 按下
		HPEN Leave2, Hover2, Press2;
		HFONT Font;
		wchar_t Name[31], ID[11];
		COLORREF FontRGB;
		BYTE b1[3], b2[3], p1[3] = { 0 }, p2[3] = { 0 };
	}Button[MAX_BUTTON];//只有按钮使用了ID
	struct FrameEx//控件框结构体
	{
		int Left, Top, Width, Height, Page;
		COLORREF rgb;//自定义颜色
		wchar_t Name[51];
	}Frame[MAX_FRAME];//现在为了节约内存空间都用MAX_XXX了 ， 具体可以到GUI.h里改
	struct CheckEx//选择框结构体
	{
		int Left, Top, Page, Width;//width跟绘制无关，用来检测是否按下
		bool Value;
		wchar_t Name[51];
	}Check[MAX_CHECK];
	struct LineEx//线段
	{
		int StartX, StartY, EndX, EndY, Page;//线段的起始坐标和终点坐标
		COLORREF Color;
	}Line[MAX_LINE];
	struct TextEx//文字
	{
		int Left, Top, Page;
		COLORREF rgb;
		wchar_t Name[11];//这里的"Name"其实是GUIString的ID
	}Text[MAX_TEXT];
	struct EditEx//输入框
	{
		int Left, Top, Width, Height, Page;
		int strWidth, strHeight, Pos1, Pos2, XOffset, strLength;
		int* strW;
		bool Press;
		wchar_t* str, ID[11], OStr[21];
		HFONT font;
	}Edit[MAX_EDIT];
	struct GUIString//带ID标签的字符串
	{
		wchar_t* str, * ID;
		bool first = true;
	}string[MAX_STRING];

	std::map<unsigned int, wchar_t*> str;//GUIstr的ID ->编号(用于索引)
	std::map<unsigned int, int>but;//button的ID -> 编号
	HFONT DefFont;//默认的字体
	int Msv;//鼠标移出检测变量
	int CurString, CurButton, CurFrame, CurCheck, CurLine, CurText, CurEdit;//各种控件的数量
	double DPI = 1;
	int CoverButton, CoverCheck, CoverEdit;//当前被鼠标覆盖的东西
	bool Obredraw = false;//是否启用ObjectRedraw技术
	bool ButtonEffect = false;//是否开启渐变色
	int CurWnd;//当前的页面
	int Press;//鼠标左键是否按下
	POINT CaretPos;
	bool ShowCrt = false;
	struct Mypair { int first, second; };

	std::stack <Mypair>rs;//重绘列表 1=Frame,2=Button,3=Check,4=Text,5=Edit
	std::stack <RECT>es;//清理列表
	HDC hdc;//缓存dc
	HDC tdc;//真实dc
	HBITMAP EditBitmap;//Edit专用缓存bitmap
	HBITMAP Bitmap;//缓存窗口bitmap
	int Width, Height;//窗口的宽和高
	HWND hWnd;//Class绘制的窗体的hwnd
	HINSTANCE hInstance;//程序的hInst
	int EditPrv = 0;//之前被激活的edit序号
//没有任何private变量或函数= =
}Main;

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)//主计时器
{
	UNREFERENCED_PARAMETER(hWnd); UNREFERENCED_PARAMETER(nMsg); UNREFERENCED_PARAMETER(dwTime);
	switch (nTimerid)
	{
	case 1://按钮特效
	{
		if (!Main.ButtonEffect)break;
		for (int i = 1; i <= Main.CurButton; ++i)
		{
			if (Main.CoverButton != i && Main.Button[i].Percent > 0)
			{//不在鼠标上的按钮颜色逐渐变淡
				Main.Button[i].Percent -= Delta;
				if (Main.Button[i].Percent < 0)Main.Button[i].Percent = 0;
				Main.ButtonRedraw(i);
			}
		}
		if (Main.CoverButton != -1 && Main.Button[Main.CoverButton].Percent < 100)
		{//在鼠标上的按钮颜色以双倍速度变深
			Main.Button[Main.CoverButton].Percent += 2 * Delta;
			if (Main.Button[Main.CoverButton].Percent > 100)Main.Button[Main.CoverButton].Percent = 100;
			Main.ButtonRedraw(Main.CoverButton);
		}
		break;
	}
	}
}

void MakeIPstr(wchar_t* dst, wchar_t* s1, wchar_t* s2, wchar_t* s3, wchar_t* s4)//将ip地址的四个部分整合在一起
{
	wcscpy(dst, s1);
	wcscat(dst, L".");
	wcscat(dst, s2);
	wcscat(dst, L".");
	wcscat(dst, s3);
	wcscat(dst, L".");
	wcscat(dst, s4);
}

void SetTextBar(const wchar_t* a)
{
	Main.SetStr(a, L"textstr");
	Main.Readd(4, 11);
	RECT rc{ 15,470,500,492 };
	Main.es.push(rc);
	Main.Redraw(rc);
}

void shutdown2010(char* a, int cse)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)return;

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;

	addrSrv.sin_addr.S_un.S_addr = inet_addr(a);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4605);
	char aa[583] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x2a,0x02,0x00,0x00,0x9b,0xd1,0xfe,0x53,0x66,0x9d,0xe0,0x42,0x87,0xe1,0x8b,0xbd,0x91,0x42,0xd8,0x1e,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x1d,0x02,0x00,0x00,0x1d,0x02,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x14 ,0x00 ,0x00 ,0x10 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x59 ,0x65 ,0x08 ,0x5e ,0x06 ,0x5c ,0x73 ,0x51 ,0xed ,0x95 ,0xa8 ,0x60 ,0x84 ,0x76 ,0xa1 ,0x8b ,0x97 ,0x7b ,0x3a ,0x67 ,0x02 ,0x30 };
	char bb[583] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x2a,0x02,0x00,0x00,0xb9,0x84,0x4a,0x23,0x8f,0x35,0x0b,0x48,0x8f,0x3b,0xab,0x04,0xba,0x51,0xfc,0x2a,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x1d,0x02,0x00,0x00,0x1d,0x02,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x13 ,0x00 ,0x00 ,0x10 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x59 ,0x65 ,0x08 ,0x5e ,0x06 ,0x5c ,0xcd ,0x91 ,0x2f ,0x54 ,0xa8 ,0x60 ,0x84 ,0x76 ,0xa1 ,0x8b ,0x97 ,0x7b ,0x3a ,0x67 ,0x02 ,0x30 };
	char cc[583] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x2a,0x02,0x00,0x00,0x70,0xc5,0x84,0xb1,0x6c,0xa1,0x55,0x4c,0x9f,0x55,0x46,0x27,0x77,0x37,0x73,0x5d,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x1d,0x02,0x00,0x00,0x1d,0x02,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x02 ,0x00 ,0x00 ,0x10 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x59 ,0x65 ,0x08 ,0x5e ,0x06 ,0x5c ,0x73 ,0x51 ,0xed ,0x95 ,0xa8 ,0x60 ,0x84 ,0x76 ,0x94 ,0x5e ,0x28 ,0x75 ,0x0b ,0x7a ,0x8f ,0x5e };
	if (cse == 1)
	{
		aa[19] = (char)GetTickCount();
		sendto(sockClient, aa, 582, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	if (cse == 2)
	{
		bb[19] = (char)GetTickCount();
		sendto(sockClient, bb, 582, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	if (cse == 3)
	{
		cc[19] = (char)GetTickCount();
		cc[20] = (char)(GetTickCount() * 2);
		sendto(sockClient, cc, 582, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	closesocket(sockClient);
	WSACleanup();
	return;
}
void filestart(bool start)//在发送文件前需要执行这个函数
{//但同时，它能让教师端认为同时有多个老师在线，因此能让教师端退出
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)return;

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;

	wchar_t tmp[100];
	char tmp2[100]; size_t t; int i = 0;
	wcscpy_s(tmp, Main.Edit[4].str);
	s(tmp);
	_itow_s(i, tmp, 10);
	if (i > 200) {//隐藏的自定义广播端口功能
		MakeIPstr(tmp, Main.Edit[4].str, L"2", L"2", L"1");
		wcstombs_s(&t, tmp2, tmp, 30);
		s(tmp2);
		addrSrv.sin_addr.S_un.S_addr = inet_addr(tmp2);
	}
	else
		addrSrv.sin_addr.S_un.S_addr = inet_addr("225.2.2.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(5512);

	char bb[] = { 0x4d,0x45,0x53,0x53,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xc0,0xa8,0x3d,0x80,0x2d,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x80,0xc0,0xa8,0x3d,0x81,0xa6,0x15,0xe1,0x02,0x02,0x0c,0xa9,0x15,0x00,0x50,0x00,0x00,0xa0,0x05,0x00,0x00,0x10,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00 };
	char aa[] = { 0x4d,0x45,0x53,0x53,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0 };
	char cc[] = { 0x41,0x4e,0x4e,0x4f,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,0x3d,0x81,0x98,0xc7,0x83,0x08,0x00,0x00,0x00,0x00,0x98,0xc7,0x83,0x08,0x01,0x00,0x00,0x00,0x68,0x64,0x3b,0xa5,0xf0,0x6c,0x18,0x00,0xa8,0xc7,0x83,0x08,0xf4,0x3f,0x18,0x00,0xf0,0x3f,0x18,0x00,0x64,0x60,0x18,0x00,0x00,0x00,00,00,00,00,00,00 };
	char dd[] = { 0x3e,0x31,0x00,0x00,0xc0,0xa8,0x3d,0x80,0x02,0x00,0x00,00,0x01,0x00,00,00 };


	if (start)
		sendto(sockClient, bb, 61, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)),
		sendto(sockClient, cc, 72, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	else
		sendto(sockClient, aa, 25, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	//Sleep(1000);
	closesocket(sockClient);
	WSACleanup();
	return;
}

void shutdown2016(char* a, int cse)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)return;

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;

	addrSrv.sin_addr.S_un.S_addr = inet_addr(a);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4705);
	char aa[583] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x2a,0x02,0x00,0x00,0x9b,0xd1,0xfe,0x53,0x66,0x9d,0xe0,0x42,0x87,0xe1,0x8b,0xbd,0x91,0x42,0xd8,0x1e,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x1d,0x02,0x00,0x00,0x1d,0x02,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x14 ,0x00 ,0x00 ,0x10 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x59 ,0x65 ,0x08 ,0x5e ,0x06 ,0x5c ,0x73 ,0x51 ,0xed ,0x95 ,0xa8 ,0x60 ,0x84 ,0x76 ,0xa1 ,0x8b ,0x97 ,0x7b ,0x3a ,0x67 ,0x02 ,0x30 };
	char bb[583] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x2a,0x02,0x00,0x00,0xb9,0x84,0x4a,0x23,0x8f,0x35,0x0b,0x48,0x8f,0x3b,0xab,0x04,0xba,0x51,0xfc,0x2a,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x1d,0x02,0x00,0x00,0x1d,0x02,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x13 ,0x00 ,0x00 ,0x10 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x59 ,0x65 ,0x08 ,0x5e ,0x06 ,0x5c ,0xcd ,0x91 ,0x2f ,0x54 ,0xa8 ,0x60 ,0x84 ,0x76 ,0xa1 ,0x8b ,0x97 ,0x7b ,0x3a ,0x67 ,0x02 ,0x30 };
	char cc[583] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x2a,0x02,0x00,0x00,0x70,0xc5,0x84,0xb1,0x6c,0xa1,0x55,0x4c,0x9f,0x55,0x46,0x27,0x77,0x37,0x73,0x5d,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x1d,0x02,0x00,0x00,0x1d,0x02,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x02 ,0x00 ,0x00 ,0x10 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x59 ,0x65 ,0x08 ,0x5e ,0x06 ,0x5c ,0x73 ,0x51 ,0xed ,0x95 ,0xa8 ,0x60 ,0x84 ,0x76 ,0x94 ,0x5e ,0x28 ,0x75 ,0x0b ,0x7a ,0x8f ,0x5e };
	char dd[226] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0xc6,0x00,0x00,0x00,0x41,0xe2,0xe9,0x39,0x5a,0xa3,0x0b,0x44,0x94,0xbd,0x7b,0xcc,0xf2,0x95,0x0d,0xe8,0x20,0x4e,0x00,0x00,0xc0,0xa8,0x50,0x01,0xb9,0x00,0x00,0x00,0xb9,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x04,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xe1,0x02,0x03,0x0b,0xa6,0x17,0xe1,0x02,0x03,0x0c,0xa9,0x17,0x01,0x00,0x11,0x2b,0x00,0x00,0x10,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x5e,0x01,0x00,0x00,0x31,0x00,0x31,0x00,0x02,0x00,0x00,0x00,0x00,0x50,0x00,0x00,0xa0,0x05,0x00,0x00,0x01,0x00,0x00,0x00,0x19,0x00,0x00,0x00,0x4b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,0x50,0x01,0x04,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x03,0xe0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	if (cse == 1)
	{
		aa[19] = (char)GetTickCount();
		sendto(sockClient, aa, 582, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	if (cse == 2)
	{
		bb[19] = (char)GetTickCount();
		sendto(sockClient, bb, 582, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	if (cse == 3)
	{
		cc[19] = (char)GetTickCount();
		cc[20] = (char)(GetTickCount() * 2);
		sendto(sockClient, cc, 582, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	if (cse == 4)
	{
		//s(0);
		dd[19] = (char)GetTickCount();
		dd[20] = (char)(GetTickCount() * 2);
		sendto(sockClient, dd, 226, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
		//	sendto(sockClient, ee,44, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	}
	closesocket(sockClient);
	WSACleanup();
	return;
}

void act2016(int Case)//将关机or重启命令依次执行(2015~2017版)
{
	int a = min(_wtoi(Main.Edit[7].str), _wtoi(Main.Edit[8].str)), b = max(_wtoi(Main.Edit[7].str), _wtoi(Main.Edit[8].str));
	for (int i = a; i <= b; ++i)
	{
		wchar_t tmp[100], num[10];
		char tmp2[100]; size_t t;
		_itow_s(i, num, 10);
		MakeIPstr(tmp, Main.Edit[4].str, Main.Edit[5].str, Main.Edit[6].str, num);
		wcstombs_s(&t, tmp2, tmp, 30);
		wchar_t txt[120];
		if (Case == ACT_SHUTDOWN)wcscpy_s(txt, L"正在关机:");
		if (Case == ACT_RESTART)wcscpy_s(txt, L"正在重启:");
		if (Case == ACT_CLOSE)wcscpy_s(txt, L"正在关闭对方程序:");
		if (Case == ACT_WINDOWFY)wcscpy_s(txt, L"正在窗口化极域:");
		wcscat_s(txt, tmp);
		SetTextBar(txt);
		shutdown2016(tmp2, Case);
	}
	SetTextBar(L"命令执行完成");
}
void act2010(int Case)//将关机or重启命令依次执行(2007 2010版)
{
	int a = min(_wtoi(Main.Edit[7].str), _wtoi(Main.Edit[8].str)), b = max(_wtoi(Main.Edit[7].str), _wtoi(Main.Edit[8].str));
	for (int i = a; i <= b; ++i)
	{
		wchar_t tmp[100], num[10];
		char tmp2[100]; size_t t;
		_itow_s(i, num, 10);
		MakeIPstr(tmp, Main.Edit[4].str, Main.Edit[5].str, Main.Edit[6].str, num);
		wcstombs_s(&t, tmp2, tmp, 30);
		wchar_t txt[120];
		if (Case == ACT_SHUTDOWN)wcscpy_s(txt, L"正在关机:");
		if (Case == ACT_RESTART)wcscpy_s(txt, L"正在重启:");
		if (Case == ACT_CLOSE)wcscpy_s(txt, L"正在关闭对方程序:");
		if (Case == ACT_WINDOWFY)wcscpy_s(txt, L"正在窗口化极域:");
		wcscat_s(txt, tmp);
		SetTextBar(txt);
		shutdown2010(tmp2, Case);
	}
	SetTextBar(L"命令执行完成");
}

void text2016(char* a, int cse, char* text, int len)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)return;

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}
	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;

	addrSrv.sin_addr.S_un.S_addr = inet_addr(a);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4705);
	char aa[906] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x6e,0x03,0x00,0x00,0x24,0x2d,0xb8,0xd1,0x58,0xf0,0xed,0x43,0x91,0x86,0x4d,0x05,0x61,0x92,0xf7,0x0e,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x61,0x03,0x00,0x00,0x61,0x03,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x0f ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 };
	char bb[955] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x9e,0x03,0x00,0x00,0x8c,0x35,0x18,0xc4,0x67,0x02,0x59,0x46,0x82,0x64,0x29,0x2b,0x99,0xa6,0xbb,0x4a,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x91,0x03,0x00,0x00,0x91,0x03,0x00,0x00 ,0x00 ,0x08 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x05 ,0x00 ,0x00 ,0x00 };
	char cc[955] = { 0x44,0x4d,0x4f,0x43,0x00,0x00,0x01,0x00,0x38,0x00,0x00,0x00,0x9e,0x95,0xd1,0xb0,0x9e,0x6a,0xc7,0x42,0x9a,0x90,0x54,0xac,0xbe,0x11,0xe4,0x03,0x20,0x4e,0,0,0xc0,0xa8,0x50,0x01,0x85,0x01,0x00,0x00,0x85,0x01,0x00,0x00 ,0x00 ,0x02 ,0x00 ,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x18 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 };

	if (cse == 1)
	{
		aa[19] = (char)GetTickCount();
		aa[20] = (char)(rand() * 2);
		for (int i = 60; i < 60 + len * 2; ++i)aa[i] = text[i - 60];
		sendto(sockClient, aa, 906, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}
	if (cse == 2)
	{
		cc[19] = (char)GetTickCount();
		cc[20] = (char)(GetTickCount() * 2);
		for (int i = 60; i < 60 + len * 2; ++i)cc[i] = text[i - 60];
		sendto(sockClient, cc, 955, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	}
	if (cse == 3)
	{
		bb[19] = (char)GetTickCount();
		bb[20] = (char)(GetTickCount() * 2);
		for (int i = 56; i < 56 + len * 2; ++i)bb[i] = text[i - 56];
		sendto(sockClient, bb, 955, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	}

	closesocket(sockClient);
	WSACleanup();
	return;
}

void act2016text(int Case, wchar_t* text)//将带有文字的命令依次执行
{
	int a = min(_wtoi(Main.Edit[7].str), _wtoi(Main.Edit[8].str)), b = max(_wtoi(Main.Edit[7].str), _wtoi(Main.Edit[8].str));
	wchar_t txt[1001] = { 0 };
	if (wcslen(text) > 1000)text[1000] = 0;
	wcscpy_s(txt, text);
	size_t l = wcslen(txt);
	char txt2[2001] = { 0 };

	for (int i = 0; i < (int)l; ++i) {
		txt2[i * 2 + 1] = (txt[i] >> 8); txt2[i * 2] = txt[i] - ((txt[i] >> 8) << 8);
	}
	for (int i = a; i <= b; ++i)
	{
		wchar_t tmp[100], num[10];
		char tmp2[100]; size_t t;
		_itow_s(i, num, 10);
		MakeIPstr(tmp, Main.Edit[4].str, Main.Edit[5].str, Main.Edit[6].str, num);
		wcstombs_s(&t, tmp2, tmp, 30);
		wchar_t txt[25] = L"发送中:";
		wcscat_s(txt, tmp);
		SetTextBar(txt);
		text2016(tmp2, Case, txt2, l * 2);
	}
	SetTextBar(L"命令执行完成");
}

void CheckIP()//取本机的ip地址  
{
	WSADATA wsaData;
	char name[155];
	PHOSTENT hostinfo;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
	{
		if (gethostname(name, sizeof(name)) == 0)
			if ((hostinfo = gethostbyname(name)) != NULL)
			{//wip存ip地址字符串
				strcpy(ip, inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[0]));
				for (numofips = 0; hostinfo != NULL && hostinfo->h_addr_list[numofips] != NULL; numofips++)
				{
					strcpy(Allips[numofips], inet_ntoa(*(struct in_addr*)hostinfo->h_addr_list[numofips]));
					if (numofips > 19)break;
				}
			}//清理
		WSACleanup();
	}
}

void SendIP2Edit(char* ip)//将获得的IP地址应用到Edit中
{
	char iptmp[30];
	strcpy_s(iptmp, ip);
	wchar_t temp[39] = { 0 }, * pointer = temp, * pointer2 = temp;
	charTowchar(iptmp, temp, sizeof(iptmp));

	pointer = wcsstr(pointer2, L".");
	if (pointer == 0)return;
	*pointer = 0;
	Main.SetEditStrOrFont(pointer2, 0, 4);
	pointer2 = pointer + 1;
	pointer = wcsstr(pointer2, L".");
	if (pointer == 0)return;
	*pointer = 0;
	Main.SetEditStrOrFont(pointer2, 0, 5);
	pointer2 = pointer + 1;
	pointer = wcsstr(pointer2, L".");
	if (pointer == 0)return;
	*pointer = 0;
	Main.SetEditStrOrFont(pointer2, 0, 6);
	Main.SetEditStrOrFont(pointer + 1, 0, 7);
	Main.SetEditStrOrFont(pointer + 1, 0, 8);
}


DWORD WINAPI SearchThread(LPVOID pM)
{
	SearchThreadStruct* sts = (SearchThreadStruct*)pM;
	int begin = sts->ipBegin, end = sts->ipEnd, ii = sts->ii;
	char ip123[30], tmp[10] = { 0 };
	strcpy_s(ip123, sts->ip123);
	strcat_s(ip123, ".");
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	for (int i = begin; i <= end; ++i)
	{
		char fullIP[100];
		wchar_t txt[30] = L"搜索中:", temp2[20];
		if (IPsearched[ii][i] == 1)continue;//寻找过就不要再寻找了
		strcpy_s(fullIP, ip123);
		_itoa_s(i, tmp, 10);
		strcat_s(fullIP, tmp);

		mbstowcs(temp2, fullIP, 20);
		wcscat_s(txt, temp2);
		SetTextBar(txt);


		IPAddr ipaddr = inet_addr(fullIP);
		ULONG ulHopCount, ulRTT;
		if ((BOOL)GetRTTAndHopCount(ipaddr, &ulHopCount, 1, &ulRTT))
		{
			if (IPsearched[ii][i] == 1)continue;
			wchar_t wName[101];
			IPsearched[ii][i] = 1;
			MultiByteToWideChar(CP_ACP, 0, fullIP, -1, wName, 100);
			SendMessage(List, LB_ADDSTRING, 0, (LPARAM)wName);
		}
	}


	WSACleanup();
	return 0;
}
DWORD WINAPI SearchThreadStarter(LPVOID pM)
{
	IPandi* str = (IPandi*)pM;
	int ii = str->i;
	char fullIP[30];
	strcpy(fullIP, str->ip);
	char IP123[30];
	strcpy_s(IP123, fullIP);
	*strrchr(IP123, '.') = 0;
	char* a = IP123 + strlen(IP123) + 1;

	int ipBlock = atoi(a) / 100;
	if (ipBlock != 2)
	{
		for (int i = 0; i < 3; ++i)
		{
			SearchThreadStruct tmp = { i * 20,i * 20 + 19,IP123,ii };
			CreateThread(NULL, 0, SearchThread, &tmp, 0, NULL);
			Sleep(1);
		}
		Sleep(5000);
	}
	for (int i = 0; i < 2; ++i)
	{
		SearchThreadStruct tmp = { i * 128,i * 128 + 127,IP123,ii };
		CreateThread(NULL, 0, SearchThread, &tmp, 0, NULL);
		Sleep(1);
	}
	return 0;
}


DWORD WINAPI SearchAll(LPVOID pM)//寻找局域网中所有电脑的函数
{
	for (int i = 0; i < numofips; ++i)
	{//按照不同网卡寻找
		IPandi a{ Allips[i] ,i };
		CreateThread(0, 0, SearchThreadStarter, &a, 0, NULL);
		Sleep(2);
	}
	return 0;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,//程序入口点
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	if (!InitInstance(hInstance))return FALSE;//初始化
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GUI));

	MSG msg;
	// 主消息循环: 
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

BOOL InitInstance(HINSTANCE hInstance)//初始化
{
	InitBrushes;//初始化画刷

	InitHotKey();//初始化热键系统

	CreateStr;//创建字符串

	HDC hdcScreen = GetDC(NULL);
	int yLength = GetDeviceCaps(hdcScreen, VERTRES);
	DeleteObject(hdcScreen);
	if (yLength <= 1000)Main.DPI = 0.75;//自动缩放
	//Main.DPI = 0.75;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	Main.InitClass(hInst);//初始化主类
	if (!MyRegisterClass(hInst, WndProc, szWindowClass))return FALSE;//初始化Class
	Main.Obredraw = true;//默认使用ObjectRedraw

	Main.hWnd = CreateWindowEx(WS_EX_LAYERED, szWindowClass, Main.GetStr(L"Tmain2"), WS_POPUP, 290, 290, \
		(int)(741 * Main.DPI), (int)(494 * Main.DPI), NULL, nullptr, hInst, nullptr);//创建主窗口
	if (!Main.hWnd)return FALSE;
	Main.Width = 791; Main.Height = 494;

	List = CreateWindowW(L"ListBox", NULL, WS_CHILD | LBS_STANDARD, (int)(578 * Main.DPI), (int)(90 * Main.DPI), (int)(145 * Main.DPI), (int)(360 * Main.DPI), Main.hWnd, (HMENU)1, Main.hInstance, 0);
	::SendMessage(List, WM_SETFONT, (WPARAM)Main.DefFont, 1);//创建List
	ShowWindow(List, SW_SHOW);

	Main.ButtonEffect = TRUE;//开启按钮渐变色特效
	SetTimer(Main.hWnd, 1, 33, (TIMERPROC)TimerProc);
	SetLayeredWindowAttributes(Main.hWnd, NULL, 234, LWA_ALPHA);//半透明特效

	CheckIP();//取本机的ip地址  

	Main.CreateFrame(18, 68, 275, 380, 0, L" 命令 ");//创建各种控件

	Main.CreateButton(38, 92, 105, 45, 0, L"远程关机", L"shutdown");
	Main.CreateButton(38, 151, 105, 45, 0, L"远程重启", L"restart");
	Main.CreateButton(38, 210, 105, 45, 0, L"远程启动程序", L"cmd");
	Main.CreateButton(38, 269, 105, 45, 0, L"远程网页", L"net");
	Main.CreateButton(38, 328, 105, 45, 0, L"远程消息", L"text");
	Main.CreateButton(38, 387, 105, 45, 0, L"退出教师端", L"rfile");

	Main.CreateButton(160, 92, 110, 45, 0, L"远程窗口化", L"rwindow");
	Main.CreateButton(160, 151, 110, 45, 0, L"远程关闭程序", L"rclose");

	Main.CreateEditEx(160 + 5, 210 + 1, 110 - 10, 45, 1, L"输入内容", L"cmd", 0, false);
	Main.CreateEditEx(160 + 5, 269 + 1, 110 - 10, 45, 1, L"输入网页名", L"net", 0, false);
	Main.CreateEditEx(160 + 5, 328 + 1, 110 - 10, 45, 1, L"输入消息", L"net", 0, false);

	Main.CreateFrame(315, 70, 225, 143, 0, L" IP地址 ");

	Main.CreateEditEx(335 + 5, 110, 35 - 10, 30, 1, L"192", L"IP1", 0, true);
	Main.CreateEditEx(385 + 5, 110, 35 - 10, 30, 1, L"168", L"IP2", 0, true);
	Main.CreateEditEx(435 + 5, 110, 35 - 10, 30, 1, L"1", L"IP3", 0, true);
	Main.CreateEditEx(485 + 5, 110, 35 - 10, 30, 1, L"1", L"IP4", 0, true);//from
	Main.CreateEditEx(485 + 5, 168, 35 - 10, 30, 1, L"255", L"IP5", 0, true);//to

	Main.CreateText(55, 17, 0, L"Title", RGB(255, 255, 255));
	Main.CreateText(375, 122, 0, L".", 0);
	Main.CreateText(425, 122, 0, L".", 0);
	Main.CreateText(475, 122, 0, L".", 0);
	Main.CreateText(485, 85, 0, L"fr", 0);
	Main.CreateText(495, 146, 0, L"to", 0);

	Main.CreateFrame(315, 240, 225, 85, 0, L" 极域版本 ");
	Main.CreateCheck(335, 265, 0, 140, L"2016版 & 2015版");
	Main.CreateCheck(335, 292, 0, 140, L"2012版 & 2010版");
	Main.Check[1].Value = true;

	Main.CreateText(320, 340, 0, L"t1", 0);
	Main.CreateText(320, 365, 0, L"t2", 0);
	Main.CreateText(320, 390, 0, L"t3", COLOR_ORANGE);
	Main.CreateText(320, 415, 0, L"t4", COLOR_ORANGE);

	SendIP2Edit(ip);//将获得的IP地址应用到Edit中


	Main.CreateButton(335, 155, 100, 45, 0, L"切换网卡", L"rip");
	Main.CreateButton(687, 55, 33, 28, 0, L"...", L"ri");

	Main.CreateLine(559, 50, 559, 465, 0, COLOR_NORMAL_GREY);
	Main.CreateLine(741, 50, 741, 510, 0, COLOR_NORMAL_GREY);
	Main.CreateLine(0, 464, 741, 464, 0, COLOR_NORMAL_GREY);
	Main.CreateLine(0, 493, 741, 493, 0, COLOR_NORMAL_GREY);
	Main.CreateText(15, 471, 0, L"textstr", 0);

	Main.CreateButtonEx(++Main.CurButton, 660, 10, 60, 30, 0, L"×", \
		CreateSolidBrush(COLOR_CLOSE_LEAVE), CreateSolidBrush(COLOR_CLOSE_HOVER), CreateSolidBrush(COLOR_CLOSE_PRESS), \
		CreatePen(PS_SOLID, 1, COLOR_CLOSE_LEAVE), CreatePen(PS_SOLID, 1, COLOR_CLOSE_HOVER), CreatePen(PS_SOLID, 1, COLOR_CLOSE_PRESS), \
		Main.DefFont, 1,0, COLOR_WHITE, L"Close");

	Main.CreateText(580, 62, 0, L"tr", COLOR_BLACK);

	ShowWindow(Main.hWnd, SW_SHOW);
	Main.Redraw();

	CreateThread(0, 0, SearchAll, 0, 0, NULL);//寻找局域网中的所有电脑

	typedef DWORD(CALLBACK* SEtProcessDPIAware)(void);
	SEtProcessDPIAware SetProcessDPIAware;
	HMODULE huser;//让系统不对这个程序进行缩放
	huser = LoadLibrary(L"user32.dll");//在一些笔记本上有用
	if (huser == 0)return TRUE;
	SetProcessDPIAware = (SEtProcessDPIAware)GetProcAddress(huser, "SetProcessDPIAware");
	if (SetProcessDPIAware != NULL)SetProcessDPIAware();

	return TRUE;
}

//响应函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)//主窗口响应函数
{
	switch (message)
	{
	case WM_CLOSE://关闭
	{
		PostQuitMessage(0);
		break;
	}
	case WM_CHAR://给Edit转发消息
	{
		Main.EditCHAR((wchar_t)wParam);
		break;
	}
	case WM_CREATE://创建窗口
	{
		rdc = GetDC(Main.hWnd);//创建bitmap
		hdc = CreateCompatibleDC(rdc);
		hBmp = CreateCompatibleBitmap(rdc, 800, 600);
		SelectObject(hdc, hBmp);
		ReleaseDC(Main.hWnd, rdc);
		break;
	}
	case WM_HOTKEY:
	{
		Main.EditHotKey((int)wParam);
		break;
	}
	case WM_COMMAND://当控件接收到消息时会触发这个
	{
		switch (LOWORD(wParam))
		{
		case 1:
			switch (HIWORD(wParam))
			{
			case LBN_SELCHANGE:
				wchar_t ip3[MAX_PATH] = { 0 };
				char ip4[300] = { 0 };
				SendMessage(List, LB_GETTEXT, ::SendMessage(List, LB_GETCURSEL, 0, 0), (LPARAM)ip3);
				WideCharToMultiByte(CP_ACP, 0, ip3, -1, ip4, 300, NULL, NULL);

				SendIP2Edit(ip4);
				Main.Redraw();
				ShowWindow(List, SW_HIDE);
				ShowWindow(List, SW_SHOW);
				break;
			}break;
		}
		break;
	}
	case WM_PAINT://绘图
	{
		RECT rc; bool f = false; HICON hicon;
		GetUpdateRect(hWnd, &rc, false);
		if (rc.top != 0)f = true;
		if (Main.hdc == NULL)Main.SetHDC(hdc);
		PAINTSTRUCT ps;
		rdc = BeginPaint(hWnd, &ps);
		if (!Main.es.empty())//根据es来擦除区域
		{
			SelectObject(Main.hdc, WhitePen);
			SelectObject(Main.hdc, WhiteBrush);
			while (!Main.es.empty())
			{
				Rectangle(Main.hdc, Main.es.top().left, Main.es.top().top, Main.es.top().right, Main.es.top().bottom);
				Main.es.pop();
			}
		}
		if (!Main.rs.empty())
		{
			while (!Main.rs.empty())
			{
				Main.RedrawObject(Main.rs.top().first, Main.rs.top().second);
				Main.rs.pop();//根据rs用redrawobject绘制
			}
			goto finish;
		}
		SetBkMode(rdc, TRANSPARENT);
		SetBkMode(hdc, TRANSPARENT);

		SelectObject(hdc, WhiteBrush);//白色背景
		SelectObject(hdc, NormalGreyPen);
		Rectangle(hdc, 0, 0, (int)(900 * Main.DPI), (int)(Main.Height * Main.DPI + 1));

		SelectObject(hdc, TitlePen);
		SelectObject(hdc, TitleBrush);
		Rectangle(hdc, 0, 0, (int)(900 * Main.DPI), (int)(50 * Main.DPI));

		SetTextColor(hdc, RGB(0, 0, 0));
		SelectObject(hdc, BlackPen);
		SelectObject(hdc, WhiteBrush);

		Main.DrawEVERYTHING();//重绘全部
		hicon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_GUI));
		DrawIconEx(Main.hdc, (int)(18 * Main.DPI), (int)(10 * Main.DPI), hicon, (int)(32 * Main.DPI), (int)(32 * Main.DPI), 0, NULL, DI_NORMAL | DI_COMPAT);
		DeleteObject(hicon);
	finish://贴图
		BitBlt(rdc, rc.left, rc.top, max((long)(Main.Width * Main.DPI), rc.right - rc.left), max((long)(Main.Height * Main.DPI), rc.bottom - rc.top), hdc, rc.left, rc.top, SRCCOPY);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_LBUTTONDOWN://点下鼠标左键时
	{
		POINT point; GetCursorPos(&point); ScreenToClient(Main.hWnd, &point);
		Main.EditGetNewInside(point);//试图预先确定一下是否点在某个控件内;
		Main.ButtonGetNewInside(point);
		Main.CheckGetNewInside(point);//点在控件内 -> 触发控件特效
		if (Main.CoverButton != -1 || Main.CoverCheck != 0 || Main.CoverEdit != 0)Main.LeftButtonDown();
		else
		{
			PostMessage(Main.hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);//点在外面 -> 拖动窗口
		}
		break;
	}
	case WM_LBUTTONUP://抬起鼠标左键时
		if (Main.CoverButton != -1)//这时候就要做出相应的动作了
		{
			Main.Press = 0;
			Main.ButtonRedraw(Main.CoverButton);
			Main.ButtonRedraw(Main.CoverButton);
			if (Main.CoverEdit != 0)
			{
				int tmp0 = Main.CoverEdit;
				Main.EditUnHotKey();
				Main.Edit[Main.CoverEdit].Press = false;
				Main.CoverEdit = 0;
				Main.EditRedraw(tmp0);
				if (Main.Edit[tmp0].Pos2 != -1)break;
			}
		}
		if (Main.CoverEdit == 0)Main.EditUnHotKey();
		Main.Edit[Main.CoverEdit].Press = false;
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(Main.hWnd, &point);

		switch (Main.CoverButton)//按钮
		{
		case 1://远程关机
		{
			if (Main.Check[1].Value)act2016(ACT_SHUTDOWN);
			if (Main.Check[2].Value)act2010(ACT_SHUTDOWN);
			break;
		}
		case 2://远程重启
		{
			if (Main.Check[1].Value)act2016(ACT_RESTART);
			if (Main.Check[2].Value)act2010(ACT_RESTART);
			break;
		}
		case 3://远程启动程序
			if (Main.Edit[1].str == 0)break;
			if (*Main.Edit[1].str == 0)break;
			act2016text(ACTEXT_EXE, Main.Edit[1].str);
			break;
		case 4://远程网页
			if (Main.Edit[2].str == 0)break;
			if (*Main.Edit[2].str == 0)break;
			act2016text(ACTEXT_HTTP, Main.Edit[2].str);
			break;
		case 5://发消息
			if (Main.Edit[3].str == 0)break;
			if (*Main.Edit[3].str == 0)break;
			act2016text(ACTEXT_MESSAGE, Main.Edit[3].str);
			break;
		case 6://关闭教师端
			filestart(true);
			break;
		case 7://远程极域窗口化
		{
			if (Main.Check[1].Value)act2016(ACT_WINDOWFY);
			break;
		}
		case 8://远程关闭程序
		{
			if (Main.Check[1].Value)act2016(ACT_CLOSE);
			if (Main.Check[2].Value)act2010(ACT_CLOSE);
			break;
		}
		case 9://切换网卡
		{
			if (++curips >= numofips)curips = 0;
			SendIP2Edit(Allips[curips]);
			IPandi a{ Allips[curips] ,curips };
			CreateThread(0, 0, SearchThread, &a, 0, NULL);
			Main.Redraw();
			break;
		}
		case 10://重新寻找局域网所有电脑
		{
			CreateThread(0, 0, SearchAll, 0, 0, NULL);
			break;
		}
		case 11://退出
		{
			PostQuitMessage(0);
			break;
		}
		break;
		}

		if (Main.CoverCheck != 0)
		{
			Main.Check[Main.CoverCheck].Value = !Main.Check[Main.CoverCheck].Value;
			Main.Readd(REDRAW_CHECK, Main.CoverCheck);
			Main.Redraw(Main.GetRECTc(Main.CoverCheck));
		}

		break;

	case WM_MOUSEMOVE: {Main.MouseMove(); break; }

	case WM_MOUSELEAVE://TrackMouseEvent带来的消息
		PostMessage(Main.hWnd, WM_MOUSEMOVE, NULL, 0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}