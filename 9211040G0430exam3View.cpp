
// 9211040G0430exam3View.cpp: CMy9211040G0430exam3View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "9211040G0430exam3.h"
#endif

#include "9211040G0430exam3Doc.h"
#include "9211040G0430exam3View.h"
#include "simpleDIB.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy9211040G0430exam3View

IMPLEMENT_DYNCREATE(CMy9211040G0430exam3View, CView)

BEGIN_MESSAGE_MAP(CMy9211040G0430exam3View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMy9211040G0430exam3View 构造/析构

CMy9211040G0430exam3View::CMy9211040G0430exam3View() noexcept
{
	// TODO: 在此处添加构造代码


}

CMy9211040G0430exam3View::~CMy9211040G0430exam3View()
{
}

BOOL CMy9211040G0430exam3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMy9211040G0430exam3View 绘图

void CMy9211040G0430exam3View::OnDraw(CDC* pDC)
{
	CMy9211040G0430exam3Doc* pDoc = GetDocument();
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



	//自动阈值迭代法	
	int T = 0;
	int T0 = 0;
	int Ttemp = 0;

	int sum = 0;
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int c = dib.pData[(j * width) + i];
			sum += c;
		}
	}
	T0 = sum / N;  //对图像求取灰度平均值作为初始阈值T0
	Ttemp = T0;

	int flag = 0;
	do {
		flag = 0;
		//根据阈值T将图像分割为前景R1和背景R2
		int a[256] = { 0 };
		int b[256] = { 0 };
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				int c = dib.pData[j * width + i];
				if (c < Ttemp)
					a[c] += 1;
				else
					b[c] += 1;
			}
		}

		//两者的平均灰度值 u1 和 u2；
		int sum_a = 0;
		int num_a = 0;
		for (int i = 0; i < 256; i++) {
			num_a += a[i];
			sum_a += i * a[i];
		}
		float avg_a = sum_a / num_a;

		int sum_b = 0;
		int num_b = 0;
		for (int i = 0; i < 256; i++) {
			num_b += b[i];
			sum_b += i * b[i];
		}
		float avg_b = sum_b / num_b;

		//新阈值 T=(u1+u2)/2
		if ((int)(avg_a / 2 + avg_b / 2) != Ttemp) {
			Ttemp = (int)((avg_a + avg_b) / 2);
			flag = 1;
		}
		else { //T不再变化则为阈值，否则迭代计算
			T = Ttemp;
			flag = 0;
		}
	} while (flag == 1);

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int c = dib.pData[j * width + i];
			if (c > T)
				pDC->SetPixel(i + 420, height - j, RGB(255, 255, 255));
			else
				pDC->SetPixel(i + 420, height - j, RGB(0, 0, 0));
		}
	}


	//Otsu法(大津法)
	int Tbest = 0;
	float sigmab2best = 0;
	//令阈值T从 1 - 255 开始，根据阈值 T 将图像分割为前景 R1 和背景 R2，分别求出两者的平均灰度值 u1 和 u2，平均概率值 w1 和 w2
	for (int Ttemp = 1; Ttemp < 256; Ttemp++) {
		float avg_1 = 0, avg_2 = 0, w_1 = 0, w_2 = 0, sigmab2 = 0;
		//根据阈值Ttemp将图像分割为前景R1和背景R2
		int a_1[256] = { 0 };
		int b_2[256] = { 0 };
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				int c = dib.pData[j * width + i];
				if (c < Ttemp)
					a_1[c] += 1;
				else
					b_2[c] += 1;
			}
		}

		//两者的平均灰度值 u1 和 u2,平均概率值 w1 和 w2
		int sum_1 = 0;
		int num_1 = 0;
		for (int i = 0; i < 256; i++) {
			num_1 += a_1[i];
			sum_1 += i * a_1[i];
		}
		if (num_1 != 0)
			avg_1 = sum_1 / num_1;
		else
			avg_1 = 0;

		int sum_2 = 0;
		int num_2 = 0;
		for (int i = 0; i < 256; i++) {
			num_2 += b_2[i];
			sum_2 += i * b_2[i];
		}
		if (num_2 != 0)
			avg_2 = sum_2 / num_2;
		else
			avg_2 = 0;


		w_1 = (float)num_1 / N;
		w_2 = (float)num_2 / N;

		//计算前景和背景的类间方差
		sigmab2 = w_1 * w_2 * (avg_1 - avg_2) * (avg_1 - avg_2);

		//找出最大类间方差
		if (sigmab2 > sigmab2best)
		{
			sigmab2best = sigmab2;
			Tbest = Ttemp;
		}
	}

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int c = dib.pData[j * width + i];
			if (c > Tbest)
				pDC->SetPixel(i + 830, height - j, RGB(255, 255, 255));
			else
				pDC->SetPixel(i + 830, height - j, RGB(0, 0, 0));
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
	data.Format(_T("自动阈值迭代法处理后(T=46)"));
	pDC->TextOutW(360, 280, data);
	data.Format(_T("Otsu法处理后(T=51)"));
	pDC->TextOutW(830, 280, data);
}


// CMy9211040G0430exam3View 打印

BOOL CMy9211040G0430exam3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMy9211040G0430exam3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMy9211040G0430exam3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMy9211040G0430exam3View 诊断

#ifdef _DEBUG
void CMy9211040G0430exam3View::AssertValid() const
{
	CView::AssertValid();
}

void CMy9211040G0430exam3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy9211040G0430exam3Doc* CMy9211040G0430exam3View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy9211040G0430exam3Doc)));
	return (CMy9211040G0430exam3Doc*)m_pDocument;
}
#endif //_DEBUG


// CMy9211040G0430exam3View 消息处理程序
