#pragma once


// CLPLSDlg 对话框

class CLPLSDlg : public CDialog
{
	DECLARE_DYNAMIC(CLPLSDlg)

public:
	CLPLSDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLPLSDlg();

// 对话框数据
	enum { IDD = IDD_LPLS_SHARP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_n1;
	int m_n2;
	int m_n3;
	int m_n4;
	int m_n5;
	int m_n6;
	int m_n7;
	int m_n8;
	int m_n9;
	afx_msg void OnBnClickedLpls4();
	afx_msg void OnBnClickedLpls8();
};
