
// myDibView.h : CmyDibView ��Ľӿ�
//

#pragma once

class CmyDibView : public CView
{
protected: // �������л�����
	CmyDibView();
	DECLARE_DYNCREATE(CmyDibView)

// ����
public:
	CmyDibDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CmyDibView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
private:
public:
	afx_msg void OnGeomTran();
	// ��ʾdib��ָ��ͼ��
	void ShowDib(CDC* pDC, CmyDibDoc* pDoc,HDIB hDib);
	afx_msg void OnGeomMirv();
	afx_msg void OnRhSobel();
	afx_msg void OnEdgeRobert();
	afx_msg void OnLplsSharp();
	afx_msg void OnPrewwit();
	afx_msg void OnAddcolor();
	afx_msg void OnHmirr();
};

#ifndef _DEBUG  // myDibView.cpp �еĵ��԰汾
inline CmyDibDoc* CmyDibView::GetDocument() const
   { return reinterpret_cast<CmyDibDoc*>(m_pDocument); }
#endif

