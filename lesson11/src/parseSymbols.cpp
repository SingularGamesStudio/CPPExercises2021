#include "parseSymbols.h"
#include <iostream>
#include <filesystem>
#include <libutils/rasserts.h>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

cv::Scalar randColor() {
    return cv::Scalar(128 + rand() % 128, 128 + rand() % 128, 128 + rand() % 128); // можно было бы брать по модулю 255, но так цвета будут светлее и контрастнее
}


cv::Mat drawContours(int rows, int cols, std::vector<std::vector<cv::Point>> contoursPoints) {

    // создаем пустую черную картинку
    cv::Mat blackImage(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
    // теперь мы на ней хотим нарисовать контуры
    cv::Mat imageWithContoursPoints = blackImage.clone();
    for (int contourI = 0; contourI < contoursPoints.size(); ++contourI) {
        // сейчас мы смотрим на контур номер contourI

        cv::Scalar contourColor = randColor(); // выберем для него случайный цвет
        std::vector<cv::Point> points = contoursPoints[contourI];
        for (auto point:points) {
            imageWithContoursPoints.at<cv::Vec3b>(point.y, point.x) = cv::Vec3b(contourColor[0], contourColor[1], contourColor[2]);
        }

    }

    return imageWithContoursPoints;
}


bool cmp(const cv::Rect &a, const cv::Rect &b){
    return (a.tl().y+a.br().y)<(b.tl().y+b.br().y);
}

bool cmp1(const cv::Rect &a, const cv::Rect &b){
    return (a.tl().x+a.br().x)<(b.tl().x+b.br().x);
}

std::vector<vector<cv::Mat>> splitSymbols(cv::Mat img)
{
    cv::Mat img1;
    cv::cvtColor(img.clone(), img1, cv::COLOR_BGR2GRAY);

    cv::Mat binary;
    cv::adaptiveThreshold(img1, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 10);

    cv::Mat binary_dilated;
    cv::dilate(binary, binary_dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));

    cv::Mat binary_eroded;
    cv::erode(binary_dilated, binary_eroded, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));

    binary = binary_eroded;

    std::vector<std::vector<cv::Point>> contoursPoints2;
    cv::findContours(binary, contoursPoints2, cv::RETR_EXTERNAL , cv::CHAIN_APPROX_NONE);
    for(int i = 0; i<contoursPoints2.size(); i++){
        if(contoursPoints2[i].size()<15){
            contoursPoints2.erase(contoursPoints2.begin()+i);
            i--;
        }

    }

    vector<Rect> sorted;
    vector<int> hei;

    for (int contourI = 0; contourI < contoursPoints2.size(); ++contourI) {
        std::vector<cv::Point> points = contoursPoints2[contourI]; // перем очередной контур
        cv::Rect box = cv::boundingRect(points); // строим прямоугольник по всем пикселям контура (bounding box = бокс ограничивающий объект)
        sorted.push_back(box);
        hei.push_back(-box.tl().y+box.br().y);
    }
    sort(hei.begin(), hei.end());
    int med = hei[hei.size()/2];
    sort(sorted.begin(), sorted.end(), cmp);
    int last = -1;
    vector<vector<cv::Mat>> symbols;
    vector<vector<cv::Rect>> rects;
    rects.push_back(vector<cv::Rect>());
    for(auto z:sorted){
        int pos = (z.tl().y+z.br().y);
        if(last!=-1){
            if(pos-last>med){
                rects.push_back(vector<cv::Rect>());
            }
        }
        rects.back().push_back(z);
        last = pos;
    }
    for(int i = 0; i< symbols.size(); i++){
        sort(rects[i].begin(), rects[i].end(), cmp1);
        symbols.push_back(vector<cv::Mat>());
        for(auto z:rects[i]){
            symbols[i].push_back(img(z).clone());
        }
    }
    return symbols;

}
