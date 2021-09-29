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

    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);

        Mask mask = createMask(content.frame.clone(), base.clone());


        Mat res = content.frame.clone();
        for(int i = 0; i<base.rows; i++){
            for(int j = 0; j<base.cols; j++){
                bool ok = 0;
                for(int di = -1; di<2; di++){
                    for(int dj = -1; dj<2; dj++){
                        if(i+di>=0 && i+di<base.rows && j+dj>=0 && j+dj<base.cols){
                            Vec3b color1 = base.at<Vec3b>(i, j);
                            Vec3b color2 = content.frame.at<Vec3b>(i+di, j+dj);
                            if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<21){
                                ok = 1;
                            }
                        }
                    }
                }
                if(ok)
                    res.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
        cv::Mat background = imread("lesson03/data/castle_large.jpg");
        for(int i = 0; i<res.rows; i++){
            for(int j = 0; j<res.cols; j++){
                Vec3b colNow = res.at<Vec3b>(i, j);
                if(colNow[0]==0 && colNow[1]==0 && colNow[2]==0)
                    res.at<Vec3b>(i, j) = background.at<Vec3b>((int)((((double)i)*background.rows)/res.rows), (int)((((double)j)*background.cols)/res.cols));
            }
        }

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
