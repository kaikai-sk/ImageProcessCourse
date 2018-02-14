
// myDibDoc.cpp : CmyDibDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "myDib.h"
#endif

#include "myDibDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CmyDibDoc

IMPLEMENT_DYNCREATE(CmyDibDoc, CDocument)

BEGIN_MESSAGE_MAP(CmyDibDoc, CDocument)
	ON_COMMAND(ID_FILE_REOPEN, &CmyDibDoc::OnFileReopen)
END_MESSAGE_MAP()


// CmyDibDoc ����/����

CmyDibDoc::CmyDibDoc()
{
	// Ĭ�ϱ���ɫ����ɫ
	m_refColorBKG = 0x00808080;
	// ��ʼ������
	m_nOrigonX=0;
	m_nOrigonY=0;
	m_nX = m_nOrigonX;
	m_nY = m_nOrigonY;
	m_pDibImage = NULL;
	m_hDIB = NULL;
	m_palDIB = NULL;
	m_sizeDoc = CSize(1, 1);
}

CmyDibDoc::~CmyDibDoc()
{
	// �ж�DIB�����Ƿ����
	if (m_hDIB != NULL)
	{
		// ���DIB����
		::GlobalFree((HGLOBAL)m_hDIB);
	}

	// �жϵ�ɫ���Ƿ����
	if (m_palDIB != NULL)
	{
		// �����ɫ��
		delete m_palDIB;
		m_palDIB = NULL;
	}

	// �ж�DibImage�����Ƿ����
	if (m_pDibImage != NULL)
	{
		// ���DibImage����
		delete m_pDibImage;
		m_pDibImage = NULL;
	}
}

int CmyDibDoc::GetX()
{
	return this->m_nX;
}
int CmyDibDoc::GetOrigonX()
{
	return this->m_nOrigonX;
}
void CmyDibDoc::SetX(int x)
{
	this->m_nX = x;
}
void CmyDibDoc::SetOrigonX(int x)
{
	this->m_nOrigonX = x;
}
int CmyDibDoc::GetY()
{
	return this->m_nY;
}
int CmyDibDoc::GetOrigonY()
{
	return this->m_nOrigonY;
}
void CmyDibDoc::SetY(int y)
{
	this->m_nY = y;
}
void CmyDibDoc::SetOrigonY(int y)
{
	this->m_nOrigonY = y;
}

BOOL CmyDibDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CmyDibDoc ���л�

void CmyDibDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  �ڴ���Ӵ洢����
	}
	else
	{
		// TODO:  �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CmyDibDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CmyDibDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CmyDibDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CmyDibDoc ���

#ifdef _DEBUG
void CmyDibDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CmyDibDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CmyDibDoc ����


BOOL CmyDibDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;

	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		//���ļ�ʧ��ʱ���ô˺�����������һ����ɴ˴�ʧ�ܵ�ԭ�����Ϣ
		ReportSaveLoadException(lpszPathName, &fe, FALSE,
			AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	DeleteContents();//1.ɾ���ĵ������ݣ�2ȷ��һ���ĵ���ʹ��ǰΪ�ա�
	BeginWaitCursor();//������������ʾɳ©��꣬��ϵͳæʱ

	if (m_pDibImage != NULL)
	{
		delete m_pDibImage;
		m_pDibImage = NULL;
	}
	m_pDibImage = new CMyDib;

	TRY
	{
		m_hDIB = m_pDibImage->ReadDIBFile(file);
	}
	CATCH(CFileException ,eLoad)
	{
		//�ر���˶�����������ļ���ʹ�ļ����ɶ���д������������˶���ʱδ�ر��ļ������캯���ر�����
		file.Abort();
		EndWaitCursor();

		ReportSaveLoadException(lpszPathName, eLoad, FALSE,
			AFX_IDP_FAILED_TO_OPEN_DOC);

		m_hDIB = NULL;

		if (m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return FALSE;
	}
	END_CATCH

	InitDIBData();
	EndWaitCursor();//ֹͣɳ©���

	// �ж϶�ȡ�ļ��Ƿ�ɹ�
	if (m_hDIB == NULL)
	{
		CString strMsg;
		strMsg = "��ȡͼ��ʱ���������ǲ�֧�ָ����͵�ͼ���ļ���";
		MessageBox(NULL, strMsg, _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);

		if (m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return FALSE;
	}

	SetPathName(lpszPathName);			// �����ļ�����	
	SetModifiedFlag(FALSE);				// ��ʼ������ΪFALSE

	return TRUE;
}

// ��ʼ��DIB����
void CmyDibDoc::InitDIBData()
{
	if (m_palDIB != NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;
	}
	if (m_hDIB == NULL)
	{
		return;
	}

	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)m_hDIB);

	// �ж�ͼ���Ƿ����
	if (m_pDibImage->DIBWidth(lpDIB) > INT_MAX || m_pDibImage->DIBHeight(lpDIB)
		> INT_MAX)
	{
		::GlobalUnlock((HGLOBAL)m_hDIB);
		::GlobalFree((HGLOBAL)m_hDIB);

		m_hDIB = NULL;

		CString strMsg;
		strMsg = "BMPͼ��̫��";
		MessageBox(NULL, strMsg, _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);

		return;
	}

	//������CPoint
	m_sizeDoc = CSize((int)m_pDibImage->DIBWidth(lpDIB),
		(int)m_pDibImage->DIBHeight(lpDIB));

	::GlobalUnlock((HGLOBAL)m_hDIB);

	// �����µ�ɫ��
	m_palDIB = new CPalette;
	if (m_palDIB == NULL)
	{
		::GlobalFree((HGLOBAL)m_hDIB);
		m_hDIB = NULL;

		return;
	}

	if (m_pDibImage->CreateDIBPalette(m_hDIB, m_palDIB) == NULL)
	{
		delete m_palDIB;
		m_palDIB = NULL;

		return;
	}
}


BOOL CmyDibDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;

	if (!file.Open(lpszPathName, CFile::modeCreate | CFile::modeReadWrite
		| CFile::shareExclusive, &fe))
	{
		ReportSaveLoadException(lpszPathName, &fe, TRUE, AFX_IDP_INVALID_FILENAME);
		return FALSE;
	}

	BOOL bSuccess = FALSE;
	TRY
	{
		BeginWaitCursor();
		bSuccess = m_pDibImage->SaveDIB(m_hDIB, file);
		file.Close();
	}
	CATCH(CException, eSave)
	{
		file.Abort();
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName, eSave, TRUE,
			AFX_IDP_FAILED_TO_SAVE_DOC);

		return FALSE;
	}
	END_CATCH

		EndWaitCursor();
	SetModifiedFlag(FALSE);

	if (!bSuccess)
	{
		// ����ʧ�ܣ�������������ʽ��DIB�����Զ�ȡ���ǲ��ܱ���
		// ������SaveDIB��������
		CString strMsg;
		strMsg = "�޷�����BMPͼ��";
		MessageBox(NULL, strMsg, _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);
	}

	return bSuccess;
}


void CmyDibDoc::OnFileReopen()
{
	// ���´�ͼ�񣬷��������޸�

	// �жϵ�ǰͼ���Ƿ��Ѿ����Ķ�
	if (IsModified())
	{
		if (MessageBox(NULL, _T("���´�ͼ�񽫶�ʧ���иĶ����Ƿ������"),
			_T("ϵͳ��ʾ"), MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			return;
		}
	}

	CFile file;
	CFileException fe;
	CString strPathName;
	strPathName = GetPathName();

	if (!file.Open(strPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		ReportSaveLoadException(strPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);

		return;
	}

	BeginWaitCursor();

	if (m_pDibImage != NULL)
	{
		delete m_pDibImage;
		m_pDibImage = NULL;
	}
	m_pDibImage = new CMyDib;

	TRY
	{
		m_hDIB = m_pDibImage->ReadDIBFile(file);
	}
	CATCH(CFileException, eLoad)
	{
		file.Abort();
		EndWaitCursor();

		ReportSaveLoadException(strPathName, eLoad, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);

		m_hDIB = NULL;
		if (m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return;
	}
	END_CATCH

		InitDIBData();

	if (m_hDIB == NULL)
	{
		CString strMsg;
		strMsg = "��ȡͼ��ʱ���������ǲ�֧�ָ����͵�ͼ���ļ���";
		MessageBox(NULL, strMsg, _T("ϵͳ��ʾ"), MB_ICONINFORMATION | MB_OK);

		EndWaitCursor();

		if (m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return;
	}

	SetModifiedFlag(FALSE);
	UpdateAllViews(NULL);
	EndWaitCursor();

	return;
}
