#include "some_math.h"
#include<bits/stdc++.h>
using namespace std;




//NOT SURE WHETHER IT WORKS, JETBRAINS STILL HASN'T ANSWERED TO MY CLION LICENSE APPLICATION




int fibbonachiRecursive(int n) {
    if(n<2)
        return 1;
    // TODO 03 реализуйте функцию которая считает числа Фибоначчи - https://ru.wikipedia.org/wiki/%D0%A7%D0%B8%D1%81%D0%BB%D0%B0_%D0%A4%D0%B8%D0%B1%D0%BE%D0%BD%D0%B0%D1%87%D1%87%D0%B8
    return fibbonachiRecursive(n-1)+fibbonachiRecursive(n-2);
}

int fibbonachiFast(int n) {
    // TODO 04 реализуйте быструю функцию Фибоначчи с использованием std::vector
    vector<int> fib(n+1);
    fib[0] = 1;
    fib[1] = 1;
    for(int i = 2; i<=n; i++){
        fib[i] = fib[i-1]+fib[i-2];
    }
    return fib[n];
}

double solveLinearAXB(double a, double b) {
    if(a==0){
        if(b==0)
            return -DBL_MAX;
        return DBL_MAX;
    }
    return -b/a;
    // TODO 10 решите линейное уравнение a*x+b=0 а если решения нет - верните наибольшее значение double - найдите как это сделать в интернете по запросу "so cpp double max value" (so = stackoverflow = сайт где часто можно найти ответы на такие простые запросы), главное НЕ КОПИРУЙТЕ ПРОСТО ЧИСЛО, ПОЖАЛУЙСТААаа
    // если решений сколь угодно много - верните максимальное значение со знаком минус
}

vector<double> solveSquare(double a, double b, double c) {
    if(a==0){
        double ans = solveLinearAXB(b, c);
        if(ans==DBL_MAX){
            return vector<double>();
        } else if(ans==-DBL_MAX){
            //???(infinite number of solutions)
            return vector<double>();
        }
        return vector<double>(1, ans);
    } else {
        double d = b*b-4*a*c;
        if(d<0){
            return vector<double>();
        } else if(d==0){
            return vector<double>(1, -b/(2*a));
        } else {
            int x1 = (-b+sqrt(d))/(2*a), x2 = (-b-sqrt(d))/(2*a);
            if(x2<x1)
                swap(x1, x2);
            vector<double> result;
            result.push_back(x1);
            result.push_back(x2);
            return result;
        }
    }
    // TODO 20 решите квадратное уравнение вида a*x^2+b*x+c=0
    // если корня два - они должны быть упорядочены по возрастанию
    // чтобы добавить в вектор элемент - нужно вызвать у него метод push_back:
}
