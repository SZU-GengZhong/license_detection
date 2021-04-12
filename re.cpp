#include "re.h";
#include "StdAfx.h"
using namespace std;

struct point {
	double x, y;
};
int in_range(int it, int min, int max) {
	it = it < min ? min : it;
	return it > max ? max : it;
}
void resize(int srcH, int srcW, int dstH, int dstW, unsigned char src[], unsigned char dst[]) {
	point sO, dO, S, D;
	sO.x = srcW / 2.0; sO.y = srcH / 2.0;
	dO.x = dstW / 2.0; dO.y = dstH / 2.0;//原点计算

	double sigmaX = srcW * 1.0 / dstW, sigmaY = srcH * 1.0 / dstH;

	for (int i = 0; i < dstH; i++) {
		for (int j = 0; j < dstW; j++) {
			D.x = j - dO.x;
			D.y = i - dO.y;

			S.x = D.x * sigmaX + sO.x;
			S.y = D.y * sigmaY + sO.y;

			int x1, x2, y1, y2;
			if (S.x > 0) {
				x1 = (int)(S.x); x2 = x1 + 1;
			}
			else {
				x1 = (int)(S.x) - 1; x2 = x1 + 1;
			}
			if (S.y > 0) {
				y1 = (int)(S.y); y2 = y1 + 1;
			}
			else {
				y1 = (int)(S.y) - 1; y2 = y1 + 1;
			}
			//对应点的确定
			x1 = in_range(x1, 0, srcW - 1);
			x2 = in_range(x2, 0, srcW - 1);
			y1 = in_range(y1, 0, srcH - 1);
			y2 = in_range(y2, 0, srcH - 1);//避免出界
			double Q1, Q2, Q;
			if (x1 == x2) {
				Q1 = src[y1 * srcW + x1];
				Q2 = src[y2 * srcW + x1];
			}
			else {
				Q1 = (x2 - S.x) * src[y1 * srcW + x1] + (S.x - x1) * src[y1 * srcW + x2];
				Q2 = (x2 - S.x) * src[y2 * srcW + x1] + (S.x - x1) * src[y2 * srcW + x2];
			}
			if (y1 == y2)Q = Q1;
			else
				Q = (y2 - S.y) * Q1 + (S.y - y1) * Q2;//结果的计算

			dst[i * dstW + j] = int(Q) < 127 ? 0 :255;
		}
	}
}
