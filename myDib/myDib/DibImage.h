#pragma once

#include <complex>
using namespace std;


DECLARE_HANDLE(HDIB);		// DIB句柄

// Dib文件头标志（字符串"BM"，写DIB时用到该常数）
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')//就是0x4D42

// 判断是否是Win 3.0的DIB
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

#define PALVERSION   0x300	// DIB常量

// 计算矩形区域的宽度
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
// 计算矩形区域的高度
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// 在计算图像大小时，采用公式：biSizeImage = biWidth' × biHeight。
// 是biWidth'，而不是biWidth，这里的biWidth'必须是4的整倍数，表示
// 大于或等于biWidth的，离4最近的整倍数。WIDTHBYTES就是用来计算
// biWidth'
#define LineBytes(bits)    (((bits) + 31) / 32 * 4)

class CMyDib
{
public:
	CMyDib();
	virtual ~CMyDib();

	//  DIB(Independent Bitmap) 函数
	HDIB    ReadDIBFile(CFile& file);
	DWORD   DIBWidth(LPSTR lpDIB);
	DWORD   DIBHeight(LPSTR lpDIB);
	BOOL    CreateDIBPalette(HDIB hDIB, CPalette* cPal);
	WORD    DIBNumColors(LPSTR lpbi);
	BOOL    PaintDIB(HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
	LPSTR   FindDIBData(LPSTR lpDib);
	WORD    PaletteSize(LPSTR lpbi);
	bool    SaveDIB(HDIB hDib, CFile& file);

	// 图像几何变换函数
	bool	MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bDirection);
	bool	TranslationDIB1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight,
		LONG lXOffset, LONG lYOffset);
	// 图像模板变换函数及锐化算子
	bool Template(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int iTempH, int iTempW,
		int iTempMX, int iTempMY,FLOAT * fpArray, FLOAT fCoef);
	bool EdgeDetection(LPSTR lpDIBBits, DWORD lmageWidth, DWORD lmageHeight);//索贝尔算子提取边缘
	bool RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);//罗伯特算子
	bool PrewittEdge(LPSTR lpDIBBits, unsigned long lWidth, unsigned long lHeight);
};

