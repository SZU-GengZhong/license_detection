// Image.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <commdlg.h>
#include <direct.h>
#include "Image.h"

#include "re.h"
#include "py_call.h"
#include "Mytool.h"
#include "canny.h"

HDC hWinDC;
HDC mdc;
char ImgFileName[512];
int ImageWidth;
int ImageHeight;
int ImageColor;
int Avg_s, Avg_v;
BITMAPFILEHEADER bfImage;
BITMAPINFOHEADER biImage;

/************************************************************************************************
*																								*
*	读图像文件函数ReadImage(LPSTR ImageFileName, char *Image, int wImage, int hImage)			*
*																								*
*   	从图像文件ImageFileName中读出图像至Image[]向量中										*
*		Image－存放图像点阵的向量																*
*		wImage－图像宽度																		*
*		hImage－图像高度																		*
*																								*
************************************************************************************************/
void ReadImage(LPSTR ImageFileName, char* Image, int wImage, int hImage) //读取图像信息并保存在Image[][]中
{
	OFSTRUCT of;
	HFILE Image_fp;
	
	Image_fp = OpenFile(ImageFileName, &of, OF_READ);
	if (Image_fp == HFILE_ERROR)
	{
		MessageBox(NULL, "打开读图像文件出错！", "出错信息", MB_OK);
	}

	_llseek(Image_fp, 0, 0);
	_lread(Image_fp, Image, wImage * hImage);
	_lclose(Image_fp);

}

/************************************************************************************************
*																								*
*	写图像文件函数WriteImage(LPSTR ImageFileName, char *Image, int wImage, int hImage)			*
*																								*
*   	将图像Image[]写入图像文件ImageFileName中												*
*		Image－存放图像点阵的向量																*
*		wImage－图像宽度																		*
*		hImage－图像高度																		*
*																								*
************************************************************************************************/
void WriteImage(LPSTR ImageFileName, char* Image, int wImage, int hImage)
{
	OFSTRUCT of;
	HFILE Image_fp;

	Image_fp = OpenFile(ImageFileName, &of, OF_CREATE);
	_llseek(Image_fp, 0, 0);
	_lwrite(Image_fp, Image, wImage * hImage);

	_lclose(Image_fp);

	return;
}

/************************************************************************************************
*																								*
*	在屏幕上显示图像函数ShowImage(char *Image, int wImage, int hImage, int xPos, int yPos)		*
*																								*
*   	Image	－存放图像点阵的向量															*
*		wImage	－图像宽度																		*
*		hImage	－图像高度																		*
*		xPos	－图像显示开始位置x坐标															*
*		yPos	－图像显示开始位置y坐标															*
*																								*
************************************************************************************************/
void ShowImage(unsigned char* Image, int wImage, int hImage, int xPos, int yPos)
{
	int i, j;
	int coff;

	for (i = 0; i < hImage; i++)
	{
		for (j = 0; j < wImage; j++)
		{
			coff = (BYTE)Image[i * wImage + j];
			SetPixel(hWinDC, j + xPos, i + yPos, RGB(coff, coff, coff));

		}

	}
}

