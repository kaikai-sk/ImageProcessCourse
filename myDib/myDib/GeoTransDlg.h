#pragma once


// CGeoTransDlg 对话框

class CGeoTransDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeoTransDlg)

public:
	CGeoTransDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGeoTransDlg();

// 对话框数据
	enum { IDD = IDD_TRANS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	long m_XOffset;
	long m_YOffset;
};
