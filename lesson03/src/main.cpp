#include <iostream>
#include <filesystem> // это нам понадобится чтобы создать папку для результатов
#include <libutils/rasserts.h>
#include "helper_functions.h"

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем читать и сохранять картинки

using namespace cv;
using namespace std;

void task1() {
    cv::Mat imgUnicorn = cv::imread("lesson03/data/unicorn.png");  // загружаем картинку с единорогом
    rassert(!imgUnicorn.empty(), 3428374817241); // проверяем что картинка загрузилась (что она не пустая)

    // TODO выведите в консоль разрешение картинки (ширина x высота)
    int width = imgUnicorn.cols; // как в ООП - у картинки есть поля доступные через точку, они называются cols и rows - попробуйте их
    int height = imgUnicorn.rows;
    std::cout << "Unicorn image loaded: " << width << "x" << height << std::endl;

    // создаем папку в которую будем сохранять результаты - lesson03/resultsData/
    std::string resultsDir = "lesson03/resultsData/";
    if (!std::filesystem::exists(resultsDir)) { // если папка еще не создана
        std::filesystem::create_directory(resultsDir); // то создаем ее
    }

    cv::Mat blueUnicorn = makeAllBlackPixelsBlue(imgUnicorn.clone(), false); // TODO реализуйте функцию которая каждый пиксель картинки который близок к белому - делает синим
    std::string filename = resultsDir + "01_blue_unicorn.jpg"; // удобно в начале файла писать число, чтобы файлы были в том порядке в котором мы их создали
    cv::imwrite(filename, blueUnicorn);

    cv::Mat invertedUnicorn = invertImageColors(imgUnicorn.clone()); // TODO реализуйте функцию которая каждый цвет картинки инвертирует
    // TODO сохраните резльутат в ту же папку, но файл назовите "02_inv_unicorn.jpg"
    filename = resultsDir + "02_inv_unicorn.jpg";
    imwrite(filename, invertedUnicorn);

    cv::Mat castle = imread("lesson03/data/castle.png"); // TODO считайте с диска картинку с замком - castle.png
    rassert(!castle.empty(), "no castle file");
    cv::Mat unicornInCastle = addBackgroundInsteadOfBlackPixels(imgUnicorn.clone(), castle.clone()); // TODO реализуйте функцию которая все черные пиксели картинки-объекта заменяет на пиксели с картинки-фона
    filename = resultsDir + "03_unicorn_castle.jpg";
    imwrite(filename, unicornInCastle);
    // TODO сохраните результат в ту же папку, назовите "03_unicorn_castle.jpg"

    cv::Mat largeCastle = imread("lesson03/data/castle_large.jpg");; // TODO считайте с диска картинку с большим замком - castle_large.png
    rassert(!largeCastle.empty(), "no big castle file");
    cv::Mat unicornInLargeCastle = addBackgroundInsteadOfBlackPixelsLargeBackground(imgUnicorn.clone(), largeCastle.clone(), false); // TODO реализуйте функцию так, чтобы нарисовался объект ровно по центру на данном фоне, при этом черные пиксели объекта не должны быть нарисованы
    // TODO сохраните результат - "04_unicorn_large_castle.jpg"
    filename = resultsDir + "04_unicorn_large_castle.jpg";
    imwrite(filename, unicornInLargeCastle);

    // TODO сделайте то же самое, но теперь пусть единорог рисуется N раз (случайно выбранная переменная от 0 до 100)
    cv::Mat manyUnicorns = manyObjects(imgUnicorn.clone(), largeCastle.clone());
    filename = resultsDir + "05_unicorns_otake.jpg";
    imwrite(filename, manyUnicorns);

    // TODO растяните картинку единорога так, чтобы она заполнила полностью большую картинку с замком "06_unicorn_upscale.jpg"
    cv::Mat bigUnicorn = bigObject(imgUnicorn.clone(), largeCastle.clone());
    filename = resultsDir + "06_unicorn_upscale.jpg";
    imwrite(filename, bigUnicorn);
}



void task2() {
    cv::Mat imgUnicorn = cv::imread("lesson03/data/unicorn.png");
    rassert(!imgUnicorn.empty(), 3428374817241);

    // cv::waitKey - функция некоторое время ждет не будет ли нажата кнопка клавиатуры, если да - то возвращает ее код
    int updateDelay = 10; // указываем сколько времени ждать нажатия кнопки клавиатуры - в миллисекундах
    while (cv::waitKey(updateDelay) != 32) {
        Mat imgUnicornRes = makeAllBlackPixelsBlue(imgUnicorn.clone(), true);
        cv::imshow("lesson03 window", imgUnicornRes);
        // TODO сделайте функцию которая будет все черные пиксели (фон) заменять на случайный цвет (аккуратно, будет хаотично и ярко мигать, не делайте если вам это противопоказано)
    }
}



