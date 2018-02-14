#include "stdafx.h"
#include "DibImage.h"

#include "math.h"
#include<complex>
using namespace std;

#define PI 3.141592653

CMyDib::CMyDib()
{
}

CMyDib::~CMyDib()
{
}

/*************************************************************************
* 函数名称：
*   ReadDIBFile()
* 参数:
*   CFile& file        - 要读取得文件文件CFile
* 返回值:
*   HDIB               - 成功返回DIB的句柄，否则返回NULL。
* 说明:
*   该函数将指定的文件中的DIB对象读到指定的内存区域中。除BITMAPFILEHEADER
* 外的内容都将被读入内存。
*************************************************************************/
HDIB CMyDib::ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	HDIB hDIB;
	LPSTR pDIB;
	DWORD dwBitsSize;

	dwBitsSize = file.GetLength();		// 获取DIB（文件）长度（字节）

	// 尝试读取DIB文件头
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		return NULL;
	}
	// 判断是否是DIB对象，检查头两个字节是否是"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		return NULL;
	}
	// 为DIB分配内存
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return NULL;
	}

	pDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER))		// 读象素
	{
		::GlobalUnlock((HGLOBAL)hDIB);
		::GlobalFree((HGLOBAL)hDIB);
		return NULL;
	}

	::GlobalUnlock((HGLOBAL)hDIB);
	return hDIB;
}

/*************************************************************************
* 函数名称：
*   DIBWidth()
* 参数:
*   LPSTR lpbi         - 指向DIB对象的指针
* 返回值:
*   DWORD              - DIB中图像的宽度
* 说明:
*   该函数返回DIB中图像的宽度。对于Windows 3.0 DIB，返回BITMAPINFOHEADER
* 中的biWidth值；对于其它返回BITMAPCOREHEADER中的bcWidth值。
************************************************************************/
DWORD CMyDib::DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// 返回DIB中图像的宽度
	if (IS_WIN30_DIB(lpDIB))
	{
		return lpbmi->biWidth;		// 对于Windows 3.0 DIB，返回lpbmi->biWidth
	}
	else
	{
		return (DWORD)lpbmc->bcWidth;	// 对于其它格式的DIB，返回lpbmc->bcWidth
	}
}

/*************************************************************************
* 函数名称：
*   DIBHeight()
* 参数:
*   LPSTR lpDIB        - 指向DIB对象的指针
* 返回值:
*   DWORD              - DIB中图像的高度
* 说明:
*   该函数返回DIB中图像的高度。对于Windows 3.0 DIB，返回BITMAPINFOHEADER
* 中的biHeight值；对于其它返回BITMAPCOREHEADER中的bcHeight值。
************************************************************************/
DWORD CMyDib::DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPCOREHEADER lpbmc;	// 指向BITMAPCOREINFO结构的指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// 返回DIB中图像的宽度
	if (IS_WIN30_DIB(lpDIB))
	{
		return lpbmi->biHeight;		// 对于Windows 3.0 DIB，返回lpbmi->biHeight
	}
	else
	{
		return (DWORD)lpbmc->bcHeight;	// 对于其它格式的DIB，返回lpbmc->bcHeight
	}
}

