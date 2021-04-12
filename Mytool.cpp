#include "Mytool.h"
#include "StdAfx.h"



int* judgeBlue(unsigned char r, unsigned char g, unsigned char b, int ss, int vv) {
	static double  H, S, V, Cmax, Cmin, delta;
	Cmax = (double)max(max(r, g), b) / 255;
	Cmin = (double)min(min(r, g), b) / 255;
	delta = Cmax - Cmin;
	static int* res = new int[2]();
	if (delta == 0)
		H = 0;
	else if (Cmax == (double)r / 255)
		H = ((double)(g - b) / delta / 255) * 60;
	else if (Cmax == (double)g / 255)
		H = (((double)(b - r) / delta / 255) + 2) * 60;
	else
		H = (((double)(r - g) / delta / 255) + 4) * 60;
	if (Cmax == 0)
		S = 0;
	else
		S = (delta / Cmax);
	V = Cmax;
	if (H < 0)
		H += 360;
	if (H < 0 || S < 0 || V < 0)
		r = 0;
	unsigned char h = H / 2, s = S * 255, v = V * 255;
	//Image[index + 2] = h;
	//Image[index + 1] = s;
	//Image[index] = v;
	res[0] = s;
	res[1] = v;
	if (h >= 100 && h <= 124 && s >= ss && v >= vv)
		return res;
	res[0] = 0;
	res[1] = 0;
	return res;
}

/************************************************************************************************
*																								*
*   将彩色图像变为灰度图像																		*
*																								*
************************************************************************************************/

unsigned char* RGBToHSV(char* Image, int wImage, int hImage) {
	int i, j;
	unsigned char r, g, b;
	//每行宽度
	unsigned int m_row_size = 4 * ((ImageColor * wImage + 3) / 4);
	//二值化的蓝色区域图形
	unsigned char* oImage = new unsigned char[wImage * hImage]();
	long s_sum = 0, v_sum = 0;
	int count = 0;
	int min_s = 200, min_v = 200;
	int* s;
	for (i = 0; i < hImage; i++) {
		for (j = 0; j < wImage; j++) {
			//int index = (hImage - i - 1) * m_row_size + j * ImageColor;
			int index = (i * wImage + j) * ImageColor;
			r = (BYTE)Image[index + 2];
			g = (BYTE)Image[index + 1];
			b = (BYTE)Image[index];
			s = judgeBlue(r, g, b, 105, 50);
			if (s[0] > 0) {
				s_sum += s[0];
				v_sum += s[1];
				min_s = min(min_s, s[0]);
				min_v = min(min_v, s[1]);
				count++;
			}
		}
	}
	Avg_s = ((s_sum / count) + min_s) / 2;
	Avg_v = ((v_sum / count) + min_v) / 2;
	//Avg_s = 100;
	//Avg_v = 50;
	for (i = 0; i < hImage; i++) {
		for (j = 0; j < wImage; j++) {
			//int index = (hImage - i - 1) * m_row_size + j * ImageColor;
			int index = (i * wImage + j) * ImageColor;
			r = (BYTE)Image[index + 2];
			g = (BYTE)Image[index + 1];
			b = (BYTE)Image[index];
			s = judgeBlue(r, g, b, Avg_s, Avg_v);
			if (s[0] > 0)
				oImage[i * wImage + j] = 255;
		}
	}

	return oImage;
}


