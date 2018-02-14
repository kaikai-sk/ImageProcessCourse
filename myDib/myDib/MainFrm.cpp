
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "myDib.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO:  在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO:  如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

/******************************************************************************
*	作用:		获得指定目录下所有的图像文件名
*	参数:
*		path	指定目录路径
*		ary		保存图像文件名的字符串容器
******************************************************************************/
void CMainFrame::GetAllFileNames(const CString& path, CStringArray& ary)
{
	ary.RemoveAll();

	CFileFind fileFind;
	CString filesPath = path + L"\\*.*";

	if (fileFind.FindFile(filesPath))
	{

		CString strPicPath;
		CString strExt;

		// 如果该目录还有文件
		while (fileFind.FindNextFile())
		{
			strPicPath = fileFind.GetFileName();
			// 获取后缀名
			strExt = strPicPath.Right(3);
			strExt.MakeLower();

			// 如果后缀名为bmp,jpg则判断是图片文件
			// 此处可以扩充功能
			if (strExt == "bmp" || strExt == "jpg")
				ary.Add(strPicPath);
		}

		strPicPath = fileFind.GetFileName();
		// 获取后缀名
		strExt = strPicPath.Right(3);
		strExt.MakeLower();

		// 如果后缀名为bmp,jpg则判断是图片文件
		// 此处可以扩充功能
		if (strExt == "bmp" || strExt == "jpg")
			ary.Add(strPicPath);
	}

	fileFind.Close();
}