/*************************************************************************
* 函数名称：
*   CreateDIBPalette()
* 参数:
*   HDIB hDIB          - 指向DIB对象的指针
*   CPalette* pPal     - 指向DIB对象调色板的指针
* 返回值:
*   BOOL               - 创建成功返回TRUE，否则返回FALSE。
* 说明:
*   该函数按照DIB创建一个逻辑调色板，从DIB中读取颜色表并存到调色板中，
* 最后按照该逻辑调色板创建一个新的调色板，并返回该调色板的句柄。这样
* 可以用最好的颜色来显示DIB图像。
************************************************************************/
BOOL CMyDib::CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{

	LPLOGPALETTE lpPal;		// 指向逻辑调色板的指针
	HANDLE hLogPal;			// 逻辑调色板的句柄
	HPALETTE hPal = NULL;	// 调色板的句柄
	int i;					// 循环变量	
	WORD wNumColors;		// 颜色表中的颜色数目	
	LPSTR lpbi;				// 指向DIB的指针	
	LPBITMAPINFO lpbmi;		// 指向BITMAPINFO结构的指针（Win3.0）	
	LPBITMAPCOREINFO lpbmc;	// 指向BITMAPCOREINFO结构的指针	
	BOOL bWinStyleDIB;		// 表明是否是Win3.0 DIB的标记	
	BOOL bResult = FALSE;	// 创建结果

	if (hDIB == NULL)
	{
		return FALSE;
	}

	lpbi = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);	// 锁定DIB
	lpbmi = (LPBITMAPINFO)lpbi;		// 获取指向BITMAPINFO结构的指针（Win3.0）
	lpbmc = (LPBITMAPCOREINFO)lpbi;	// 获取指向BITMAPCOREINFO结构的指针
	wNumColors = DIBNumColors(lpbi);// 获取DIB中颜色表中的颜色数目

	if (wNumColors != 0)
	{
		// 分配为逻辑调色板内存
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
			+sizeof(PALETTEENTRY)
			* wNumColors);
		// 如果内存不足，退出
		if (hLogPal == 0)
		{
			::GlobalUnlock((HGLOBAL)hDIB);	// 解除锁定
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)hLogPal);
		lpPal->palVersion = PALVERSION;			// 设置版本号
		lpPal->palNumEntries = (WORD)wNumColors;// 设置颜色数目
		bWinStyleDIB = IS_WIN30_DIB(lpbi);		// 判断是否是WIN3.0的DIB

		// 读取调色板
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// 读取红色绿色蓝色分量
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				// 保留位
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				// 读取红色绿色蓝色分量
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				// 保留位
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		bResult = pPal->CreatePalette(lpPal);// 按照逻辑调色板创建调色板，并返回指针
		::GlobalUnlock((HGLOBAL)hLogPal);	// 解除锁定
		::GlobalFree((HGLOBAL)hLogPal);	// 释放逻辑调色板
	}

	::GlobalUnlock((HGLOBAL)hDIB);			// 解除锁定
	return bResult;
}

/*************************************************************************
* 函数名称：
*   DIBNumColors()
* 参数:
*   LPSTR lpbi         - 指向DIB对象的指针
* 返回值:
*   WORD               - 返回调色板中颜色的种数
* 说明:
*   该函数返回DIB中调色板的颜色的种数。对于单色位图，返回2，
* 对于16色位图，返回16，对于256色位图，返回256；对于真彩色
* 位图（24位），没有调色板，返回0。
************************************************************************/
WORD CMyDib::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;

	// 对于Windows的DIB, 实际颜色的数目可以比象素的位数要少。
	// 对于这种情况，则返回一个近似的数值。

	// 判断是否是WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;	// 读取dwClrUsed值

		if (dwClrUsed != 0)
		{
			// 如果dwClrUsed（实际用到的颜色数）不为0，直接返回该值
			return (WORD)dwClrUsed;
		}
	}

	// 读取象素的位数
	if (IS_WIN30_DIB(lpbi))
	{
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;	// 读取biBitCount值
	}
	else
	{
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;	// 读取biBitCount值
	}

	// 按照象素的位数计算颜色数目
	switch (wBitCount)
	{
	case 1:
		return 2;
		break;
	case 4:
		return 16;
		break;
	case 8:
		return 256;
		break;
	default:
		return 0;
		break;
	}
}

