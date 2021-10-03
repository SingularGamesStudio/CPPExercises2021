#include "mask.h"
#include<iostream>
#include <random>
#include <libutils/rasserts.h>

using namespace cv;
using namespace std;

Mask::Mask(int _n, int _m) {
    n = _n;
    m = _m;
    a = new bool * [n];
    for(int i = 0; i<n; i++){
        a[i] = new bool[m];
        for(int j = 0; j<m; j++){
            a[i][j] = 0;
        }
    }
}

Mask::Mask() {}


Mat Mask::use(Mat image, Mat background) {
    //rassert(image.rows!=background.rows || image.cols!=background.cols, "image size not equal to background");
    rassert(image.rows==n && image.cols==m, "image size not equal to mask");
    Mat res = image.clone();
    for(int i = 0; i<n; i++){
        for(int j = 0; j<m; j++){
            if(a[i][j]==0)
                res.at<Vec3b>(i, j) = background.at<Vec3b>(((double)i/ n)*background.rows, ((double)j/ m)*background.cols);
        }
    }
    return res;
}

double norm (Mat m){
    double ans = 0;
    for(int i = 0; i<m.rows; i++){
        for(int j = 0; j<m.cols; j++){
            Vec3b color = m.at<Vec3b>(i, j);
            ans+=sqrt(color[0]*color[0]+color[1]*color[1]+color[2]*color[2]);
        }
    }
    ans = ans/m.rows/m.cols;
    return ans;
}

Mask expand(Mask mask, int r, bool exp1, int numNeed){
    int sum = 0;
    bool calc = 0;
    Mask nw = mask;
    for(int i = 0; i<mask.n; i++){
        calc = 0;
        for(int j = 0; j<mask.m; j++){
            if(mask.a[i][j]!=exp1){
                if(calc){
                    int di = -r;
                    for(int dj = -r+1; dj<r; dj++){
                        if(i+di<mask.n && i+di>=0 && j+dj<mask.m && j+dj>=0){
                            sum-=(mask.a[i][j]==exp1);
                        }
                    }
                    di = r-1;
                    for(int dj = -r+1; dj<r; dj++){
                        if(i+di<mask.n && i+di>=0 && j+dj<mask.m && j+dj>=0){
                            sum+=(mask.a[i][j]==exp1);
                        }
                    }
                } else {
                    sum = 0;
                    for(int di = -r+1; di<r; di++){
                        for(int dj = -r+1; dj<r; dj++){
                            if(i+di<mask.n && i+di>=0 && j+dj<mask.m && j+dj>=0){
                                sum+=(mask.a[i][j]==exp1);
                            }
                        }
                    }
                }
                calc = 1;
                if(sum>=numNeed){
                    nw.a[i][j] = exp1;
                }
            } else calc = 0;
        }
    }
    return nw;
}

Mask createMask(Mat image, Mat background){
    rassert(image.rows==background.rows && image.cols==background.cols, "image size not equal to background");
    Mask res = Mask(image.rows, image.cols);
    double norm1 = norm(image);
    double norm2 = norm(background);
    for(int i = 0; i<background.rows; i++){
        for(int j = 0; j<background.cols; j++){
            bool ok = 0;
            for(int di = -1; di<2; di++){
                for(int dj = -1; dj<2; dj++){
                    if(i+di>=0 && i+di<background.rows && j+dj>=0 && j+dj<background.cols){
                        Vec3b c1 = background.at<Vec3b>(i, j);
                        Vec3b c2 = image.at<Vec3b>(i+di, j+dj);
                        double color1[3];
                        double color2[3];
                        color1[0] = c1[0]/norm1;
                        color1[1]=c1[1]/norm1;
                        color1[2]=c1[2]/norm1;
                        color2[0] = c2[0]/norm1;
                        color2[1]=c2[1]/norm1;
                        color2[2]=c2[2]/norm1;
                        if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<0.2){
                            ok = 1;
                        }
                    }
                }
            }
            if(ok)
                res.a[i][j] = 0;
            else res.a[i][j] = 1;
        }
    }
    const int r = 4;
    res = expand(res, r, 1, r);
    res = expand(res, r, 0, r);
    res = expand(res, r, 0, r);
    res = expand(res, r, 1, r);
    return res;
}