/************************************************************************************************
*																								*
*	获取文件名函数OpenImageFile(char *OPDLTitle)												*
*																								*
************************************************************************************************/
void OpenImageFile(char* OPDLTitle)
{
	char FileTitle[100], ImgDlgFileDir[512];
	OPENFILENAME ofn;

	_getcwd(ImgDlgFileDir,MAX_PATH);

	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = TEXT("bmp files\0*.bmp\0All File\0*.*\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = ImgFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = FileTitle;
	ofn.nMaxFileTitle = 99;
	ofn.lpstrInitialDir = ImgDlgFileDir;
	ofn.lpstrTitle = OPDLTitle;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = "bmp";
	ofn.lCustData = NULL;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;
	ImgFileName[0] = '\0';
	GetOpenFileName(&ofn);

	_getcwd(ImgDlgFileDir, MAX_PATH);

}

void WriteBMPImage(LPSTR ImageFileName, char* Image, int wImage, int hImage)
{
	OFSTRUCT of;
	HFILE Image_fp;
	
	unsigned int m_row_size = 4 * ((3 * wImage + 3) / 4);
	biImage.biSizeImage = m_row_size * hImage * 3;
	biImage.biSize = 40;
	bfImage.bfOffBits = 54;
	biImage.biWidth = wImage;
	biImage.biHeight = hImage;
	bfImage.bfSize = biImage.biSizeImage + bfImage.bfOffBits;
	
	Image_fp = OpenFile(ImageFileName, &of, OF_CREATE);
	_llseek(Image_fp, 0, 0);
	_lwrite(Image_fp, (char*)&bfImage, sizeof(BITMAPFILEHEADER));
	_lwrite(Image_fp, (char*)&biImage, sizeof(BITMAPINFOHEADER));
	_llseek(Image_fp, bfImage.bfOffBits, 0);
	_lwrite(Image_fp, Image, biImage.biSizeImage);

	_lclose(Image_fp);
	
	return;
}

void RGBToGrayProcessing(char* oImage, char* nImage, int wImage, int hImage)
{
	int i, j;
	int r, g, b, Y;
	
	for (i = 0; i < hImage; i++) {
		for (j = 0; j < wImage; j++) {
			r = (unsigned char)oImage[i * wImage * 3 + j * 3 + 2];
			g = (unsigned char)oImage[i * wImage * 3 + j * 3 + 1];
			b = (unsigned char)oImage[i * wImage * 3 + j * 3 ];
			Y = (int)(r * 0.299 + g * 0.587 + b * 0.114);
			Y = (int)(r * 19595 + g * 38469 + b * 7472) >> 16;
			if (Y > 255) Y = 255;
			nImage[i * wImage + j] = (unsigned char)Y;
		}
	}

}
void RGBToGrayProcessing2(char* oImage, char* nImage, int wImage, int hImage)
{
	int i, j;
	int r, g, b, Y;
	unsigned int m_row_size = 4 * ((3 * wImage + 3) / 4);

	for (i = 0; i < hImage; i++) {
		for (j = 0; j < wImage; j++) {
			int index = ((i * wImage) + j) * ImageColor;
			r = (unsigned char)oImage[index];
			g = (unsigned char)oImage[index + 1];
			b = (unsigned char)oImage[index + 2];
			Y = (int)(r * 0.299 + g * 0.587 + b * 0.114);
			Y = (int)(r * 19595 + g * 38469 + b * 7472) >> 16;
			if (Y > 255) Y = 255;
			nImage[i * wImage + j] = (unsigned char)Y;
		}
	}

}

/************************************************************************************************
*																								*
*   读图像文件																					*
*																								*
************************************************************************************************/
BOOL ReadImage(LPSTR ImageFileName, char* oImage, int wImage, int hImage, int ColorImg) //读取图像信息并保存在Image[][]中
{
	OFSTRUCT of;
	HFILE Image_fp;
	int ImgSize;
	int iFileStartPos = 0;
	int COLORIMAGE = 24;
	ImgSize = 1;
	if (ColorImg == COLORIMAGE) ImgSize = 3;

	const char* pFile = strrchr(ImageFileName, '.'); // 判断输入的文件名最后输出.的位置
	if (pFile != NULL)
	{ // 输入不为空
		if (_strcmpi(pFile, ".bmp") == 0) {
			return(ReadBmpFile(ImageFileName, oImage));
		}

	}
	Image_fp = OpenFile(ImageFileName, &of, OF_READ);
	if (Image_fp == HFILE_ERROR)
	{
		MessageBox(NULL, ImageFileName, "打开文件出错信息", MB_OK);
		return FALSE;
	}

	/*
		if (strcmp(iFileExtension, "bmp") == 0) {
			return(ReadBmpFileHead(Image_fp, oImage));
		}



		if (_llseek(Image_fp, 0, 2) != ImageWidth*ImageHeight)
			DialogBox(hInst, (LPCTSTR)IDD_IMAGEINFORMATIONBOX, hWind, (DLGPROC)DlgImageInformation);

	*/

//jmpReadImage:
	_llseek(Image_fp, iFileStartPos, 0);
	_lread(Image_fp, oImage, ImageWidth * ImageHeight * ImgSize);
	_lclose(Image_fp);


	return TRUE;
}

/************************************************************************************************
*																								*
*   重排图像像素，并且缩小过大图像																					*
*																								*
************************************************************************************************/
void tranImg(char* oImage, char* nImage, int wImage, int hImage,int rad) {
	int i, j;
	int r, g, b, Y;
	unsigned int m_row_size = 4 * ((3 * wImage + 3) / 4);
	unsigned int m_row_size2 = 4 * ((3 * wImage / rad + 3) / 4);

	
	
	for (i = 0; i < hImage-rad; i+=rad) {
		for (j = 0; j < wImage - rad; j+= rad) {
			int index = (hImage - i - 1) * m_row_size + j * ImageColor;
			//int index2 = (hImage / rad  - i / rad - 1) * m_row_size2 + j /rad * ImageColor;
			int index2 = (wImage * i  / (rad * rad) + j / rad) * ImageColor;
			nImage[index2] = oImage[index];
			nImage[index2+1] = oImage[index + 1];
			nImage[index2+2] = oImage[index + 2];
			//SetPixel(hWinDC, j/rad , i /rad, RGB(nImage[index2+2], nImage[index2+1], nImage[index2]));
			
		}
	}
}

/************************************************************************************************
*																								*
*   从BMP图像中，读出图像头信息，主要包括图像长度和宽度											*
*																								*
************************************************************************************************/
BOOL ReadBmpFile(LPSTR ImageFileName, char* oImage)
{
	OFSTRUCT of;
	HFILE Image_fp;
	
	Image_fp = OpenFile(ImageFileName, &of, OF_READ);
	if (Image_fp == HFILE_ERROR)
	{
		MessageBox(NULL, ImageFileName, "打开文件出错信息", MB_OK);
		return FALSE;
	}

	_llseek(Image_fp, 0, 0);
	_lread(Image_fp, &bfImage, sizeof(BITMAPFILEHEADER));

	if ((bfImage.bfType != 0x4d42)) {		// "BM"
		MessageBox(NULL, NULL, "打开bmp文件出错信息", MB_OK);
		return FALSE;
	}

	_lread(Image_fp, &biImage, sizeof(BITMAPINFOHEADER));
	ImageWidth = biImage.biWidth;
	ImageHeight = biImage.biHeight;
	ImageColor = 1;
	if (biImage.biBitCount == 24) ImageColor = 3; 

	_llseek(Image_fp, bfImage.bfOffBits, 0);
	_lread(Image_fp, oImage, biImage.biSizeImage);
	_lclose(Image_fp);


	int rad = 1;
	for (int i = 2; i < 10; i += 2) {
		if (ImageWidth < 1920 && ImageHeight < 1080)
			break;
		if (ImageWidth / i < 1920 && ImageHeight / i < 1080) {
			rad = i;
			break;
		}
	}
	char* nImage = new char[1920 * 1080 * 3]();
	tranImg(oImage, nImage, ImageWidth, ImageHeight, rad);
	ImageWidth = ImageWidth / rad;
	ImageHeight = ImageHeight / rad;
	oImage = nImage;
	
	//ShowBMPImage((unsigned char*)oImage, ImageWidth, ImageHeight, 0, 0);
	/*
	char* nImg = new char[ImageWidth * ImageHeight];

	RGBToGrayProcessing2(oImage, nImg, ImageWidth, ImageHeight);
	
	unsigned char* eImg = new unsigned char[ImageWidth * ImageHeight];

	unsigned char** edge = &eImg;

	canny((unsigned char*)nImg, ImageWidth, ImageHeight, edge);
	
	//ShowBMPImage((unsigned char*)oImage, ImageWidth, ImageHeight, 0, 0);
	ShowImage(*edge, ImageWidth, ImageHeight, 1000, 0);
	return TRUE;
	*/
	unsigned char* gray= RGBToHSV(oImage, ImageWidth, ImageHeight);
	//ShowImage(gray, ImageWidth, ImageHeight, 500, 0);
	//int 
	erosion(gray, ImageWidth, ImageHeight, 1, 1);
	dilation(gray, ImageWidth, ImageHeight, 2, 1);

	//erosion(gray, ImageWidth, ImageHeight, 2, 2);
	//dilation(gray, ImageWidth, ImageHeight, 2, 2);

	dilation(gray, ImageWidth, ImageHeight,2,2);
	erosion(gray, ImageWidth, ImageHeight, 2, 2);

	int ww = ImageWidth / 50 > 12 ? ImageWidth / 50 : 12;
	int hh = ImageHeight / 100 > 4 ? ImageHeight / 100 : 4;
	dilation(gray, ImageWidth, ImageHeight, ww, hh);
	erosion(gray, ImageWidth, ImageHeight, ww, hh);
	//dilation(gray, ImageWidth, ImageHeight, ww/2, hh);
	//erosion(gray, ImageWidth, ImageHeight, ww/2, hh);
	//erosion(gray, ImageWidth, ImageHeight, 1, 1);
	//ShowImage(gray, ImageWidth, ImageHeight, 0, 0);
	//ShowImage(gray, ImageWidth, ImageHeight, ImageWidth/-3, ImageHeight / -2);
	

	int *rect = make_mask(gray, ImageWidth, ImageHeight);
	
	//ShowBMPImage((unsigned char*)oImage, ImageWidth, ImageHeight, 0, 0);
	//showLicense((unsigned char*)oImage, ImageWidth, ImageHeight, rect);
	
	
	
	
	char* license = cut_license(rect, oImage);

	
	int h = rect[3] - rect[1], w = rect[2] - rect[0];

	//ShowBMPImage((unsigned char*)license, w, h, 300, 300);


	char* license_gray = new char[h * w]();
	
	RGBToGrayProcessing(license, license_gray, w, h);

	//ShowImage((unsigned char*)license_gray, w, h, 0, 0);

	OTSU((unsigned char*)license_gray, h, w);

	//Otsu((unsigned char*)license_gray, h, w);
	

	//ShowImage((unsigned char*)license_gray, w, h, 0, 0);
	
	//dilation((unsigned char*)license_gray, w, h, 1, 1);
	//erosion((unsigned char*)license_gray, w, h, 1, 1);


	unsigned char * new_img =  remove_upanddown_border((unsigned char*)license_gray,rect);
	
	h = rect[3] - rect[1];
	w = rect[2] - rect[0];
	/*
	//char* license = new char[h * w * ImageColor]();
	//prepare data for write
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int index = ((rect[1] + i) * ImageWidth + (rect[0] + j)) * ImageColor;
			int index2 = (i * w + j) * 3;
			license[index2] = oImage[index];
			license[index2 + 1] = oImage[index + 1];
			license[index2 + 2] = oImage[index + 2];
			//SetPixel(hWinDC, j, i, RGB(license[index2 + 2], license[index2 + 1], license[index2]));
		}
	}

	RGBToGrayProcessing(license, license_gray, w, h);

	OTSU((unsigned char*)license_gray, h, w);
	*/

	ShowImage((unsigned char*)new_img, w, h, 0, 0);

	char_segmentation(new_img, w, h,rect,oImage);

	

	//Rotate(license, -30, h, w);

	

	//WriteBMPImage("F:\\detection\\data\\ww.bmp", license, w, h, bfImage, biImage);
	//paint border for license
	
	return TRUE;
}







void ShowBMPImage(unsigned char* Image, int wImage, int hImage, int xPos, int yPos)
{
	int i, j;
	int r, g, b;
	for (i = 0; i < hImage; i++)
	{
		for (j = 0; j < wImage; j++)
		{
			int index = (i * wImage + j) * ImageColor;
			r = Image[index + 2];
			g = Image[index + 1];
			b = Image[index + 0];
			SetPixel(hWinDC, j + xPos, i + yPos, RGB(r, g, b));
		}

	}
}


char* cut_license(int* rect ,char* oImage) {
	int h = rect[3] - rect[1], w = rect[2] - rect[0];
	float* rows_count = new float[h]();
	float* cols_count = new float[w]();
	long point_sum = 0;
	int min_h = 999;
	int max_h = -1;
	int max_w = -1, min_w = 999;
	
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			//int index = (ImageHeight - rect[1] - i - 1) * m_row_size1 + (rect[0] + j) * ImageColor;
			int index = ((rect[1] + i) * ImageWidth + (rect[0] + j)) * ImageColor;
			if (judgeBlue(oImage[index + 2], oImage[index + 1], oImage[index], Avg_s,Avg_v)[0]!=0) {
				rows_count[i] += 1;
				cols_count[j] += 1;
				point_sum += 1;
				max_h = max(i, max_h);
				min_h = min(i, min_h);
				max_w = max(j, max_w);
				min_w = min(j, min_w);
				
			}
		}
	}
	//int rows_avg = point_sum / h;
	int cols_avg = point_sum / w /2;
	int flag = min_w;
	for (int i = 0; i < w-1; i++) {
		if (min_w==0 ) {
			if ( (cols_count[i + 1] - cols_count[i]) / (cols_count[i]+1) > 1 )
				if( (float) (w - i) / w > 0.9 && cols_count[i+1] > cols_avg)
					min_w = max(min_w,i+1);
		}
		/*
		else {
			if ((cols_count[i] - cols_count[i + 1]) / cols_count[i + 1] > 0.5 &&(float) (w - i) / w < 0.05 && cols_count[i] > cols_avg) {
				max_w = min(max_w,i);
				break;
			}
		}
		*/
	}
	for (int i = 0; i < h - 1; i++) {
		if (min_h == 0) {
			if ((rows_count[i + 1] - rows_count[i]) / rows_count[i] > 0.5 && (float)(h - i) / h > 0.8)
				min_h = max(i+1,min_h);
		}
		else {
			if ((rows_count[i] - rows_count[i + 1]) / rows_count[i + 1] > 0.5 && (float)(h - i) / h < 0.2) {
				max_h = min(i, max_h);
				break;
			}	
		}
	}
	int a = max_h;
	rect[0] =  rect[0]+min_w ;
	rect[1] =  rect[1]+min_h ;
	rect[2] =  rect[0]+max_w - min_w;
	rect[3] =  rect[1]+max_h - min_h ;
	h = rect[3] - rect[1];
	w = rect[2] - rect[0];
	
	char* license = new char[h * w * ImageColor]();
	//prepare data for write
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			//int index = (ImageHeight - rect[1] - i - 1) * m_row_size1 + (rect[0] + j) * ImageColor;
			int index = ((rect[1] + i) * ImageWidth + (rect[0] + j)) * ImageColor;
			// int index2 = (h - i - 1) * m_row_size2 + j * ImageColor;
			int index2 = (i * w + j) * 3;
			license[index2] = oImage[index];
			license[index2 + 1] = oImage[index + 1];
			license[index2 + 2] = oImage[index + 2];
			//SetPixel(hWinDC, j, i, RGB(license[index2 + 2], license[index2 + 1], license[index2]));
		}
	}
	return license;
}



