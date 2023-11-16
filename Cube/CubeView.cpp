
// CubeView.cpp: CCubeView 类的实现
//

#include "pch.h"
#include "framework.h"
//#include "Cube.hpp"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Cube.h"
#endif

#include "CubeDoc.h"
#include "CubeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCubeView

IMPLEMENT_DYNCREATE(CCubeView, CView)

BEGIN_MESSAGE_MAP(CCubeView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCubeView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CCubeView 构造/析构

CCubeView::CCubeView() noexcept
{
	// TODO: 在此处添加构造代码
	lm_clicked = false;
}

CCubeView::~CCubeView()
{
}

BOOL CCubeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCubeView 绘图

void CCubeView::OnDraw(CDC* pDC)
{
	CCubeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	DoubleBuffer();
}


// CCubeView 打印


void CCubeView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCubeView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCubeView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCubeView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CCubeView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCubeView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCubeView 诊断

#ifdef _DEBUG
void CCubeView::AssertValid() const
{
	CView::AssertValid();
}

void CCubeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCubeDoc* CCubeView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCubeDoc)));
	return (CCubeDoc*)m_pDocument;
}
#endif //_DEBUG

// CCubeView 消息处理程序


// 绘图函数,绘制立方体当前状态
void CCubeView::DrawCube(CDC* pDC)
{
	// TODO: 在此处添加实现代码.
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(0, 0, 0));//涂黑背景
	pDC->SetMapMode(MM_ANISOTROPIC);//设置映射模式
	pDC->SetWindowExt(rect.Width(), rect.Height());//设置窗口
	pDC->SetViewportExt(rect.Width(), -rect.Height());//x→y↑
	pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);//中心为坐标原点

	//创建并设置灰色画笔
	CPen GreyPen, * pOldPen;
	GreyPen.CreatePen(PS_SOLID, 5, RGB(127, 127, 127));
	pOldPen = pDC->SelectObject(&GreyPen);
	//创建六色画刷
	CBrush Brush[6], * pOldBrush;
	Brush[Cube::Color::blue].CreateSolidBrush(RGB(0, 0, 255));//蓝
	Brush[Cube::Color::red].CreateSolidBrush(RGB(255, 0, 0));//红
	Brush[Cube::Color::yellow].CreateSolidBrush(RGB(255, 255, 0));//黄
	Brush[Cube::Color::white].CreateSolidBrush(RGB(255, 255, 255));//白
	Brush[Cube::Color::orange].CreateSolidBrush(RGB(255, 128, 0));//橙
	Brush[Cube::Color::green].CreateSolidBrush(RGB(0, 255, 0));//绿
	pOldBrush = pDC->SelectObject(&Brush[0]);

	//将三维坐标系的点坐标转换为透视投影后的屏幕坐标
	Matrix CP = cub.P * cub.T_ws;
	std::vector<CPoint>P(cub.P.size());
	for (int i = 0;i < P.size();i++)
		P[i] = Cube::trans_point(CP[i]);
	//消隐算法，判断各面的可见性
	cub.HiddenSurfaceRemovalAlgorithm();

	//根据可见性绘制各面
	for (int i = 0;i < cub.Fs.size();i++)
	{
		if (cub.Fs[i].visible == true)
		{
			pDC->BeginPath();
			pDC->MoveTo(P[cub.Fs[i].P_idx[3]]);
			for (int j = 0;j < cub.Fs[i].P_idx.size();j++)
				pDC->LineTo(P[cub.Fs[i].P_idx[j]]);
			pDC->EndPath();
			pDC->SelectObject(&Brush[i/9]);
			pDC->StrokeAndFillPath();
		}
	}

	//结束
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	GreyPen.DeleteObject();
	for (int i = 0;i < 6;i++)
		Brush[i].DeleteObject();
}

void CCubeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_st_pos = point;//记录鼠标起始位置
	lm_clicked = true;
	CP_st = cub.P;
	CView::OnLButtonUp(nFlags, point);
}

void CCubeView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lm_clicked = false;
	//获得移动距离
	int dx = point.x - m_st_pos.x;
	int dy = point.y - m_st_pos.y;
	//根据移动距离计算旋转角度并旋转
	cub.rotate(dx, dy);
	DoubleBuffer();
	CView::OnLButtonUp(nFlags, point);
}


void CCubeView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (lm_clicked == true)
	{
		//获得移动距离
		int dx = point.x - m_st_pos.x;
		int dy = point.y - m_st_pos.y;
		//根据移动距离计算旋转角度并旋转
		cub.rotate(dx, dy);
		DoubleBuffer();
		cub.P = CP_st;
		//因为每次都是根据初始状态计算旋转角度来实时更新状态
		//所以每次完成后需要还原到初始状态便于下次计算
		//等松开左键才真正确定旋转角度
	}
	CView::OnMouseMove(nFlags, point);
}


void CCubeView::DoubleBuffer(void)
{
	// TODO: 在此处添加实现代码.
	CDC* pDC = GetDC();
	CRect rect;
	GetClientRect(&rect);
	pDC->SetMapMode(MM_ANISOTROPIC);//设置映射模式
	pDC->SetWindowExt(rect.Width(), rect.Height());//设置窗口
	pDC->SetViewportExt(rect.Width(), -rect.Height());//x→y↑
	pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);//中心为坐标原点

	//创建与指定设备兼容的设备上下文（DC）和位图（Bitmap）
	CDC memDC;
	CBitmap NewBitmap, * pOldBitmap;
	memDC.CreateCompatibleDC(pDC);
	NewBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	pOldBitmap = memDC.SelectObject(&NewBitmap);
	//在新建的位图上绘图
	DrawCube(&memDC);
	//将内存设备上下文的位图传输到显示设备上下文中
	pDC->BitBlt(
		-rect.Width() / 2, -rect.Height() / 2,
		rect.Width(), rect.Height(),
		&memDC,
		-rect.Width() / 2, -rect.Height() / 2,
		SRCCOPY);
	//结束
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	NewBitmap.DeleteObject();
}
