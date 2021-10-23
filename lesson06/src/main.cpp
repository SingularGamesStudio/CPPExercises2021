#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "blur.h"


using namespace std;
using namespace cv;

string resultsDir = "lesson06/resultsData/";

void testSomeBlur() {
    std::string name = "valve";
    cv::Mat img = cv::imread("lesson05/data/" + name + ".jpg");
    rassert(!img.empty(), 23981920813);
    Mat res = blur(img.clone(), 1);

    cv::imwrite(resultsDir + name + "_blur.jpg", res);
}

void testManySigmas() {
    std::string name = "valve";
    cv::Mat img = cv::imread("lesson05/data/" + name + ".jpg");
    rassert(!img.empty(), 23981920813);
    for(double sig = 0.6; sig<5; sig+=0.3){
        Mat res = blur(img.clone(), sig);
        cv::imwrite(resultsDir + name + to_string(sig) + "_blur.jpg", res);
    }
}

int main() {
    try {
        if (!filesystem::exists(resultsDir)) { // если папка еще не создана
            filesystem::create_directory(resultsDir); // то создаем ее
        }
        testSomeBlur();
        testManySigmas();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

