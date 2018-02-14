#pragma once

#include "DibImage.h"
#include "myDibDoc.h"

// CPreViewDlg 对话框

class CPreViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreViewDlg)

public:
	CPreViewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPreViewDlg();

// 对话框数据

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	void ShowDib(CDC* pDC, CmyDibDoc* pDoc, HDIB hDIB);
};
