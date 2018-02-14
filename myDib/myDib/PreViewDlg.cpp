// PreViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "myDib.h"
#include "PreViewDlg.h"
#include "afxdialogex.h"
#include "myDibDoc.h"
#include "MainFrm.h"
#include "myDibView.h"

// CPreViewDlg �Ի���

IMPLEMENT_DYNAMIC(CPreViewDlg, CDialog)

CPreViewDlg::CPreViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPreViewDlg::IDD, pParent)
{
}

CPreViewDlg::~CPreViewDlg()
{
}

void CPreViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPreViewDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPreViewDlg ��Ϣ�������


void CPreViewDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	BeginWaitCursor();//��ʾɳ©

	//���ȴӶԻ���İ�ť�������л��MainFrameָ��
	CMainFrame *pMain = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	//Ȼ������Ӧ��ͼָ��
	CmyDibView *pView = (CmyDibView *)pMain->GetActiveView();
	//�������ͼ��Ӧ�ĵ�ǰ�ĵ�ָ�� 
	 //CDocument* pCurrentDoc =(pMain->GetActiveDocument());
	CmyDibDoc* pCurrentDoc = (CmyDibDoc*)(pMain->GetActiveDocument());

	CDC*	pDC = GetDC();
	
	ASSERT_VALID(pCurrentDoc);//����ã��������ڲ��ĺϷ���
	HDIB hDIB = pCurrentDoc->GetHDIB();
	ShowDib(pDC, pCurrentDoc, hDIB);

	EndWaitCursor();
}

//���ܣ���ʾdib��ָ��ͼ��,���ڶ�ԭ����dib�����ػ�
//�������豸�����ģ��ļ���ָ�룬Դͼ��ľ��
//����ֵ��void
void CPreViewDlg::ShowDib(CDC* pDC, CmyDibDoc* pDoc, HDIB hDIB)
{
	if (hDIB != NULL)
	{
		LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);//�����ڴ�
		//�õ�ͼ��Ŀ��
		int cxDIB = (int)pDoc->GetDibImage()->DIBWidth(lpDIB);
		int cyDIB = (int)pDoc->GetDibImage()->DIBHeight(lpDIB);

		::GlobalUnlock((HGLOBAL)hDIB);//���ڴ���н���

		//���������������ʾͼƬ
		CRect rcDIB;
		rcDIB.top = pDoc->GetOrigonY();
		rcDIB.left = pDoc->GetOrigonX();
		rcDIB.right = cxDIB + pDoc->GetOrigonX();
		rcDIB.bottom = cyDIB + pDoc->GetOrigonY();

		CRect rcDest;

		//�����ǰ���豸�����������ڴ�ӡ�Ļ����ͽ��а�ҳ���С����
		//����������ڴ�ӡ���Ͳ���������
		if (pDC->IsPrinting())
		{
			// �Ǵ�ӡ���������ͼ���λ�úʹ�С���Ա����ҳ��

			// ��ȡ��ӡҳ���ˮƽ���(����)
			int cxPage = pDC->GetDeviceCaps(HORZRES);
			// ��ȡ��ӡҳ��Ĵ�ֱ�߶�(����)
			int cyPage = pDC->GetDeviceCaps(VERTRES);
			// ��ȡ��ӡ��ÿӢ��������
			int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

			// �����ӡͼ���С�����ţ�����ҳ���ȵ���ͼ���С��
			rcDest.top = rcDest.left = 0;
			rcDest.bottom = (int)(((double)cyDIB * cxPage * cyInch)
				/ ((double)cxDIB * cxInch));
			rcDest.right = cxPage;

			// �����ӡͼ��λ�ã���ֱ���У�
			int temp = cyPage - (rcDest.bottom - rcDest.top);
			rcDest.bottom += temp / 2;
			rcDest.top += temp / 2;
		}
		else
		{
			// ��������ͼ��
			rcDest = rcDIB;
		}

		pDoc->GetDibImage()->PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}
