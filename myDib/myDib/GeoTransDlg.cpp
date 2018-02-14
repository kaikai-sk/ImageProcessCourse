// GeoTransDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myDib.h"
#include "GeoTransDlg.h"
#include "afxdialogex.h"


// CGeoTransDlg 对话框

IMPLEMENT_DYNAMIC(CGeoTransDlg, CDialog)

CGeoTransDlg::CGeoTransDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeoTransDlg::IDD, pParent)
	, m_XOffset(0)
	, m_YOffset(0)
{

}

CGeoTransDlg::~CGeoTransDlg()
{
}

void CGeoTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_XOFFSET, m_XOffset);
	DDX_Text(pDX, IDC_EDIT_YOFFSET, m_YOffset);
}


BEGIN_MESSAGE_MAP(CGeoTransDlg, CDialog)
END_MESSAGE_MAP()


// CGeoTransDlg 消息处理程序
