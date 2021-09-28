#include "helper_functions.h"
#include<iostream>
#include <random>
#include <libutils/rasserts.h>

using namespace cv;
using namespace std;
mt19937 rnd(time(NULL));
cv::Mat makeAllBlackPixelsBlue(cv::Mat image, bool randomly) {
    // TODO реализуйте функцию которая каждый черный пиксель картинки сделает синим
    Vec3b newCol = Vec3b(rnd()%256, rnd()%256, rnd()%256);
    for(int i = 0; i<image.rows; i++){
        for(int j = 0; j<image.cols; j++){
            Vec3b color = image.at<Vec3b>(i, j);
            if(color[0]<=3 && color[1]<=3 && color[2]<=3){
                if(!randomly)
                    image.at<Vec3b>(i, j) = Vec3b(255, 0, 0);
                else
                    image.at<Vec3b>(i, j) = newCol;
            }
        }
    }
//
//    // ниже приведен пример как узнать цвет отдельного пикселя - состоящий из тройки чисел BGR (Blue Green Red)
//    // чем больше значение одного из трех чисел - тем насыщеннее его оттенок
//    // всего их диапазон значений - от 0 до 255 включительно
//    // т.е. один байт, поэтому мы используем ниже тип unsigned char - целое однобайтовое неотрицательное число
//    cv::Vec3b color = image.at<cv::Vec3b>(13, 5); // взяли и узнали что за цвет в пикселе в 14-ом ряду (т.к. индексация с нуля) и 6-ой колонке
//    unsigned char blue = color[0]; // если это число равно 255 - в пикселе много синего, если равно 0 - в пикселе нет синего
//    unsigned char green = color[1];
//    unsigned char red = color[2];
//    // как получить белый цвет? как получить черный цвет? как получить желтый цвет?
//    // поэкспериментируйте! например можете всю картинку заполнить каким-то одним цветом
//
//    // пример как заменить цвет по тем же координатам
//    red = 255;
//    // запустите эту версию функции и посмотрите на получившуюся картинку - lesson03/resultsData/01_blue_unicorn.jpg
//    // какой пиксель изменился? почему он не чисто красный?
//    image.at<cv::Vec3b>(13, 5) = cv::Vec3b(blue, green, red);

    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    // TODO реализуйте функцию которая каждый цвет картинки инвертирует:
    // т.е. пусть ночь станет днем, а сумрак рассеется
    // иначе говоря замените каждое значение яркости x на (255-x) (т.к находится в диапазоне от 0 до 255)
    for(int i = 0; i<image.rows; i++){
        for(int j = 0; j<image.cols; j++){
            Vec3b color = image.at<Vec3b>(i, j);
            image.at<Vec3b>(i, j) = Vec3b(255-color[0], 255-color[1], 255-color[2]);
        }
    }
    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {
    // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    // т.е. что-то вроде накладного фона получится

    // гарантируется что размеры картинок совпадают - проверьте это через rassert, вот например сверка ширины:
    rassert(object.cols == background.cols, "pictures are not of equal size");
    rassert(object.rows == background.rows, "pictures are not of equal size");

    for(int i = 0; i<object.rows; i++){
        for(int j = 0; j<object.cols; j++){
            Vec3b color = object.at<Vec3b>(i, j);
            if(color[0]<=3 && color[1]<=3 && color[2]<=3){
                Vec3b color1 = background.at<Vec3b>(i, j);
                object.at<Vec3b>(i, j) = Vec3b(color1[0], color1[1], color1[2]);
            }
        }
    }

    return object;
}

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground, bool randomly) {
    // теперь вам гарантируется что largeBackground гораздо больше - добавьте проверок этого инварианта (rassert-ов)
    pair<int, int> vec;
    if(randomly){
        vec.first = rnd()%(largeBackground.rows-object.rows-2);
        vec.second = rnd()%(largeBackground.cols-object.cols-2);
    } else {
        vec.first = largeBackground.rows/2-object.rows/2;
        vec.second = largeBackground.cols/2-object.cols/2;
    }
    rassert(largeBackground.cols>object.cols+3 && largeBackground.rows>object.rows+3, "background too small")
    // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы
    for(int i = 0; i<object.rows; i++){
        for(int j = 0; j<object.cols; j++){
            Vec3b color = object.at<Vec3b>(i, j);
            if(color[0]>3 || color[1]>3 || color[2]>3){
                Vec3b color1 = object.at<Vec3b>(i, j);
                largeBackground.at<Vec3b>(i+vec.first, j+vec.second) = Vec3b(color1[0], color1[1], color1[2]);
            }
        }
    }
    return largeBackground;
}


cv::Mat manyObjects(cv::Mat object, cv::Mat largeBackground) {
    int cnt = rnd()%100+1;
    for(int i = 0; i<cnt; i++){
        addBackgroundInsteadOfBlackPixelsLargeBackground(object.clone(), largeBackground, true);
    }
    return largeBackground;
}

cv::Mat bigObject(cv::Mat object, cv::Mat largeBackground){
    Mat newObject = largeBackground.clone();
    for(int i = 0; i<newObject.rows; i++){
        for(int j = 0; j<newObject.cols; j++){
            newObject.at<Vec3b>(i, j) = object.at<Vec3b>((int)((((double)i)*object.rows)/newObject.rows), (int)((((double)j)*object.cols)/newObject.cols));
        }
    }
    return addBackgroundInsteadOfBlackPixels(newObject, largeBackground);
}