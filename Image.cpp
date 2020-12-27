// Image.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <commdlg.h>
#include <direct.h>
#include <math.h>
#include "Image.h"
#include <queue>
#include <map>
using namespace std;

HDC hWinDC;
HDC mdc;
char ImgFileName[512];
int ImageWidth;
int ImageHeight;
int ImageColor;

/************************************************************************************************
*																								*
*	��ͼ���ļ�����ReadImage(LPSTR ImageFileName, char *Image, int wImage, int hImage)			*
*																								*
*   	��ͼ���ļ�ImageFileName�ж���ͼ����Image[]������										*
*		Image�����ͼ����������																*
*		wImage��ͼ����																		*
*		hImage��ͼ��߶�																		*
*																								*
************************************************************************************************/
void ReadImage(LPSTR ImageFileName, char* Image, int wImage, int hImage) //��ȡͼ����Ϣ��������Image[][]��
{
	OFSTRUCT of;
	HFILE Image_fp;

	Image_fp = OpenFile(ImageFileName, &of, OF_READ);
	if (Image_fp == HFILE_ERROR)
	{
		MessageBox(NULL, "�򿪶�ͼ���ļ�����", "������Ϣ", MB_OK);
	}

	_llseek(Image_fp, 0, 0);
	_lread(Image_fp, Image, wImage * hImage);
	_lclose(Image_fp);

}

/************************************************************************************************
*																								*
*	дͼ���ļ�����WriteImage(LPSTR ImageFileName, char *Image, int wImage, int hImage)			*
*																								*
*   	��ͼ��Image[]д��ͼ���ļ�ImageFileName��												*
*		Image�����ͼ����������																*
*		wImage��ͼ����																		*
*		hImage��ͼ��߶�																		*
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
*	����Ļ����ʾͼ����ShowImage(char *Image, int wImage, int hImage, int xPos, int yPos)		*
*																								*
*   	Image	�����ͼ����������															*
*		wImage	��ͼ����																		*
*		hImage	��ͼ��߶�																		*
*		xPos	��ͼ����ʾ��ʼλ��x����															*
*		yPos	��ͼ����ʾ��ʼλ��y����															*
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

void Rotate( char* src, int angle,int height,int width)
{
	double theta = angle * 3.141592 / 180.0;
	float c = cos(theta);
	float s = sin(theta);

	/* �����е����� */
	int h = (height + 1) / 2;
	int w = (width + 1) / 2;

	/* �������4��������ת���Ӧ������ */
	float x0 = -w * c - h * s;
	float y0 = -h * c + w * s;
	float x1 = w * c - h * s;
	float y1 = -h * c - w * s;
	float x2 = w * c + h * s;
	float y2 = h * c - w * s;
	float x3 = -w * c + h * s;
	float y3 = h * c + w * s;

	/* ������ת���ͼ��ĸ߶ȺͿ�� */
	float width1 = fabs(x2 - x0);
	float width2 = fabs(x3 - x1);
	float height1 = fabs(y2 - y0);
	float height2 = fabs(y3 - y1);


	w = (width1 > width2 ? width1 : width2) ;
	h = (height1 > height2 ? height1 : height2) ;

	/* ����ͼ�����ݻ����� */
	//dst.rows = h;
	//dst.cols = w;
	//dst.Create(dst.rows, dst.cols, 3);
	

	/* ����ԭʼ���������Ӧ��Ŀ������ */
	int di = x0 + w / 2;
	int dj = y0 + h / 2;
	//h = 1.5 * h;
	//w = 1.5 * w;
	char* dst = new char[h * w * 3]();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int x = i * c + j * s + di;
			int y = j * c - i * s + dj;
			int dstOft = (x * w + y) * 3;
			int srcOft = (i * width + j) * 3;

			dst[dstOft + 0] = src[srcOft + 0]; // B
			dst[dstOft + 1] = src[srcOft + 1]; // G
			dst[dstOft + 2] = src[srcOft + 2]; // R
		}
	}
	int i, j;
	int r, g, b;
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			int index =  (i  * w  + j) * ImageColor;
			r = dst[index + 2];
			g = dst[index + 1];
			b = dst[index];
			SetPixel(hWinDC, j , i , RGB(r, g, b));
		}

	}
}
/************************************************************************************************
*																								*
*	��ȡ�ļ�������OpenImageFile(char *OPDLTitle)												*
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

void WriteBMPImage(LPSTR ImageFileName, char* Image, int wImage, int hImage, BITMAPFILEHEADER bfImage, BITMAPINFOHEADER biImage)
{
	OFSTRUCT of;
	HFILE Image_fp;
	/*
	* �Ҷ�δ���
	rgbq* fq = (rgbq*)malloc(256 * sizeof(rgbq));
	for (int i = 0;i < 256;i++)
	{
		 fq[i].rgbBlue = fq[i].rgbGreen = fq[i].rgbRed = i;
		  //fq[i].rgbReserved=0;
	}
	biImage.biBitCount = 8;
	biImage.biSizeImage = biImage.biSizeImage / 3;
	biImage.biSize = 40;
	bfImage.bfOffBits = 54 + 256 * sizeof(rgbq);
	bfImage.bfSize = biImage.biSizeImage + bfImage.bfOffBits;
	*/
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
	//_lwrite(Image_fp, (char*)&fq, 256 * sizeof(rgbq));
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
			r = (unsigned char)oImage[i * wImage * 3 + j * 3];
			g = (unsigned char)oImage[i * wImage * 3 + j * 3 + 1];
			b = (unsigned char)oImage[i * wImage * 3 + j * 3 + 2];
			Y = (int)(r * 0.299 + g * 0.587 + b * 0.114);
			if (Y > 255) Y = 255;

			nImage[i * wImage + j] = (unsigned char)Y;
		}
	}

}