vector<pair<int, int>> find_waves(double threshold, int* histogram, int len) {
	""" 根据设定的阈值和图片直方图，找出波峰，用于分隔字符 """;
	int	up_point = -1;  // 上升点
	bool is_peak = false;
	if (histogram[0] > threshold) {
		up_point = 0;
		is_peak = true;
	}
	vector<pair<int,int>> wave_peaks(10);

	for (int i = 0; i < len; i++) {
		if (is_peak && histogram[i] < threshold) {
			if (i - up_point > 2) {
				is_peak = false;
				wave_peaks.push_back(pair<int, int>(up_point, i));
			}
		}
		else if (!is_peak && histogram[i] >= threshold) {
			is_peak = true;
			up_point = i;
				
		}
	}
	if (is_peak && up_point != -1 && len - 1 - up_point > 4)
		wave_peaks.push_back(pair<int, int>(up_point, len - 1));
	return wave_peaks;
}

unsigned char* remove_upanddown_border(unsigned char* img,int* rect) {
	int h = rect[3] - rect[1];
	int w = rect[2] - rect[0];
	int* row_histogram = new int[h]();
	int row_min = w * 255;
	int row_sum = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (img[i * w + j] != 0)
			row_histogram[i] += 1;
		}
		row_min = min(row_min, row_histogram[i]);
		row_sum += row_histogram[i];
	}
	double row_average = row_sum / h;
	double	row_threshold = (row_min + row_average) / 2;
	vector<pair<int, int>> wave_peaks = find_waves(row_threshold, row_histogram,h);
	//挑选跨度最大的波峰
	int	wave_span = 0;
	pair<int, int> selected_wave;
	for (int i = 0; i < wave_peaks.size(); i++) {
		pair<int, int> wave_peak = wave_peaks[i];
		int span = wave_peak.second - wave_peak.first;
		if (span > wave_span) {
			wave_span = span;
			selected_wave = wave_peak;
		}
	}
	int new_h = (selected_wave.second - selected_wave.first);
	unsigned char* new_img = new unsigned char[w * new_h]();
	for (int j = 0; j < w; j++) {
		for (int i = 0; i < new_h; i++) {
			new_img[i * w + j] = img[(i+ selected_wave.first)*w+j];
		}
	}
	rect[1] += selected_wave.first;
	rect[3] -= h - new_h - selected_wave.first;
	return new_img;
	
	//char_segmentation(new_img, w, new_h);
	
	
		
}

