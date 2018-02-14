// LPLSDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "myDib.h"
#include "LPLSDlg.h"
#include "afxdialogex.h"


// CLPLSDlg 对话框

IMPLEMENT_DYNAMIC(CLPLSDlg, CDialog)

CLPLSDlg::CLPLSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLPLSDlg::IDD, pParent)
	, m_n1(0)
	, m_n2(-1)
	, m_n3(0)
	, m_n4(-1)
	, m_n5(5)
	, m_n6(-1)
	, m_n7(0)
	, m_n8(-1)
	, m_n9(0)
{

}

CLPLSDlg::~CLPLSDlg()
{
}

void CLPLSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_n1);
	DDX_Text(pDX, IDC_EDIT2, m_n2);
	DDX_Text(pDX, IDC_EDIT3, m_n3);
	DDX_Text(pDX, IDC_EDIT4, m_n4);
	//  DDX_Text(pDX, IDC_EDIT5, m_n5);
	DDX_Text(pDX, IDC_EDIT5, m_n5);
	DDX_Text(pDX, IDC_EDIT6, m_n6);
	DDX_Text(pDX, IDC_EDIT7, m_n7);
	DDX_Text(pDX, IDC_EDIT8, m_n8);
	DDX_Text(pDX, IDC_EDIT9, m_n9);
}


BEGIN_MESSAGE_MAP(CLPLSDlg, CDialog)
	ON_BN_CLICKED(IDC_LPLS_4, &CLPLSDlg::OnBnClickedLpls4)
	ON_BN_CLICKED(IDC_LPLS_8, &CLPLSDlg::OnBnClickedLpls8)
END_MESSAGE_MAP()


// CLPLSDlg 消息处理程序


void CLPLSDlg::OnBnClickedLpls4()
{
	UpdateData(true);
	m_n1 = 0;
	m_n2 = -1;
	m_n3 = 0;
	m_n4 = -1;
	m_n5 = 4;
	m_n6 = -1;
	m_n7 = 0;
	m_n8 = -1;
	m_n9 = 0;
	UpdateData(false);
}


void CLPLSDlg::OnBnClickedLpls8()
{
	UpdateData(true);
	m_n1 = -1;
	m_n2 = -1;
	m_n3 = -1;
	m_n4 = -1;
	m_n5 = 8;
	m_n6 = -1;
	m_n7 = -1;
	m_n8 = -1;
	m_n9 = -1;
	UpdateData(false);
}
