
// CubeView.h: CCubeView 类的接口
//

#pragma once
#include "Cube.hpp"


class CCubeView : public CView
{
protected: // 仅从序列化创建
	CCubeView() noexcept;
	DECLARE_DYNCREATE(CCubeView)

	// 特性
public:
	CCubeDoc* GetDocument() const;

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
public:


	// 实现
public:
	virtual ~CCubeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	void DrawObject(CDC* pDC);// 绘图函数（测试）
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
	Cube cub;//立方体
	CPoint m_st_pos;//鼠标起始位置
};

#ifndef _DEBUG  // CubeView.cpp 中的调试版本
inline CCubeDoc* CCubeView::GetDocument() const
{
	return reinterpret_cast<CCubeDoc*>(m_pDocument);
}
#endif

