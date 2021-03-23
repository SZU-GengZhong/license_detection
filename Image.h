// Image.h

void ShowImage(unsigned char *,int, int, int ,int);			//��ʾͼ��
void ReadImage(LPSTR, char *, int, int);	//��ͼ��ϵ��
void WriteImage(LPSTR , char *, int, int) ;	//дͼ���ļ�
void OpenImageFile(char *);
BOOL ReadBmpFile(LPSTR , char *);
void ShowBMPImage(unsigned char *, int, int, int, int);	
unsigned char* RGBToHSV(char* , int , int );
void dilation(unsigned char*, int, int, int, int);
void erosion(unsigned char*, int, int, int, int);
int* make_mask(unsigned char* data, long width, long height);
char* cut_license(int* rect, char* oImage);
void OTSU(unsigned char* img, int r, int c);
unsigned char* remove_upanddown_border(unsigned char* img, int w, int h);
void char_segmentation(unsigned char* img, int w, int h);
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
    unsigned char rgbBlue; //��ɫ��������
    unsigned char rgbGreen;//��ɫ��������
    unsigned char rgbRed;//��ɫ��������
    unsigned char rgbReserved;
}rgbq;
#pragma pack()

/*					hWinDC = GetDC(hWnd);
					OpenImageFile("��ͼ���ļ�");
					ReadImage(ImgFileName, NewImage, 256, 256);	//��ͼ��ϵ��
					ShowImage(image, 256, 256, 100, 100);			//��ʾͼ��
*/