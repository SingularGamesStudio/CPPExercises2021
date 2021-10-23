#include "blur.h"
#include <iostream>
#include <libutils/rasserts.h>

using namespace std;
using namespace cv;
const double pi = acos(-1);
cv::Mat blur(cv::Mat img, double sigma) {
    Mat res = img.clone();
    double kernel[7][7];
    for(int i = -3; i<4; i++){
        for(int j = -3; j<4; j++){
            kernel[i+3][j+3] = (1.0/(2*pi*sigma*sigma))*exp(-((double)(i*i+j*j))/(2*sigma*sigma));
        }
    }

    for(int i = 0; i<img.rows; i++){
        for(int j = 0; j<img.cols; j++){
            double r = 0, g = 0, b = 0;
            for(int di = -3; di<4; di++){
                for(int dj = -3; dj<4; dj++){
                    if(i+di>=0 && j+dj>=0 && i+di<img.rows && j+dj<img.cols){
                        Vec3b col = img.at<Vec3b>(i+di, j+dj);
                        r+=kernel[di+3][dj+3]*col[0];
                        g+=kernel[di+3][dj+3]*col[1];
                        b+=kernel[di+3][dj+3]*col[2];
                    }
                }
            }
            res.at<Vec3b>(i, j) = Vec3b ((int)r, (int)g, (int)b);
        }
    }
    return res;
}
