#ifndef __SIMPLE_DIB__
#define __SIMPLE_DIB__

#include <fstream>
using namespace std;

//该类只用来读取及存储8位灰度图及24位真彩色图
class SimpleDIB
{
public:
	SimpleDIB()
	{
		pbfh = NULL;
		pbi = NULL;//BITMAPINFO
		pbih = NULL;
		pQuad = NULL;
		pData = NULL;
	}

	~SimpleDIB()
	{
		clear();
	}

	//清除开辟的内存
	void clear()
	{
		if (pbfh != NULL)
			delete pbfh;
		if (pbi != NULL)
			delete[] pbi;
		if (pData != NULL)
			delete[] pData;

		pbfh = NULL;
		pbi = NULL;//BITMAPINFO
		pbih = NULL;
		pQuad = NULL;
		pData = NULL;

	}

	//判断当前有没有正常读入一幅图片
	BOOL valid()
	{
		if (pbfh == NULL)
			return FALSE;
		if (pbi == NULL)
			return FALSE;
		if (pbih == NULL)
			return FALSE;
		if (pQuad == NULL)
			return FALSE;
		if (pData == NULL)
			return FALSE;

		return TRUE;
	}


	//从位图中读取数据
	BOOL ReadDIB(char* fileName)
	{
		clear();
		//打开要读写的文件
		ifstream bmpFile;
		bmpFile.open(fileName, ios::in | ios::binary);
		if (bmpFile.fail())
			return FALSE;

		pbfh = new BITMAPFILEHEADER;

		//开辟BITMAPINFOHEADER+RGBQUAD的所需要的空间。因为这里只对8位及24位位图进行读取
		//对于8位位图，有256个调色板项，而24位位图没有调色板
		//这里只是为了方便，开辟了最大的空间，在实际程序中请根据实际需要多少调色板项开辟内存空间！！
		pbi = new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];

		//pbi的内容最开始的地方为BITMAPINFOHEADER，后面为RGBQUAD
		pbih = (BITMAPINFOHEADER*)pbi;
		pQuad = (RGBQUAD*)(pbi + sizeof(BITMAPINFOHEADER));


		//读取文件头信息
		bmpFile.read((char*)pbfh, sizeof(BITMAPFILEHEADER));
		//判断是不是位图，不是退出
		if (pbfh->bfType != 0x4d42) //'BM"
			return FALSE;

		//读取位图信息头
		bmpFile.read((char*)pbih, sizeof(BITMAPINFOHEADER));

		//只读8位及24位位图
		if (pbih->biBitCount != 8 && pbih->biBitCount != 24)
			return FALSE;

		//如果是8位位图，且字段biClrUsed==0，此时需要读调色板信息
		if (pbih->biBitCount == 8 && pbih->biClrUsed == 0)
			bmpFile.read((char*)pQuad, 256 * sizeof(RGBQUAD));

		//读取位图数据，因为pbih->biSizeImage并不可靠，所以还是要自己计算一下
		int lwidth = widthAlign(pbih->biWidth, pbih->biBitCount);
		int sizeImage = lwidth * pbih->biHeight;
		pData = new BYTE[sizeImage];
		bmpFile.read((char*)pData, sizeImage);

		//关闭文件
		bmpFile.close();

