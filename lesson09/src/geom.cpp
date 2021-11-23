#include <iostream>
#include "hough.h"
#include <cmath>
#define x first
#define y second
#define int long long
#define ld long double
using namespace std;


const ld pi = acos(-1);
const ld eps = 1e-9;

struct dot;

struct ddot{
    ld x, y;

    ddot(dot p);

    ddot()
    {
        x = 0;
        y = 0;
    }
    ddot(ld _x, ld _y)
    {
        x = _x;
        y = _y;
    }
    ddot operator+(ddot p)
    {
        return ddot(x + p.x, y + p.y);
    }
    ddot operator-(ddot p)
    {
        return ddot(x - p.x, y - p.y);
    }
    ld operator%(ddot p)
    {
        return x * p.y - y * p.x;
    }

    ld operator*(ddot p)
    {
        return x * p.x + y * p.y;
    }

    ddot operator*(ld k)
    {
        return ddot{ x * k, y * k };
    }

    ld getlen()
    {
        return sqrt(x * x + y * y);
    }

    ld getsquarelen()
    {
        return x * x + y * y;
    }

    bool operator==(ddot p)
    {
        return abs(p.x-x)<eps && abs(p.y-y)<eps;
    }

    ddot normalized()
    {
        return ddot(x / getlen(), y / getlen());
    }
};

struct dot {
    int x, y;
    dot()
    {
        x = 0;
        y = 0;
    }