/*************************************************************************
* 函数名称：
*   PaintDIB()
* 参数:
*   HDC hDC            - 输出设备DC
*   LPRECT lpDCRect    - 绘制矩形区域
*   HDIB hDIB          - 指向DIB对象的指针
*   LPRECT lpDIBRect   - 要输出的DIB区域
*   CPalette* pPal     - 指向DIB对象调色板的指针
* 返回值:
*   BOOL               - 绘制成功返回TRUE，否则返回FALSE。
* 说明:
*   该函数主要用来绘制DIB对象。其中调用了StretchDIBits()或者
* SetDIBitsToDevice()来绘制DIB对象。输出的设备由由参数hDC指
* 定；绘制的矩形区域由参数lpDCRect指定；输出DIB的区域由参数
* lpDIBRect指定。
************************************************************************/
BOOL CMyDib::PaintDIB(HDC     hDC,
	LPRECT  lpDCRect,
	HDIB    hDIB,
	LPRECT  lpDIBRect,
	CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // BITMAPINFOHEADER指针
	LPSTR    lpDIBBits;           // DIB象素指针
	BOOL     bSuccess = FALSE;      // 成功标志
	HPALETTE hPal = NULL;           // DIB调色板
	HPALETTE hOldPal = NULL;        // 以前的调色板

	if (hDIB == NULL)
	{
		return FALSE;
	}

	lpDIBHdr = (LPSTR)::GlobalLock((HGLOBAL)hDIB);// 锁定DIB	
	lpDIBBits = FindDIBData(lpDIBHdr);	// 找到DIB图像象素起始位置

	if (pPal != NULL)					// 获取DIB调色板，并选中它
	{
		hPal = (HPALETTE)pPal->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);	// 选中调色板
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);			// 设置显示模式

	// 判断是调用StretchDIBits()还是SetDIBitsToDevice()来绘制DIB对象
	if ((RECTWIDTH(lpDCRect) == RECTWIDTH(lpDIBRect)) &&
		(RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// 原始大小，不用拉伸。
		bSuccess = ::SetDIBitsToDevice(hDC,                    // hDC
			lpDCRect->left,             // DestX
			lpDCRect->top,              // DestY
			RECTWIDTH(lpDCRect),        // nDestWidth
			RECTHEIGHT(lpDCRect),       // nDestHeight
			lpDIBRect->left,            // SrcX
			(int)DIBHeight(lpDIBHdr) -
			lpDIBRect->top -
			RECTHEIGHT(lpDIBRect),   // SrcY
			0,                          // nStartScan
			(WORD)DIBHeight(lpDIBHdr),  // nNumScans
			lpDIBBits,                  // lpBits
			(LPBITMAPINFO)lpDIBHdr,     // lpBitsInfo
			DIB_RGB_COLORS);            // wUsage
	}
	else
	{
		// 非原始大小，拉伸。
		bSuccess = ::StretchDIBits(hDC,                          // hDC
			lpDCRect->left,                 // DestX
			lpDCRect->top,                  // DestY
			RECTWIDTH(lpDCRect),            // nDestWidth
			RECTHEIGHT(lpDCRect),           // nDestHeight
			lpDIBRect->left,                // SrcX
			lpDIBRect->top,                 // SrcY
			RECTWIDTH(lpDIBRect),           // wSrcWidth
			RECTHEIGHT(lpDIBRect),          // wSrcHeight
			lpDIBBits,                      // lpBits
			(LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
			DIB_RGB_COLORS,                 // wUsage
			SRCCOPY);                       // dwROP
	}

	::GlobalUnlock((HGLOBAL)hDIB);				// 解除锁定	
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);	// 恢复以前的调色板
	}
	return bSuccess;
}

//功能：该函数计算DIB中图像象素的起始位置，并返回指向它的指针。
//参数:指向DIB对象的指针
//返回值:指向DIB图像象素起始位置的指针
LPSTR CMyDib::FindDIBData(LPSTR lpDib)
{
	return (lpDib + *(LPDWORD)lpDib + PaletteSize(lpDib));
}

/*************************************************************************
* 函数名称：
*   PaletteSize()
* 参数:
*   LPSTR lpbi         - 指向DIB对象的指针
* 返回值:
*   WORD               - DIB中调色板的大小
* 说明:
*   该函数返回DIB中调色板的大小。对于Windows 3.0 DIB，返回颜色数目×
* RGBQUAD的大小；对于其它返回颜色数目×RGBTRIPLE的大小。
************************************************************************/
WORD CMyDib::PaletteSize(LPSTR lpbi)
{
	// 计算DIB中调色板的大小
	if (IS_WIN30_DIB(lpbi))
	{
		//返回颜色数目×RGBQUAD的大小
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
	}
	else
	{
		//返回颜色数目×RGBTRIPLE的大小
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
	}
}

