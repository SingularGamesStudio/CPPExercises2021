#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <set>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>
#include <random>

#include <libutils/rasserts.h>
#include <libutils/fast_random.h>

using namespace std;
using namespace cv;

// Эта функция говорит нам правда ли пиксель отмаскирован, т.е. отмечен как "удаленный", т.е. белый
bool isPixelMasked(cv::Mat mask, int j, int i) {
    rassert(j >= 0 && j < mask.rows, 372489347280017);
    rassert(i >= 0 && i < mask.cols, 372489347280018);
    rassert(mask.type() == CV_8UC3, 2348732984792380019);
    Vec3b color = mask.at<Vec3b>(i, j);
    if(color[0]+color[1]+color[2]>100)
        return 1;
    return 0;
}
mt19937 rnd(42);
void run(int caseNumber, std::string caseName) {
    std::cout << "_________Case #" << caseNumber << ": " <<  caseName << "_________" << std::endl;

    cv::Mat original = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_original.jpg");
    cv::Mat mask = cv::imread("lesson18/data/" + std::to_string(caseNumber) + "_" + caseName + "/" + std::to_string(caseNumber) + "_mask.png");
    rassert(!original.empty(), 324789374290018);
    rassert(!mask.empty(), 378957298420019);

    rassert(mask.cols==original.cols && mask.rows==original.rows, 1283912);
     std::cout << "Image resolution: " << mask.cols << "x" << mask.rows << std::endl;

    // создаем папку в которую будем сохранять результаты - lesson18/resultsData/ИМЯ_НАБОРА/
    std::string resultsDir = "lesson18/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    resultsDir += std::to_string(caseNumber) + "_" + caseName + "/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }
    vector<pair<Mat, Mat>> pyramid;
    Mat img = original.clone();
    const int PYRAMID_MIN_SIZE = 20; // до какой поры уменьшать картинку? давайте уменьшать пока картинка больше 20 пикселей
    while (img.rows > PYRAMID_MIN_SIZE && img.rows > PYRAMID_MIN_SIZE) { // или пока больше (2 * размер окна для оценки качества)
        pyramid.push_back({img.clone(), mask.clone()}); // мы могли бы воспользоваться push_back но мы хотим вставлять картинки в начало вектора
        cv::pyrDown(img, img); // эта функция уменьшает картинку в два раза
        cv::pyrDown(mask, mask); // эта функция уменьшает картинку в два раза
    }
    cv::Mat shiftslast;
    reverse(pyramid.begin(), pyramid.end());
    for(int layer = 0; layer<pyramid.size(); layer++){
        img = pyramid[layer].first;
        mask = pyramid[layer].second;
        string nowdir = resultsDir+"/layer_"+ to_string(layer)+"/";
        if (!std::filesystem::exists(nowdir)) { // если папка еще не создана
            std::filesystem::create_directory(nowdir); // то создаем ее
        }
        // сохраняем в папку с результатами оригинальную картинку и маску
        cv::imwrite(nowdir + "0original.png", original);
        cv::imwrite(nowdir + "1mask.png", mask);

        int cntmasked = 0;
        for(int i = 0; i<mask.rows; i++){
            for(int j = 0; j<mask.cols; j++){
                if(isPixelMasked(mask, i, j)){
                    img.at<Vec3b>(i, j) = Vec3b(rnd()%255, rnd()%255, rnd()%255);
                    cntmasked++;
                }
            }
        }
        cout << "Number of masked pixels: " <<  cntmasked << "/" << mask.rows*mask.cols << " = " << cntmasked*100.0/mask.rows/mask.cols << "%";

        cv::Mat shifts(img.rows, img.cols, CV_32SC2, cv::Scalar(0, 0)); // матрица хранящая смещения, изначально заполнена парами нулей
        if(layer!=0){
            for(int i = 0; i<mask.rows; i++){
                for(int j = 0; j<mask.cols; j++){
                    shifts.at<Vec2i>(i, j) = Vec2i(shiftslast.at<Vec2i>(i/2, j/2)[0]*2, shiftslast.at<Vec2i>(i/2, j/2)[1]*2);
                }
            }
        }
        for(int i = 0; i<mask.rows; i++){
            for(int j = 0; j<mask.cols; j++){

            }
        }
        shiftslast = shifts.clone();
    }
}


int main() {
    try {
        run(1, "mic");
//        run(2, "flowers");
//        run(3, "baloons");
//        run(4, "brickwall");
//        run(5, "old_photo");
//        run(6, "your_data");

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
