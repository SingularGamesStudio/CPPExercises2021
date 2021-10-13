#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "sobel.h"

#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

string resultsDir = "lesson05/resultsData/";

void testBGRToGray() {
    std::string name = "valve";
    cv::Mat img = cv::imread("lesson05/data/" + name + ".jpg");
    rassert(!img.empty(), 23981920813);

    cv::Mat gray = convertBGRToGray(img);

    cv::imwrite(resultsDir + name + "_grey.jpg", gray);
}

void testSobel(const std::string &name) {
    cv::Mat img = cv::imread("lesson05/data/" + name + ".jpg");
    rassert(!img.empty(), 23981920813);
    Mat gray = convertBGRToGray(img);
    // т.е. посчитайте производную по x и по y (в каждом пикселе хранятся две эти производные)
    cv::Mat dxy = sobelDXY(gray); // обратите внимание что внутри ждут черно-белую картинку, значит нашу картинку надо перед Собелем преобразовать
    cout << "a";
    cv::Mat dx = convertDXYToDX(dxy);
    cv::imwrite(resultsDir + name + "_dx.jpg", dx);

    cv::Mat dy = convertDXYToDY(dxy);
    cv::imwrite(resultsDir + name + "_dy.jpg", dy);

    cv::Mat gradientStrength = convertDXYToGradientLength(dxy);
    cv::imwrite(resultsDir + name + "_gradientLength.jpg", gradientStrength);
    // для valve.jpg должно быть похоже на картинку с википедии - https://ru.wikipedia.org/wiki/%D0%9E%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D0%BE%D1%80_%D0%A1%D0%BE%D0%B1%D0%B5%D0%BB%D1%8F
}

int main() {
    try {
        if (!filesystem::exists(resultsDir)) { // если папка еще не создана
            filesystem::create_directory(resultsDir); // то создаем ее
        }
        testBGRToGray();

        for (int i = 1; i <= 4; ++i) {
            testSobel("line0" + std::to_string(i));
        }

        for (int i = 1; i <= 4; ++i) {
            testSobel("line1" + std::to_string(i));
        }

        testSobel("line21_water_horizont");
        testSobel("multiline1_paper_on_table");
        testSobel("multiline2_paper_on_table");

        testSobel("valve");

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