//功能：水平移动DIB图像。函数不会改变图像的大小，移出的部分图像
//		将截去，空白部分用白色填充。
//参数：lpDIBBits    指向源DIB图像指针
//		lWidth 源图像宽度（象素数）
//		lHeight - 源图像高度（象素数）
//		lXOffset     - X轴平移量（象素数）
//		lYOffset     - Y轴平移量（象素数）
//返回值：平移成功返回TRUE，否则返回FALSE。
bool CMyDib::TranslationDIB1(LPSTR lpDIBBits, long lWidth, long lHeight,
	long lXOffset, long lYOffset)
{
	LPSTR	lpSrc;					// 指向源图像的指针	
	LPSTR	lpDst;					// 指向要复制区域的指针
	LPSTR	lpNewDIBBits;			// 指向复制图像的指针
	HLOCAL	hNewDIBBits;
	LONG	i;						// 象素在新DIB中的坐标
	LONG	j;
	LONG	i0;						// 象素在源DIB中的坐标
	LONG	j0;
	LONG lLineBytes;				// 图像每行的字节数

	lLineBytes = LineBytes(lWidth * 8);// 计算图像每行的字节数

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	for (i = 0; i < lHeight; i++)					// 每行
	{
		for (j = 0; j < lWidth; j++)					// 每列
		{
			// 指向新DIB第i行，第j个象素的指针
			// 注意由于DIB中图像第一行其实保存在最后一行的位置，因此lpDst
			// 值不是(char *)lpNewDIBBits + lLineBytes * i + j，而是
			// (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;

			// 计算该象素在源DIB中的坐标
			i0 = i - lXOffset;
			j0 = j - lYOffset;

			/*i0 = i;
			j0 = j;*/

			// 判断是否在源图范围内
			if ((j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// 指向源DIB第i0行，第j0个象素的指针
				// 同样要注意DIB上下倒置的问题
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;
				*lpDst = *lpSrc;					// 复制象素
			}
			else
			{
				 //对于源图中没有的象素，直接赋值为255
				*((unsigned char*)lpDst) = 255;
			}
		}
	}

	// 复制平移后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
* 函数名称：
*   SaveDIB()
* 参数:
*   HDIB hDib          - 要保存的DIB
*   CFile& file        - 保存文件CFile
* 返回值:
*   BOOL               - 成功返回TRUE，否则返回FALSE或者CFileException
* 说明:
*   该函数将指定的DIB对象保存到指定的CFile中。该CFile由调用程序打开和关闭。
*************************************************************************/
bool CMyDib::SaveDIB(HDIB hDib, CFile& file)
{
	BITMAPFILEHEADER bmfHdr;	// Bitmap文件头	
	LPBITMAPINFOHEADER lpBI;	// 指向BITMAPINFOHEADER的指针
	DWORD dwDIBSize;			// DIB大小

	if (hDib == NULL)
	{
		return FALSE;
	}

	// 读取BITMAPINFO结构，并锁定
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL)hDib);
	if (lpBI == NULL)
	{
		return FALSE;
	}

	// 判断是否是WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// 不支持其它类型的DIB保存
		::GlobalUnlock((HGLOBAL)hDib);
		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////
	// 填充文件头///////////////////////////////////////////////////////////	
	bmfHdr.bfType = DIB_HEADER_MARKER;		// 文件类型"BM"

	// 计算DIB大小时，最简单的方法是调用GlobalSize()函数。但是全局内存大小并
	// 不是DIB真正的大小，它总是多几个字节。这样就需要计算一下DIB的真实大小。

	// 文件头大小＋颜色表大小
	// （BITMAPINFOHEADER和BITMAPCOREHEADER结构的第一个DWORD都是该结构的大小）
	dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);

	// 计算图像大小
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// 对于RLE位图，没法计算大小，只能信任biSizeImage内的值
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;			// 象素的大小
		dwBmBitsSize = LineBytes((lpBI->biWidth)*((DWORD)lpBI->biBitCount))
			* lpBI->biHeight;		// 大小为Width * Height
		dwDIBSize += dwBmBitsSize;	// 计算出DIB真正的大小

		// 更新biSizeImage（很多BMP文件头中biSizeImage的值是错误的）
		lpBI->biSizeImage = dwBmBitsSize;
	}

	// 计算文件大小：DIB大小＋BITMAPFILEHEADER结构大小
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);

	// 两个保留字
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// 计算偏移量bfOffBits，它的大小为Bitmap文件头大小＋DIB头大小＋颜色表大小
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+lpBI->biSize
		+ PaletteSize((LPSTR)lpBI);

	/////////////////////////////////////////////////////////////////////////
	// 尝试写文件////////////////////////////////////////////////////////////
	TRY
	{
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));	// 写文件头
		file.Write(lpBI, dwDIBSize);					// 写DIB头和象素
	}
	CATCH(CFileException, e)
	{
		::GlobalUnlock((HGLOBAL)hDib);
		THROW_LAST();
	}
	END_CATCH

		::GlobalUnlock((HGLOBAL)hDib);
	return TRUE;
}

