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
* �������ƣ�
*   ReadDIBFile()
* ����:
*   CFile& file        - Ҫ��ȡ���ļ��ļ�CFile
* ����ֵ:
*   HDIB               - �ɹ�����DIB�ľ�������򷵻�NULL��
* ˵��:
*   �ú�����ָ�����ļ��е�DIB�������ָ�����ڴ������С���BITMAPFILEHEADER
* ������ݶ����������ڴ档
*************************************************************************/
HDIB CMyDib::ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	HDIB hDIB;
	LPSTR pDIB;
	DWORD dwBitsSize;

	dwBitsSize = file.GetLength();		// ��ȡDIB���ļ������ȣ��ֽڣ�

	// ���Զ�ȡDIB�ļ�ͷ
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		return NULL;
	}
	// �ж��Ƿ���DIB���󣬼��ͷ�����ֽ��Ƿ���"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		return NULL;
	}
	// ΪDIB�����ڴ�
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		return NULL;
	}

	pDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER))		// ������
	{
		::GlobalUnlock((HGLOBAL)hDIB);
		::GlobalFree((HGLOBAL)hDIB);
		return NULL;
	}

	::GlobalUnlock((HGLOBAL)hDIB);
	return hDIB;
}

/*************************************************************************
* �������ƣ�
*   DIBWidth()
* ����:
*   LPSTR lpbi         - ָ��DIB�����ָ��
* ����ֵ:
*   DWORD              - DIB��ͼ��Ŀ��
* ˵��:
*   �ú�������DIB��ͼ��Ŀ�ȡ�����Windows 3.0 DIB������BITMAPINFOHEADER
* �е�biWidthֵ��������������BITMAPCOREHEADER�е�bcWidthֵ��
************************************************************************/
DWORD CMyDib::DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// ����DIB��ͼ��Ŀ��
	if (IS_WIN30_DIB(lpDIB))
	{
		return lpbmi->biWidth;		// ����Windows 3.0 DIB������lpbmi->biWidth
	}
	else
	{
		return (DWORD)lpbmc->bcWidth;	// ����������ʽ��DIB������lpbmc->bcWidth
	}
}

/*************************************************************************
* �������ƣ�
*   DIBHeight()
* ����:
*   LPSTR lpDIB        - ָ��DIB�����ָ��
* ����ֵ:
*   DWORD              - DIB��ͼ��ĸ߶�
* ˵��:
*   �ú�������DIB��ͼ��ĸ߶ȡ�����Windows 3.0 DIB������BITMAPINFOHEADER
* �е�biHeightֵ��������������BITMAPCOREHEADER�е�bcHeightֵ��
************************************************************************/
DWORD CMyDib::DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	LPBITMAPCOREHEADER lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// ����DIB��ͼ��Ŀ��
	if (IS_WIN30_DIB(lpDIB))
	{
		return lpbmi->biHeight;		// ����Windows 3.0 DIB������lpbmi->biHeight
	}
	else
	{
		return (DWORD)lpbmc->bcHeight;	// ����������ʽ��DIB������lpbmc->bcHeight
	}
}