void my_reszie(unsigned char* img, int ow, int oh, int nw, int nh,int xPos) {
	unsigned char* new_img = new unsigned char[nw * nh * 3];
	double w_ratio = (double)ow / nw;
	double h_ratio = (double)oh / nh;
	for (int i = 0; i < nw; ++i)
	{
		int sx = floor(i * w_ratio);
		sx = min(sx, ow - 1);
		for (int j = 0; j < nh; ++j)
		{
			int sy = floor(j * h_ratio);
			sy = min(sy, oh - 1);
			//重排成BMP格式，满足对齐所以无需计算row_size
			int index = ((nh - j - 1) * nw + i) * 3;
			new_img[index ] = img[ow * sy + sx];
			new_img[index  +1] = img[ow * sy + sx];
			new_img[index  +2] = img[ow * sy + sx];

		}
	}
	/*
	for (int j = 0; j < nh; j++) {
		for (int k = 0; k < nw; k++) {
			BYTE coff = (BYTE)new_img[j * nw + k];
			SetPixel(hWinDC, 100+k, 100+ j, RGB(coff, coff, coff));
		}
	}
	*/
	
	char name[30];
	sprintf(name, "%d.bmp", xPos);
	//ShowBMPImage(new_img, nw, nh, xPos, 100);
	WriteBMPImage(name, (char*)new_img, nw, nh);

}