struct MyVideoContent {
    cv::Mat frame;
    int lastClickX;
    int lastClickY;
};
vector<pair<int, int>> clicks3;
pair<int, int> sz3;
bool inv3 = false;
void onMouseClick3(int event, int x, int y, int flags, void *pointerToMyVideoContent) {
    MyVideoContent &content = *((MyVideoContent*) pointerToMyVideoContent);
    // не обращайте внимание на предыдущую строку, главное что важно заметить:
    // content.frame - доступ к тому кадру что был только что отображен на экране
    // content.lastClickX - переменная которая вам тоже наверняка пригодится
    // вы можете добавить своих переменных в структурку выше (считайте что это описание объекта из ООП, т.к. почти полноценный класс)

    if (event == cv::EVENT_LBUTTONDOWN) { // если нажата левая кнопка мыши
        int x1 = y, y1 = x;
        std::cout << "Left click at x=" << x1 << ", y=" << y1 << std::endl;
        clicks3.push_back({x1, y1});
    } else if (event == cv::EVENT_RBUTTONDOWN) { // если нажата левая кнопка мыши
        inv3 = !inv3;
    }
}
void task3() {
    // давайте теперь вместо картинок подключим видеопоток с веб камеры:
    cv::VideoCapture video(0);
    // если у вас нет вебкамеры - подключите ваш телефон к компьютеру как вебкамеру - это должно быть не сложно (загуглите)
    // альтернативно если у вас совсем нет вебки - то попробуйте запустить с видеофайла, но у меня не заработало - из-за "there is API version mismath: plugin API level (0) != OpenCV API level (1)"
    // скачайте какое-нибудь видео с https://www.videezy.com/free-video/chroma-key
    // например https://www.videezy.com/elements-and-effects/5594-interactive-hand-gesture-sliding-finger-studio-green-screen
    // если вы увидите кучу ошибок в консоли навроде "DynamicLib::libraryLoad load opencv_videoio_ffmpeg451_64.dll => FAILED", то скопируйте файл C:\...\opencv\build\x64\vc14\bin\opencv_videoio_ffmpeg451_64.dll в папку с проектом
    // и укажите путь к этому видео тут:
//    cv::VideoCapture video("lesson03/data/Spin_1.mp4");

    rassert(video.isOpened(), 3423948392481); // проверяем что видео получилось открыть

    MyVideoContent content; // здесь мы будем хранить всякие полезности - например последний видео кадр, координаты последнего клика и т.п.
    // content.frame - доступ к тому кадру что был только что отображен на экране
    // content.lastClickX - переменная которая вам тоже наверняка пригодится
    // вы можете добавить своих переменных в структурку выше (считайте что это описание объекта из ООП, т.к. почти полноценный класс)
    // просто перейдите к ее объявлению - удерживая Ctrl сделайте клик левой кнопкой мыши по MyVideoContent - и вас телепортирует к ее определению

    while (video.isOpened()) { // пока видео не закрылось - бежим по нему
        bool isSuccess = video.read(content.frame); // считываем из видео очередной кадр
        rassert(isSuccess, 348792347819); // проверяем что считывание прошло успешно
        rassert(!content.frame.empty(), 3452314124643); // проверяем что кадр не пустой
        sz3 = {content.frame.rows, content.frame.cols};
        for(auto z:clicks3){
            content.frame.at<Vec3b>(z.first, z.second) = Vec3b(0, 255, 0);
        }
        if(inv3){
            invertImageColors(content.frame);
        }

        cv::imshow("video", content.frame); // покаызваем очередной кадр в окошке
        cv::setMouseCallback("video", onMouseClick3, &content); // делаем так чтобы функция выше (onMouseClick) получала оповещение при каждом клике мышкой

        int key = cv::waitKey(10);
        if(key==27 || key==32)
            break;
        // TODO добавьте завершение программы в случае если нажат пробел
        // TODO добавьте завершение программы в случае если нажат Escape (придумайте как нагуглить)

        // TODO сохраняйте в вектор (std::vector<int>) координаты всех кликов мышки
        // TODO и перед отрисовкой очередного кадра - заполняйте все уже прокликанные пиксели красным цветом

        // TODO сделайте по правому клику мышки переключение в режим "цвета каждого кадра инвертированы" (можете просто воспользоваться функцией invertImageColors)
    }
}



