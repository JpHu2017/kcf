/**
* @Copyright (c) 2018 by JpHu
* @Date 2018-5-16
* @Company CuiZhou
* @Email hujp@cuizhouai.com
* @Function
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "kcftracker.hpp"

using namespace std;
using namespace cv;

std::vector<cv::Point> draw_polygon;
std::vector<cv::Point> final_polygon;
bool drawing_poly = false;
bool gotPoly = false;
void mouseHandler(int event, int x, int y, int flags, void *param) {
    //选定
    int err = 5; //10像素内
    switch (event) {
        case CV_EVENT_MOUSEMOVE:
            if (drawing_poly) {
                draw_polygon = final_polygon;
                draw_polygon.push_back(cv::Point(x, y));
            }
            break;
        case CV_EVENT_LBUTTONDOWN:
            drawing_poly = true;
            if (final_polygon.size() > 2) //选中封闭形状结束
            {
                cv::Point pt1(x, y);
                cv::Point pt2 = final_polygon[0];
                double dd = (pt1.x - pt2.x) * (pt1.x - pt2.x) + (pt1.y - pt2.y) * (pt1.y - pt2.y);
                if(dd < err * err) {
                    gotPoly = true;
                    final_polygon.push_back(final_polygon[0]);
                    break;
                }
            }
            final_polygon.push_back(cv::Point(x, y));
            break;
        case CV_EVENT_RBUTTONDOWN: //右击结束
            gotPoly = true;
            break;
    }
}
// 选取矩形框, 左键两次选框, 右键结束
void selectRect(const cv::Mat& img, cv::Rect& r, const double& scale){
    final_polygon.clear();
    draw_polygon.clear();
    std::vector<std::pair<int, int> >roi;
    std::vector<cv::Point> polygon;
    cv::namedWindow("img", CV_WINDOW_AUTOSIZE);
    cv::Mat _img = img.clone();
    while (!gotPoly)
    {
        //放入循环中，为了避免点击图片右上角“叉号”后，程序陷入死循环
        cv::setMouseCallback("img", mouseHandler, NULL);
        cv::Mat showImg;
        cv::resize(_img, showImg, cv::Size(int(_img.cols*scale), int(_img.rows*scale)));
        if (draw_polygon.size() == 2) {
            cv::rectangle(showImg, cv::Rect(draw_polygon[0], draw_polygon[1]), cv::Scalar(0,255,0), 2);
        } else if (draw_polygon.size() > 2) {
            break;
        }
        cv::imshow("img", showImg);
        cv::waitKey(40);
    }
    cv::setMouseCallback("img", NULL, NULL);
    cv::destroyWindow("img");
    r = cv::Rect(cv::Point(final_polygon[0].x/scale,final_polygon[0].y/scale),
                 cv::Point(final_polygon[1].x/scale,final_polygon[1].y/scale));
    drawing_poly = false;
    gotPoly = false;
}

vector<Scalar> colors = {{255,0,0}, {0,255,0}, {0,0,255},
                         {255,255,0}, {255,0,255}, {0,255,255},
                         {0,0,0}, {255,255,255}};

int main(int argc, char* argv[]) {
    if (argc > 5) return -1;
    bool HOG = true;
    bool FIXEDWINDOW = false;
    bool MULTISCALE = true;
    bool SILENT = true;
    bool LAB = false;
    for(int i = 0; i < argc; i++){
        if ( strcmp (argv[i], "hog") == 0 )
            HOG = true;
        if ( strcmp (argv[i], "fixed_window") == 0 )
            FIXEDWINDOW = true;
        if ( strcmp (argv[i], "singlescale") == 0 )
            MULTISCALE = false;
        if ( strcmp (argv[i], "show") == 0 )
            SILENT = false;
        if ( strcmp (argv[i], "lab") == 0 ){
            LAB = true;
            HOG = true;
        }
        if ( strcmp (argv[i], "gray") == 0 )
            HOG = false;
    }
    Mat frame;
    // 定义kcf跟踪器
    KCFTracker tmpTracker(HOG, FIXEDWINDOW, MULTISCALE, LAB);
    // 获取摄像头照片
    VideoCapture cap(0);
    int count_ = 0;
    while(cap.read(frame))
    {
        if(frame.empty())
            continue;
        if(count_ >= 0)
            break;
        count_++;
    }
    // 选取初始矩形框
    cv::Rect objRect;
    selectRect(frame, objRect, 1.0);
    std::cout << objRect << std::endl;
    int count = 0;
    while(cap.read(frame))
    {
        if(frame.empty())
            continue;
        // 给定初始框
        if (count == 0) {
            tmpTracker.init(objRect,frame);
        }
        else{ // 跟踪
            Rect rect = tmpTracker.update(frame);
            rectangle(frame, rect, colors[0], 2, 8);
        }
        count++;
        imshow("img",frame);
        waitKey(30);
    }
    return 0;
}