void char_segmentation(unsigned char* img, int w, int h,int* rect,char* oImage) {
	int* col_histogram = new int[w]();
	int col_min = h;
	int col_sum = 0;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			if(img[j * w + i] == 255)
				col_histogram[i] += 1;
		}
		col_min = min(col_min, col_histogram[i]);
		col_sum += col_histogram[i];
	}
	double start_avg = 0;
	for (int i = 0; i < w/7; i++) {
		start_avg += col_histogram[i];
	}
	start_avg = start_avg / w * 7 / 2.5;
	double col_average = col_sum / w;
	double col_threshold = (col_min + col_average) / 3.5;
	double col_start_threshold = (col_min + col_average) / 2.5;
	if (col_start_threshold > start_avg)
		col_start_threshold = start_avg;
	int tmp = 0, new_peak = 0, maxBlock = 0,flag=-1;
	vector<int*> zifuBlock;
	int jiange = w / 50;
	
	for (int i = 0; i < w; i++) {
		if (col_histogram[i] > col_threshold && new_peak != 0) {
			tmp += col_histogram[i];
			flag = -1;
		}
		else if (col_histogram[i] > col_start_threshold && new_peak == 0) {
			flag = -1;
			tmp = col_histogram[i];
			new_peak = i;
		}
		else if (tmp != 0) {
			if (flag == -1 && (i-new_peak > jiange )) {
				flag = i;
				continue;
			}
			else if (i - flag <jiange ) {
				continue;
			}
			else {
				flag = -1;
			}
			if(i - new_peak > w/10)
				jiange = w / 70;
			col_start_threshold = (col_min + col_average) / 3.5;
			col_threshold = (col_min + col_average) / 4.5;
			maxBlock = max(tmp, maxBlock);
			int* t = new int[]{ tmp, new_peak, i };
			zifuBlock.push_back(t);
			tmp = 0;
			new_peak = 0;
		}
	}
	if (tmp != 0) {
		maxBlock = max(tmp, maxBlock);
		int* t = new int[] { tmp, new_peak, w };
		zifuBlock.push_back(t);
	}
	vector<int*> licence_block;
	double avg_w = 0;
	for (int i = 0; i < zifuBlock.size(); i++) {
		//排除噪点 ，选用的参照标准为大于数字1的面积
		if (zifuBlock[i][0] > 0.6 * h * (w/40)) {
			//排除左边缘
			if (licence_block.size() == 0 && zifuBlock[i][0] < 0.3 * maxBlock)
				continue;
			licence_block.push_back(zifuBlock[i]);
			avg_w += zifuBlock[i][2] - zifuBlock[i][1];
		}
		if (licence_block.size() == 7)
			break;
	}
	avg_w = avg_w / 7;
	/*
	for (int i = 0; i < licence_block.size(); i++) {
		int* block = licence_block[i];
		for (int j = 0; j < h; j++) {
			for (int k = block[1]; k < block[2]; k++) {
				BYTE coff = (BYTE)img[j * w + k];
				SetPixel(hWinDC, k , j , RGB(coff, coff, coff));
			}
		}
	}
	*/
	py_init();
	for (int i = 1; i < licence_block.size(); i++) {
		int* block = licence_block[i];
		int ow = block[2] - block[1];
		int nw = ow;
		//如果为1，则向两边扩张
		if (nw < avg_w/1.5) {
			nw = avg_w;
			nw += ow / 4;
		}else
			nw += ow/4;
		int nh = h + h/6;

		unsigned char* cur_block = new unsigned char[nh * nw * 3]();
		unsigned char* first_block = new unsigned char[nh * nw]();
		int h1 = (nh - h) / 2, h2 = nh - h1 - (nh - h) % 2;
		int w1 = (nw - ow) / 2 + (nw - ow) % 2, w2 = nw - w1;
		for (int j = h1; j < h2; j++) {
			for (int k = w1; k < w2; k++) {
				first_block[j * nw + k] = img[(j - h1) * w + block[1] + k - w1];
				//cur_block[(j * nw + k) * 3] = oImage[((j - h1 + rect[1]) * ImageWidth + rect[0] + block[1] + k - w1) * 3];
				//cur_block[(j * nw + k) * 3 + 1] = oImage[((j - h1 + rect[1]) * ImageWidth + rect[0] + block[1] + k - w1) * 3 +1];
				//cur_block[(j * nw + k) * 3 + 2] = oImage[((j - h1 + rect[1]) * ImageWidth + rect[0] + block[1] + k - w1) * 3 + 2];
			}
		}
		//ShowBMPImage(cur_block, nw, nh, 0, 0);
		//RGBToGrayProcessing((char*)cur_block, (char*)first_block, nh, nw);

		unsigned char* out = new unsigned char[32 * 40]();
		resize(nh, nw, 40, 32, first_block, out);
		my_reszie(out, 32, 40, 32, 40, i * 40);
		ShowImage(out, 32, 40, i * 40, 100);
		char* num = svm((char*)out);
		TextOut(hWinDC, i*20,500, num, 1);
		
		//my_reszie(first_block, nw, nh, 32, 40, i * 40);
	}
	//py_finalize();
	//for ch
	int tar_width = 20;
	int tar_heigth = 32;
	int* block = licence_block[0];
	int ow = block[2] - block[1];
	int nw = ow;
	//如果为1，则向两边扩张
	if (nw < tar_width)
		nw = tar_width;
	else
		nw += ow/4;
	int nh = h;
	if (nh <= tar_heigth)
		nh = tar_heigth;
	else
		nh += nh/8;
	unsigned char* first_block = new unsigned char[nh * nw]();
	int h1 = (nh - h) / 2, h2 = nh - h1 - (nh - h) % 2;
	int w1 = (nw - ow) / 2 + (nw - ow) % 2, w2 = nw - w1 ;
	for (int j = h1; j < h2; j++) {
		for (int k = w1; k < w2; k++) {
			first_block[j * nw + k] = img[(j - h1) * w + block[1] + k - w1];
		}
	}
	
	unsigned char* out = new unsigned char[32 * 40]();
	resize(nh, nw, 32, 20, first_block, out);
	my_reszie(out, 20, 32, 20, 32,0);
	ShowImage(first_block, nw, nh, 0, 100);
	char* ch = ch_svm((char*)out);
	TextOut(hWinDC, 0, 500, ch, 2);
	//my_reszie(first_block, nw, nh, 20, 32, 0 * 40);
	
	
}