/*************************************************************************
* �������ƣ�
*   CreateDIBPalette()
* ����:
*   HDIB hDIB          - ָ��DIB�����ָ��
*   CPalette* pPal     - ָ��DIB�����ɫ���ָ��
* ����ֵ:
*   BOOL               - �����ɹ�����TRUE�����򷵻�FALSE��
* ˵��:
*   �ú�������DIB����һ���߼���ɫ�壬��DIB�ж�ȡ��ɫ���浽��ɫ���У�
* ����ո��߼���ɫ�崴��һ���µĵ�ɫ�壬�����ظõ�ɫ��ľ��������
* ��������õ���ɫ����ʾDIBͼ��
************************************************************************/
BOOL CMyDib::CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{

	LPLOGPALETTE lpPal;		// ָ���߼���ɫ���ָ��
	HANDLE hLogPal;			// �߼���ɫ��ľ��
	HPALETTE hPal = NULL;	// ��ɫ��ľ��
	int i;					// ѭ������	
	WORD wNumColors;		// ��ɫ���е���ɫ��Ŀ	
	LPSTR lpbi;				// ָ��DIB��ָ��	
	LPBITMAPINFO lpbmi;		// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��	
	LPBITMAPCOREINFO lpbmc;	// ָ��BITMAPCOREINFO�ṹ��ָ��	
	BOOL bWinStyleDIB;		// �����Ƿ���Win3.0 DIB�ı��	
	BOOL bResult = FALSE;	// �������

	if (hDIB == NULL)
	{
		return FALSE;
	}

	lpbi = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);	// ����DIB
	lpbmi = (LPBITMAPINFO)lpbi;		// ��ȡָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	lpbmc = (LPBITMAPCOREINFO)lpbi;	// ��ȡָ��BITMAPCOREINFO�ṹ��ָ��
	wNumColors = DIBNumColors(lpbi);// ��ȡDIB����ɫ���е���ɫ��Ŀ

	if (wNumColors != 0)
	{
		// ����Ϊ�߼���ɫ���ڴ�
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
			+sizeof(PALETTEENTRY)
			* wNumColors);
		// ����ڴ治�㣬�˳�
		if (hLogPal == 0)
		{
			::GlobalUnlock((HGLOBAL)hDIB);	// �������
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL)hLogPal);
		lpPal->palVersion = PALVERSION;			// ���ð汾��
		lpPal->palNumEntries = (WORD)wNumColors;// ������ɫ��Ŀ
		bWinStyleDIB = IS_WIN30_DIB(lpbi);		// �ж��Ƿ���WIN3.0��DIB

		// ��ȡ��ɫ��
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				// ��ȡ��ɫ��ɫ��ɫ����
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				// ����λ
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				// ��ȡ��ɫ��ɫ��ɫ����
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				// ����λ
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		bResult = pPal->CreatePalette(lpPal);// �����߼���ɫ�崴����ɫ�壬������ָ��
		::GlobalUnlock((HGLOBAL)hLogPal);	// �������
		::GlobalFree((HGLOBAL)hLogPal);	// �ͷ��߼���ɫ��
	}

	::GlobalUnlock((HGLOBAL)hDIB);			// �������
	return bResult;
}

/*************************************************************************
* �������ƣ�
*   DIBNumColors()
* ����:
*   LPSTR lpbi         - ָ��DIB�����ָ��
* ����ֵ:
*   WORD               - ���ص�ɫ������ɫ������
* ˵��:
*   �ú�������DIB�е�ɫ�����ɫ�����������ڵ�ɫλͼ������2��
* ����16ɫλͼ������16������256ɫλͼ������256���������ɫ
* λͼ��24λ����û�е�ɫ�壬����0��
************************************************************************/
WORD CMyDib::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;

	// ����Windows��DIB, ʵ����ɫ����Ŀ���Ա����ص�λ��Ҫ�١�
	// ��������������򷵻�һ�����Ƶ���ֵ��

	// �ж��Ƿ���WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;	// ��ȡdwClrUsedֵ

		if (dwClrUsed != 0)
		{
			// ���dwClrUsed��ʵ���õ�����ɫ������Ϊ0��ֱ�ӷ��ظ�ֵ
			return (WORD)dwClrUsed;
		}
	}

	// ��ȡ���ص�λ��
	if (IS_WIN30_DIB(lpbi))
	{
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;	// ��ȡbiBitCountֵ
	}
	else
	{
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;	// ��ȡbiBitCountֵ
	}

	// �������ص�λ��������ɫ��Ŀ
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
* �������ƣ�
*   PaintDIB()
* ����:
*   HDC hDC            - ����豸DC
*   LPRECT lpDCRect    - ���ƾ�������
*   HDIB hDIB          - ָ��DIB�����ָ��
*   LPRECT lpDIBRect   - Ҫ�����DIB����
*   CPalette* pPal     - ָ��DIB�����ɫ���ָ��
* ����ֵ:
*   BOOL               - ���Ƴɹ�����TRUE�����򷵻�FALSE��
* ˵��:
*   �ú�����Ҫ��������DIB�������е�����StretchDIBits()����
* SetDIBitsToDevice()������DIB����������豸���ɲ���hDCָ
* �������Ƶľ��������ɲ���lpDCRectָ�������DIB�������ɲ���
* lpDIBRectָ����
************************************************************************/
BOOL CMyDib::PaintDIB(HDC     hDC,
	LPRECT  lpDCRect,
	HDIB    hDIB,
	LPRECT  lpDIBRect,
	CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // BITMAPINFOHEADERָ��
	LPSTR    lpDIBBits;           // DIB����ָ��
	BOOL     bSuccess = FALSE;      // �ɹ���־
	HPALETTE hPal = NULL;           // DIB��ɫ��
	HPALETTE hOldPal = NULL;        // ��ǰ�ĵ�ɫ��

	if (hDIB == NULL)
	{
		return FALSE;
	}

	lpDIBHdr = (LPSTR)::GlobalLock((HGLOBAL)hDIB);// ����DIB	
	lpDIBBits = FindDIBData(lpDIBHdr);	// �ҵ�DIBͼ��������ʼλ��

	if (pPal != NULL)					// ��ȡDIB��ɫ�壬��ѡ����
	{
		hPal = (HPALETTE)pPal->m_hObject;
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);	// ѡ�е�ɫ��
	}

	::SetStretchBltMode(hDC, COLORONCOLOR);			// ������ʾģʽ

	// �ж��ǵ���StretchDIBits()����SetDIBitsToDevice()������DIB����
	if ((RECTWIDTH(lpDCRect) == RECTWIDTH(lpDIBRect)) &&
		(RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// ԭʼ��С���������졣
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
		// ��ԭʼ��С�����졣
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

	::GlobalUnlock((HGLOBAL)hDIB);				// �������	
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);	// �ָ���ǰ�ĵ�ɫ��
	}
	return bSuccess;
}

