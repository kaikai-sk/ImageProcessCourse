#pragma once

#include "DibImage.h"
#include "myDibDoc.h"

// CPreViewDlg �Ի���

class CPreViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CPreViewDlg)

public:
	CPreViewDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPreViewDlg();

// �Ի�������

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	void ShowDib(CDC* pDC, CmyDibDoc* pDoc, HDIB hDIB);
};
