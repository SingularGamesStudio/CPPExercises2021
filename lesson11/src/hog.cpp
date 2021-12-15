#include "hog.h"

#include <libutils/rasserts.h>

#include <opencv2/imgproc.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

const double pi = acos(-1);
const double eps = 1e-6;

bool init = false;
std::vector<HoG> hogs[26];
using namespace std;

HoG buildHoG(cv::Mat grad_x, cv::Mat grad_y) {
    rassert(grad_x.type() == CV_32FC1, 2378274892374008);
    rassert(grad_y.type() == CV_32FC1, 2378274892374008);

    rassert(grad_x.rows == grad_y.rows, 3748247980010);
    rassert(grad_x.cols == grad_y.cols, 3748247980011);
    int height = grad_x.rows;
    int width = grad_x.cols;

    HoG hog(NBINS, 0);

    // 1) увеличьте размер вектора hog до NBINS (ведь внутри это просто обычный вектор вещественных чисел)
    // 2) заполните его нулями
    // 3) пробегите по всем пикселям входной картинки и посмотрите на каждый градиент
    // (определенный двумя числами: dx проекцией на ось x в grad_x, dy проекцией на ось y в grad_y)
    // 4) определите его силу (корень из суммы квадратов), определите его угол направления:
    // рекомендую воспользоваться atan2(dy, dx) - он возвращает радианы - https://en.cppreference.com/w/cpp/numeric/math/atan2
    // прочитайте по ссылке на документацию (в прошлой строке) - какой диапазон значений у угла-результата atan2 может быть?
    // 5) внесите его силу как голос за соответствующую его углу корзину
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            double dx = grad_x.at<float>(j, i);
            double dy = grad_y.at<float>(j, i);
            double strength = sqrt(dx * dx + dy * dy);
            ///if (strength < 10) // пропускайте слабые градиенты, это нужно чтобы игнорировать артефакты сжатия в jpeg (например в line01.jpg пиксели не идеально белые/черные, есть небольшие отклонения)
            ///    continue;

            int bin = floor(((atan2(dy, dx)+pi)/(2*pi)-eps)*NBINS);

            rassert(bin >= 0, 3842934728039);
            rassert(bin < NBINS, 34729357289040);
            hog[bin] += strength;
        }
    }
    double sum = 0;
    for (double f:hog)
        sum+=f;
    for (double &f:hog)
        f/=sum;
    rassert(hog.size() == NBINS, 23478937290010);
    return hog;
}

// Эта функция просто преобразует картинку в черно-белую, строит градиенты и вызывает buildHoG объявленный выше
HoG buildHoG(cv::Mat originalImg) {
    cv::Mat img = originalImg.clone();

    rassert(img.type() == CV_8UC3, 347283678950077);

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY); // преобразуем в оттенки серого

    cv::Mat grad_x, grad_y; // в этих двух картинках мы получим производную (градиент=gradient) по оси x и y
    // для этого достаточно дважды применить оператор Собеля (реализованный в OpenCV)
    cv::Sobel(img, grad_x, CV_32FC1, 1, 0);
    cv::Sobel(img, grad_y, CV_32FC1, 0, 1);
    rassert(!grad_x.empty(), 234892748239070017);
    rassert(!grad_y.empty(), 234892748239070018);

    HoG hog = buildHoG(grad_x, grad_y);
    return hog;
}

// Пример корректного вывода (выводите не само значение накопленных голосов за каждое направление, а процент от общей суммы голосов):
// HoG[22.5=0%, 67.5=78%, 112.5=21%, 157.5=0%, 202.5=0%, 247.5=0%, 292.5=0%, 337.5=0%]
std::ostream &operator<<(std::ostream &os, const HoG &hog) {
    rassert(hog.size() == NBINS, 234728497230016);

    os << "HoG[";
    for (int bin = 0; bin < NBINS; ++bin) {
        double angleInDegrees = (360.0/NBINS)*bin+(360.0/NBINS/2.0);
        double percentage = hog[bin];
        os << (double)((int)(angleInDegrees * 10 + 0.5))/10.0 << "=" << (double)((int)(percentage * 1000 + 0.5))/10.0 << "%, ";
    }
    os << "]";
    return os;
}


