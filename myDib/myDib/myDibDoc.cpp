
// myDibDoc.cpp : CmyDibDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


// CmyDibDoc 构造/析构

CmyDibDoc::CmyDibDoc()
{
	// 默认背景色，灰色
	m_refColorBKG = 0x00808080;
	// 初始化变量
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
	// 判断DIB对象是否存在
	if (m_hDIB != NULL)
	{
		// 清除DIB对象
		::GlobalFree((HGLOBAL)m_hDIB);
	}

	// 判断调色板是否存在
	if (m_palDIB != NULL)
	{
		// 清除调色板
		delete m_palDIB;
		m_palDIB = NULL;
	}

	// 判断DibImage对象是否存在
	if (m_pDibImage != NULL)
	{
		// 清除DibImage对象
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

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CmyDibDoc 序列化

void CmyDibDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CmyDibDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
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

// 搜索处理程序的支持
void CmyDibDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
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

// CmyDibDoc 诊断

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


// CmyDibDoc 命令


BOOL CmyDibDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;

	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		//打开文件失败时调用此函数，并返回一个造成此次失败的原因的信息
		ReportSaveLoadException(lpszPathName, &fe, FALSE,
			AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}
	DeleteContents();//1.删除文档的数据；2确信一个文档在使用前为空。
	BeginWaitCursor();//本函数用于显示沙漏光标，当系统忙时

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
		//关闭与此对象相关联的文件并使文件不可读、写，如果在析构此对象时未关闭文件，则构造函数关闭它。
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
	EndWaitCursor();//停止沙漏光标

	// 判断读取文件是否成功
	if (m_hDIB == NULL)
	{
		CString strMsg;
		strMsg = "读取图像时出错！可能是不支持该类型的图像文件！";
		MessageBox(NULL, strMsg, _T("系统提示"), MB_ICONINFORMATION | MB_OK);

		if (m_pDibImage != NULL)
		{
			delete m_pDibImage;
			m_pDibImage = NULL;
		}

		return FALSE;
	}

	SetPathName(lpszPathName);			// 设置文件名称	
	SetModifiedFlag(FALSE);				// 初始化脏标记为FALSE

	return TRUE;
}

// 初始化DIB对象
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

	// 判断图像是否过大
	if (m_pDibImage->DIBWidth(lpDIB) > INT_MAX || m_pDibImage->DIBHeight(lpDIB)
		> INT_MAX)
	{
		::GlobalUnlock((HGLOBAL)m_hDIB);
		::GlobalFree((HGLOBAL)m_hDIB);

		m_hDIB = NULL;

		CString strMsg;
		strMsg = "BMP图像太大！";
		MessageBox(NULL, strMsg, _T("系统提示"), MB_ICONINFORMATION | MB_OK);

		return;
	}

	//类似于CPoint
	m_sizeDoc = CSize((int)m_pDibImage->DIBWidth(lpDIB),
		(int)m_pDibImage->DIBHeight(lpDIB));

	::GlobalUnlock((HGLOBAL)m_hDIB);

	// 创建新调色板
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
		// 保存失败，可能是其它格式的DIB，可以读取但是不能保存
		// 或者是SaveDIB函数有误
		CString strMsg;
		strMsg = "无法保存BMP图像！";
		MessageBox(NULL, strMsg, _T("系统提示"), MB_ICONINFORMATION | MB_OK);
	}

	return bSuccess;
}


void CmyDibDoc::OnFileReopen()
{
	// 重新打开图像，放弃所有修改

	// 判断当前图像是否已经被改动
	if (IsModified())
	{
		if (MessageBox(NULL, _T("重新打开图像将丢失所有改动！是否继续？"),
			_T("系统提示"), MB_ICONQUESTION | MB_YESNO) == IDNO)
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
		strMsg = "读取图像时出错！可能是不支持该类型的图像文件！";
		MessageBox(NULL, strMsg, _T("系统提示"), MB_ICONINFORMATION | MB_OK);

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