		return TRUE;
	}

	//此函数用于将pbmpdata以位图的形式写入文件fileName中
	//需要的参数有，pbmpdata中位图数据的位数bitcount，本函数只支持8位及24位
	//pbmpdata中位图数据的宽度width，pbmpdata中位图数据的高度height
	//注意，pbmpdata必须为纯位图数据，不含有每行4字节对齐的填充数据，此函数会完成4字节对齐的数据填充功能
	static BOOL WriteDIB(char* fileName, BYTE* pbmpdata, int bitcount, int width, int height)
	{
		if (pbmpdata == NULL)
			return FALSE;
		//只支持写入8位及24位位图
		if (bitcount != 8 && bitcount != 24)
			return FALSE;

		//计算一行数据实际存入文件的时候会占用多少字节
		int lwidth = widthAlign(width, bitcount);
		//计算文件的总长度
		int sizeImage = lwidth * height;

		//计算调色板数据量的大小
		int rgbQuadSize = 0;
		if (bitcount == 8)
			rgbQuadSize = 256 * sizeof(RGBQUAD);

		//定义文件头
		BITMAPFILEHEADER bfh;
		bfh.bfType = 0x4d42;	//'BM'
		//文件的长度
		bfh.bfSize = 0;//sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+rgbQuadSize+sizeImage;
		bfh.bfReserved1 = 0;
		bfh.bfReserved2 = 0;
		//计算从文件头到位图数据之间的偏移量
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + rgbQuadSize;


		//定义位图信息头
		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = width;
		bih.biHeight = height;
		bih.biPlanes = 1;//必须为1
		bih.biBitCount = bitcount;
		bih.biCompression = BI_RGB;//未压缩的BMP数据
		bih.biSizeImage = 0;//sizeImage;可以填0
		bih.biXPelsPerMeter = 0;//可以填0
		bih.biYPelsPerMeter = 0;//可以填0
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		ofstream bmpFile(fileName, ios::out | ios::binary);
		bmpFile.write((char*)&bfh, sizeof(bfh));//写入文件信息头
		bmpFile.write((char*)&bih, sizeof(bih));//写入位图信息头

		//如果是8位位图，需要写入调色板信息,注意，此处调色板的赋值方法只适用于8位灰度图像
		if (bitcount == 8)
		{
			for (int i = 0; i < 256; i++)
			{
				RGBQUAD rgbQuad;
				rgbQuad.rgbBlue = i;
				rgbQuad.rgbGreen = i;
				rgbQuad.rgbRed = i;
				rgbQuad.rgbReserved = 0;

				bmpFile.write((char*)&rgbQuad, sizeof(RGBQUAD));
			}
		}

		//以行为单位写入位图数据本身
		BYTE* lineData = new BYTE[lwidth];//每一行实际写入文件的时候要多少字节，是经过4字节对齐填充过后的
		BYTE* ptmp = pbmpdata;	//临时指针，用于指向写入文件的位图数据
		int bytesPerLine = width * bitcount / 8; //实际的位图数据，没有经过4字节对齐填充

		for (int i = 0; i < height; i++)//以行为单位写位图数据
		{
			memset(lineData, 0, lwidth);
			memcpy(lineData, ptmp, bytesPerLine);
			ptmp += bytesPerLine;

			bmpFile.write((char*)lineData, lwidth);
		}

		delete[] lineData;

		bmpFile.close();

		return TRUE;
	}

#ifdef _WINDOWS
	//left,top,width,height确定绘制的区域,输出的大小与图像大小相同
	//如果width,height长度为-1，则用图像本身的宽及高
	void Draw(CDC* pDC, int left = 0, int top = 0)
	{
		if (valid() == FALSE)
			return;

		int	width = pbih->biWidth;

		int	height = pbih->biHeight;

		StretchDIBits(pDC->GetSafeHdc(), left, top, width, height,
			0, 0, pbih->biWidth, pbih->biHeight, pData,
			(BITMAPINFO*)pbi, DIB_RGB_COLORS, SRCCOPY);
	}

	//逐像素的绘制图片，用于程序调试，实际绘图的时候不会这么做
	void DrawByPixel(CDC* pDC, int left = 0, int top = 0)
	{
		if (valid() == FALSE)
			return;

		//计算一行数据实际存入文件的时候会占用多少字节
		int lwidth = widthAlign(pbih->biWidth, pbih->biBitCount);
		BYTE* lineData = new BYTE[lwidth];

		if (pbih->biBitCount == 8)//灰度图
		{
			for (int y = 0; y < pbih->biHeight; y++)
			{
				memcpy(lineData, pData + lwidth * y, lwidth);
				BYTE* ptmp = lineData;

				for (int x = 0; x < pbih->biWidth; x++)
				{
					BYTE c = *ptmp++;
					RGBQUAD rgbQuad = pQuad[c];
					//因为位图存储的时候是上下颠倒的，所以输出的时候y值为height-y
					pDC->SetPixel(x + left, pbih->biHeight - y - 1 + top, RGB(rgbQuad.rgbRed, rgbQuad.rgbGreen, rgbQuad.rgbBlue));
				}
			}
		}

		if (pbih->biBitCount == 24)//真彩色图
		{
			for (int y = 0; y < pbih->biHeight; y++)
			{
				memcpy(lineData, pData + lwidth * y, lwidth);
				BYTE* ptmp = lineData;

				for (int x = 0; x < pbih->biWidth; x++)
				{
					BYTE b = *ptmp++;
					BYTE g = *ptmp++;
					BYTE r = *ptmp++;

					pDC->SetPixel(x + left, pbih->biHeight - y - 1 + top, RGB(r, g, b));
				}
			}
		}

		delete[] lineData;

	}
#endif

	//每行数据在存入文件的时候必须是4字节对齐的，即不是4字节倍数的时候进行填充，以使总长度为4的倍数
	//如一行有3个像素，每像素是24位（3字节），则总长度为9字节，实际存入文件的时候填充为12字节
	static int widthAlign(int width, int bitcount)
	{
		int lwidth = (width * bitcount / 8 + 3) / 4 * 4;
		return lwidth;
	}

public:	//注意，此处用public只是为了程序调用方便，在实际编程中应该用private或protected
	BITMAPFILEHEADER* pbfh;
	char* pbi;//BITMAPINFO
	BITMAPINFOHEADER* pbih;
	RGBQUAD* pQuad;
	BYTE* pData;
};


#endif //__SIMPLE_DIB__