//功能：该函数用来镜像DIB图像。可以指定镜像的方式是水平还是垂直。
//参数:
//   LPSTR lpDIBBits - 指向源DIB图像指针
//   LONG  lWidth - 源图像宽度（象素数）
//   LONG  lHeight - 源图像高度（象素数）
//   BOOL  bDirection - 镜像的方向，TRUE表示水平镜像，FALSE表示垂直镜像
//返回值:镜像成功返回TRUE，否则返回FALSE。
bool CMyDib::MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight,
	BOOL bDirection)
{
	LPSTR	lpSrc;						// 指向源图像的指针	
	LPSTR	lpDst;						// 指向要复制区域的指针	
	LPSTR	lpBits;						// 指向复制图像的指针
	HLOCAL	hBits;
	LONG	i;							// 循环变量
	LONG	j;
	LONG lLineBytes;					// 图像每行的字节数	

	lLineBytes = LineBytes(lWidth * 8);// 计算图像每行的字节数		
	hBits = LocalAlloc(LHND, lLineBytes);	// 暂时分配内存，以保存一行图像
	if (hBits == NULL)
	{
		return FALSE;
	}

	lpBits = (char *)LocalLock(hBits);

	// 判断镜像方式
	if (bDirection)		// 水平镜像
	{
		// 针对图像每行进行操作
		for (i = 0; i < lHeight; i++)
		{
			// 针对每行图像左半部分进行操作
			for (j = 0; j < lWidth / 2; j++)
			{
				// 指向倒数第i行，第j个象素的指针
				lpSrc = (char *)lpDIBBits + lLineBytes * i + j;
				// 指向倒数第i行，倒数第j个象素的指针
				lpDst = (char *)lpDIBBits + lLineBytes * (i + 1) - j;

				// 备份一个象素
				*lpBits = *lpDst;

				// 将倒数第i行，第j个象素复制到倒数第i行，倒数第j个象素
				*lpDst = *lpSrc;
				// 将倒数第i行，倒数第j个象素复制到倒数第i行，第j个象素
				*lpSrc = *lpBits;
			}
		}
	}
	else				// 垂直镜像
	{
		// 针对上半图像进行操作
		for (i = 0; i < lHeight / 2; i++)
		{
			// 指向倒数第i行象素起点的指针
			lpSrc = (char *)lpDIBBits + lLineBytes * i;
			// 指向第i行象素起点的指针
			lpDst = (char *)lpDIBBits + lLineBytes * (lHeight - i - 1);

			// 备份一行，宽度为lWidth
			memcpy(lpBits, lpDst, lLineBytes);

			// 将倒数第i行象素复制到第i行
			memcpy(lpDst, lpSrc, lLineBytes);
			// 将第i行象素复制到倒数第i行
			memcpy(lpSrc, lpBits, lLineBytes);
		}
	}

	LocalUnlock(hBits);
	LocalFree(hBits);
	return TRUE;
}

