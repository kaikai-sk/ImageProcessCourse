#pragma once
#include "afxwin.h"


// ChigpassPara �Ի���

class ChigpassPara : public CDialog
{
	DECLARE_DYNAMIC(ChigpassPara)

public:
	ChigpassPara(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ChigpassPara();

// �Ի�������
	enum { IDD = IDD_HIGHPASS_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_filtertype;
	int m_highfreq1;
	int m_highfreq2;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CEdit m_freq2control;
	afx_msg void OnBnClickedOk();
};