//���ܣ��ú�������DIB��ͼ�����ص���ʼλ�ã�������ָ������ָ�롣
//����:ָ��DIB�����ָ��
//����ֵ:ָ��DIBͼ��������ʼλ�õ�ָ��
LPSTR CMyDib::FindDIBData(LPSTR lpDib)
{
	return (lpDib + *(LPDWORD)lpDib + PaletteSize(lpDib));
}

/*************************************************************************
* �������ƣ�
*   PaletteSize()
* ����:
*   LPSTR lpbi         - ָ��DIB�����ָ��
* ����ֵ:
*   WORD               - DIB�е�ɫ��Ĵ�С
* ˵��:
*   �ú�������DIB�е�ɫ��Ĵ�С������Windows 3.0 DIB��������ɫ��Ŀ��
* RGBQUAD�Ĵ�С����������������ɫ��Ŀ��RGBTRIPLE�Ĵ�С��
************************************************************************/
WORD CMyDib::PaletteSize(LPSTR lpbi)
{
	// ����DIB�е�ɫ��Ĵ�С
	if (IS_WIN30_DIB(lpbi))
	{
		//������ɫ��Ŀ��RGBQUAD�Ĵ�С
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
	}
	else
	{
		//������ɫ��Ŀ��RGBTRIPLE�Ĵ�С
		return (WORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
	}
}

//���ܣ�ˮƽ�ƶ�DIBͼ�񡣺�������ı�ͼ��Ĵ�С���Ƴ��Ĳ���ͼ��
//		����ȥ���հײ����ð�ɫ��䡣
//������lpDIBBits    ָ��ԴDIBͼ��ָ��
//		lWidth Դͼ���ȣ���������
//		lHeight - Դͼ��߶ȣ���������
//		lXOffset     - X��ƽ��������������
//		lYOffset     - Y��ƽ��������������
//����ֵ��ƽ�Ƴɹ�����TRUE�����򷵻�FALSE��
bool CMyDib::TranslationDIB1(LPSTR lpDIBBits, long lWidth, long lHeight,
	long lXOffset, long lYOffset)
{
	LPSTR	lpSrc;					// ָ��Դͼ���ָ��	
	LPSTR	lpDst;					// ָ��Ҫ���������ָ��
	LPSTR	lpNewDIBBits;			// ָ����ͼ���ָ��
	HLOCAL	hNewDIBBits;
	LONG	i;						// ��������DIB�е�����
	LONG	j;
	LONG	i0;						// ������ԴDIB�е�����
	LONG	j0;
	LONG lLineBytes;				// ͼ��ÿ�е��ֽ���

	lLineBytes = LineBytes(lWidth * 8);// ����ͼ��ÿ�е��ֽ���

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	for (i = 0; i < lHeight; i++)					// ÿ��
	{
		for (j = 0; j < lWidth; j++)					// ÿ��
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			// ע������DIB��ͼ���һ����ʵ���������һ�е�λ�ã����lpDst
			// ֵ����(char *)lpNewDIBBits + lLineBytes * i + j������
			// (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;

			// �����������ԴDIB�е�����
			i0 = i - lXOffset;
			j0 = j - lYOffset;

			/*i0 = i;
			j0 = j;*/

			// �ж��Ƿ���Դͼ��Χ��
			if ((j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// ָ��ԴDIB��i0�У���j0�����ص�ָ��
				// ͬ��Ҫע��DIB���µ��õ�����
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;
				*lpDst = *lpSrc;					// ��������
			}
			else
			{
				 //����Դͼ��û�е����أ�ֱ�Ӹ�ֵΪ255
				*((unsigned char*)lpDst) = 255;
			}
		}
	}

	// ����ƽ�ƺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

/*************************************************************************
* �������ƣ�
*   SaveDIB()
* ����:
*   HDIB hDib          - Ҫ�����DIB
*   CFile& file        - �����ļ�CFile
* ����ֵ:
*   BOOL               - �ɹ�����TRUE�����򷵻�FALSE����CFileException
* ˵��:
*   �ú�����ָ����DIB���󱣴浽ָ����CFile�С���CFile�ɵ��ó���򿪺͹رա�
*************************************************************************/
bool CMyDib::SaveDIB(HDIB hDib, CFile& file)
{
	BITMAPFILEHEADER bmfHdr;	// Bitmap�ļ�ͷ	
	LPBITMAPINFOHEADER lpBI;	// ָ��BITMAPINFOHEADER��ָ��
	DWORD dwDIBSize;			// DIB��С

	if (hDib == NULL)
	{
		return FALSE;
	}

	// ��ȡBITMAPINFO�ṹ��������
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL)hDib);
	if (lpBI == NULL)
	{
		return FALSE;
	}

	// �ж��Ƿ���WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// ��֧���������͵�DIB����
		::GlobalUnlock((HGLOBAL)hDib);
		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////
	// ����ļ�ͷ///////////////////////////////////////////////////////////	
	bmfHdr.bfType = DIB_HEADER_MARKER;		// �ļ�����"BM"

	// ����DIB��Сʱ����򵥵ķ����ǵ���GlobalSize()����������ȫ���ڴ��С��
	// ����DIB�����Ĵ�С�������Ƕ༸���ֽڡ���������Ҫ����һ��DIB����ʵ��С��

	// �ļ�ͷ��С����ɫ���С
	// ��BITMAPINFOHEADER��BITMAPCOREHEADER�ṹ�ĵ�һ��DWORD���Ǹýṹ�Ĵ�С��
	dwDIBSize = *(LPDWORD)lpBI + PaletteSize((LPSTR)lpBI);

	// ����ͼ���С
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// ����RLEλͼ��û�������С��ֻ������biSizeImage�ڵ�ֵ
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		DWORD dwBmBitsSize;			// ���صĴ�С
		dwBmBitsSize = LineBytes((lpBI->biWidth)*((DWORD)lpBI->biBitCount))
			* lpBI->biHeight;		// ��СΪWidth * Height
		dwDIBSize += dwBmBitsSize;	// �����DIB�����Ĵ�С

		// ����biSizeImage���ܶ�BMP�ļ�ͷ��biSizeImage��ֵ�Ǵ���ģ�
		lpBI->biSizeImage = dwBmBitsSize;
	}

	// �����ļ���С��DIB��С��BITMAPFILEHEADER�ṹ��С
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);

	// ����������
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// ����ƫ����bfOffBits�����Ĵ�СΪBitmap�ļ�ͷ��С��DIBͷ��С����ɫ���С
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+lpBI->biSize
		+ PaletteSize((LPSTR)lpBI);

	/////////////////////////////////////////////////////////////////////////
	// ����д�ļ�////////////////////////////////////////////////////////////
	TRY
	{
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));	// д�ļ�ͷ
		file.Write(lpBI, dwDIBSize);					// дDIBͷ������
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

