#include "hough.h"
#include <iostream>

#include <libutils/rasserts.h>

using namespace std;

double toRadians(double degrees)
{
    const double PI = 3.14159265358979323846264338327950288;
    return degrees * PI / 180.0;
}

double estimateR(double x0, double y0, double theta0radians)
{
    double r0 = x0 * cos(theta0radians) + y0 * sin(theta0radians);
    return r0;
}

cv::Mat buildHough(cv::Mat sobel) {// единственный аргумент - это результат свертки Собелем изначальной картинки
    // проверяем что входная картинка - одноканальная и вещественная:
    rassert(sobel.type() == CV_32FC1, 237128273918006);


    // Эта функция по картинке с силами градиентов (после свертки оператором Собеля) строит пространство Хафа
    // Вы можете либо взять свою реализацию из прошлого задания, либо взять эту заготовку:

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = width + height;

    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа
    cv::Mat accumulator(max_r, max_theta, CV_32FC1, 0.0f); // зануление аккумулятора через указание значения по умолчанию в конструкторе

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    for (int y0 = 0; y0 < height; ++y0) {
        for (int x0 = 0; x0 < width; ++x0) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(y0, x0);

            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):
            int last = (int) round(estimateR(x0, y0, toRadians(0)));
            for (int theta0 = 1; theta0 + 1 < max_theta; ++theta0) {
                double theta0radians = toRadians(theta0);
                int theta1 = theta0;
                int r0 = (int) round(estimateR(x0, y0, theta0radians)); // оцениваем r0 и округляем его до целого числа
                if (r0 < 0) {
                    last = -1;
                    continue;
                }
                if(r0 >= max_r){
                    last = max_r;
                    continue;
                }

                // https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/09/lesson9-hough2-interpolation-extremum-detection.html
                if(r0<last)
                    last--;
                else last++;
                for(int i = min(r0, last); i<=max(r0, last); i++) {
                    int len = max(r0, last)-min(r0, last);
                    float wei;
                    if(len==0)
                        wei = 0.5;
                    else wei = ((float)(i-min(r0, last)))/len;
                    //cout << len << " ";
                    accumulator.at<float>(i, theta0-1) += strength*(1.0-wei);
                    accumulator.at<float>(i, theta0) += strength*wei;
                }
                last = r0;
            }
        }
    }

    return accumulator;
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> winners;

    for (int theta = 0; theta < max_theta; ++theta) {
        for (int r = 0; r < max_r; ++r) {
            bool ok = 1;
            float votes = houghSpace.at<float>(r, theta);
            vector<pair<int, int>> neighbors;
            for(int dtheta = -1; dtheta<2; dtheta++){
                for(int dr = -1; dr<2; dr++){
                    if(theta+dtheta>=0 && r+dr>=0 && r+dr<max_r && theta+dtheta<max_theta && !(dtheta==0 && dr==0))
                        neighbors.push_back({r+dr, theta+dtheta});
                }
            }
            if(r==0){
                neighbors.push_back({0, (theta+180-1)%360});
                neighbors.push_back({0, (theta+180)%360});
                neighbors.push_back({0, (theta+180+1)%360});
            }
            if(theta==359){
                if(r>0)
                    neighbors.push_back({r-1, 0});
                neighbors.push_back({r, 0});
                if(r+1<max_r)
                    neighbors.push_back({r+1, 0});
            }
            if(theta==0){
                if(r>0)
                    neighbors.push_back({r-1, 359});
                neighbors.push_back({r, 359});
                if(r+1<max_r)
                    neighbors.push_back({r+1, 359});
            }
            for(auto z:neighbors){
                if(houghSpace.at<float>(z.first, z.second)>votes){
                    ok = 0;
                    break;
                }
            }
            if (ok) {
                PolarLineExtremum line(theta, r, votes);
                winners.push_back(line);
            }
        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner)
{
    std::vector<PolarLineExtremum> strongLines;
    double maxvotes = 0;
    for (auto line:allLines){
        maxvotes = max(maxvotes, line.votes);
    }
    for (auto line:allLines){
        if(line.votes>thresholdFromWinner*maxvotes)
            strongLines.push_back(line);
    }
    return strongLines;
}
