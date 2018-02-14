// PreViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myDib.h"
#include "PreViewDlg.h"
#include "afxdialogex.h"
#include "myDibDoc.h"
#include "MainFrm.h"
#include "myDibView.h"

// CPreViewDlg 对话框

IMPLEMENT_DYNAMIC(CPreViewDlg, CDialog)

CPreViewDlg::CPreViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreViewDlg::IDD, pParent)
{
}

CPreViewDlg::~CPreViewDlg()
{
}

void CPreViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPreViewDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPreViewDlg 消息处理程序


void CPreViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	BeginWaitCursor();//显示沙漏

	//首先从对话框的按钮处理函数中获得MainFrame指针
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	//然后获得相应视图指针
	CmyDibView *pView = (CmyDibView *)pMain->GetActiveView();
	//最后获得视图对应的当前文档指针 
	 //CDocument* pCurrentDoc =(pMain->GetActiveDocument());
	CmyDibDoc* pCurrentDoc = (CmyDibDoc*)(pMain->GetActiveDocument());

	CDC*	pDC = GetDC();
	
	ASSERT_VALID(pCurrentDoc);//宏调用，检查对象内部的合法性
	HDIB hDIB = pCurrentDoc->GetHDIB();
	ShowDib(pDC, pCurrentDoc, hDIB);

	EndWaitCursor();
}

//功能：显示dib所指的图像,用于对原来的dib进行重绘
//参数：设备上下文，文件类指针，源图像的句柄
//返回值：void
void CPreViewDlg::ShowDib(CDC* pDC, CmyDibDoc* pDoc, HDIB hDIB)
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
		}

		pDoc->GetDibImage()->PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}