//���ܣ��ú�����������DIBͼ�񡣿���ָ������ķ�ʽ��ˮƽ���Ǵ�ֱ��
//����:
//   LPSTR lpDIBBits - ָ��ԴDIBͼ��ָ��
//   LONG  lWidth - Դͼ���ȣ���������
//   LONG  lHeight - Դͼ��߶ȣ���������
//   BOOL  bDirection - ����ķ���TRUE��ʾˮƽ����FALSE��ʾ��ֱ����
//����ֵ:����ɹ�����TRUE�����򷵻�FALSE��
bool CMyDib::MirrorDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight,
	BOOL bDirection)
{
	LPSTR	lpSrc;						// ָ��Դͼ���ָ��	
	LPSTR	lpDst;						// ָ��Ҫ���������ָ��	
	LPSTR	lpBits;						// ָ����ͼ���ָ��
	HLOCAL	hBits;
	LONG	i;							// ѭ������
	LONG	j;
	LONG lLineBytes;					// ͼ��ÿ�е��ֽ���	

	lLineBytes = LineBytes(lWidth * 8);// ����ͼ��ÿ�е��ֽ���		
	hBits = LocalAlloc(LHND, lLineBytes);	// ��ʱ�����ڴ棬�Ա���һ��ͼ��
	if (hBits == NULL)
	{
		return FALSE;
	}

	lpBits = (char *)LocalLock(hBits);

	// �жϾ���ʽ
	if (bDirection)		// ˮƽ����
	{
		// ���ͼ��ÿ�н��в���
		for (i = 0; i < lHeight; i++)
		{
			// ���ÿ��ͼ����벿�ֽ��в���
			for (j = 0; j < lWidth / 2; j++)
			{
				// ָ������i�У���j�����ص�ָ��
				lpSrc = (char *)lpDIBBits + lLineBytes * i + j;
				// ָ������i�У�������j�����ص�ָ��
				lpDst = (char *)lpDIBBits + lLineBytes * (i + 1) - j;

				// ����һ������
				*lpBits = *lpDst;

				// ��������i�У���j�����ظ��Ƶ�������i�У�������j������
				*lpDst = *lpSrc;
				// ��������i�У�������j�����ظ��Ƶ�������i�У���j������
				*lpSrc = *lpBits;
			}
		}
	}
	else				// ��ֱ����
	{
		// ����ϰ�ͼ����в���
		for (i = 0; i < lHeight / 2; i++)
		{
			// ָ������i����������ָ��
			lpSrc = (char *)lpDIBBits + lLineBytes * i;
			// ָ���i����������ָ��
			lpDst = (char *)lpDIBBits + lLineBytes * (lHeight - i - 1);

			// ����һ�У����ΪlWidth
			memcpy(lpBits, lpDst, lLineBytes);

			// ��������i�����ظ��Ƶ���i��
			memcpy(lpDst, lpSrc, lLineBytes);
			// ����i�����ظ��Ƶ�������i��
			memcpy(lpSrc, lpBits, lLineBytes);
		}
	}

	LocalUnlock(hBits);
	LocalFree(hBits);
	return TRUE;
}