    dot(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
    dot operator+(dot p)
    {
        return dot(x + p.x, y + p.y);
    }
    dot operator-(dot p)
    {
        return dot(x - p.x, y - p.y);
    }
    int operator%(dot p)
    {
        return x * p.y - y * p.x;
    }

    operator ddot(){
        return ddot(x, y);
    }

    int operator*(dot p)
    {
        return x * p.x + y * p.y;
    }

    dot operator*(int k)
    {
        return dot{ x * k, y * k };
    }

    ld getlen()
    {
        return sqrt(x * x + y * y);
    }

    int getsquarelen()
    {
        return x * x + y * y;
    }

    bool operator==(dot p)
    {
        return (p.x == x) && (p.y == y);
    }

    ddot normalized()
    {
        return ddot(x, y).normalized();
    }
};

ddot::ddot(dot p){
        x = p.x;
        y = p.y;
    }

bool iscent(dot p1, dot p2, ddot c)
{
    if ((c.x <= p1.x + eps && c.x + eps >= p2.x) || (c.x + eps >= p1.x && c.x <= p2.x + eps)) {
        if ((c.y <= p1.y + eps && c.y + eps >= p2.y) || (c.y + eps >= p1.y && c.y <= p2.y + eps)) {
            return 1;
        }
    }
    return 0;
}

dot infv = dot(LLONG_MAX, LLONG_MAX);

ld getangle(dot a, dot b)
{
    return atan2(a % b, a * b);
}

ld getgoodangle(dot a, dot b)
{
    ld res = atan2(a % b, a * b);
    if (res < 0)
        res += 2 * pi;
    return res;
}

ld getverygoodangle(dot a, dot b)
{
    ld res = atan2(a % b, a * b);
    if (res < 0)
        res += 2 * pi;
    if (res > pi)
        res = 2 * pi - res;
    return res;
}

struct line {
    int a, b, c;
    dot on1, on2;
    line(dot p, dot q)
    {
        a = p.y - q.y;
        b = q.x - p.x;
        c = -a * p.x - b * p.y;
        on1 = p;
        on2 = q;
    }
    line(int k, int _b)
    {
        a = k;
        b = -1;
        c = _b;
        on1 = dot(1, a + c);
        on2 = dot(2, 2 * a + c);
    }
    line(int _a, int _b, int _c)
    {
        a = _a;
        b = _b;
        c = _c;
    }
    bool ison(dot p)
    {
        if (a * p.x + b * p.y + c == 0)
            return 1;
        return 0;
    }
    ddot intersect(line l)
    {
        if (l.b != 0) {
            ld x = ((((ld)b) * l.c / l.b - c) / (a - ((ld)b) * l.a / l.b));
            ld y = (-x * l.a - l.c) / l.b;
            return ddot(x, y);
        }
        else {
            ld x = -((ld)l.c) / l.a;
            ld y = (-x * a - c) / b;
            return ddot(x, y);
        }
    }
};

struct dline {
    ld a, b, c;
    ddot on1, on2;
    dline(ddot p, ddot q)
    {
        a = p.y - q.y;
        b = q.x - p.x;
        c = -a * p.x - b * p.y;
        on1 = p;
        on2 = q;
    }
    dline(ld k, ld _b)
    {
        a = k;
        b = -1;
        c = _b;
        on1 = ddot(1, a + c);
        on2 = ddot(2, 2 * a + c);
    }
    dline(ld _a, ld _b, ld _c)
    {
        a = _a;
        b = _b;
        c = _c;
    }
    bool ison(ddot p)
    {
        if (abs(a * p.x + b * p.y + c) < eps)
            return 1;
        return 0;
    }
    ddot intersect(dline l)
    {
        if (abs(l.b) >eps) {
            ld x = ((((ld)b) * l.c / l.b - c) / (a - ((ld)b) * l.a / l.b));
            ld y = (-x * l.a - l.c) / l.b;
            return ddot(x, y);
        }
        else  if(abs(l.a)>eps && abs(b)>eps){
            ld x = -((ld)l.c) / l.a;
            ld y = (-x * a - c) / b;
            return ddot(x, y);
        } else return ddot(-1000000000, -1000000000);
    }
};

ld getdist(line l, dot a)
{
    if (l.ison(a))
        return 1;
    dot parr = l.on1 - l.on2;
    dot norm = dot(-parr.y, parr.x);
    dot r = l.on1 - a;
    ld ang = getverygoodangle(r, norm);
    if (ang > pi / 2) {
        norm = norm * -1;
        ang = pi - ang;
    }
    ang = pi / 2 - ang;
    return r.getlen() * sin(ang);
}

ddot perp(line l, dot a)
{
    dot parr = l.on1 - l.on2;
    dot norm = dot(-parr.y, parr.x);
    dot r = l.on1 - a;
    ld ang = getverygoodangle(r, norm);
    if (ang > pi / 2) {
        norm = norm * -1;
        ang = pi - ang;
    }
    ang = pi / 2 - ang;
    ld dd = r.getlen() * sin(ang);
    ddot ddd = norm.normalized()*dd;
    return ddd+ddot(a);
}


cv::Point PolarLineExtremum::intersect(PolarLineExtremum that)
{
    // Одна прямая - наш текущий объект (this) у которого был вызван этот метод, у этой прямой такие параметры:
    double theta0 = ((90-(int)this->theta)+360)%360;
    double r0 = this->r;

    // Другая прямая - другой объект (that) который был передан в этот метод как аргумент, у этой прямой такие параметры:
    double theta1 = ((90-(int)that.theta)+360)%360;
    double r1 = that.r;

    // TODO реализуйте поиск пересечения этих двух прямых, напоминаю что формула прямой описана тут - https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/02/lesson8-hough-transform.html
    // после этого загуглите как искать пересечение двух прямых, пример запроса: "intersect two 2d lines"
    // и не забудьте что cos/sin принимают радианы (используйте toRadians)

    ddot p1 = ddot(r0*cos(theta0/180.0*pi), r0*sin(theta0/180.0*pi));
    ddot pperp = ddot(10*cos(theta0/180.0*pi), 10*sin(theta0/180.0*pi));
    ddot p2 = ddot(p1.x-pperp.y, p1.y+pperp.x);
    dline l0 = dline(p1, p2);
    p1 = ddot(r1*cos(theta1/180.0*pi), r1*sin(theta1/180.0*pi));
    pperp = ddot(10*cos(theta1/180.0*pi), 10*sin(theta1/180.0*pi));
    p2 = ddot(p1.x-pperp.y, p1.y+pperp.x);
    dline l1 = dline(p1, p2);

    ddot res = l1.intersect(l0);

    int x = (int) round(res.x);
    int y = (int) round(res.y);
    return cv::Point(x, y);
}