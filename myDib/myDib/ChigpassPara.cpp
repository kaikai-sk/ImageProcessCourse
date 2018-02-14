// ChigpassPara.cpp : 实现文件
//

#include "stdafx.h"
#include "myDib.h"
#include "ChigpassPara.h"
#include "afxdialogex.h"


// ChigpassPara 对话框

IMPLEMENT_DYNAMIC(ChigpassPara, CDialog)

ChigpassPara::ChigpassPara(CWnd* pParent /*=NULL*/)
	: CDialog(ChigpassPara::IDD, pParent)
	, m_filtertype(0)
	, m_highfreq1(0)
	, m_highfreq2(0)
{

}

ChigpassPara::~ChigpassPara()
{
}

void ChigpassPara::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_filtertype);
	DDX_Text(pDX, IDC_FREQ1, m_highfreq1);
	DDX_Text(pDX, IDC_FREQ2, m_highfreq2);
	DDX_Control(pDX, IDC_FREQ2, m_freq2control);
}


BEGIN_MESSAGE_MAP(ChigpassPara, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &ChigpassPara::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &ChigpassPara::OnBnClickedRadio2)
	ON_BN_CLICKED(IDOK, &ChigpassPara::OnBnClickedOk)
END_MESSAGE_MAP()


// ChigpassPara 消息处理程序

void ChigpassPara::OnBnClickedRadio1()
{
	m_filtertype = 0;
	UpdateData(FALSE);
	m_freq2control.EnableWindow(TRUE);
}


void ChigpassPara::OnBnClickedRadio2()
{
	m_filtertype = 1;
	UpdateData(FALSE);
	m_freq2control.EnableWindow(FALSE);
}


void ChigpassPara::OnBnClickedOk()
{
	UpdateData(true);
	if (m_highfreq1>255 || m_highfreq1<0 || m_highfreq2>255 || m_highfreq2<0)
	{
		MessageBox(_T("所设频率参数不正确!"), MB_OK);
		return;
	}
	CDialog::OnOK();
}