//���ܣ������ϵ�ģ������
//����:
// lpDIBBits    ָ��ԴDIBͼ��ָ�� lWidth       Դͼ���ȣ���������
// lHeight      Դͼ��߶ȣ��������� iTempH		 ģ��ĸ߶�
// iTempW	     ģ��Ŀ�� iTempMX	ģ�������Ԫ��X���� ( < iTempW - 1)
// iTempMY		 ģ�������Ԫ��Y���� ( < iTempH - 1) fpArray	 ָ��ģ�������ָ��
// FLOAT fCoef		ģ��ϵ��
//����ֵ:�ɹ�����TRUE�����򷵻�FALSE��
bool CMyDib::Template(LPSTR lpDIBBits, long lWidth, long lHeight,
	int iTempH, int iTempW, int iTempMX, int iTempMY,
	float * fpArray, float fCoef)
{
	LPSTR	lpNewDIBBits;		// ָ����ͼ���ָ��
	HLOCAL	hNewDIBBits;        //ָ��һ���ڴ������ָ��
	unsigned char*	lpSrc;		// ָ��Դͼ���ָ��	
	unsigned char*	pTemp;		// ָ��Ҫ���¸�ֵ��������ص�ָ��	
	float	result;			// ������
	long byteofLine;			// ͼ��ÿ�е��ֽ���
	byteofLine = LineBytes(lWidth * 8);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, byteofLine * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}
	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	// ��ʼ��ͼ��Ϊԭʼͼ��
	memcpy(lpNewDIBBits, lpDIBBits, byteofLine * lHeight);

	for (int i = iTempMY; i < lHeight - 2*iTempMY; i++)	// ��(��ȥ��Ե����)
	{
		for (int j = iTempMX; j < lWidth - 2*iTempMX; j++)// ��(��ȥ��Ե����)
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			//lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * i + j;
			//�����Ļ�ͼ��ᵹ��
			pTemp = (unsigned char*)lpNewDIBBits + byteofLine * (lHeight - 1 - i) + j;
			result = 0;
			for (int k = 0; k < iTempH; k++)
			{
				for (int l = 0; l < iTempW; l++)
				{
					// ָ��DIB��i - iTempMY + k�У���j - iTempMX + l�����ص�ָ��
					lpSrc = (unsigned char*)lpDIBBits + byteofLine * (lHeight - 1
						- (i - iTempMY + k)) + (j - iTempMX + l);
					// ��������ֵ
					result += (*lpSrc) * fpArray[k * iTempW + l];
				}
			}
			result *= fCoef;					// ����ϵ��			
			result = (FLOAT)fabs(result);	// ȡ����ֵ
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
	// ���Ʊ任���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, byteofLine * lHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	return TRUE;
}

