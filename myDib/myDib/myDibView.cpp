
// myDibView.cpp : CmyDibView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "myDib.h"
#endif

#include "myDibDoc.h"
#include "myDibView.h"
#include "GeoTransDlg.h"
#include "LPLSDlg.h"
#include "PreViewDlg.h"
#include "DibImage.h"
#include "ChigpassPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

BOOL WINAPI TranslationDIB(LPSTR lpDIBBits, LONG lWidth, LONG lHeight, LONG lXOffset, LONG lYOffset)
{
	// ָ��Դͼ���ָ��
	LPSTR	lpSrc;

	// ָ��Ҫ���������ָ��
	LPSTR	lpDst;

	// ָ����ͼ���ָ��
	LPSTR	lpNewDIBBits;
	HLOCAL	hNewDIBBits;

	// ��������DIB�е�����
	LONG	i;
	LONG	j;

	// ������ԴDIB�е�����
	LONG	i0;
	LONG	j0;

	// ͼ��ÿ�е��ֽ���
	LONG lLineBytes;

	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);

	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		// �����ڴ�ʧ��
		return FALSE;
	}

	// �����ڴ�
	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);

	// ÿ��
	for (i = 0; i < lHeight; i++)
	{
		// ÿ��
		for (j = 0; j < lWidth; j++)
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			// ע������DIB��ͼ���һ����ʵ���������һ�е�λ�ã����lpDst
			// ֵ����(char *)lpNewDIBBits + lLineBytes * i + j������
			// (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j
			lpDst = (char *)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;

			// �����������ԴDIB�е�����
			i0 = i - lXOffset;
			j0 = j - lYOffset;

			// �ж��Ƿ���Դͼ��Χ��
			if ((j0 >= 0) && (j0 < lWidth) && (i0 >= 0) && (i0 < lHeight))
			{
				// ָ��ԴDIB��i0�У���j0�����ص�ָ��
				// ͬ��Ҫע��DIB���µ��õ�����
				lpSrc = (char *)lpDIBBits + lLineBytes * (lHeight - 1 - i0) + j0;

				// ��������
				*lpDst = *lpSrc;
			}
			else
			{
				// ����Դͼ��û�е����أ�ֱ�Ӹ�ֵΪ255
				*((unsigned char*)lpDst) = 255;
			}

		}
	}

	// ����ƽ�ƺ��ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);

	// �ͷ��ڴ�
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	// ����
	return TRUE;
}

// CmyDibView

IMPLEMENT_DYNCREATE(CmyDibView, CView)

BEGIN_MESSAGE_MAP(CmyDibView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_GEOM_TRAN, &CmyDibView::OnGeomTran)
	ON_COMMAND(ID_GEOM_MIRV, &CmyDibView::OnGeomMirv)
	ON_COMMAND(ID_RH_Sobel, &CmyDibView::OnRhSobel)
	ON_COMMAND(ID_EDGE_ROBERT, &CmyDibView::OnEdgeRobert)
	ON_COMMAND(ID_LPLS_SHARP, &CmyDibView::OnLplsSharp)
	ON_COMMAND(IDM_PREWWIT, &CmyDibView::OnPrewwit)
	ON_COMMAND(IDM_AddColor, &CmyDibView::OnAddcolor)
	ON_COMMAND(IDM_HMIRR, &CmyDibView::OnHmirr)
END_MESSAGE_MAP()

// CmyDibView ����/����

CmyDibView::CmyDibView()
{
	// TODO:  �ڴ˴���ӹ������
}

CmyDibView::~CmyDibView()
{
}

BOOL CmyDibView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CmyDibView ����

void CmyDibView::OnDraw(CDC* pDC)
{
	BeginWaitCursor();//��ʾɳ©

	CmyDibDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);//����ã��������ڲ��ĺϷ���
	HDIB hDIB = pDoc->GetHDIB();
	ShowDib(pDC, pDoc, hDIB);
	EndWaitCursor();
}

//���ܣ���ʾdib��ָ��ͼ��,���ڶ�ԭ����dib�����ػ�
//�������豸�����ģ��ļ���ָ�룬Դͼ��ľ��
//����ֵ��void
void CmyDibView::ShowDib(CDC* pDC, CmyDibDoc* pDoc,HDIB hDIB)
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
			/*rcDest.top = pDoc->GetY();
			rcDest.left = pDoc->GetX();
			rcDest.right = pDoc->GetX() + cxDIB;
			rcDest.bottom = pDoc->GetY() + cyDIB;*/
		}
		
		pDoc->GetDibImage()->PaintDIB(pDC->m_hDC, &rcDest, pDoc->GetHDIB(),
			&rcDIB, pDoc->GetDocPalette());
	}
}

// CmyDibView ��ӡ
BOOL CmyDibView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CmyDibView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CmyDibView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}


// CmyDibView ���

#ifdef _DEBUG
void CmyDibView::AssertValid() const
{
	CView::AssertValid();
}

