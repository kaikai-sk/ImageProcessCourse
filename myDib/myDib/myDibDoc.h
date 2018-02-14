
// myDibDoc.h : CmyDibDoc 类的接口
//


#pragma once

#include "DibImage.h"

class CmyDibDoc : public CDocument
{
protected: // 仅从序列化创建
	CmyDibDoc();
	DECLARE_DYNCREATE(CmyDibDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CmyDibDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
protected:
	int m_nOrigonX;
	int m_nOrigonY;//初始状态显示图片的左上角坐标
	int m_nX;
	int m_nY;//做改动的图象显示的左上角坐标
	CMyDib* m_pDibImage;
	HDIB m_hDIB;//dib句柄
	CPalette* m_palDIB;//调色板指针
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
	COLORREF m_refColorBKG;		// 背景色
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	// 初始化DIB对象
	void InitDIBData();
	
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnFileReopen();
};
