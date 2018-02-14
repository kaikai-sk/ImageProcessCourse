
// myDibView.cpp : CmyDibView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "myDib.h"
#endif

#include "myDibDoc.h"
#include "myDibView.h"
#include "GeoTransDlg.h"
#include "LPLSDlg.h"
#include "PreViewDlg.h"
#include "DibImage.h"
#include "ChigpassPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

BOOL WINAPI TranslationDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, LONG lXOffset, LONG lYOffset)
{
	// 指向源图像的指针
	LPSTR	lpSrc;

	// 指向要复制区域的指针
	LPSTR	lpDst;

	// 指向复制图像的指针
	LPSTR	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	// 象素在新DIB中的坐标
	LONG	i;
	LONG	j;

	// 象素在源DIB中的坐标
	LONG	i0;
	LONG	j0;

	// 图像每行的字节数
	LONG lLineBytes;

	// 计算图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);

	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return FALSE;
	}

	// 锁定内存
	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	// 每行
	for (i = 0; i < lHeight; i++)
	{
		// 每列
		for (j = 0; j < lWidth; j++)
		{
			// 指向新DIB第i行，第j个象素的指针
			// 注意由于DIB中图像第一行其实保存在最后一行的位置，因此lpDst
			// 值不是(char *)lpNewDIBBits + lLineBytes * i + j，而是
			// (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;

			// 计算该象素在源DIB中的坐标
			i0 = i - lXOffset;
			j0 = j - lYOffset;

			// 判断是否在源图范围内
			if ((j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// 指向源DIB第i0行，第j0个象素的指针
				// 同样要注意DIB上下倒置的问题
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;

				// 复制象素
				*lpDst = *lpSrc;
			}
			else
			{
				// 对于源图中没有的象素，直接赋值为255
				*((unsigned char*)lpDst) = 255;
			}

		}
	}

	// 复制平移后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	// 释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	// 返回
	return TRUE;
}

// CmyDibView

IMPLEMENT_DYNCREATE(CmyDibView, CView)

BEGIN_MESSAGE_MAP(CmyDibView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_GEOM_TRAN, &CmyDibView::OnGeomTran)
	ON_COMMAND(ID_GEOM_MIRV, &CmyDibView::OnGeomMirv)
	ON_COMMAND(ID_RH_Sobel, &CmyDibView::OnRhSobel)
	ON_COMMAND(ID_EDGE_ROBERT, &CmyDibView::OnEdgeRobert)
	ON_COMMAND(ID_LPLS_SHARP, &CmyDibView::OnLplsSharp)
	ON_COMMAND(IDM_PREWWIT, &CmyDibView::OnPrewwit)
	ON_COMMAND(IDM_AddColor, &CmyDibView::OnAddcolor)
	ON_COMMAND(IDM_HMIRR, &CmyDibView::OnHmirr)
END_MESSAGE_MAP()

// CmyDibView 构造/析构

CmyDibView::CmyDibView()
{
	// TODO:  在此处添加构造代码
}

CmyDibView::~CmyDibView()
{
}

BOOL CmyDibView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CmyDibView 绘制

void CmyDibView::OnDraw(CDC* pDC)
{
	BeginWaitCursor();//显示沙漏

	CmyDibDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);//宏调用，检查对象内部的合法性
	HDIB hDIB = pDoc->GetHDIB();
	ShowDib(pDC, pDoc, hDIB);
	EndWaitCursor();
}

//功能：显示dib所指的图像,用于对原来的dib进行重绘
//参数：设备上下文，文件类指针，源图像的句柄
//返回值：void
void CmyDibView::ShowDib(CDC* pDC, CmyDibDoc* pDoc,HDIB hDIB)
{
	if (hDIB != NULL)
	{
		LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);//锁定内存
		//得到图像的宽高
		int cxDIB = (int)pDoc->GetDibImage()->DIBWidth(lpDIB);
		int cyDIB = (int)pDoc->GetDibImage()->DIBHeight(lpDIB);

		::GlobalUnlock((HGLOBAL)hDIB);//对内存进行解锁

		//矩形类对象，用于显示图片
		CRect rcDIB;
		rcDIB.top = pDoc->GetOrigonY();
		rcDIB.left = pDoc->GetOrigonX();
		rcDIB.right = cxDIB + pDoc->GetOrigonX();
		rcDIB.bottom = cyDIB + pDoc->GetOrigonY();

		CRect rcDest;

		//如果当前的设备上下文是用于打印的话，就进行按页面大小缩放
		//如果不是用于打印，就不进行缩放
		if (pDC->IsPrinting())
		{
			// 是打印，计算输出图像的位置和大小，以便符合页面

			// 获取打印页面的水平宽度(象素)
			int cxPage = pDC->GetDeviceCaps(HORZRES);
			// 获取打印页面的垂直高度(象素)
			int cyPage = pDC->GetDeviceCaps(VERTRES);
			// 获取打印机每英寸象素数
			int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

			// 计算打印图像大小（缩放，根据页面宽度调整图像大小）
			rcDest.top = rcDest.left = 0;
			rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch)
				/ ((double)cxDIB * cxInch));
			rcDest.right = cxPage;

			// 计算打印图像位置（垂直居中）
			int temp = cyPage - (rcDest.bottom - rcDest.top);
			rcDest.bottom += temp / 2;
			rcDest.top += temp / 2;
		}
		else
		{
			// 不必缩放图像
			rcDest = rcDIB;
			/*rcDest.top = pDoc->GetY();
			rcDest.left = pDoc->GetX();
			rcDest.right = pDoc->GetX() + cxDIB;
			rcDest.bottom = pDoc->GetY() + cyDIB;*/
		}
		
		pDoc->GetDibImage()->PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}

