#include <iostream>
#include <filesystem>
#include <libutils/rasserts.h>
#include <random>

#include "disjoint_set.h"
#include "morphology.h"

#include <opencv2/highgui.hpp>

using namespace std;

mt19937 rnd(42);
void testingMyDisjointSets() {
    DisjointSet set(5);
    rassert(set.count_differents() == 5, 2378923791);
    for (int element = 0; element < 5; ++element) {
        rassert(set.get_set(element) == element, 23892803643);
        rassert(set.get_set_size(element) == 1, 238928031);
    }
    set = DisjointSet(10000000);
    for(int i = 0; i<10000000; i++){
        int v1 = rnd()%10000000, v2 = rnd()%10000000;
        set.union_sets(v1, v2);
        rassert(set.get_set(v1)==set.get_set(v2), "aaa");
    }
}
//!!!дальше писал в прошлом файле чтобы все классы не переносить

// TODO 200 перенесите сюда основную часть кода из прошлого задания про вычитание фона по первому кадру, но:
// 1) добавьте сохранение на диск визуализации картинок:
// 1.1) картинка эталонного фона
// 1.2) картинка текущего кадра
// 1.3) картинка визуализирующая маску "похож ли пиксель текущего кадра на эталонный фон"
// 1.4) картинка визуализирующая эту маску после применения волшебства морфологии
// 1.5) картинка визуализирующая эту маску после применения волшебства СНМ (системы непересекающихся множеств)
// 2) сохраняйте эти картинки визуализации только для тех кадров, когда пользователем был нажат пробел (код 32)
// 3) попробуйте добавить с помощью нажатия каких то двух кнопок "усиление/ослабление подавления фона"
// 4) попробуйте поменять местами морфологию и СНМ
// 5) попробуйте добавить настройку параметров морфологии и СНМ по нажатию кнопок (и выводите их значения в консоль)
void backgroundMagickStreaming() {

}

int main() {
    try {
        testingMyDisjointSets();
//        backgroundMagickStreaming();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
