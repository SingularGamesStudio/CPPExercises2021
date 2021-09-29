#pragma once

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем работать с картинками (знаем про тип cv::Mat)

using namespace cv;

struct Mask;

//Mask expand(Mask mask, int r, bool inv, int numb);

Mask createMask(Mat image, Mat background);