/************************************************************************************************
*																								*
*   ��ͼ���ļ�																					*
*																								*
************************************************************************************************/
BOOL ReadImage(LPSTR ImageFileName, char* oImage, int wImage, int hImage, int ColorImg) //��ȡͼ����Ϣ��������Image[][]��
{
	OFSTRUCT of;
	HFILE Image_fp;
	int ImgSize;
	int iFileStartPos = 0;
	int COLORIMAGE = 24;
	ImgSize = 1;
	if (ColorImg == COLORIMAGE) ImgSize = 3;

	const char* pFile = strrchr(ImageFileName, '.'); // �ж�������ļ���������.��λ��
	if (pFile != NULL)
	{ // ���벻Ϊ��
		if (_strcmpi(pFile, ".bmp") == 0) {
			return(ReadBmpFile(ImageFileName, oImage));
		}

	}
	Image_fp = OpenFile(ImageFileName, &of, OF_READ);
	if (Image_fp == HFILE_ERROR)
	{
		MessageBox(NULL, ImageFileName, "���ļ�������Ϣ", MB_OK);
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


void showLicense(unsigned char* Image, int wImage, int hImage, int* rect) {
	int i, j;
	int r, g, b;
	unsigned int m_row_size = 4 * ((ImageColor * wImage + 3) / 4);
	for ( i = rect[1];i < rect[3];i++) {
		for ( j = rect[0];j < rect[2];j++) {
			int index = (hImage - i - 1) * m_row_size + j * ImageColor;
			r = Image[index + 2];
			g = Image[index + 1];
			b = Image[index];
			SetPixel(hWinDC, j , i , RGB(r, g, b));
		}
	}
}

/************************************************************************************************
*																								*
*   ��BMPͼ���У�����ͼ��ͷ��Ϣ����Ҫ����ͼ�񳤶ȺͿ��											*
*																								*
************************************************************************************************/
BOOL ReadBmpFile(LPSTR ImageFileName, char* oImage)
{
	OFSTRUCT of;
	HFILE Image_fp;
	BITMAPFILEHEADER bfImage;
	BITMAPINFOHEADER biImage;
	Image_fp = OpenFile(ImageFileName, &of, OF_READ);
	if (Image_fp == HFILE_ERROR)
	{
		MessageBox(NULL, ImageFileName, "���ļ�������Ϣ", MB_OK);
		return FALSE;
	}

	_llseek(Image_fp, 0, 0);
	_lread(Image_fp, &bfImage, sizeof(BITMAPFILEHEADER));

	if ((bfImage.bfType != 0x4d42)) {		// "BM"
		MessageBox(NULL, NULL, "��bmp�ļ�������Ϣ", MB_OK);
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

	//ShowBMPImage((unsigned char*)oImage, ImageWidth, ImageHeight, 0, 0);

	unsigned char* gray= RGBToHSV(oImage, ImageWidth, ImageHeight);
	erosion(gray, ImageWidth, ImageHeight,1,1);
	dilation(gray, ImageWidth, ImageHeight,2,2);
	dilation(gray, ImageWidth, ImageHeight,2,2);

	
	int *rect = make_mask(gray, ImageWidth, ImageHeight);
	//ShowImage((char*)gray, ImageWidth, ImageHeight, 0, 0);
	//ShowBMPImage((unsigned char*)oImage, ImageWidth, ImageHeight, 0, 0);
	//showLicense((unsigned char*)oImage, ImageWidth, ImageHeight, rect);
	
	
	int h = rect[3] - rect[1], w = rect[2] - rect[0];
	
	char* license = cut_license(rect, oImage);

	char* license_gray = new char[h * w]();

	//ShowImage(license_gray, w, h, 0, 0);

	RGBToGrayProcessing(license, license_gray, w, h);

	OTSU((unsigned char*)license_gray,h,w);

	unsigned char * new_img =  remove_upanddown_border((unsigned char*)license_gray, w, h);

	char_segmentation(new_img, w, ImageHeight);

	

	//Rotate(license, -30, h, w);

	//ShowBMPImage((unsigned char*)license, w, h, 0, 0);

	//WriteBMPImage("F:\\detection\\data\\ww.bmp", license, w, h, bfImage, biImage);
	//paint border for license
	/*for (int i = rect[0];i <= rect[2];i++) {
		SetPixel(hWinDC, i, rect[1], RGB(0, 0, 255));
		SetPixel(hWinDC, i, rect[3], RGB(0, 0, 255));
	}
	for (int i = rect[1];i <= rect[3];i++) {
		SetPixel(hWinDC, rect[0], i, RGB(0, 0, 255));
		SetPixel(hWinDC, rect[2], i, RGB(0, 0, 255));
	}*/
	return TRUE;
}



/************************************************************************************************
*																								*
*   ����ɫͼ���Ϊ�Ҷ�ͼ��																		*
*																								*
************************************************************************************************/


unsigned char* RGBToHSV(char* Image, int wImage, int hImage) {
	int i, j;
	unsigned char r, g, b;
	double  H, S, V;
	//ÿ�п��
	unsigned int m_row_size = 4 * ((ImageColor * wImage + 3) / 4);
	//��ֵ������ɫ����ͼ��
	unsigned char* oImage = new unsigned char[wImage * hImage];
	memset(oImage, 0, wImage * hImage);
	for (i = 0; i < hImage; i++) {
		for (j = 0; j < wImage; j++) {
			int index = (hImage - i - 1) * m_row_size + j * ImageColor;
			r = (BYTE) Image[index + 2];
			g = (BYTE) Image[index + 1];
			b = (BYTE) Image[index];
			double Cmax = (double) max(max(r, g), b) / 255;
			double Cmin = (double) min(min(r, g), b) / 255;
			double delta = Cmax - Cmin;
			if (delta == 0)
				H = 0;
			else if (Cmax == (double)r / 255)
				H = ((double)(g - b) / delta / 255)  * 60;
			else if (Cmax == (double)g / 255)
				H = (((double)(b - r) / delta / 255) + 2) * 60;
			else 
				H = (((double)(r - g) / delta / 255) + 4) * 60;
			if (Cmax == 0)
				S = 0;
			else
				S = (delta / Cmax);
			V =Cmax;
			if (H < 0)
				H += 360;
			if (H < 0 || S < 0 || V < 0)
				r = 0;
			unsigned char h = H / 2, s = S * 255, v = V * 255;
			//Image[index + 2] = h;
			//Image[index + 1] = s;
			//Image[index] = v;
			if (h >= 100 && h <= 124 && s >= 140 && v >= 70)
				oImage[i * wImage + j] = 255;
			}
		
	}
	return oImage;
}


int* make_mask(unsigned char* data,int width,int height) {
	int nr = height,nc = width;
	int index = 0;
	int areas = 0,max_w,min_w,max_h,min_h;
	int* rect = new int[4];
	double wucha2 = 100;
	for (int r = 0; r < nr; ++r) {
		for (int c = 0; c < nc; ++c) {
			if (data[r*width+c] == 255) {
				index+=10;
				data[r * width + c] = index;
				queue<pair<int, int>> neighbors;
				neighbors.push({ r, c });
				max_w = min_w = c;
				max_h = min_h = r;
				areas = 0;
				while (!neighbors.empty()) {
					auto rc = neighbors.front();
					neighbors.pop();
					int row = rc.first, col = rc.second;
					max_h = max(row, max_h);
					min_h = min(row, min_h);
					max_w = max(col, max_w);
					min_w = min(col, min_w);
					areas++;
					if (row - 1 >= 0 && data[(row - 1)*width + col] == 255) {
						neighbors.push({ row - 1, col });
						data[(row - 1) * width + col] = index;
					}
					if (row + 1 < nr && data[(row + 1) * width + col] == 255) {
						neighbors.push({ row + 1, col });
						data[(row + 1) * width + col] = index;
					}
					if (col - 1 >= 0 && data[(row * width) + col - 1] == 255) {
						neighbors.push({ row, col - 1 });
						data[(row * width) + col - 1] = index;
					}
					if (col + 1 < nc && data[(row * width) + col + 1] == 255) {
						neighbors.push({ row, col + 1 });
						data[(row * width) + col + 1] = index;
					}
				}
				//�������ͨ�����Ƿ����Ϊ����
				double wucha =(double) (max_h - min_h) * (max_w - min_w) / areas;
				if (areas > 200 && wucha > 0.75 && wucha < 1.5) {
					//������ж�
					double tmp = abs((double)(max_w - min_w) / (max_h - min_h) - 3.14);
					if (tmp < wucha2) {
						rect[0] = min_w;
						rect[1] = min_h;
						rect[2] = max_w;
						rect[3] = max_h;
						wucha2 = tmp;
					}
				}
				
			}
		}
	}
	return rect;
	
}

/************************************************************************************************
*																								*
*   �Զ�ֵ����ͼ�ν�������										*
*																								*
************************************************************************************************/
void dilation(unsigned char* data, int width, int height,int kernel_w,int kernel_h)
{
	int i, j, flag;
	unsigned char* tmpdata = new unsigned char[height * width * sizeof(unsigned char)];
	memcpy(tmpdata, data, height * width * sizeof(unsigned char));
	for (i = 0;i < height ;i++)
	{
		for (j = 0;j < width ;j++)
		{
			flag = 1;
			if (tmpdata[i * width + j] == 255)
			{
				flag = 0;
			}
			else {
				for (int m = i - kernel_h;m < i + 1 + kernel_h;m++)
				{
					if (m < 0 || m >= height)
						continue;
					for (int n = j - kernel_w; n < j + 1 + kernel_w;n++)
					{
						if (n<0 || n>width)
							continue;
						if (tmpdata[m * width + n] == 255) {
							flag = 0;
							break;
						}
					}
					if (flag)
						break;
				}
			}
			if (flag == 0)
			{
				data[i * width + j] = 255;
			}
			else
			{
				data[i * width + j] = 0;
			}
		}
	}
	delete[] tmpdata;
	
}

/************************************************************************************************
*																								*
*   �Զ�ֵ����ͼ�ν��и�ʴ										*
*																								*
************************************************************************************************/
void erosion(unsigned char* data, int width, int height, int kernel_w, int kernel_h)
{
	int i, j, flag;
	unsigned char* tmpdata = new unsigned char[height * width * sizeof(unsigned char)];
	memcpy(tmpdata, data, height * width * sizeof(unsigned char));
	for (i = 0;i < height;i++)
	{
		for (j = 0;j < width;j++)
		{
			flag = 1;
			if (tmpdata[i * width + j] == 0)
			{
				flag = 0;
			}
			else {
				for (int m = i - kernel_h;m < i + 1 + kernel_h;m++)
				{
					if (m < 0 || m >= height)
						continue;
					for (int n = j - kernel_w; n < j + 1 + kernel_w;n++)
					{
						if (n<0 || n>width)
							continue;
						if (tmpdata[m * width + n] == 0) {
							flag = 0;
							break;
						}
					}
					if (flag==0)
						break;
				}
			}
			if (flag == 0)
			{
				data[i * width + j] = 0;
			}
			else
			{
				data[i * width + j] = 255;
			}
		}
	}
	delete[] tmpdata;
}


void ShowBMPImage(unsigned char* Image, int wImage, int hImage, int xPos, int yPos)
{
	int i, j;
	int r, g, b;
	unsigned int m_row_size = 4 * ((ImageColor * wImage + 3) / 4);
	for (i = 0; i < hImage; i++)
	{
		for (j = 0; j < wImage; j++)
		{
			int index = (hImage - i - 1) * m_row_size + j * ImageColor;
			r = Image[index + 2];
			g = Image[index + 1];
			b = Image[index];
			SetPixel(hWinDC, j + xPos, i + yPos, RGB(r, g, b));
		}

	}
}

char* cut_license(int* rect ,char* oImage) {
	int h = rect[3] - rect[1], w = rect[2] - rect[0];
	unsigned int m_row_size1 = 4 * ((ImageColor * ImageWidth + 3) / 4);
	unsigned int m_row_size2 = 4 * ((ImageColor * w + 3) / 4);
	char* license = new char[h * m_row_size2 * ImageColor]();
	//prepare data for write
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int index = (ImageHeight - rect[1] - i - 1) * m_row_size1 + (rect[0] + j) * ImageColor;
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

void OTSU(unsigned char* img, int r ,int c) {
	const int Grayscale = 256;
	int graynum[Grayscale] = { 0 };
	int thresh;
	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {        //ֱ��ͼͳ��
			graynum[img[i * c + j]]++;
		}
	}

	double P[Grayscale] = { 0 };
	double PK[Grayscale] = { 0 };
	double MK[Grayscale] = { 0 };
	double srcpixnum = r * c, sumtmpPK = 0, sumtmpMK = 0;
	for (int i = 0; i < Grayscale; ++i) {
		P[i] = graynum[i] / srcpixnum;   //ÿ���Ҷȼ����ֵĸ���
		PK[i] = sumtmpPK + P[i];         //�����ۼƺ� 
		sumtmpPK = PK[i];
		MK[i] = sumtmpMK + i * P[i];       //�Ҷȼ����ۼӾ�ֵ                                                                                                                                                                                                                                                                                                                                                                                                        
		sumtmpMK = MK[i];
	}

	//������䷽��
	double Var = 0;
	for (int k = 0; k < Grayscale; ++k) {
		if ((MK[Grayscale - 1] * PK[k] - MK[k]) * (MK[Grayscale - 1] * PK[k] - MK[k]) / (PK[k] * (1 - PK[k])) > Var) {
			Var = (MK[Grayscale - 1] * PK[k] - MK[k]) * (MK[Grayscale - 1] * PK[k] - MK[k]) / (PK[k] * (1 - PK[k]));
			thresh = k;
		}
	}

	//��ֵ����
	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			if (img[i * c + j] > thresh)
				img[i * c + j] = 255;
			else
				img[i * c + j] = 0;
		}
	}
}

vector<pair<int, int>> find_waves(double threshold, int* histogram, int len) {
	""" �����趨����ֵ��ͼƬֱ��ͼ���ҳ����壬���ڷָ��ַ� """;
	int	up_point = -1;  // ������
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
unsigned char* remove_upanddown_border(unsigned char* img,int w,int h) {
	int* row_histogram = new int[h]();
	int row_min = w * 255;
	int row_sum = 0;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			row_histogram[i] += img[i * w + j];
		}
		row_min = min(row_min, row_histogram[i]);
		row_sum += row_histogram[i];
	}
	double row_average = row_sum / h;
	double	row_threshold = (row_min + row_average) / 2;
	vector<pair<int, int>> wave_peaks = find_waves(row_threshold, row_histogram,h);
	//��ѡ������Ĳ���
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
	/*
	for (int j = 0; j < w; j++){
		for (int i = 0; i < selected_wave.first; i++) {
			img[i * w + j] = 0;
		}
		for (int i = selected_wave.second; i < h; i++) {
			img[i * w + j] = 0;
		}
	}
	*/
	int new_h = (selected_wave.second - selected_wave.first);
	unsigned char* new_img = new unsigned char[w * new_h]();
	for (int j = 0; j < w; j++) {
		for (int i = 0; i < new_h; i++) {
			new_img[i * w + j] = img[(i+ selected_wave.first)*w+j];
		}
	}
	ImageHeight = new_h;
	return new_img;
	
	//char_segmentation(new_img, w, new_h);
	
	
		
}
void my_reszie(unsigned char* img, int ow, int oh, int nw, int nh) {
	unsigned char* new_img = new unsigned char[nw * nh];
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
			new_img[nw * j + i] = img[ow * sy + sx];
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
	ShowImage(new_img, nw, nh, 100, 100);

}
void char_segmentation(unsigned char* img, int w, int h) {
	int* col_histogram = new int[w]();
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if(img[i * w + j] == 255)
				col_histogram[j] += 1;
		}
	}
	int tmp = 0, new_peak = 0, maxBlock = 0;
	vector<int*>	zifuBlock;
	for (int i = 2; i < w ; i++) {
		if (col_histogram[i] != 0 && new_peak != 0)
			tmp += col_histogram[i];
		else if (col_histogram[i] != 0 && new_peak == 0) {
			tmp = col_histogram[i];
			new_peak = i;
		}
		else if (tmp != 0) {
			maxBlock = max(tmp, maxBlock);
			int* t = new int[]{ tmp, new_peak, i };
			zifuBlock.push_back(t);
			tmp = 0;
			new_peak = 0;
		}
	}
	vector<int*> licence_block;
	for (int i = 0; i < zifuBlock.size(); i++) {
		if (zifuBlock[i][0] > 0.05 * maxBlock)
			licence_block.push_back(zifuBlock[i]);
		if (licence_block.size() == 7)
			break;
	}
	
	for (int i = 0; i < licence_block.size(); i++) {
		int* block = licence_block[i];
		for (int j = 0; j < h; j++) {
			for (int k = block[1]; k < block[2]; k++) {
				BYTE coff = (BYTE)img[j * w + k];
				SetPixel(hWinDC, k , j , RGB(coff, coff, coff));
			}
		}
	}
	
	int* block = licence_block[0];
	int nw = block[2] - block[1];
	unsigned char* first_block = new unsigned char[h * nw]();
	for (int j = 0; j < h; j++) {
		for (int k = 0; k < nw; k++) {
			first_block[j * nw + k] = img[j * w + block[1] + k];
		}
	}
	my_reszie(first_block, nw, h, 32, 40);
}
