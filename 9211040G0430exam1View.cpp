
// 9211040G0430exam1View.cpp: CMy9211040G0430exam1View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "9211040G0430exam1.h"
#endif

#include "9211040G0430exam1Doc.h"
#include "9211040G0430exam1View.h"
#include "simpleDIB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy9211040G0430exam1View

IMPLEMENT_DYNCREATE(CMy9211040G0430exam1View, CView)

BEGIN_MESSAGE_MAP(CMy9211040G0430exam1View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy9211040G0430exam1View 构造/析构

CMy9211040G0430exam1View::CMy9211040G0430exam1View() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy9211040G0430exam1View::~CMy9211040G0430exam1View()
{
}

BOOL CMy9211040G0430exam1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy9211040G0430exam1View 绘图

// 灰度线性变换及直方图均衡化
void CMy9211040G0430exam1View::OnDraw(CDC* pDC)
{
	CMy9211040G0430exam1Doc* pDoc = GetDocument();
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

	int gray_origin[256] = { 0 };
	int gray_glt[256] = { 0 };
	int gray_he[256] = { 0 };

	float p[256] = { 0 };
	float s[256] = { 0 };
	int s_t[256] = { 0 };


	BYTE* pdata = dib.pData;

	//遍历位图数据，储存灰度值信息
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
		{
			int c = dib.pData[y*width+x];
			//pDC->SetPixel(x, height - y, RGB(c, c, c));
			gray_origin[c]++;
		}
	}

	//灰度线性变换
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
		{
			int c = dib.pData[y * width + x];
			int s = 0;

			float x1 = 20, y1 = 60, x2 = 140, y2 = 180;
			if (c < x1) {
				s = (y1 / x1) * c;
			}
			else if (x1 <= c && c <= x2) {
				s = ((y2 - y1) / (x2 - x1)) * (c - x1) + y1;
			}
			else { // c > x2
				s = ((255 - y2) / (255 - x2)) * (c - x2) + y2;
			}

			//非线性D(B)=D(A)+0.8*D(A)*(255-D(A))/255
			//s = c + (0.8 * c * (255 - c)) / 255;

			pDC->SetPixel(x + 400, height - y, RGB(s, s, s));
			gray_glt[s]++;
		}
	}


	//直方图均衡化
	//统计每个灰度出现的概率
	for (int i = 0; i < 256; i++)
	{
		p[i] = ((float)gray_origin[i] / N);
	}
	//计算每个灰度累计概率
	s[0] = p[0];
	for (int i = 1; i < 256; i++)
	{
		s[i] = s[i - 1] + p[i];
	}
	//计算均衡化后的灰度值
	for (int i = 0; i < 256; i++)
	{
		s_t[i] = (int)(255 * s[i] + 0.5);
	}
	//直方图均衡化, 更新原图每个点的像素值
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int c = dib.pData[y * width + x];
			c = s_t[c];
			pDC->SetPixel(x + 800, height - y, RGB(c, c, c));
			gray_he[c]++;
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

	//灰度直方图绘制
	//原图
	for (int x = 0; x < 256; x++)
	{
		float a = sqrt(gray_origin[x]);

		for (int y = 0; y < a; y++)
			pDC->SetPixel(x + 10, 350 - y, RGB(0, 0, 255));

		// 输出文本 
		data.Format(_T("原图"));
		pDC->TextOutW(100, 380, data);
	}

	//灰度线性变换
	for (int x = 0; x < 256; x++)
	{
		float a = sqrt(gray_glt[x]);
		for (int y = 0; y < a; y++)
			pDC->SetPixel(x + 400, 350 - y, RGB(0, 0, 255));

		// 输出文本
		data.Format(_T("灰度线性变换后"));
		pDC->TextOutW(425, 380, data);
	}

	//直方图均衡化
	for (int x = 0; x < 256; x++)
	{
		float a = sqrt(gray_he[x]);
		for (int y = 0; y < a; y++)
			pDC->SetPixel(x + 800, 350 - y, RGB(0, 0, 255));

		// 输出文本
		data.Format(_T("直方图均衡化后"));
		pDC->TextOutW(825, 380, data);
	}
}


// CMy9211040G0430exam1View 打印

BOOL CMy9211040G0430exam1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy9211040G0430exam1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy9211040G0430exam1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy9211040G0430exam1View 诊断

#ifdef _DEBUG
void CMy9211040G0430exam1View::AssertValid() const
{
	CView::AssertValid();
}

void CMy9211040G0430exam1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy9211040G0430exam1Doc* CMy9211040G0430exam1View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy9211040G0430exam1Doc)));
	return (CMy9211040G0430exam1Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy9211040G0430exam1View 消息处理程序
