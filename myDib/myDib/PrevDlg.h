#pragma once
#include "afxwin.h"


// CPrevDlg �Ի���

class CPrevDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrevDlg)

public:
	CPrevDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPrevDlg();

	void Redraw(void);					// Ԥ�����ػ溯��

// �Ի�������
	enum { IDD = IDD_PREV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// �ؼ�����
	CString m_strNum;// ͼ��������Ϣ�ı�
	CStatic m_staPrev;//ͼ��Ԥ����
	CScrollBar m_ScrollBar;//// ˮƽ������
	afx_msg void OnBnClickedOpen();
private:
	int				m_nPicNum;			// ��ǰĿ¼���ļ�����
	CString			m_strPath;			// ��ǰ��Ŀ¼
	CStringArray	m_FilesNameAry;		// ���浱ǰĿ¼�������ļ���������
	int				m_nPos;				// ��ǰ�ļ��������е�����
};
