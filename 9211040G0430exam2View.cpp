
// 9211040G0430exam2View.cpp: CMy9211040G0430exam2View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "9211040G0430exam2.h"
#endif

#include "9211040G0430exam2Doc.h"
#include "9211040G0430exam2View.h"

#include "simpleDIB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy9211040G0430exam2View

IMPLEMENT_DYNCREATE(CMy9211040G0430exam2View, CView)

BEGIN_MESSAGE_MAP(CMy9211040G0430exam2View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy9211040G0430exam2View 构造/析构

CMy9211040G0430exam2View::CMy9211040G0430exam2View() noexcept
{
	// TODO: 在此处添加构造代码

}

CMy9211040G0430exam2View::~CMy9211040G0430exam2View()
{
}

BOOL CMy9211040G0430exam2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy9211040G0430exam2View 绘图

void CMy9211040G0430exam2View::OnDraw(CDC* pDC)
{
	CMy9211040G0430exam2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	SimpleDIB dib;
	if (dib.ReadDIB("1.bmp"))
		dib.DrawByPixel(pDC, 10, 1);

	int width = dib.pbih->biWidth;
	int height = dib.pbih->biHeight;

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

	//中值滤波
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
		{
			//3*3
			int kernel[9] = { dib.pData[(y - 1) * width + x - 1],dib.pData[(y - 1) * width + x],dib.pData[(y - 1) * width + x + 1],dib.pData[y * width + x - 1],dib.pData[y * width + x],dib.pData[y * width + x + 1],dib.pData[(y + 1) * width + x - 1],dib.pData[(y + 1) * width + x],dib.pData[(y + 1) * width + x + 1] };

			int flag = 0;
			int mid = 0;
			do {//冒泡排序
				flag = 0;
				int temp = 0;
				for (int m = 0; m < 8; m++) {
					if (kernel[m] < kernel[m + 1]) {
						temp = kernel[m];
						kernel[m] = kernel[m + 1];
						kernel[m + 1] = temp;
						flag = 1;
					}
				}
			} while (flag == 1);
			mid = kernel[4];
			pDC->SetPixel(x + 400, height - y, RGB(mid, mid, mid));
		}
	}
	// 输出文本
	data.Format(_T("原图"));
	pDC->TextOutW(100, 270, data);
	data.Format(_T("(3*3)中值滤波后"));
	pDC->TextOutW(415, 270, data);


	//Sobel算子
	int xedge[3][3] = { {-1,0,1}, {-2,0,2},{-1,0,1} };
	int yedge[3][3] = { {-1,-2,-1},{0,0,0},{1,2,1} };
	////Prewitt算子
	//int xedge[3][3] = { {-1,0,1}, {-1,0,1},{-1,0,1} };
	//int yedge[3][3] = { {-1,-1,-1},{0,0,0},{1,1,1} };

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int dx = 0, dy = 0, c = 0, sx = 0, sy = 0;
			for (int a = -1; a <= 1; a++) {
				for (int b = -1; b <= 1; b++) {
					dx = xedge[1 + a][1 + b];
					dy = yedge[1 + a][1 + b];
					c = dib.pData[(y + a) * width + (x + b)];
					sx = sx + c * dx;
					sy = sy + c * dy;
					//sx = max(0, min(255, sx));
					//sy = max(0, min(255, sy));
				}
			}
			/*int sxabs = abs(sx);
			int syabs = abs(sy);
			int r = sxabs + syabs;*/
			int r = max(0, min(255, abs(sx) + abs(sy)));
			pDC->SetPixel(x + 800, height - y, RGB(r, r, r));
		}
	}
	data.Format(_T("Sobel算子处理后"));
	pDC->TextOutW(815, 270, data);
}


// CMy9211040G0430exam2View 打印

BOOL CMy9211040G0430exam2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy9211040G0430exam2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy9211040G0430exam2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy9211040G0430exam2View 诊断

#ifdef _DEBUG
void CMy9211040G0430exam2View::AssertValid() const
{
	CView::AssertValid();
}

void CMy9211040G0430exam2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy9211040G0430exam2Doc* CMy9211040G0430exam2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy9211040G0430exam2Doc)));
	return (CMy9211040G0430exam2Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy9211040G0430exam2View 消息处理程序
