// PrevDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "myDib.h"
#include "PrevDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CPrevDlg �Ի���

IMPLEMENT_DYNAMIC(CPrevDlg, CDialog)

CPrevDlg::CPrevDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrevDlg::IDD, pParent)
	, m_strNum(_T(""))
{

}

CPrevDlg::~CPrevDlg()
{
}

void CPrevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PICNUM, m_strNum);
	DDX_Control(pDX, IDC_PREV, m_staPrev);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
}


BEGIN_MESSAGE_MAP(CPrevDlg, CDialog)
	ON_BN_CLICKED(IDC_OPEN, &CPrevDlg::OnBnClickedOpen)
END_MESSAGE_MAP()


/******************************************************************************
*	����:		Ԥ�����ػ溯��
******************************************************************************/
void CPrevDlg::Redraw(void)
{
	CDC* pDC = m_staPrev.GetDC();
	Graphics graph(pDC->GetSafeHdc());
	CRect rect;

	m_staPrev.GetClientRect(rect);
	pDC->FillRect(rect, &CBrush(RGB(211, 211, 211)));

	if (m_nPos >= 0)
	{
		CString m_strFilePath = m_strPath + L"\\" + m_FilesNameAry[m_nPos];
		Image image(m_strFilePath);
		CRect rect;

		m_staPrev.GetClientRect(rect);

		REAL width = (REAL)image.GetWidth();
		REAL height = (REAL)image.GetHeight();
		REAL c = 0.0f;
		REAL x = 0.0f;
		REAL y = 0.0f;

		// ���ʺϿ��ģʽ����
		width = (REAL)rect.Width();
		c = width / image.GetWidth();

		// ���û�г���
		if (c * image.GetHeight() < rect.Height())
			height = c * image.GetHeight();
		// ����������ʺϸ߶�ģʽ
		else
		{
			height = (REAL)rect.Height();
			c = height / image.GetHeight();
			width = c * image.GetWidth();
		}

		// ʹͼ�����
		if ((UINT)rect.Width() > width)
			x = (rect.Width() - width) / 2.0f;

		if ((UINT)rect.Height() > height)
			y = ((UINT)rect.Height() - height) / 2.0f;

		graph.DrawImage(&image, x, y, width, height);
	}

	ReleaseDC(pDC);
}

// CPrevDlg ��Ϣ�������


void CPrevDlg::OnBnClickedOpen()
{
	CFileDialog dlg(
		true, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		L"����֧���ļ�(*.bmp;*.jpg)|*.bmp;*.jpg|λͼ�ļ�(*.bmp)|*.bmp|JPG�ļ�(*.jpg)||");

	if (dlg.DoModal() == IDOK)
	{
		CString strTemp = dlg.GetPathName();
		int nIndex = strTemp.ReverseFind('\\');
		m_strPath = strTemp.Left(nIndex);	// ��ȡ��ǰ���ļ�������Ŀ¼

		// ��ô�Ŀ¼�µ�����ͼ���ļ�
		CMainFrame::GetAllFileNames(m_strPath, m_FilesNameAry);
		m_nPicNum = (int)m_FilesNameAry.GetCount();

		m_nPos = 0;
		CString strFileName = dlg.GetFileName();

		// ���㵱ǰ�ļ�������
		while (m_nPos < m_nPicNum && m_FilesNameAry[m_nPos] != strFileName)
			m_nPos++;

		m_ScrollBar.SetScrollRange(0, m_nPicNum - 1);
		m_ScrollBar.SetScrollPos(m_nPos);
		m_strNum.Format(L"�� %d ��,�� %d ��", m_nPos + 1, m_nPicNum);
		UpdateData(FALSE);

		Redraw();
	}
}