//功能：空域上的模板运算
//参数:
// lpDIBBits    指向源DIB图像指针 lWidth       源图像宽度（象素数）
// lHeight      源图像高度（象素数） iTempH		 模板的高度
// iTempW	     模板的宽度 iTempMX	模板的中心元素X坐标 ( < iTempW - 1)
// iTempMY		 模板的中心元素Y坐标 ( < iTempH - 1) fpArray	 指向模板数组的指针
// FLOAT fCoef		模板系数
//返回值:成功返回TRUE，否则返回FALSE。
bool CMyDib::Template(LPSTR lpDIBBits, long lWidth, long lHeight,
	int iTempH, int iTempW, int iTempMX, int iTempMY,
	float * fpArray, float fCoef)
{
	LPSTR	lpNewDIBBits;		// 指向复制图像的指针
	HLOCAL	hNewDIBBits;        //指向一块内存区域的指针
	unsigned char*	lpSrc;		// 指向源图像的指针	
	unsigned char*	pTemp;		// 指向要重新赋值区域的像素点指针	
	float	result;			// 计算结果
	long byteofLine;			// 图像每行的字节数
	byteofLine = LineBytes(lWidth * 8);

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, byteofLine * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	// 初始化图像为原始图像
	memcpy(lpNewDIBBits, lpDIBBits, byteofLine * lHeight);

	for (int i = iTempMY; i < lHeight - 2*iTempMY; i++)	// 行(除去边缘几行)
	{
		for (int j = iTempMX; j < lWidth - 2*iTempMX; j++)// 列(除去边缘几列)
		{
			// 指向新DIB第i行，第j个象素的指针
			//lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * i + j;
			//这样的话图像会倒置
			pTemp = (unsigned char*)lpNewDIBBits + byteofLine * (lHeight - 1 - i) + j;
			result = 0;
			for (int k = 0; k < iTempH; k++)
			{
				for (int l = 0; l < iTempW; l++)
				{
					// 指向DIB第i - iTempMY + k行，第j - iTempMX + l个象素的指针
					lpSrc = (unsigned char*)lpDIBBits + byteofLine * (lHeight - 1
						- (i - iTempMY + k)) + (j - iTempMX + l);
					// 保存象素值
					result += (*lpSrc) * fpArray[k * iTempW + l];
				}
			}
			result *= fCoef;					// 乘上系数			
			result = (FLOAT)fabs(result);	// 取绝对值
			if (result > 255)
			{
				*pTemp = 255;
			}
			else
			{
				*pTemp = (unsigned char)(result + 0.5);
			}
		}
	}
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, byteofLine * lHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

//功能：Sobel边缘检测
//参数：lpDIBBits    指向源DIB图像指针
//		lWidth  源图像宽度
//		lHeight 源图像高度（象素数）
//返回值：边缘检测成功返回TRUE，否则返回FALSE。
bool CMyDib::EdgeDetection(LPSTR lpDIBBits, unsigned long lmageWidth, unsigned long lmageHeight)
{
	int nTempWidth;		//模板的宽度
	int nTempHeight;	 // 模板的高度
	int nTempCenX;		//模板中心的X坐标（相对于模板）
	int nTempCenY;		//模板中心的Y坐标（相对于模板）
	float* Tempdata;	//模板数组的指针	
	double TempCoef;	//模板系数
	//循环变量
	int i;
	int j;
	// 指向缓存图像的指针
	unsigned char*	lpDst1;
	unsigned char*	lpDst2;
	// 指向存贮图像的指针
	LPBYTE lpImage1;
	LPBYTE lpImage2;

	// 暂时分配内存，以保存新图像
	lpImage1 = (LPBYTE) new  char[lmageWidth*lmageHeight];
	lpImage2 = (LPBYTE) new  char[lmageWidth*lmageHeight];
	// 判断是否内存分配失败
	if (lpImage1 == NULL)
	{
		// 分配内存失败
		return FALSE;
	}
	// 判断是否内存分配失败
	if (lpImage2 == NULL)
	{
		// 分配内存失败
		return FALSE;
	}
	// 初始化图像为原始图像
	memcpy(lpImage1, lpDIBBits, lmageWidth*lmageHeight);
	memcpy(lpImage2, lpDIBBits, lmageWidth*lmageHeight);
	lpDst1 = (unsigned char*)lpImage1;
	lpDst2 = (unsigned char*)lpImage2;
	//Sobel算子水平检测模板参数
	nTempWidth = 3;
	nTempHeight = 3;
	nTempCenX = 1;
	nTempCenY = 1;
	TempCoef = 1;
	Tempdata = new float[9];//Sobel算子模板为3X3
	//水平向模板参数,检测出图像中的水平方向的边缘
	Tempdata[0] = -1; Tempdata[1] = -2; Tempdata[2] = -1;
	Tempdata[3] = 0;  Tempdata[4] = 0;  Tempdata[5] = 0;
	Tempdata[6] = 1;  Tempdata[7] = 2;  Tempdata[8] = 1;
	if (!Template((LPSTR)lpImage1, lmageWidth, lmageHeight, nTempWidth, nTempHeight,
		nTempCenX, nTempCenY, Tempdata, TempCoef))//调用模板操作函数Template()对图像进行Sobel模板纵向检测
	{
		return FALSE;
	}
	//垂直方向模板参数
	Tempdata[0] = -1;  Tempdata[1] = 0;  Tempdata[2] = 1;
	Tempdata[3] = -2;  Tempdata[4] = 0;  Tempdata[5] = 2;
	Tempdata[6] = -1;  Tempdata[7] = 0;  Tempdata[8] = 1;
	if (!Template((LPSTR)lpImage2, lmageWidth, lmageHeight, nTempWidth, nTempHeight,
		nTempCenX, nTempCenY, Tempdata, TempCoef))//调用模板操作函数Template()对图像进行Sobel模板横向检测
	{
		return FALSE;
	}
	delete[] Tempdata;
	//求两幅缓存图像的最大值
	for (j = 0; j < lmageHeight; j++)
	{
		for (i = 0; i <lmageWidth; i++)
		{

			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (unsigned char*)lpImage1 + lmageWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (unsigned char*)lpImage2 + lmageWidth * j + i;

			if (*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}

		}
	}
	//复制经过模板运算后的图像到源图像
	memcpy(lpDIBBits, lpImage1, lmageWidth*lmageHeight);
	return true;
}

//功能：Robert边缘检测
//参数：lpDIBBits    指向源DIB图像指针
//		lWidth  源图像宽度
//		lHeight 源图像高度（象素数）
//返回值:边缘检测成功返回TRUE，否则返回FALSE。
bool CMyDib::RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{
	LPSTR	lpSrc;			// 指向源图像的指针	
	LPSTR	pTemp;			// 指向缓存图像的指针	
	LPSTR	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	double result;			//像素值
	unsigned char pixel[4];

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为255
	pTemp = (char *)lpNewDIBBits;
	memset(pTemp, (BYTE)255, lWidth * lHeight);

	//使用水平方向的结构元素进行腐蚀
	for (long j = lHeight - 1; j > 0; j--)
	{
		for (long i = 0; i <lWidth - 1; i++)
		{
			//由于使用2×2的模板，为防止越界，所以不处理最下边和最右边的两列像素

			// 指向源图像第j行，第i个象素的指针			
			lpSrc = (char *)lpDIBBits + lWidth * j + i;
			// 指向目标图像第j行，第i个象素的指针			
			pTemp = (char *)lpNewDIBBits + lWidth * j + i;

			//取得当前指针处2*2区域的像素值，注意要转换为unsigned char型
			pixel[0] = (unsigned char)*lpSrc;
			pixel[1] = (unsigned char)*(lpSrc + 1);
			pixel[2] = (unsigned char)*(lpSrc - lWidth);
			pixel[3] = (unsigned char)*(lpSrc - lWidth + 1);

			//计算目标图像中的当前点
			result = sqrt((double)(pixel[0] - pixel[3])*(pixel[0] - pixel[3]) + \
				(pixel[1] - pixel[2])*(pixel[1] - pixel[2]));
			*pTemp = (unsigned char)result;
		}
	}

	// 复制修改后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

//功能：用Prewitt边缘检测算子对图像进行边缘检测运算。要求目标图像为灰度图像。
//参数:lpDIBBits   指向源DIB图像指针;lWidth       源图像宽度（象素数)
//		lHeight    源图像高度（象素数）
//返回值:边缘检测成功返回TRUE，否则返回FALSE。
bool CMyDib::PrewittEdge(LPSTR lpDIBBits, unsigned long lWidth, unsigned long lHeight)
{
	unsigned char*	lpDst1;				// 指向缓存图像的指针
	unsigned char*	lpDst2;
	LPSTR	lpNewDIBImg1;		// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBImg1;
	LPSTR	lpNewDIBImg2;
	HLOCAL	hNewDIBImg2;

	long i, j;					// 循环变量	
	int		iTempH;				// 模板高度	
	int		iTempW;				// 模板宽度	
	FLOAT	fTempC;				// 模板系数	
	int		iTempMX;			// 模板中心元素X坐标
	int		iTempMY;			// 模板中心元素Y坐标
	float   aValue[9];			// 模板数组

	// 暂时分配内存，以保存新图像
	hNewDIBImg1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBImg1 == NULL)
	{
		return FALSE;
	}

	lpNewDIBImg1 = (char *)LocalLock(hNewDIBImg1);

	// 暂时分配内存，以保存新图像
	hNewDIBImg2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBImg2 == NULL)
	{
		return FALSE;
	}

	lpNewDIBImg2 = (char *)LocalLock(hNewDIBImg2);

	// 拷贝源图像到缓存图像中
	lpDst1 = (unsigned char *)lpNewDIBImg1;
	memcpy(lpNewDIBImg1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (unsigned char *)lpNewDIBImg2;
	memcpy(lpNewDIBImg2, lpDIBBits, lWidth * lHeight);


	// 设置Prewitt模板参数
	iTempW = 3;
	iTempH = 3;
	fTempC = 1.0;
	iTempMX = 1;
	iTempMY = 1;
	aValue[0] = -1.0;  aValue[1] = -1.0;  aValue[2] = -1.0;
	aValue[3] = 0.0;   aValue[4] = 0.0;   aValue[5] = 0.0;
	aValue[6] = 1.0;   aValue[7] = 1.0;   aValue[8] = 1.0;
	
	if (!Template(lpNewDIBImg1, lWidth, lHeight,
		iTempH, iTempW, iTempMX, iTempMY, aValue, fTempC))
	{
		return FALSE;
	}

	// 设置Prewitt模板参数
	aValue[0] = 1.0;  aValue[1] = 0.0;  aValue[2] = -1.0;
	aValue[3] = 1.0;  aValue[4] = 0.0;  aValue[5] = -1.0;
	aValue[6] = 1.0;  aValue[7] = 0.0;  aValue[8] = -1.0;

	if (!Template(lpNewDIBImg2, lWidth, lHeight,
		iTempH, iTempW, iTempMX, iTempMY, aValue, fTempC))
	{
		return FALSE;
	}

	//求两幅缓存图像的最大值
	for (j = 0; j <lHeight; j++)
	{
		for (i = 0; i <lWidth - 1; i++)
		{
			// 指向缓存图像1倒数第j行，第i个象素的指针			
			lpDst1 = (unsigned char *)lpNewDIBImg1 + lWidth * j + i;
			// 指向缓存图像2倒数第j行，第i个象素的指针			
			lpDst2 = (unsigned char *)lpNewDIBImg2 + lWidth * j + i;

			if (*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// 复制经过模板运算后的图像到源图像
	memcpy(lpDIBBits, lpNewDIBImg1, lWidth * lHeight);

	LocalUnlock(hNewDIBImg1);
	LocalFree(hNewDIBImg1);
	LocalUnlock(hNewDIBImg2);
	LocalFree(hNewDIBImg2);

	return TRUE;
}



