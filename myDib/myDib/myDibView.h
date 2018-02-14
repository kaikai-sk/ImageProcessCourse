
// myDibView.h : CmyDibView 类的接口
//

#pragma once

class CmyDibView : public CView
{
protected: // 仅从序列化创建
	CmyDibView();
	DECLARE_DYNCREATE(CmyDibView)

// 特性
public:
	CmyDibDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CmyDibView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
private:
public:
	afx_msg void OnGeomTran();
	// 显示dib所指的图像
	void ShowDib(CDC* pDC, CmyDibDoc* pDoc,HDIB hDib);
	afx_msg void OnGeomMirv();
	afx_msg void OnRhSobel();
	afx_msg void OnEdgeRobert();
	afx_msg void OnLplsSharp();
	afx_msg void OnPrewwit();
	afx_msg void OnAddcolor();
	afx_msg void OnHmirr();
};

#ifndef _DEBUG  // myDibView.cpp 中的调试版本
inline CmyDibDoc* CmyDibView::GetDocument() const
   { return reinterpret_cast<CmyDibDoc*>(m_pDocument); }
#endif

