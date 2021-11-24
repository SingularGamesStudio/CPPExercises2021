#include "hough.h"
#include <iostream>
#include <libutils/rasserts.h>
#include <random>
#include <opencv2/imgproc.hpp>


using namespace std;

mt19937 rnd(42);

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


cv::Mat buildHough(cv::Mat sobel) {
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

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace, cv::Mat blurredHough)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> winners;

    for (int theta = 0; theta < max_theta; ++theta) {
        for (int r = 0; r < max_r; ++r) {
            bool ok = 1;
            float votes = blurredHough.at<float>(r, theta);
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
                if(blurredHough.at<float>(z.first, z.second)>votes){
                    ok = 0;
                    break;
                }
            }
            if (ok) {
                PolarLineExtremum line(theta, r, houghSpace.at<float>(r, theta));
                winners.push_back(line);
            }
        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum> allLines, double thresholdFromWinner, int min_r_diff)
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

    std::vector<PolarLineExtremum> lines_new;

    int min_theta_diff = 5;

    for(auto v1:strongLines){
        bool ok = 1;
        for(auto v2:lines_new){
            if(abs((int)v1.theta-(int)v2.theta)<=min_theta_diff || abs((int)v1.theta-(int)v2.theta)>=360-min_theta_diff-1){//theta is close (mod 360)
                if(abs((int)v1.r-(int)v2.r)<=min_r_diff){//r is close
                    ok = 0;
                    break;
                }
            }
            if(abs(v1.r)+abs(v2.r)<=min_r_diff){//r close to 0 and theta1=theta+180
                if(abs(abs((int)v1.theta-(int)v2.theta)-180)<=min_theta_diff){
                    ok = 0;
                    break;
                }
            }
        }
        if(ok)
            lines_new.push_back(v1);
    }
    strongLines = lines_new;

    return strongLines;
}

cv::Mat drawCirclesOnExtremumsInHoughSpace(cv::Mat houghSpace, std::vector<PolarLineExtremum> lines, int radius)
{

    // делаем копию картинки с пространством Хафа (чтобы не портить само пространство Хафа)
    cv::Mat houghSpaceWithCrosses = houghSpace.clone();

    // проверяем что пространство состоит из 32-битных вещественных чисел (т.е. картинка одноканальная)
    rassert(houghSpaceWithCrosses.type() == CV_32FC1, 347823472890137);

    // но мы хотим рисовать КРАСНЫЙ кружочек вокруг найденных экстремумов, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку
    cv::cvtColor(houghSpaceWithCrosses, houghSpaceWithCrosses, cv::COLOR_GRAY2BGR);
    // проверяем что теперь все хорошо и картинка трехканальная (но при этом каждый цвет - 32-битное вещественное число)
    rassert(houghSpaceWithCrosses.type() == CV_32FC3, 347823472890148);

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];

        // Пример как рисовать кружок в какой-то точке (закомментируйте его):
        cv::Point point(line.theta, line.r);
        cv::Scalar color(0, 0, 255); // BGR, т.е. красный цвет
        cv::circle(houghSpaceWithCrosses, point, radius, color);

    }

    return houghSpaceWithCrosses;
}

bool ins(cv::Point a, int x, int y){
    if(a.x>x || a.x<-1 || a.y>y || a.y<-1)
        return 0;
    return 1;
}

// TODO Реализуйте эту функцию - пусть она скопирует картинку и отметит на ней прямые в соответствии со списком прямых
cv::Mat drawLinesOnImage(cv::Mat img, std::vector<PolarLineExtremum> lines)
{
    // делаем копию картинки (чтобы при рисовании не менять саму оригинальную картинку)
    cv::Mat imgWithLines = img.clone();

    // проверяем что картинка черно-белая (мы ведь ее такой сделали ради оператора Собеля) и 8-битная
    rassert(imgWithLines.type() == CV_8UC1, 45728934700167);

    // но мы хотим рисовать КРАСНЫЕ прямые, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку с 8 битами в каждом пикселе
    cv::cvtColor(imgWithLines, imgWithLines, cv::COLOR_GRAY2BGR);
    rassert(imgWithLines.type() == CV_8UC3, 45728934700172);

    // выпишем размер картинки
    int width = imgWithLines.cols;
    int height = imgWithLines.rows;

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];

        // нам надо найти точки на краях картинки
        cv::Point pointA;
        cv::Point pointB;


        // напоминаю - чтобы посмотреть какие аргументы требует функция (или в данном случае конструктор объекта) - нужно:
        // 1) раскомментировать эти четыре строки ниже
        // 2) поставить каретку (указатель где вы вводите новые символы) внутри скобок функции (или конструктора, т.е. там где были три вопроса: ???)
        // 3) нажать Ctrl+P чтобы показать список параметров (P=Parameters)
        PolarLineExtremum leftImageBorder(0, 0, 0);
        PolarLineExtremum bottomImageBorder(90, height, 0);
        PolarLineExtremum rightImageBorder(0, width, 0);
        PolarLineExtremum topImageBorder(90, 0, 0);
        vector<cv::Point> all;
        all.push_back(line.intersect(leftImageBorder));
        all.push_back(line.intersect(rightImageBorder));
        all.push_back(line.intersect(topImageBorder));
        all.push_back(line.intersect(bottomImageBorder));
//        cout << all[0].x << " " << all[0].y << "\n";
//        cout << all[1].x << " " << all[1].y << "\n";
//        cout << all[2].x << " " << all[2].y << "\n";
//        cout << all[3].x << " " << all[3].y << "\n\n";
        for(int i = 0; i<4; i++){
            for(int j = i+1; j<4; j++){
                if(ins(all[i], height, width) && ins(all[j], height, width)){
                    pointA = all[i];
                    pointB = all[j];
                }
            }
        }
//        cout << pointA.x << " " << pointA.y << "\n";
//        cout << pointB.x << " " << pointB.y << "\n";
        // TODO переделайте так чтобы цвет для каждой прямой был случайным (чтобы легче было различать близко расположенные прямые)
        cv::Scalar color(rnd()%255, rnd()%255, rnd()%255);
        cv::line(imgWithLines, cv::Point (pointA.y, pointA.x), cv::Point (pointB.y, pointB.x), color);
    }

    return imgWithLines;
}
