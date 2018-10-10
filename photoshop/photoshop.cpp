#include <opencv2/opencv.hpp>  
#include <iostream>  

using namespace cv;
using namespace std;

int main()
{
	Mat img = imread("img1.bmp",0);     //第一帧图片
	medianBlur(img, img, 3);
	Mat img2;                           //第一帧图片的彩色RGB
	cvtColor(img, img2, COLOR_GRAY2BGR);

	Mat nex, nex2;                      //第二帧图片以及彩色RGB
	Mat diffImg;                          //差异帧
	
	int x, y;
	int w, h;
	x = 249, y = 116, w = 14, h = 26;
	//cin >> x >> y >> w >> h;              //第一帧的框取坐标
										  
	
	{										//在第一帧的图中画出示意性红框
		for (int i = x;i < x + w;i++) {
			for (int j = 0;j <= 1;j++) {
				Point p = Point(i, y + h*j - 1);
				circle(img2, p, 0, Scalar(0, 0, 255));
			}
		}
		for (int j = y;j < y + h;j++) {
			for (int i = 0;i <= 1;i++) {
				Point p = Point(i*w + x - 1, j);
				circle(img2, p, 0, Scalar(0, 0, 255));
			}
		}
	}

	
	Mat subimg = img(Rect(x, y, w, h));        //局部图
	MatND histimg;
	const int channels[1] = { 0 };
	const int histSize[1] = { 256 };
	float subranges[2] = { 0,255 };
	const float *ranges[1] = { subranges };
	calcHist(&subimg, 1, channels, Mat(), histimg, 1, histSize, ranges, true, false);           //局部图的灰度直方图
	normalize(histimg, histimg, 0, 1, NORM_MINMAX, -1, Mat());

	nex = imread("img2.bmp", 0);
	medianBlur(nex, nex, 3);
	cvtColor(nex, nex2, COLOR_GRAY2BGR);
	subtract(nex, img, diffImg);                                   //提取差异图
	threshold(diffImg, diffImg, 50.0, 255, THRESH_BINARY);          //二值化

	vector<vector<Point>> cont;                   //二值化后的边界
	vector<Vec4i> hierarchy;
	findContours(diffImg, cont, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int maxsize = 0, conum=0;
	for (int i = 0;i < cont.size();i++) {
		if (cont[i].size() > maxsize) {
			conum = i;
			maxsize = cont[i].size();
		}
	}

	int nexw, nexh, nexx, nexy;
	nexx = img.cols, nexy = img.rows;
	int maxx = 0, maxy = 0;
	for (int i = 0;i < cont[conum].size();i++) {
		Point p = cont[conum].at(i);
		if (p.x > maxx)
			maxx = p.x;
		if (p.x < nexx)
			nexx = p.x;
		if (p.y > maxy)
			maxy = p.y;
		if (p.y  < nexy)
			nexy = p.y;
	}

	nexw = maxx - nexx;
	nexh = maxy - nexy;

	
	Point center = Point((maxx + nexx) / 2, (maxy + nexy) / 2);

	double min = 1000;
	int mw, mh;
	for (int weight = w*0.9;weight <= w*1.2;weight++) {
		for (int height = h*0.9;height <= h*1.2;height++) {               //以中心为中心寻找合适的边框大小
			int cx, cy;
			cx = center.x - weight / 2;
			cy = center.y - height / 2;
			Mat subnex = nex(Rect(cx, cy, weight, height));             //提取局部
			MatND histnex;
			calcHist(&subnex, 1, channels, Mat(), histnex, 1, histSize, ranges, true, false);
			normalize(histnex, histnex, 0, 1, NORM_MINMAX, -1, Mat());
			double base = compareHist(histnex, histimg, 2);             //Bhattacharyya方式对比直方图
			//cout << base << endl;
			if (base < min) {
				mw = weight;
				mh = height;
				min = base;
			}
		}
	}

	{													//在第二张图中重新画出红框
		//mw = w;
		//mh = h;
		int cx = center.x - mw / 2;
		int cy = center.y - mh / 2;
		cout << "新定位边框: " << cx << " " << cy << " " << mw << " " << mh << endl;
		cout << "直方图相似度Bhattacharyya 系数 " << min << endl;
		//cout << mw << mh << cx << cy;
		for (int i = cx;i < cx + mw;i++) {
			for (int j = 0;j <= 1;j++) {
				Point p = Point(i, cy + mh*j - 1);
				circle(nex2, p, 0, Scalar(0, 0, 255));
			}
		}
		for (int j = cy;j < cy + mh;j++) {
			for (int i = 0;i <= 1;i++) {
				Point p = Point(i*mw + cx - 1, j);
				circle(nex2, p, 0, Scalar(0, 0, 255));
			}
		}
	}


	namedWindow("Test1", CV_WINDOW_AUTOSIZE);
	imshow("Test1", img2);
	imwrite("frame1.bmp", img2);

	namedWindow("Test2", CV_WINDOW_AUTOSIZE);
	imshow("Test2", nex2);
	imwrite("frame2.bmp", nex2);

	namedWindow("Test3", CV_WINDOW_AUTOSIZE);
	imshow("Test3", diffImg);
	imwrite("diff.bmp", diffImg);

	//等待键盘任意键按下关闭此窗口  
	waitKey(0);
	return 0;
}