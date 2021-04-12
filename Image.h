// Image.h

void ShowImage(unsigned char *,int, int, int ,int);			//显示图像
void ReadImage(LPSTR, char *, int, int);	//读图像系数
void WriteImage(LPSTR , char *, int, int) ;	//写图像文件
void OpenImageFile(char *);
BOOL ReadBmpFile(LPSTR , char *);
void ShowBMPImage(unsigned char *, int, int, int, int);	
char* cut_license(int* rect, char* oImage);
unsigned char* remove_upanddown_border(unsigned char* img,int* rect);
void char_segmentation(unsigned char* img, int w, int h,int* rect,char*);
extern HDC hWinDC;
extern HDC mdc;
extern char ImgFileName[512];


#define XPOS			20
#define YPOS			20
#define MAXIMAGEWIDTH	2400
#define MAXIMAGEHEIGHT	2400
#define IMAGEGAP		40

#pragma pack(1)
typedef struct rgbqtag
{
    unsigned char rgbBlue; //蓝色分量亮度
    unsigned char rgbGreen;//绿色分量亮度
    unsigned char rgbRed;//红色分量亮度
    unsigned char rgbReserved;
}rgbq;
#pragma pack()

/*					hWinDC = GetDC(hWnd);
					OpenImageFile("打开图像文件");
					ReadImage(ImgFileName, NewImage, 256, 256);	//读图像系数
					ShowImage(image, 256, 256, 100, 100);			//显示图像
*/