// CmyDibView 打印
BOOL CmyDibView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CmyDibView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CmyDibView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CmyDibView 诊断

#ifdef _DEBUG
void CmyDibView::AssertValid() const
{
	CView::AssertValid();
}

void CmyDibView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmyDibDoc* CmyDibView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmyDibDoc)));
	return (CmyDibDoc*)m_pDocument;
}
#endif //_DEBUG



void CmyDibView::OnGeomTran()
{
	// 平移位图
	CmyDibDoc* pDoc = GetDocument();

	LPSTR	lpDIB;				// 指向DIB的指针	
	LPSTR   lpDIBBits;			// 指向DIB象素指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// 判断是否是8-bpp位图（这里为了方便，只处理8-bpp位图的平移，其它的可以类推）
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("目前只支持256色位图的平移！"), _T("系统提示"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());

		return;
	}

	LONG lXOffset;
	LONG lYOffset;
	CGeoTransDlg dlgPara;
	dlgPara.m_XOffset = 100;
	dlgPara.m_YOffset = 100;

	// 显示对话框，提示用户设定平移量
	if (dlgPara.DoModal() != IDOK)
	{
		return;
	}
	lXOffset = dlgPara.m_XOffset;
	lYOffset = dlgPara.m_YOffset;
	delete dlgPara;

	BeginWaitCursor();
	pDoc->SetX(pDoc->GetX() + lXOffset);
	pDoc->SetY(pDoc->GetY() + lYOffset);
	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// 调用TranslationDIB()函数平移DIB
	if (pDoc->GetDibImage()->TranslationDIB1(lpDIBBits,
		pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB), lXOffset,lYOffset))
	{
		pDoc->SetModifiedFlag(TRUE);		// 设置脏标记		
		pDoc->UpdateAllViews(NULL);			// 更新视图
		Invalidate();
	}
	else
	{
		MessageBox(_T("分配内存失败！"), _T("系统提示"), MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	EndWaitCursor();
}





void CmyDibView::OnGeomMirv()
{
	// 垂直镜像

	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// 指向DIB的指针	
	LPSTR    lpDIBBits;			// 指向DIB象素指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// 判断是否是8-bpp位图（这里处理8-bpp位图的垂直镜像）
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("目前只支持256色位图的垂直镜像！"), _T("系统提示"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	BeginWaitCursor();

	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// 调用MirrorDIB()函数垂直镜像DIB
	if (pDoc->GetDibImage()->MirrorDIB(lpDIBBits, pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB), FALSE))
	{
		pDoc->SetModifiedFlag(TRUE);		// 设置脏标记		
		pDoc->UpdateAllViews(NULL);			// 更新视图
	}
	else
	{
		MessageBox(_T("分配内存失败！"), _T("系统提示"), MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	EndWaitCursor();
}

//功能：索贝尔算子锐化
void CmyDibView::OnRhSobel()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// 指向DIB的指针	
	LPSTR    lpDIBBits;			// 指向DIB象素指针	

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	
	DWORD width, length;
	width = pDoc->GetDibImage()->DIBWidth(lpDIB);
	length = pDoc->GetDibImage()->DIBHeight(lpDIB);

	pDoc->GetDibImage()->EdgeDetection(lpDIBBits, width, length);
	Invalidate();
}

//Robert边缘检测运算
void CmyDibView::OnEdgeRobert()
{
	CmyDibDoc* pDoc = GetDocument();
	LPSTR	lpDIB;				// 指向DIB的指针
	LPSTR   lpDIBBits;			// 指向DIB象素指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// 判断是否是8-bpp位图
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("目前只支持256色位图的运算！"), _T("系统提示"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}
	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// 调用RobertDIB()函数对DIB进行边缘检测
	if (pDoc->GetDibImage()->RobertDIB(lpDIBBits,
		WIDTHBYTES(pDoc->GetDibImage()->DIBWidth(lpDIB) * 8),
		pDoc->GetDibImage()->DIBHeight(lpDIB)))
	{
		pDoc->SetModifiedFlag(TRUE);		// 设置脏标记		
		pDoc->UpdateAllViews(NULL);			// 更新视图
	}
	else
	{
		MessageBox(_T("分配内存失败！"), _T("系统提示"), MB_ICONINFORMATION | MB_OK);
	}
	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
}

//拉普拉斯算子菜单的响应函数
void CmyDibView::OnLplsSharp()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// 指向DIB的指针	
	LPSTR    lpDIBBits;			// 指向DIB象素指针	
	int		iTempH=3;				// 模板高度	
	int		iTempW=3;				// 模板宽度	
	float	fTempC=1;				// 模板系数	
	int		iTempMX=1;			// 模板中心元素X坐标	
	int		iTempMY=1;			// 模板中心元素Y坐标	
	float	aValue[9];			// 模板元素数组

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);

	// 判断是否是8-bpp位图
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("只支持256色位图的锐化！"), _T("系统提示"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	CLPLSDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		aValue[0] = dlg.m_n1;  aValue[1] = dlg.m_n2;  aValue[2] = dlg.m_n3;
		aValue[3] = dlg.m_n4;  aValue[4] = dlg.m_n5;  aValue[5] = dlg.m_n6;
		aValue[6] = dlg.m_n7;  aValue[7] = dlg.m_n8;  aValue[8] = dlg.m_n9;

		// 调用Template()函数用拉普拉斯模板锐化DIB
		if (pDoc->GetDibImage()->Template(lpDIBBits, pDoc->GetDibImage()->DIBWidth(lpDIB),
			pDoc->GetDibImage()->DIBHeight(lpDIB), iTempH, iTempW, iTempMX,
			iTempMY, aValue, fTempC))
		{
			pDoc->SetModifiedFlag(TRUE);		// 设置脏标记		
			pDoc->UpdateAllViews(NULL);			// 更新视图
		}
		else
		{
			MessageBox(_T("分配内存失败！"), _T("系统提示"), MB_ICONINFORMATION | MB_OK);
		}
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	}
}