//���ܣ�Sobel��Ե���
//������lpDIBBits    ָ��ԴDIBͼ��ָ��
//		lWidth  Դͼ����
//		lHeight Դͼ��߶ȣ���������
//����ֵ����Ե���ɹ�����TRUE�����򷵻�FALSE��
bool CMyDib::EdgeDetection(LPSTR lpDIBBits, unsigned long lmageWidth, unsigned long lmageHeight)
{
	int nTempWidth;		//ģ��Ŀ��
	int nTempHeight;	 // ģ��ĸ߶�
	int nTempCenX;		//ģ�����ĵ�X���꣨�����ģ�壩
	int nTempCenY;		//ģ�����ĵ�Y���꣨�����ģ�壩
	float* Tempdata;	//ģ�������ָ��	
	double TempCoef;	//ģ��ϵ��
	//ѭ������
	int i;
	int j;
	// ָ�򻺴�ͼ���ָ��
	unsigned char*	lpDst1;
	unsigned char*	lpDst2;
	// ָ�����ͼ���ָ��
	LPBYTE lpImage1;
	LPBYTE lpImage2;

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	lpImage1 = (LPBYTE) new  char[lmageWidth*lmageHeight];
	lpImage2 = (LPBYTE) new  char[lmageWidth*lmageHeight];
	// �ж��Ƿ��ڴ����ʧ��
	if (lpImage1 == NULL)
	{
		// �����ڴ�ʧ��
		return FALSE;
	}
	// �ж��Ƿ��ڴ����ʧ��
	if (lpImage2 == NULL)
	{
		// �����ڴ�ʧ��
		return FALSE;
	}
	// ��ʼ��ͼ��Ϊԭʼͼ��
	memcpy(lpImage1, lpDIBBits, lmageWidth*lmageHeight);
	memcpy(lpImage2, lpDIBBits, lmageWidth*lmageHeight);
	lpDst1 = (unsigned char*)lpImage1;
	lpDst2 = (unsigned char*)lpImage2;
	//Sobel����ˮƽ���ģ�����
	nTempWidth = 3;
	nTempHeight = 3;
	nTempCenX = 1;
	nTempCenY = 1;
	TempCoef = 1;
	Tempdata = new float[9];//Sobel����ģ��Ϊ3X3
	//ˮƽ��ģ�����,����ͼ���е�ˮƽ����ı�Ե
	Tempdata[0] = -1; Tempdata[1] = -2; Tempdata[2] = -1;
	Tempdata[3] = 0;  Tempdata[4] = 0;  Tempdata[5] = 0;
	Tempdata[6] = 1;  Tempdata[7] = 2;  Tempdata[8] = 1;
	if (!Template((LPSTR)lpImage1, lmageWidth, lmageHeight, nTempWidth, nTempHeight,
		nTempCenX, nTempCenY, Tempdata, TempCoef))//����ģ���������Template()��ͼ�����Sobelģ��������
	{
		return FALSE;
	}
	//��ֱ����ģ�����
	Tempdata[0] = -1;  Tempdata[1] = 0;  Tempdata[2] = 1;
	Tempdata[3] = -2;  Tempdata[4] = 0;  Tempdata[5] = 2;
	Tempdata[6] = -1;  Tempdata[7] = 0;  Tempdata[8] = 1;
	if (!Template((LPSTR)lpImage2, lmageWidth, lmageHeight, nTempWidth, nTempHeight,
		nTempCenX, nTempCenY, Tempdata, TempCoef))//����ģ���������Template()��ͼ�����Sobelģ�������
	{
		return FALSE;
	}
	delete[] Tempdata;
	//����������ͼ������ֵ
	for (j = 0; j < lmageHeight; j++)
	{
		for (i = 0; i <lmageWidth; i++)
		{

			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (unsigned char*)lpImage1 + lmageWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (unsigned char*)lpImage2 + lmageWidth * j + i;

			if (*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}

		}
	}
	//���ƾ���ģ��������ͼ��Դͼ��
	memcpy(lpDIBBits, lpImage1, lmageWidth*lmageHeight);
	return true;
}

