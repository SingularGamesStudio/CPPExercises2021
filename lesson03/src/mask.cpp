#include "mask.h"
#include<iostream>
#include <random>
#include <libutils/rasserts.h>

using namespace cv;
using namespace std;

struct Fenv{
    int ** t;
    int n, m;

    fenv(){}

    fenv(int n, int m){
        t = new int*[n];
        for(int i = 0; i<n; i++){
            t[i] = new int[m];
            for(int j = 0; j<m; j++){
                t[i][j] = 0;
            }
        }
    }

    int sum (int x, int y)
    {
        int result = 0;
        for (int i = x; i >= 0; i = (i & (i+1)) - 1)
            for (int j = y; j >= 0; j = (j & (j+1)) - 1)
                result += t[i][j];
            return result;
    }

    void change (int x, int y, int delta)
    {
        for (int i = x; i < n; i = (i | (i+1)))
            for (int j = y; j < m; j = (j | (j+1)))
                t[i][j] += delta;
    }

    int sum (int x1, int y1, int x2, int y2)
    {
        return sum (x2, y2) + sum(x1, y1) - sum(x1, y2) - sum(x2, y1);
    }
};

struct Mask{
    bool** a;
    int n, m;
    Fenv f;
    Mask(){

    }
    Mask(int _n, int _m){
        n = _n;
        m = _m;
        f = Fenv(n, m);
    }
    void set(int i, int j, int v){
        if(a[i][j]!=v){
            if(a[i][j]==1)
                f.change(i, j, -1);
            else f.change(i, j, 1);
            a[i][j] = v;
        }
    }
    void buildFenv(){
        f = Fenv(n, m);
        for(int i = 0; i<n; i++){
            for(int j = 0; j<m; j++){
                if(a[i][j])
                    f.change(i, j, 1);
            }
        }
    }
    Mat use(Mat image, Mat background){
        rassert(image.rows!=background.rows || image.cols!=background.cols, "image size not equal to background");
        rassert(image.rows!=n || image.cols!=m, "image size not equal to mask");
        Mat res = image.clone();
        for(int i = 0; i<n; i++){
            for(int j = 0; j<m; j++){
                if(a[i][j]==0)
                    res.at<Vec3b>(i, j) = background.at<Vec3b>(i, j);
            }
        }
        return res;
    }
};

Mask expand(Mask mask, int r, bool exp1, int numNeed){
    for(int i = 0; i<mask.n; i++){
        for(int j = 0; j<mask.m; j++){

        }
    }
    return mask;
}

Mask createMask(Mat image, Mat background){
    rassert(image.rows!=background.rows || image.cols!=background.cols, "image size not equal to background");
    Mask res = Mask(image.rows, image.cols);
    for(int i = 0; i<background.rows; i++){
        for(int j = 0; j<background.cols; j++){
            bool ok = 0;
            for(int di = -1; di<2; di++){
                for(int dj = -1; dj<2; dj++){
                    if(i+di>=0 && i+di<background.rows && j+dj>=0 && j+dj<background.cols){
                        Vec3b color1 = background.at<Vec3b>(i, j);
                        Vec3b color2 = image.at<Vec3b>(i+di, j+dj);
                        if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<21){
                            ok = 1;
                        }
                    }
                }
            }
            if(ok)
                res.set(i, j, 0);
            res.set(i, j, 1);
        }
    }
}
