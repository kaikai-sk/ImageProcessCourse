
// myDibDoc.h : CmyDibDoc ��Ľӿ�
//


#pragma once

#include "DibImage.h"

class CmyDibDoc : public CDocument
{
protected: // �������л�����
	CmyDibDoc();
	DECLARE_DYNCREATE(CmyDibDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CmyDibDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
protected:
	int m_nOrigonX;
	int m_nOrigonY;//��ʼ״̬��ʾͼƬ�����Ͻ�����
	int m_nX;
	int m_nY;//���Ķ���ͼ����ʾ�����Ͻ�����
	CMyDib* m_pDibImage;
	HDIB m_hDIB;//dib���
	CPalette* m_palDIB;//��ɫ��ָ��
	CSize m_sizeDoc;
public:
	int GetX();
	int GetOrigonX();
	void SetX(int x);
	void SetOrigonX(int x);
	int GetY();
	int GetOrigonY();
	void SetY(int y);
	void SetOrigonY(int y);

	HDIB GetHDIB() const
	{
		return m_hDIB;
	}
	void SetHDIB(HDIB m_hDib)
	{
		this->m_hDIB = m_hDib;
	}
	CPalette* GetDocPalette() const
	{
		return m_palDIB;
	}
	CSize GetDocSize() const
	{
		return m_sizeDoc;
	}
	CMyDib* GetDibImage() const
	{
		return m_pDibImage;
	}
public:
	COLORREF m_refColorBKG;		// ����ɫ
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	// ��ʼ��DIB����
	void InitDIBData();
	
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileReopen();
};
