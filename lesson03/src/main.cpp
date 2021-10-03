#include <iostream>
#include <filesystem> // это нам понадобится чтобы создать папку для результатов
#include <libutils/rasserts.h>
#include "mask.h"

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем читать и сохранять картинки

using namespace cv;
using namespace std;

struct MyVideoContent {
    cv::Mat frame;
    int lastClickX;
    int lastClickY;
};
void onMouseClick4(int event, int x, int y, int flags, void *pointerToMyVideoContent) {
    MyVideoContent &content = *((MyVideoContent*) pointerToMyVideoContent);
    if (event == cv::EVENT_LBUTTONDOWN) {

    }
}

void task4() {
    cv::VideoCapture video(0);
    rassert(video.isOpened(), 3423948392481);

    MyVideoContent content;
    cout << "Exit the view of the camera, then press <Space>";
    Mat base;
    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);
        cv::imshow("video", content.frame);
        int key = cv::waitKey(10);
        if(key==32){
            base = content.frame.clone();
            break;
        }
    }
    cv::Mat background = imread("lesson03/data/castle_large.jpg");
    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);
        Mask mask = createMask(content.frame.clone(), base.clone());
        Mat res = mask.use(content.frame.clone(), background);


        cv::imshow("video", res);

        int key = cv::waitKey(10);
        if(key==27 || key==32)
            break;
    }
}

int main() {
    try {
        task4();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