vector<pair<int, int>> clicks4;
pair<int, int> sz4;
bool inv4 = false;
void onMouseClick4(int event, int x, int y, int flags, void *pointerToMyVideoContent) {
    MyVideoContent &content = *((MyVideoContent*) pointerToMyVideoContent);
    // не обращайте внимание на предыдущую строку, главное что важно заметить:
    // content.frame - доступ к тому кадру что был только что отображен на экране
    // content.lastClickX - переменная которая вам тоже наверняка пригодится
    // вы можете добавить своих переменных в структурку выше (считайте что это описание объекта из ООП, т.к. почти полноценный класс)

    if (event == cv::EVENT_LBUTTONDOWN) { // если нажата левая кнопка мыши
        int x1 = y, y1 = x;
        std::cout << "Left click at x=" << x1 << ", y=" << y1 << std::endl;
        clicks3.push_back({x1, y1});
    } else if (event == cv::EVENT_RBUTTONDOWN) { // если нажата левая кнопка мыши
        inv3 = !inv3;
    }
}
void task4() {

    cv::VideoCapture video(0);
    rassert(video.isOpened(), 3423948392481);

    MyVideoContent content;
    cout << "Exit the view of the camera, then press <Space>";
    Mat base;
    while (video.isOpened()) { // пока видео не закрылось - бежим по нему
        bool isSuccess = video.read(content.frame); // считываем из видео очередной кадр
        rassert(isSuccess, 348792347819); // проверяем что считывание прошло успешно
        rassert(!content.frame.empty(), 3452314124643); // проверяем что кадр не пустой
        cv::imshow("video", content.frame); // покаызваем очередной кадр в окошке
        int key = cv::waitKey(10);
        if(key==32){
            base = content.frame.clone();
            break;
        }
    }

    while (video.isOpened()) { // пока видео не закрылось - бежим по нему
        bool isSuccess = video.read(content.frame); // считываем из видео очередной кадр
        rassert(isSuccess, 348792347819); // проверяем что считывание прошло успешно
        rassert(!content.frame.empty(), 3452314124643); // проверяем что кадр не пустой
        Mat res = content.frame.clone();
        for(int i = 0; i<base.rows; i++){
            for(int j = 0; j<base.cols; j++){
                bool ok = 0;
                for(int di = -0; di<1; di++){
                    for(int dj = -0; dj<1; dj++){
                        if(i+di>=0 && i+di<base.rows && j+dj>=0 && j+dj<base.cols){
                            Vec3b color1 = base.at<Vec3b>(i, j);
                            Vec3b color2 = content.frame.at<Vec3b>(i+di, j+dj);
                            if(sqrt(abs(color1[0]-color2[0])*abs(color1[0]-color2[0])+abs(color1[1]-color2[1])*abs(color1[1]-color2[1])+abs(color1[2]-color2[2])*abs(color1[2]-color2[2]))<30){
                                ok = 1;
                            }
                        }
                    }
                }
                if(ok)
                    res.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }
        cv::Mat background = imread("lesson03/data/castle_large.jpg");
        for(int i = 0; i<res.rows; i++){
            for(int j = 0; j<res.cols; j++){
                Vec3b colNow = res.at<Vec3b>(i, j);
                if(colNow[0]==0 && colNow[1]==0 && colNow[2]==0)
                    res.at<Vec3b>(i, j) = background.at<Vec3b>((int)((((double)i)*background.rows)/res.rows), (int)((((double)j)*background.cols)/res.cols));
            }
        }

        cv::imshow("video", res); // покаызваем очередной кадр в окошке

        int key = cv::waitKey(10);
        if(key==27 || key==32)
            break;
    }
    // TODO на базе кода из task3 (скопируйте просто его сюда) сделайте следующее:
    // при клике мышки - определяется цвет пикселя в который пользователь кликнул, теперь этот цвет считается прозрачным (как было с черным цветом у единорога)
    // и теперь перед отрисовкой очередного кадра надо подложить вместо прозрачных пикселей - пиксель из отмасштабированной картинки замка (castle_large.jpg)

    // TODO попробуйте сделать так чтобы цвет не обязательно совпадал абсолютно для прозрачности (чтобы все пиксели похожие на тот что был кликнут - стали прозрачными, а не только идеально совпадающие)

    // TODO подумайте, а как бы отмаскировать фон целиком несмотря на то что он разноцветный?
    // а как бы вы справились с тем чтобы из фотографии с единорогом и фоном удалить фон зная как выглядит фон?
    // а может сделать тот же трюк с вебкой - выйти из вебки в момент запуска программы, и первый кадр использовать как кадр-эталон с фоном который надо удалять (делать прозрачным)
}

int main() {
    try {
//        task1();
//        task2();
//        task3();
        task4();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