//���ܣ�Robert��Ե���
//������lpDIBBits    ָ��ԴDIBͼ��ָ��
//		lWidth  Դͼ����
//		lHeight Դͼ��߶ȣ���������
//����ֵ:��Ե���ɹ�����TRUE�����򷵻�FALSE��
bool CMyDib::RobertDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight)
{
	LPSTR	lpSrc;			// ָ��Դͼ���ָ��	
	LPSTR	pTemp;			// ָ�򻺴�ͼ���ָ��	
	LPSTR	lpNewDIBBits;	// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBBits;
	double result;			//����ֵ
	unsigned char pixel[4];

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	// ��ʼ���·�����ڴ棬�趨��ʼֵΪ255
	pTemp = (char *)lpNewDIBBits;
	memset(pTemp, (BYTE)255, lWidth * lHeight);

	//ʹ��ˮƽ����ĽṹԪ�ؽ��и�ʴ
	for (long j = lHeight - 1; j > 0; j--)
	{
		for (long i = 0; i <lWidth - 1; i++)
		{
			//����ʹ��2��2��ģ�壬Ϊ��ֹԽ�磬���Բ��������±ߺ����ұߵ���������

			// ָ��Դͼ���j�У���i�����ص�ָ��			
			lpSrc = (char *)lpDIBBits + lWidth * j + i;
			// ָ��Ŀ��ͼ���j�У���i�����ص�ָ��			
			pTemp = (char *)lpNewDIBBits + lWidth * j + i;

			//ȡ�õ�ǰָ�봦2*2���������ֵ��ע��Ҫת��Ϊunsigned char��
			pixel[0] = (unsigned char)*lpSrc;
			pixel[1] = (unsigned char)*(lpSrc + 1);
			pixel[2] = (unsigned char)*(lpSrc - lWidth);
			pixel[3] = (unsigned char)*(lpSrc - lWidth + 1);

			//����Ŀ��ͼ���еĵ�ǰ��
			result = sqrt((double)(pixel[0] - pixel[3])*(pixel[0] - pixel[3]) + \
				(pixel[1] - pixel[2])*(pixel[1] - pixel[2]));
			*pTemp = (unsigned char)result;
		}
	}

	// �����޸ĺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lWidth * lHeight);

	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	return TRUE;
}

