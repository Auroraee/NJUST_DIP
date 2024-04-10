
// 9211040G0430exam4View.cpp: CMy9211040G0430exam4View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "9211040G0430exam4.h"
#endif

#include "9211040G0430exam4Doc.h"
#include "9211040G0430exam4View.h"
#include "simpleDIB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy9211040G0430exam4View

IMPLEMENT_DYNCREATE(CMy9211040G0430exam4View, CView)

BEGIN_MESSAGE_MAP(CMy9211040G0430exam4View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy9211040G0430exam4View 构造/析构

CMy9211040G0430exam4View::CMy9211040G0430exam4View() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy9211040G0430exam4View::~CMy9211040G0430exam4View()
{
}

BOOL CMy9211040G0430exam4View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy9211040G0430exam4View 绘图

void CMy9211040G0430exam4View::OnDraw(CDC* pDC)
{
	CMy9211040G0430exam4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	SimpleDIB dib;
	if (dib.ReadDIB("1.bmp"))
		dib.DrawByPixel(pDC, 10, 1);

	int width = dib.pbih->biWidth;
	int height = dib.pbih->biHeight;
	int N = width * height;

	//灰度图像转二值图像
	int val[65536] = { 0 };

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int c = dib.pData[j * width + i];
			if (c > 51)
				val[j * width + i] = 255;
			else
				val[j * width + i] = 0;
		}
	}
	//二值化图像
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int gray = val[j * width + i];
			pDC->SetPixel(i + 400, height - j, RGB(gray, gray, gray));
		}
	}


	// 膨胀（扩大边界）
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			int gray = 0;
			int c = val[y * width + x];
			if (c == 255)
				gray = 255;
			if (c == 0) {
				// 检查相邻像素
				if (val[(y - 1) * width + x] == 255 || // 上方
					val[y * width + (x - 1)] == 255 || // 左侧
					val[y * width + (x + 1)] == 255 || // 右侧
					val[(y + 1) * width + x] == 255)   // 下方

					gray = 255; // 周围有一个白色将检测点将像素设置为白色
				else
					gray = 0;
			}
			pDC->SetPixel(x + 800, height - y, RGB(gray, gray, gray));
		}
	}


	// 腐蚀(缩小边界)
	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			int gray = 0;
			int c = val[y * width + x];
			if (c == 0)
				gray = 0;
			if (c == 255) {
				// 检查相邻像素
				if (val[(y - 1) * width + x] == 255 && // 上方
					val[y * width + (x - 1)] == 255 && // 左侧
					val[y * width + (x + 1)] == 255 && // 右侧
					val[(y + 1) * width + x] == 255)   // 下方

					gray = 255; // 周围都是白色将检测点像素设置为白色
				else
					gray = 0;
			}
			pDC->SetPixel(x + 1200, height - y, RGB(gray, gray, gray));
		}
	}

	//输出文本设置	
	pDC->SetTextColor(RGB(255, 0, 0));
	CString data;
	CFont font;
	font.CreateFont(30,                                    //   字体的高度   
		0,                                          //   字体的宽度  
		0,                                          //  nEscapement 
		0,                                          //  nOrientation   
		FW_NORMAL,                                  //   nWeight   
		FALSE,                                      //   bItalic   
		FALSE,                                      //   bUnderline   
		0,                                                   //   cStrikeOut   
		ANSI_CHARSET,                             //   nCharSet   
		OUT_DEFAULT_PRECIS,                 //   nOutPrecision   
		CLIP_DEFAULT_PRECIS,               //   nClipPrecision   
		DEFAULT_QUALITY,                       //   nQuality   
		DEFAULT_PITCH | FF_SWISS,     //   nPitchAndFamily     
		_T("宋体"));
	pDC->SelectObject(&font);

	// 输出文本
	data.Format(_T("原图"));
	pDC->TextOutW(100, 280, data);
	data.Format(_T("Otsu法处理后"));
	pDC->TextOutW(430, 280, data);
	data.Format(_T("膨胀后图像"));
	pDC->TextOutW(850, 280, data);
	data.Format(_T("腐蚀后图像"));
	pDC->TextOutW(1250, 280, data);
}


// CMy9211040G0430exam4View 打印

BOOL CMy9211040G0430exam4View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy9211040G0430exam4View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy9211040G0430exam4View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy9211040G0430exam4View 诊断

#ifdef _DEBUG
void CMy9211040G0430exam4View::AssertValid() const
{
	CView::AssertValid();
}

void CMy9211040G0430exam4View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy9211040G0430exam4Doc* CMy9211040G0430exam4View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy9211040G0430exam4Doc)));
	return (CMy9211040G0430exam4Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy9211040G0430exam4View 消息处理程序
