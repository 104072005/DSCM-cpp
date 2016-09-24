
#include<opencv2/opencv.hpp>
#include<iostream>
#include<ctime>
#include<vector>
#include"opencv2/highgui/highgui.hpp"
#include"opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;
using std::vector;
//为窗口标题定义宏
#define WINDOW_NAME1   "【左相机图片】"
#define WINDOW_NAME2 "【右相机图片】"


//全局函数声明部分
void on_MouseHandle(int event, int x, int y, int flags, void *param);
void DrawFilledCircle(Mat &img, Point dot);
void DrawRectangle(Mat &img, Rect box);
void Drawgrid(Mat& image, Rect &rectangle);
void pointsMatch(Mat &img1, Mat &img2, vector<Point2i>&corners);

//全局变量声明部分
Rect g_rectangle ;//定义窗口大小和位置
bool down=false;
Mat srcImage, dstImage;
vector<Point2i>imagecorners;

int main()
{
	g_rectangle = Rect(-1, -1, 0, 0);
	clock_t start, end;
    srcImage = imread("L1.bmp", 0);
   dstImage = imread("L1.bmp", 0);
	namedWindow(WINDOW_NAME1);
	start = clock();
	imshow(WINDOW_NAME1, srcImage);
	setMouseCallback(WINDOW_NAME1, on_MouseHandle,(void*)&srcImage);
	end = clock();
	cout << "Run time:" << (double)(end - start) / CLOCKS_PER_SEC << "S" << endl;
	waitKey(0);
    return 0;
}

//【on_MouseHandle()函数】 
//描述：鼠标回调函数，根据不同的鼠标事件进行不同的操作
//----------------------------------------------------------- 
void on_MouseHandle(int event, int x, int y, int flags, void *param)
{
	Mat &image = *(Mat*)param;
	switch (event)
	{
		//左键按下消息
	case EVENT_LBUTTONDOWN:
		{
			g_rectangle = Rect(x, y, 0, 0);//记录起始点
			down = true;
		}
		break;
		//左键按下时鼠标移动，
	case EVENT_MOUSEMOVE:
		{
			if (down == true)
			g_rectangle.width = x - g_rectangle.x;
			g_rectangle.height = y - g_rectangle.y;
		}
		break;
		//左键抬起消息
	case EVENT_LBUTTONUP:
		{
			//对宽和高小于0的处理
			if (g_rectangle.width < 0)
			{
				g_rectangle.x += g_rectangle.width;
				g_rectangle.width *= -1;
			}
			if (g_rectangle.height < 0)
			{
				g_rectangle.y += g_rectangle.height;
				g_rectangle.height *= -1;
			}
			cout << "g_rectangle=" << g_rectangle.x << " " << g_rectangle.y << " " << g_rectangle.width << " " << g_rectangle.height << endl;
			DrawRectangle(image, g_rectangle);
			Drawgrid(image, g_rectangle);
			pointsMatch(image, dstImage, imagecorners);
			imshow(WINDOW_NAME1, image);
			imshow(WINDOW_NAME2, dstImage);
			down = false;
	}
		break;
	}
}
//-------------------------
//【Drawgrid()函数】
//描述：画网格函数
//--------------------------
void Drawgrid(Mat& image, Rect &rectangle)
{
	int a = 0, b = 0;
	for (int i = rectangle.tl().x+10; i < rectangle.br().x; i += 10){
		line(image, Point(i, rectangle.tl().y), Point(i, rectangle.br ().y), Scalar(0, 255, 0), 1, 8);
		a++;
	}
	for (int i = rectangle.tl().y + 10; i < rectangle.br().y; i += 10)
	{
		line(image, Point(rectangle.tl().x, i), Point(rectangle.br().x, i), Scalar(0, 255, 0), 1, 8);
		b++;
	}
	for (int k = 0; k < b + 1; k++)
	{
		for (int w = 0; w < a + 1; w++)
		{
			if (k %2 == 0)
				imagecorners.push_back(Point2i(rectangle.tl().x + 10 * w, rectangle.tl().y + 10* k));
			else
				imagecorners.push_back(Point2i(rectangle.tl().x + 10* (a-w), rectangle.tl().y + 10* k));
		}
	}
	/*for (int k = 0; k < b + 1; k++)
		for (int w = 0; w < a + 1; w++)
		{
			imagecorners.push_back(Point2i(rectangle.tl().x + 10 * w, rectangle.tl().y + 10 * k));
		}*/
		
}
//--------------------------------------
//【DrawRectangle()函数】
//描述:自定义的矩形绘制函数
//-------------------------------------
void DrawRectangle(Mat &img, Rect box)
{
	rectangle(img, box.tl(), box.br(), Scalar(0, 255,0));//tl()返回左上角点坐标，br()返回右下角点坐标
}
//【DrawFilledCircle()函数】
//描述：实现实心圆的绘制
//-------------------------------------------
void DrawFilledCircle(Mat &img, Point dot)
{
	int thickness = 1;
	int lineType = 8;
	ellipse(img, dot, Size(2, 2), 360, 0, 360, Scalar(255, 129, 0), thickness, lineType);
}
void pointsMatch(Mat &img1, Mat &img2, vector<Point2i>&corners)
{
	for (decltype(corners.size())i = 0; i < corners.size(); i++)
	{
	Mat result;
	double maxValue;
	Point point,best;
	Point center(corners[i].x, corners[i].y);
	Mat imageROI1 = img1(Rect(center.x - 20, center.y - 20, 41, 41));
	DrawFilledCircle(img1, center);
	cout << "center=" << center << endl;
	matchTemplate(img2, imageROI1, result, TM_CCORR_NORMED);
	minMaxLoc(result, NULL, &maxValue, NULL, &best);
	point.x = best.x + 20;
	point.y = best.y + 20;
	DrawFilledCircle(img2, point);
	cout << "相关系数最大值" << maxValue << endl;
	cout << "best=" << point << endl;
	}
}