//���ܣ���Prewitt��Ե������Ӷ�ͼ����б�Ե������㡣Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
//����:lpDIBBits   ָ��ԴDIBͼ��ָ��;lWidth       Դͼ���ȣ�������)
//		lHeight    Դͼ��߶ȣ���������
//����ֵ:��Ե���ɹ�����TRUE�����򷵻�FALSE��
bool CMyDib::PrewittEdge(LPSTR lpDIBBits, unsigned long lWidth, unsigned long lHeight)
{
	unsigned char*	lpDst1;				// ָ�򻺴�ͼ���ָ��
	unsigned char*	lpDst2;
	LPSTR	lpNewDIBImg1;		// ָ�򻺴�DIBͼ���ָ��
	HLOCAL	hNewDIBImg1;
	LPSTR	lpNewDIBImg2;
	HLOCAL	hNewDIBImg2;

	long i, j;					// ѭ������	
	int		iTempH;				// ģ��߶�	
	int		iTempW;				// ģ����	
	FLOAT	fTempC;				// ģ��ϵ��	
	int		iTempMX;			// ģ������Ԫ��X����
	int		iTempMY;			// ģ������Ԫ��Y����
	float   aValue[9];			// ģ������

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBImg1 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBImg1 == NULL)
	{
		return FALSE;
	}

	lpNewDIBImg1 = (char *)LocalLock(hNewDIBImg1);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBImg2 = LocalAlloc(LHND, lWidth * lHeight);
	if (hNewDIBImg2 == NULL)
	{
		return FALSE;
	}

	lpNewDIBImg2 = (char *)LocalLock(hNewDIBImg2);

	// ����Դͼ�񵽻���ͼ����
	lpDst1 = (unsigned char *)lpNewDIBImg1;
	memcpy(lpNewDIBImg1, lpDIBBits, lWidth * lHeight);
	lpDst2 = (unsigned char *)lpNewDIBImg2;
	memcpy(lpNewDIBImg2, lpDIBBits, lWidth * lHeight);


	// ����Prewittģ�����
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

	// ����Prewittģ�����
	aValue[0] = 1.0;  aValue[1] = 0.0;  aValue[2] = -1.0;
	aValue[3] = 1.0;  aValue[4] = 0.0;  aValue[5] = -1.0;
	aValue[6] = 1.0;  aValue[7] = 0.0;  aValue[8] = -1.0;

	if (!Template(lpNewDIBImg2, lWidth, lHeight,
		iTempH, iTempW, iTempMX, iTempMY, aValue, fTempC))
	{
		return FALSE;
	}

	//����������ͼ������ֵ
	for (j = 0; j <lHeight; j++)
	{
		for (i = 0; i <lWidth - 1; i++)
		{
			// ָ�򻺴�ͼ��1������j�У���i�����ص�ָ��			
			lpDst1 = (unsigned char *)lpNewDIBImg1 + lWidth * j + i;
			// ָ�򻺴�ͼ��2������j�У���i�����ص�ָ��			
			lpDst2 = (unsigned char *)lpNewDIBImg2 + lWidth * j + i;

			if (*lpDst2 > *lpDst1)
			{
				*lpDst1 = *lpDst2;
			}
		}
	}

	// ���ƾ���ģ��������ͼ��Դͼ��
	memcpy(lpDIBBits, lpNewDIBImg1, lWidth * lHeight);

	LocalUnlock(hNewDIBImg1);
	LocalFree(hNewDIBImg1);
	LocalUnlock(hNewDIBImg2);
	LocalFree(hNewDIBImg2);

	return TRUE;
}