void CmyDibView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmyDibDoc* CmyDibView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmyDibDoc)));
	return (CmyDibDoc*)m_pDocument;
}
#endif //_DEBUG



void CmyDibView::OnGeomTran()
{
	// ƽ��λͼ
	CmyDibDoc* pDoc = GetDocument();

	LPSTR	lpDIB;				// ָ��DIB��ָ��	
	LPSTR   lpDIBBits;			// ָ��DIB����ָ��
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// �ж��Ƿ���8-bppλͼ������Ϊ�˷��㣬ֻ����8-bppλͼ��ƽ�ƣ������Ŀ������ƣ�
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("Ŀǰֻ֧��256ɫλͼ��ƽ�ƣ�"), _T("ϵͳ��ʾ"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());

		return;
	}

	LONG lXOffset;
	LONG lYOffset;
	CGeoTransDlg dlgPara;
	dlgPara.m_XOffset = 100;
	dlgPara.m_YOffset = 100;

	// ��ʾ�Ի�����ʾ�û��趨ƽ����
	if (dlgPara.DoModal() != IDOK)
	{
		return;
	}
	lXOffset = dlgPara.m_XOffset;
	lYOffset = dlgPara.m_YOffset;
	delete dlgPara;

	BeginWaitCursor();
	pDoc->SetX(pDoc->GetX() + lXOffset);
	pDoc->SetY(pDoc->GetY() + lYOffset);
	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// ����TranslationDIB()����ƽ��DIB
	if (pDoc->GetDibImage()->TranslationDIB1(lpDIBBits,
		pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB), lXOffset,lYOffset))
	{
		pDoc->SetModifiedFlag(TRUE);		// ��������		
		pDoc->UpdateAllViews(NULL);			// ������ͼ
		Invalidate();
	}
	else
	{
		MessageBox(_T("�����ڴ�ʧ�ܣ�"), _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	EndWaitCursor();
}





void CmyDibView::OnGeomMirv()
{
	// ��ֱ����

	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// ָ��DIB��ָ��	
	LPSTR    lpDIBBits;			// ָ��DIB����ָ��
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// �ж��Ƿ���8-bppλͼ�����ﴦ��8-bppλͼ�Ĵ�ֱ����
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("Ŀǰֻ֧��256ɫλͼ�Ĵ�ֱ����"), _T("ϵͳ��ʾ"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	BeginWaitCursor();

	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// ����MirrorDIB()������ֱ����DIB
	if (pDoc->GetDibImage()->MirrorDIB(lpDIBBits, pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB), FALSE))
	{
		pDoc->SetModifiedFlag(TRUE);		// ��������		
		pDoc->UpdateAllViews(NULL);			// ������ͼ
	}
	else
	{
		MessageBox(_T("�����ڴ�ʧ�ܣ�"), _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	EndWaitCursor();
}

//���ܣ�������������
void CmyDibView::OnRhSobel()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// ָ��DIB��ָ��	
	LPSTR    lpDIBBits;			// ָ��DIB����ָ��	

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	
	DWORD width, length;
	width = pDoc->GetDibImage()->DIBWidth(lpDIB);
	length = pDoc->GetDibImage()->DIBHeight(lpDIB);

	pDoc->GetDibImage()->EdgeDetection(lpDIBBits, width, length);
	Invalidate();
}

//Robert��Ե�������
void CmyDibView::OnEdgeRobert()
{
	CmyDibDoc* pDoc = GetDocument();
	LPSTR	lpDIB;				// ָ��DIB��ָ��
	LPSTR   lpDIBBits;			// ָ��DIB����ָ��
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// �ж��Ƿ���8-bppλͼ
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("Ŀǰֻ֧��256ɫλͼ�����㣡"), _T("ϵͳ��ʾ"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}
	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// ����RobertDIB()������DIB���б�Ե���
	if (pDoc->GetDibImage()->RobertDIB(lpDIBBits,
		WIDTHBYTES(pDoc->GetDibImage()->DIBWidth(lpDIB) * 8),
		pDoc->GetDibImage()->DIBHeight(lpDIB)))
	{
		pDoc->SetModifiedFlag(TRUE);		// ��������		
		pDoc->UpdateAllViews(NULL);			// ������ͼ
	}
	else
	{
		MessageBox(_T("�����ڴ�ʧ�ܣ�"), _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
	}
	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
}

//������˹���Ӳ˵�����Ӧ����
void CmyDibView::OnLplsSharp()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// ָ��DIB��ָ��	
	LPSTR    lpDIBBits;			// ָ��DIB����ָ��	
	int		iTempH=3;				// ģ��߶�	
	int		iTempW=3;				// ģ����	
	float	fTempC=1;				// ģ��ϵ��	
	int		iTempMX=1;			// ģ������Ԫ��X����	
	int		iTempMY=1;			// ģ������Ԫ��Y����	
	float	aValue[9];			// ģ��Ԫ������

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);

	// �ж��Ƿ���8-bppλͼ
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("ֻ֧��256ɫλͼ���񻯣�"), _T("ϵͳ��ʾ"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	CLPLSDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		aValue[0] = dlg.m_n1;  aValue[1] = dlg.m_n2;  aValue[2] = dlg.m_n3;
		aValue[3] = dlg.m_n4;  aValue[4] = dlg.m_n5;  aValue[5] = dlg.m_n6;
		aValue[6] = dlg.m_n7;  aValue[7] = dlg.m_n8;  aValue[8] = dlg.m_n9;

		// ����Template()������������˹ģ����DIB
		if (pDoc->GetDibImage()->Template(lpDIBBits, pDoc->GetDibImage()->DIBWidth(lpDIB),
			pDoc->GetDibImage()->DIBHeight(lpDIB), iTempH, iTempW, iTempMX,
			iTempMY, aValue, fTempC))
		{
			pDoc->SetModifiedFlag(TRUE);		// ��������		
			pDoc->UpdateAllViews(NULL);			// ������ͼ
		}
		else
		{
			MessageBox(_T("�����ڴ�ʧ�ܣ�"), _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
		}
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	}
}

//Prewitt��Ե�������
void CmyDibView::OnPrewwit()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR	lpDIB;				// ָ��DIB��ָ��	
	LPSTR   lpDIBBits;			// ָ��DIB����ָ��

	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// �ж��Ƿ���8-bppλͼ������Ϊ�˷��㣬ֻ����8-bppλͼ�ı�Ե��⣬�����Ŀ������ƣ�
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("Ŀǰֻ֧��256ɫλͼ�����㣡"), _T("ϵͳ��ʾ"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);

	// ����PrewittDIB()������DIB���б�Ե���
	if (pDoc->GetDibImage()->PrewittEdge(lpDIBBits,
		pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB)))
	{
		pDoc->SetModifiedFlag(TRUE);		// ��������		
		pDoc->UpdateAllViews(NULL);			// ������ͼ
	}
	else
	{
		MessageBox(_T("�����ڴ�ʧ�ܣ�"), _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
	}
	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
}

//ȡ���˵�����Ӧ����
void CmyDibView::OnAddcolor()
{
	CmyDibDoc* pDoc = GetDocument();

	LPSTR	lpDIB;				// ָ��DIB��ָ��	
	LPSTR   lpDIBBits;			// ָ��DIB����ָ��
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());
	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	int imgWidth = pDoc->GetDibImage()->DIBWidth(lpDIB);
	int ImgHeight = pDoc->GetDibImage()->DIBHeight(lpDIB);

	int byteofLine = LineBytes(imgWidth * 8);
	LPSTR	lpNewDIBBits;		// ָ����ͼ���ָ��
	HLOCAL	hNewDIBBits;        //ָ��һ���ڴ������ָ��
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, byteofLine * ImgHeight);
	if (hNewDIBBits == NULL)
	{
		return ;
	}
	lpNewDIBBits = (char *)LocalLock(hNewDIBBits);
	// ��ʼ��ͼ��Ϊԭʼͼ��
	memcpy(lpNewDIBBits, lpDIBBits, byteofLine * ImgHeight);

	for (int y = 0; y<ImgHeight; y++)
	{
		for (int x = 0; x<imgWidth; x++)
		{
			lpNewDIBBits[y*imgWidth + x] = 255 - lpNewDIBBits[y*imgWidth + x];
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, byteofLine * ImgHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	Invalidate();
}

//��Ӧˮƽ����Ĳ˵��ĺ���
void CmyDibView::OnHmirr()
{
	//ˮƽ����

	CmyDibDoc* pDoc = GetDocument();

	LPSTR lpDIB;				// ָ��DIB��ָ��	
	LPSTR    lpDIBBits;			// ָ��DIB����ָ��
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)pDoc->GetHDIB());

	// �ж��Ƿ���8-bppλͼ�����ﴦ��8-bppλͼ�Ĵ�ֱ����
	if (pDoc->GetDibImage()->DIBNumColors(lpDIB) != 256)
	{
		MessageBox(_T("Ŀǰֻ֧��256ɫλͼ�Ĵ�ֱ����"), _T("ϵͳ��ʾ"),
			MB_ICONINFORMATION | MB_OK);
		::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
		return;
	}

	BeginWaitCursor();

	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = pDoc->GetDibImage()->FindDIBData(lpDIB);
	// ����MirrorDIB()������ֱ����DIB
	if (pDoc->GetDibImage()->MirrorDIB(lpDIBBits, pDoc->GetDibImage()->DIBWidth(lpDIB),
		pDoc->GetDibImage()->DIBHeight(lpDIB), true))
	{
		pDoc->SetModifiedFlag(TRUE);		// ��������		
		pDoc->UpdateAllViews(NULL);			// ������ͼ
	}
	else
	{
		MessageBox(_T("�����ڴ�ʧ�ܣ�"), _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
	}

	::GlobalUnlock((HGLOBAL)pDoc->GetHDIB());
	EndWaitCursor();
}