double distance(HoG a, HoG b) {
    rassert(a.size() == NBINS, 237281947230077);
    rassert(b.size() == NBINS, 237281947230078);
    double res = 0.0;
    for(int i = 0; i<NBINS; i++){
        res+=(a[i]-b[i])*(a[i]-b[i]);
    }
    res = sqrt(res);
    // подумайте - как можно добавить независимость (инвариантность) гистаграммы градиентов к тому насколько контрастная или блеклая картинка?
    // подсказка: на контрастной картинке все градиенты гораздо сильнее, а на блеклой картинке все градиенты гораздо слабее, но пропорции между градиентами (распроцентовка) не изменны!


    return res;
}

int randFont() {
    int fonts[] = {
            cv::FONT_HERSHEY_SIMPLEX,
            cv::FONT_HERSHEY_PLAIN,
            cv::FONT_HERSHEY_DUPLEX,
            cv::FONT_HERSHEY_COMPLEX,
            cv::FONT_HERSHEY_TRIPLEX,
            cv::FONT_HERSHEY_COMPLEX_SMALL,
            cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
            cv::FONT_HERSHEY_SCRIPT_COMPLEX,
            };
    // Выбираем случайный шрифт из тех что есть в OpenCV
    int nfonts = (sizeof(fonts) / sizeof(int));
    int font = rand() % nfonts;

    // С вероятностью 20% делаем шрифт наклонным (italic)
    bool is_italic = ((rand() % 5) == 0);
    if  (is_italic) {
        font = font | cv::FONT_ITALIC;
    }

    return font;
}

double randFontScale() {
    double min_scale = 2.5;
    double max_scale = 3.0;
    double scale = min_scale + (max_scale - min_scale) * ((rand() % 100) / 100.0);
    return scale;
}

int randThickness() {
    int min_thickness = 2;
    int max_thickness = 3;
    int thickness = min_thickness + rand() % (max_thickness - min_thickness + 1);
    return thickness;
}

cv::Scalar randColor1() {
    return cv::Scalar(rand() % 128, rand() % 128, rand() % 128); // можно было бы брать по модулю 255, но так цвета будут темнее и контрастнее
}

cv::Mat generateImage(std::string text, int width=128, int height=128) {
    cv::Scalar white(255, 255, 255);
    cv::Scalar backgroundColor = white;
    // Создаем картинку на которую мы нанесем символ (пока что это просто белый фон)
    cv::Mat img(height, width, CV_8UC3, backgroundColor);

    // Выберем случайные параметры отрисовки текста - шрифт, размер, толщину, цвет
    int font = randFont();
    double fontScale = randFontScale();
    int thickness = randThickness();
    cv::Scalar color = randColor1();

    // Узнаем размер текста в пикселях (если его нарисовать с указанными параметрами)
    int baseline = 0;
    cv::Size textPixelSize = cv::getTextSize(text, font, fontScale, thickness, &baseline);

    // Рисуем этот текст идеально в середине картинки
    // (для этого и нужно было узнать размер текста в пикселях - чтобы сделать отступ от середины картинки)
    // (ведь при рисовании мы указываем координаты левого нижнего угла текста)
    int xLeft = (width / 2) - (textPixelSize.width / 2);
    int yBottom = (height / 2) + (textPixelSize.height / 2);
    cv::Point coordsOfLeftBorromCorner(xLeft, yBottom);
    cv::putText(img, text, coordsOfLeftBorromCorner, font, fontScale, color, thickness);

    return img;
}

char get(cv::Mat img){
    if(!init){
        init=true;
        for(char c = 'a'; c<='z'; c++){
            for(int i = 0; i<10; i++){
                cv::Mat img1 = generateImage(""+c);
                hogs[c-'a'].push_back(buildHoG(img1));
            }
        }
    }
    HoG my = buildHoG(img);
    pair<HoG, int> mind = {HoG(), -1};
    for(int ch = 0; ch<26; ch++){
        for(auto hog:hogs[ch]){
            if(mind.second==-1 || distance(my, hog)< distance(my, mind.first)){
                mind = {hog, ch};
            }
        }
    }
    return mind.second+'a';
}