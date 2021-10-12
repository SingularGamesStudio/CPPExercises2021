#include <iostream>
#include <filesystem> // это нам понадобится чтобы создать папку для результатов
#include <libutils/rasserts.h>
#include "lib/mask.h"

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
    Mat background = imread("lesson03/data/castle_large.jpg");
    bool save = 0;
    string resultsDir = "lesson03/resultsData/";
    if (!filesystem::exists(resultsDir)) { // если папка еще не создана
        filesystem::create_directory(resultsDir); // то создаем ее
    }

    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);
        Mask mask = createMask(content.frame.clone(), base.clone(), false, 24/*0.15*/, save);
        Mat res = mask.use(content.frame.clone(), background);
        if(save) {
            string filename = resultsDir + "raw.jpg";
            imwrite(filename, content.frame);
            filename = resultsDir + "sample.jpg";
            imwrite(filename, base);
        }
        imshow("video", res);
        save = 0;
        int key = cv::waitKey(10);
        if(key==27)
            break;
        if(key==32){
            save = 1;
        }
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