int* make_mask(unsigned char* data, long width, long height) {
	int nr = height, nc = width;
	int index = 0;
	long areas = 0, max_w, min_w, max_h, min_h;
	int* rect = new int[4];
	double wucha2 = 100;
	for (int r = 0; r < nr; ++r) {
		for (int c = 0; c < nc; ++c) {
			if (data[r * width + c] == 255) {
				index += 10;
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
					if (row - 1 >= 0 && data[(row - 1) * width + col] == 255) {
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
				//计算该联通区域是否近似为矩形
				double wucha = (double)(max_h - min_h) * (max_w - min_w) / areas;
				if (areas > 100 && areas > 0.005 * width * height  && wucha > 0.7 && wucha < 2) {
					//长宽比判断
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
*   对二值化的图形进行膨胀										*
*																								*
************************************************************************************************/
void dilation(unsigned char* data, int width, int height, int kernel_w, int kernel_h)
{
	int i, j, flag;
	unsigned char* tmpdata = new unsigned char[(long)height * (long)width * sizeof(unsigned char)];
	memcpy(tmpdata, data, (long) height * (long)width * sizeof(unsigned char));
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			flag = 0;
			if (tmpdata[i * width + j] == 255)
			{
				flag = 255;
			}
			else {
				for (int m = i - kernel_h; m < i + 1 + kernel_h; m++)
				{
					if (m < 0 || m >= height)
						continue;
					for (int n = j - kernel_w; n < j + 1 + kernel_w; n++)
					{
						if (n<0 || n>width)
							continue;
						flag = max(tmpdata[(long)m * width + n], flag);

					}
					if (flag==255)
						break;
				}
			}
			data[i * width + j] = flag;

		}
	}
	delete[] tmpdata;

}

void dilation_for_float(float** data, int width, int height, int kernel_w, int kernel_h)
{
	int i, j;
	float flag;
	float* tmpdata = new float[(long)height * (long)width * sizeof(float)];
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			tmpdata[i * width + j] = data[i][j];
		}
	}
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			flag = 0;
			if (tmpdata[i * width + j] == 255)
			{
				flag = 255;
			}
			else {
				for (int m = i - kernel_h; m < i + 1 + kernel_h; m++)
				{
					if (m < 0 || m >= height)
						continue;
					for (int n = j - kernel_w; n < j + 1 + kernel_w; n++)
					{
						if (n<0 || n>width)
							continue;
						flag = max(tmpdata[(long)m * width + n], flag);

					}
					if (flag == 255)
						break;
				}
			}
			data[i][j] = flag;

		}
	}
	delete[] tmpdata;

}

/************************************************************************************************
*																								*
*   对二值化的图形进行腐蚀										*
*																								*
************************************************************************************************/
void erosion(unsigned char* data, int width, int height, int kernel_w, int kernel_h)
{
	int i, j, flag;
	unsigned char* tmpdata = new unsigned char[(long)height * width * sizeof(unsigned char)];
	memcpy(tmpdata, data, (long)height * width * sizeof(unsigned char));
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			flag = 255;
			if (tmpdata[i * width + j] == 0)
			{
				flag = 0;
			}
			else {
				for (int m = i - kernel_h; m < i + 1 + kernel_h; m++)
				{
					if (m < 0 || m >= height)
						continue;
					for (int n = j - kernel_w; n < j + 1 + kernel_w; n++)
					{
						if (n<0 || n > width)
							continue;
						flag = min(tmpdata[(long)m * width + n], flag);
						//if (tmpdata[(long)m * width + n] == 0) {
						//	flag = 0;
						//	break;
						//}
					}
					if (flag == 0)
						break;
				}
			}
			data[i * width + j] = flag;
			/*
			if (flag == 0)
			{
				data[i * width + j] = 0;
			}
			else
			{
				data[i * width + j] = 255;
			}
			*/
		}
	}
	free(tmpdata);
}