//Prewitt边缘检测运算
void CmyDibView::OnPrewwit()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR	lpDIB;				// 指向DIB的指针	
	LPSTR   lpDIBBits;			// 指向DIB象素指针

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// 判断是否是8-bpp位图（这里为了方便，只处理8-bpp位图的边缘检测，其它的可以类推）
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("目前只支持256色位图的运算！"), _T("系统提示"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);

	// 调用PrewittDIB()函数对DIB进行边缘检测
	if (pDoc->GetDibImage()->PrewittEdge(lpDIBBits,
		pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB)))
	{
		pDoc->SetModifiedFlag(TRUE);		// 设置脏标记		
		pDoc->UpdateAllViews(NULL);			// 更新视图
	}
	else
	{
		MessageBox(_T("分配内存失败！"), _T("系统提示"), MB_ICONINFORMATION | MB_OK);
	}
	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
}

//取反菜单的响应函数
void CmyDibView::OnAddcolor()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR	lpDIB;				// 指向DIB的指针	
	LPSTR   lpDIBBits;			// 指向DIB象素指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	int imgWidth = pDoc->GetDibImage()->DIBWidth(lpDIB);
	int ImgHeight = pDoc->GetDibImage()->DIBHeight(lpDIB);

	int byteofLine = LineBytes(imgWidth * 8);
	LPSTR	lpNewDIBBits;		// 指向复制图像的指针
	HLOCAL	hNewDIBBits;        //指向一块内存区域的指针
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, byteofLine * ImgHeight);
	if (hNewDIBBits == NULL)
	{
		return ;
	}
	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);
	// 初始化图像为原始图像
	memcpy(lpNewDIBBits, lpDIBBits, byteofLine * ImgHeight);

	for (int y = 0; y<ImgHeight; y++)
	{
		for (int x = 0; x<imgWidth; x++)
		{
			lpNewDIBBits[y*imgWidth + x] = 255 - lpNewDIBBits[y*imgWidth + x];
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, byteofLine * ImgHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	Invalidate();
}

//响应水平镜像的菜单的函数
void CmyDibView::OnHmirr()
{
	//水平镜像

	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// 指向DIB的指针	
	LPSTR    lpDIBBits;			// 指向DIB象素指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// 判断是否是8-bpp位图（这里处理8-bpp位图的垂直镜像）
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("目前只支持256色位图的垂直镜像！"), _T("系统提示"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	BeginWaitCursor();

	// 找到DIB图像象素起始位置
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// 调用MirrorDIB()函数垂直镜像DIB
	if (pDoc->GetDibImage()->MirrorDIB(lpDIBBits, pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB), true))
	{
		pDoc->SetModifiedFlag(TRUE);		// 设置脏标记		
		pDoc->UpdateAllViews(NULL);			// 更新视图
	}
	else
	{
		MessageBox(_T("分配内存失败！"), _T("系统提示"), MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	EndWaitCursor();
}
