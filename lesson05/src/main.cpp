#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "sobel.h"

#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

string resultsDir = "lesson05/resultsData/";

float pi = acos(-1);

void test(const std::string &name) {
    cv::Mat img = cv::imread("lesson05/data/" + name + ".jpg");
    rassert(!img.empty(), 23981920813);
    Mat gray = convertBGRToGray(img);

    cv::Mat dxy = sobelDXY(gray);
    cv::Mat grad = convertDXYToGradientLength(dxy);
    cv::imwrite(resultsDir + name + "_grad.jpg", grad);
    Scalar color(0);
    Mat result(360, sqrt(grad.cols*grad.cols+grad.rows*grad.rows)+10, CV_32FC1);
    for(int i = 0; i<grad.rows; i++){
        for(int j = 0; j<grad.cols; j++){
            for(int f = 0; f<360; f++){
                float r = i*abs(sin(((float)f)*pi/180.0))+j*abs(cos(((float)f)*pi/180.0));
                result.at<float>(f, (int) r) += grad.at<float>(i, j)*100;
                    //cout << r << " " << sqrt(grad.cols*grad.cols+grad.rows*grad.rows)+10;
            }
        }
    }

    cv::imwrite(resultsDir + name + "_result.jpg", result);
}

int main() {
    try {
        if (!filesystem::exists(resultsDir)) { // если папка еще не создана
            filesystem::create_directory(resultsDir); // то создаем ее
        }
        for (int i = 1; i <= 4; ++i) {
            test("line0" + std::to_string(i));
        }

        for (int i = 1; i <= 4; ++i) {
            test("line1" + std::to_string(i));
        }

        test("line21_water_horizont");
        test("multiline1_paper_on_table");
        test("multiline2_paper_on_table");

        test("valve");

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
