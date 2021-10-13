#include "mask.h"
#include<iostream>
#include <random>
#include <filesystem>
#include <libutils/rasserts.h>
#include "disjoint_set.h"

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

Mask delSmall(Mask mask, int sz){
    DisjointSet ds(mask.m*mask.n);
    for(int i = 0; i<mask.n; i++){
        for(int j = 0; j<mask.m; j++){
            if(j<mask.m-1 && mask.a[i][j]==mask.a[i][j+1])
                ds.union_sets(i*mask.m+j, i*mask.m+j+1);
            if(i<mask.n-1 && mask.a[i][j]==mask.a[i+1][j])
                ds.union_sets(i*mask.m+j, i*mask.m+j+mask.m);
        }
    }
    for(int i = 0; i<mask.n; i++){
        for(int j = 0; j<mask.m; j++){
            if(ds.get_set_size(ds.get_set(i*mask.m+j))<sz){
                mask.a[i][j] = !mask.a[i][j];
            }
        }
    }
    return mask;
}

Mask expand(Mask mask, int r, bool exp1, int numNeed){
    int sum = 0;
    bool calc;
    Mask nw = Mask(mask.n, mask.m);
    for(int i = 0; i<mask.n; i++){
        for(int j = 0; j<mask.m; j++){
            nw.a[i][j] = mask.a[i][j];
        }
    }
    for(int i = 0; i<mask.n; i++){
        calc = 0;
        for(int j = 0; j<mask.m; j++){
            if(mask.a[i][j]!=exp1){
                if(calc){
                    int dj = -r;
                    for(int di = -r+1; di<r; di++){
                        if(i+di<mask.n && i+di>=0 && j+dj<mask.m && j+dj>=0){
                            sum-=(mask.a[i+di][j+dj]==exp1);
                        }
                    }
                    dj = r-1;
                    for(int di = -r+1; di<r; di++){
                        if(i+di<mask.n && i+di>=0 && j+dj<mask.m && j+dj>=0){
                            sum+=(mask.a[i+di][j+dj]==exp1);
                        }
                    }
                } else {
                    sum = 0;
                    for(int di = -r+1; di<r; di++){
                        for(int dj = -r+1; dj<r; dj++){
                            if(i+di<mask.n && i+di>=0 && j+dj<mask.m && j+dj>=0){
                                sum+=(int)(mask.a[i+di][j+dj]==exp1);
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

Mat toMat(Mask mask){
    Scalar color(0, 0, 0);
    Mat img(mask.n, mask.m, CV_8UC3, color);
    for(int i = 0; i<mask.n; i++){
        for(int j = 0; j<mask.m; j++){
            img.at<Vec3b>(i, j) = Vec3b(mask.a[i][j]*255, mask.a[i][j]*255, mask.a[i][j]*255);
        }
    }
    return img;
}

Mask createMask(Mat image, Mat background, int normalize, double treshold, bool save, double treshold2){
    string resultsDir = "lesson03/resultsData/";
    if (!filesystem::exists(resultsDir)) { // если папка еще не создана
        filesystem::create_directory(resultsDir); // то создаем ее
    }
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
                        if(normalize==1){
                            Vec3b c1 = background.at<Vec3b>(i, j);
                            Vec3b c2 = image.at<Vec3b>(i+di, j+dj);
                            double color1[3];
                            double color2[3];
                            color1[0] = c1[0]/norm1;
                            color1[1]=c1[1]/norm1;
                            color1[2]=c1[2]/norm1;
                            color2[0] = c2[0]/norm2;
                            color2[1]=c2[1]/norm2;
                            color2[2]=c2[2]/norm2;
                            if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<treshold){
                                ok = 1;
                            }
                        } else if(normalize==0){
                            Vec3b color1 = background.at<Vec3b>(i, j);
                            Vec3b color2 = image.at<Vec3b>(i+di, j+dj);
                            if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<treshold){
                                ok = 1;
                            }
                        } else if(normalize==2){
                            Vec3b c1 = background.at<Vec3b>(i, j);
                            Vec3b c2 = image.at<Vec3b>(i+di, j+dj);
                            double n1 = sqrt(c1[0]*c1[0]+c1[1]*c1[1]+c1[2]*c1[2]);
                            double n2 = sqrt(c2[0]*c2[0]+c2[1]*c2[1]+c2[2]*c2[2]);
                            double color1[3];
                            double color2[3];
                            color1[0] = c1[0]/n1;
                            color1[1]=c1[1]/n1;
                            color1[2]=c1[2]/n1;

                            color2[0] = c2[0]/n2;
                            color2[1]=c2[1]/n2;
                            color2[2]=c2[2]/n2;
                            if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<treshold){
                                ok = 1;
                            }
                        } else {
                            Vec3b c1 = background.at<Vec3b>(i, j);
                            Vec3b c2 = image.at<Vec3b>(i+di, j+dj);
                            double n1 = sqrt(c1[0]*c1[0]+c1[1]*c1[1]+c1[2]*c1[2]);
                            double n2 = sqrt(c2[0]*c2[0]+c2[1]*c2[1]+c2[2]*c2[2]);
                            double color1[3];
                            double color2[3];
                            color1[0] = c1[0]/n1;
                            color1[1]=c1[1]/n1;
                            color1[2]=c1[2]/n1;

                            color2[0] = c2[0]/n2;
                            color2[1]=c2[1]/n2;
                            color2[2]=c2[2]/n2;
                            if(abs(n1-n2)<treshold2 && sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<treshold){
                                ok = 1;
                            }
                        }
                    }
                }
            }
            if(ok)
                res.a[i][j] = 0;
            else res.a[i][j] = 1;
        }
    }
    if(save){
        string filename = resultsDir + "mask_0_base.jpg";
        imwrite(filename, toMat(res));
    }
    const int minsz = 800;
    res = delSmall(res, minsz);
    if(save){
        string filename = resultsDir + "mask_1_dsu.jpg";
        imwrite(filename, toMat(res));
    }
    const int r = 4;
    res = expand(res, r, 1, r);
    if(save){
        string filename = resultsDir + "test1.jpg";
        imwrite(filename, toMat(res));
    }
    res = expand(res, r, 0, r);
    if(save){
        string filename = resultsDir + "test2.jpg";
        imwrite(filename, toMat(res));
    }
    res = expand(res, r, 0, r);
    if(save){
        string filename = resultsDir + "test3.jpg";
        imwrite(filename, toMat(res));
    }
    res = expand(res, r, 1, r);
    if(save){
        string filename = resultsDir + "mask_2_morph.jpg";
        imwrite(filename, toMat(res));
    }

    return res;
}
