#pragma once


// CGeoTransDlg �Ի���

class CGeoTransDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeoTransDlg)

public:
	CGeoTransDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGeoTransDlg();

// �Ի�������
	enum { IDD = IDD_TRANS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	long m_XOffset;
	long m_YOffset;
};
