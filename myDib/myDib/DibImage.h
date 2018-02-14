#pragma once

#include <complex>
using namespace std;


DECLARE_HANDLE(HDIB);		// DIB���

// Dib�ļ�ͷ��־���ַ���"BM"��дDIBʱ�õ��ó�����
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')//����0x4D42

// �ж��Ƿ���Win 3.0��DIB
#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

#define PALVERSION   0x300	// DIB����

// �����������Ŀ��
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
// �����������ĸ߶�
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// �ڼ���ͼ���Сʱ�����ù�ʽ��biSizeImage = biWidth' �� biHeight��
// ��biWidth'��������biWidth�������biWidth'������4������������ʾ
// ���ڻ����biWidth�ģ���4�������������WIDTHBYTES������������
// biWidth'
#define LineBytes(bits)    (((bits) + 31) / 32 * 4)

class CMyDib
{
public:
	CMyDib();
	virtual ~CMyDib();

	//  DIB(Independent Bitmap) ����
	HDIB    ReadDIBFile(CFile& file);
	DWORD   DIBWidth(LPSTR lpDIB);
	DWORD   DIBHeight(LPSTR lpDIB);
	BOOL    CreateDIBPalette(HDIB hDIB, CPalette* cPal);
	WORD    DIBNumColors(LPSTR lpbi);
	BOOL    PaintDIB(HDC, LPRECT, HDIB, LPRECT, CPalette* pPal);
	LPSTR   FindDIBData(LPSTR lpDib);
	WORD    PaletteSize(LPSTR lpbi);
	bool    SaveDIB(HDIB hDib, CFile& file);

	// ͼ�񼸺α任����
	bool	MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, BOOL bDirection);
	bool	TranslationDIB1(LPSTR lpDIBBits, LONG lWidth, LONG lHeight,
		LONG lXOffset, LONG lYOffset);
	// ͼ��ģ��任������������
	bool Template(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, int iTempH, int iTempW,
		int iTempMX, int iTempMY,FLOAT * fpArray, FLOAT fCoef);
	bool EdgeDetection(LPSTR lpDIBBits, DWORD lmageWidth, DWORD lmageHeight);//������������ȡ��Ե
	bool RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight);//�޲�������
	bool PrewittEdge(LPSTR lpDIBBits, unsigned long lWidth, unsigned long lHeight);
};