void erosion_for_float(float** data, int width, int height, int kernel_w, int kernel_h)
{
	int i, j;
	float flag;
	float* tmpdata = new float[height * width * sizeof(float)];
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			tmpdata[i * width + j] = data[i][j];
		}
	}
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			flag = 255;
			if (tmpdata[i * width + j] == 0)
			{
				flag = 0;
			}
			else {
				for (int m = i - kernel_h; m < i + 1 + kernel_h; m++)
				{
					if (m < 0 || m >= height)
						continue;
					for (int n = j - kernel_w; n < j + 1 + kernel_w; n++)
					{
						if (n<0 || n > width)
							continue;
						flag = min(tmpdata[(long)m * width + n], flag);
						;
					}
					if (flag == 0)
						break;
				}
			}
			data[i][j] = flag;
			
		}
	}
	free(tmpdata);
}
void OTSU(unsigned char* img, int r, int c) {
	const int Grayscale = 256;
	int graynum[Grayscale] = { 0 };
	int thresh;
	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {        //直方图统计
			graynum[img[i * c + j]]++;
		}
	}

	double P[Grayscale] = { 0 };
	double PK[Grayscale] = { 0 };
	double MK[Grayscale] = { 0 };
	double srcpixnum = r * c, sumtmpPK = 0, sumtmpMK = 0;
	for (int i = 0; i < Grayscale; ++i) {
		P[i] = graynum[i] / srcpixnum;   //每个灰度级出现的概率
		PK[i] = sumtmpPK + P[i];         //概率累计和 
		sumtmpPK = PK[i];
		MK[i] = sumtmpMK + i * P[i];       //灰度级的累加均值                                                                                                                                                                                                                                                                                                                                                                                                        
		sumtmpMK = MK[i];
	}

	//计算类间方差
	double Var = 0;
	for (int k = 0; k < Grayscale; ++k) {
		if ((MK[Grayscale - 1] * PK[k] - MK[k]) * (MK[Grayscale - 1] * PK[k] - MK[k]) / (PK[k] * (1 - PK[k])) > Var) {
			Var = (MK[Grayscale - 1] * PK[k] - MK[k]) * (MK[Grayscale - 1] * PK[k] - MK[k]) / (PK[k] * (1 - PK[k]));
			thresh = k;
		}
	}
	//阈值处理
	for (int i = 0; i < r; ++i) {
		for (int j = 0; j < c; ++j) {
			if (img[i * c + j] > thresh + thresh * 0.1)
				img[i * c + j] = 255;
			else
				img[i * c + j] = 0;
			
		}
	}
}

//大津法---二值化
int Otsu(unsigned char* src, int r,int c) {
	const int Grayscale = 256;
	/*
	 *	var:类间方差
	 *	m:灰度级K的累加均值
	 *	Pk:灰度级K的累加频率
	 */
	int  thresh = 0;
	double GrayCount[Grayscale] = { 0 },
		m[Grayscale] = { 0 },
		Pk[Grayscale] = { 0 },
		var = 0, total = (double)r * (double)c;

	for (int i = 0; i < r; i++) {
		for (int j = 0; j < c; j++) 
			GrayCount[src[i * c + j]]++;	//计算0~255各灰度级的像素个数
	}
	GrayCount[0] /= total;
	Pk[0] = GrayCount[0];
	for (int i = 1; i < Grayscale; i++) {
		GrayCount[i] /= total;
		Pk[i] = GrayCount[i - 1] + GrayCount[i];
		m[i] = m[i - 1] + i * GrayCount[i];
		GrayCount[i] = Pk[i];
	}
	//求类间方差最大值和对应的阈值
	for (int i = 0; i < Grayscale; i++) {
		if (Pk[i] == 0 || 1 - Pk[i] == 0)	continue;
		if ((m[Grayscale - 1] * Pk[i] - m[i]) * (m[Grayscale - 1] * Pk[i] - m[i]) /
			(Pk[i] * (1 - Pk[i])) > var) {
			var = (m[Grayscale - 1] * Pk[i] - m[i]) * (m[Grayscale - 1] * Pk[i] - m[i]) / (Pk[i] * (1 - Pk[i]));
			thresh = i;
		}
	}
	/*
	dst = cv::Mat(src.rows, src.cols, CV_8UC1, cv::Scalar(0));
	for (int i = 0; i < r; i++) {
		uchar* ptr_src = src.ptr<uchar>(i);
		uchar* ptr_dst = dst.ptr<uchar>(i);
		for (int j = 0; j < c; j++)
			if (ptr_src[j] > thresh)	ptr_dst[j] = 255;	//大于阈值的像素值赋值255
	}
	*/
	return thresh;
}