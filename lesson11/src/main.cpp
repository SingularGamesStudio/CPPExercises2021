#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "parseSymbols.h"
#include "hog.h"

#include <opencv2/imgproc.hpp>

using namespace std;

int main() {
    try {

        std::vector<std::string> names;
        names.push_back("alphabet");
        names.push_back("line");
        names.push_back("text");
        for (int i = 0; i < names.size(); ++i) {
            for (int j = 1; j <= 5; ++j) {
                cout << names[i] << " " << j << ": \n\n";
                string full_path = "lesson11/data/" + names[i] + "/" + to_string(j) + ".png";
                std::filesystem::create_directory("lesson11/resultsData/" + names[i]);
                std::string out_path = "lesson11/resultsData/" + names[i] + "/" + to_string(j);
                std::filesystem::create_directory(out_path);
                cv::Mat img = cv::imread(full_path);
                vector<vector<cv::Mat>> letters = splitSymbols(img);
                int num = 0;
                for(auto u:letters){
                    for(auto z:u){
                        if(i==0 && j==1){
                            cv::imwrite(out_path + "/test/"+ to_string(num)+".png", z);
                            num++;
                        }
                        cout << get(z);
                    }
                    cout << "\n";
                }
            }
        }

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

