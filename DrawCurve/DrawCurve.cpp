#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

struct point
{
    int x, y;
};

struct color
{
    float r, g, b;
}
red = { 1.0f, 0.0f, 0.0f },
green = { 0.0f, 1.0f, 0.0f };

void plot(int x, int y, struct color rgb)
{
    glColor3f(rgb.r, rgb.g, rgb.b);
    glVertex2f(float(x) / (float(SCR_WIDTH) / 2), float(y) / (float(SCR_HEIGHT) / 2));
}

void DDALine(struct point p0, struct point p1, struct color rgb)
{
    int x0 = p0.x - SCR_WIDTH / 2, y0 = p0.y - SCR_HEIGHT / 2;
    int x1 = p1.x - SCR_WIDTH / 2, y1 = p1.y - SCR_HEIGHT / 2;
    float x = x0;
    float y = y0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int epsl = std::max(std::abs(dx), std::abs(dy));
    float xIncre = (float)dx / (float)epsl;
    float yIncre = (float)dy / (float)epsl;
    glPointSize(2);
    glBegin(GL_POINTS);
    for (int k = 0; k <= epsl; k++)
    {
        plot(x + 0.5f, y + 0.5f, rgb);
        x += xIncre;
        y += yIncre;
    }
    glEnd();
}

//三次贝塞尔曲线
float bezier3funcX(float _t, struct point *controlP) { // cal x coor
    float part0 = controlP[0].x * _t * _t * _t;
    float part1 = 3 * controlP[1].x * _t * _t * (1 - _t);
    float part2 = 3 * controlP[2].x * _t * (1 - _t) * (1 - _t);
    float part3 = controlP[3].x * (1 - _t) * (1 - _t) * (1 - _t);
    return part0 + part1 + part2 + part3;
}

float bezier3funcY(float _t, struct point *controlP) {    // cal y coor
    float part0 = controlP[0].y * _t * _t * _t;
    float part1 = 3 * controlP[1].y * _t * _t * (1 - _t);
    float part2 = 3 * controlP[2].y * _t * (1 - _t) * (1 - _t);
    float part3 = controlP[3].y * (1 - _t) * (1 - _t) * (1 - _t);
    return part0 + part1 + part2 + part3;
}

float cal_angle(float _t, struct point *controlP) { // cal dy / dx angle
    float _dx_1 = 3 * controlP[0].x * _t * _t;
    float _dx_2 = 3 * controlP[1].x * (_t * 2 - 3 * _t * _t);
    float _dx_3 = 3 * controlP[2].x * (1 - 4 * _t + 3 * _t * _t);
    float _dx_4 = -3 * controlP[3].x * (1 - _t) * (1 - _t);
    float _dy_1 = 3 * controlP[0].y * _t * _t;
    float _dy_2 = 3 * controlP[1].y * (_t * 2 - 3 * _t * _t);
    float _dy_3 = 3 * controlP[2].y * (1 - 4 * _t + 3 * _t * _t);
    float _dy_4 = -3 * controlP[3].y * (1 - _t) * (1 - _t);
    return std::atan2(_dy_1 + _dy_2 + _dy_3 + _dy_4, _dx_1 + _dx_2 + _dx_3 + _dx_4);
}

void BezierCurve(std::vector<struct point>& originPoint) {
    //控制点收缩系数 ，经调试0.6较好，CvPoint是opencv的，可自行定义结构体(x,y)
    float scale = 0.6;
    int originCount = originPoint.size();
    std::vector<struct point> midpoints;
    std::vector<struct point> curvePoint;

    //生成中点
    for (int i = 0; i < originCount; i++) {
        int nexti = (i + 1) % originCount;
        int x = (originPoint[i].x + originPoint[nexti].x) / 2.0;
        int y = (originPoint[i].y + originPoint[nexti].y) / 2.0;
        midpoints.push_back({ x, y });
    }

    //平移中点
    std::vector<struct point> extrapoints(2 * originCount);
    for (int i = 0; i < originCount; i++) {
        int nexti = (i + 1) % originCount;
        int backi = (i + originCount - 1) % originCount;
        struct point midinmid;
        midinmid.x = (midpoints[i].x + midpoints[backi].x) / 2.0;
        midinmid.y = (midpoints[i].y + midpoints[backi].y) / 2.0;
        int offsetx = originPoint[i].x - midinmid.x;
        int offsety = originPoint[i].y - midinmid.y;
        int extraindex = 2 * i;
        extrapoints[extraindex].x = midpoints[backi].x + offsetx;
        extrapoints[extraindex].y = midpoints[backi].y + offsety;
        //朝 originPoint[i]方向收缩
        int addx = (extrapoints[extraindex].x - originPoint[i].x) * scale;
        int addy = (extrapoints[extraindex].y - originPoint[i].y) * scale;
        extrapoints[extraindex].x = originPoint[i].x + addx;
        extrapoints[extraindex].y = originPoint[i].y + addy;

        int extranexti = (extraindex + 1) % (2 * originCount);
        extrapoints[extranexti].x = midpoints[i].x + offsetx;
        extrapoints[extranexti].y = midpoints[i].y + offsety;
        //朝 originPoint[i]方向收缩
        addx = (extrapoints[extranexti].x - originPoint[i].x) * scale;
        addy = (extrapoints[extranexti].y - originPoint[i].y) * scale;
        extrapoints[extranexti].x = originPoint[i].x + addx;
        extrapoints[extranexti].y = originPoint[i].y + addy;

    }

    struct point controlPoint[4];
    //生成4控制点，产生贝塞尔曲线
    for (int i = 0; i < originCount; i++) {
        controlPoint[0] = originPoint[i];
        int extraindex = 2 * i;
        controlPoint[1] = extrapoints[extraindex + 1];
        int extranexti = (extraindex + 2) % (2 * originCount);
        controlPoint[2] = extrapoints[extranexti];
        int nexti = (i + 1) % originCount;
        controlPoint[3] = originPoint[nexti];
        float u = 1;
        while (u >= 0) {
            int px = bezier3funcX(u, controlPoint);
            int py = bezier3funcY(u, controlPoint);
            //u的步长决定曲线的疏密
            u -= 0.005;
            //存入曲线点
            curvePoint.push_back({ px, py });
        }
    }

    int size = curvePoint.size();
    for (int i = 0; i < size - 1; ++i)
    {
        DDALine(curvePoint.at(i), curvePoint.at(i + 1), red);
    }
    for (int i = 0; i < 3; ++i)
    {
        DDALine(controlPoint[i], controlPoint[i + 1], green);
    }
    DDALine(controlPoint[3], controlPoint[0], green);
}

float F03(float t)
{
    return 1.0 / 6 * (-t * t*t + 3 * t*t - 3 * t + 1);
}

float F13(float t)
{
    return 1.0 / 6 * (3 * t*t*t - 6 * t*t + 4);
}

float F23(float t)
{
    return 1.0 / 6 * (-3 * t*t*t + 3 * t*t + 3 * t + 1);
}

float F33(float t)
{
    return 1.0 / 6 * t*t*t;
}

void ThreeOrderBSplineSmooth(std::vector<struct point>& pt)
{
    int num = pt.size();
    std::vector<struct point> temp = std::vector<struct point>(num + 2);
    std::vector<struct point> curvePoint = std::vector<struct point>(num);
    struct point NodePt1, NodePt2, NodePt3, NodePt4;
    float t;

    for (int i = 0; i < num; i++)
        temp[i + 1] = pt[i];

    temp[0].x = 2 * temp[1].x - temp[2].x;
    temp[0].y = 2 * temp[1].y - temp[2].y;

    temp[num + 1].x = 2 * temp[num].x - temp[num - 1].x;
    temp[num + 1].y = 2 * temp[num].y - temp[num - 1].y;

    for (int i = 0; i < num - 1; i++)
    {
        NodePt1 = temp[i];
        NodePt2 = temp[i + 1];
        NodePt3 = temp[i + 2];
        NodePt4 = temp[i + 3];

        if (i == num - 4)
        {
            t = 0;
            curvePoint[i].x = F03(t)*NodePt1.x + F13(t)*NodePt2.x + F23(t)*NodePt3.x + F33(t)*NodePt4.x;
            curvePoint[i].y = F03(t)*NodePt1.y + F13(t)*NodePt2.y + F23(t)*NodePt3.y + F33(t)*NodePt4.y;
            t = 1;
            curvePoint[i + 1].x = F03(t)*NodePt1.x + F13(t)*NodePt2.x + F23(t)*NodePt3.x + F33(t)*NodePt4.x;
            curvePoint[i + 1].y = F03(t)*NodePt1.y + F13(t)*NodePt2.y + F23(t)*NodePt3.y + F33(t)*NodePt4.y;
        }
        else
        {
            t = 0;
            curvePoint[i].x = F03(t)*NodePt1.x + F13(t)*NodePt2.x + F23(t)*NodePt3.x + F33(t)*NodePt4.x;
            curvePoint[i].y = F03(t)*NodePt1.y + F13(t)*NodePt2.y + F23(t)*NodePt3.y + F33(t)*NodePt4.y;
        }
    }

    for (int i = 0; i < num - 1; ++i)
    {
        DDALine(curvePoint.at(i), curvePoint.at(i + 1), red);
    }
}

void ThreeOrderBSplineInterpolatePt(std::vector<struct point>& pt, std::vector<int>& resl)
{
    assert(pt.size() == resl.size() + 1);

    int num = pt.size();
    int insertNumSum = 0;
    std::vector<struct point> temp = std::vector<struct point>(num + 2);
    struct point NodePt1, NodePt2, NodePt3, NodePt4;
    float t;

    for (auto it = resl.begin(); it != resl.end(); ++it)
    {
        insertNumSum += *it;
    }

    for (int i = 0; i < num; i++)
        temp[i + 1] = pt[i];

    temp[0].x = 2 * temp[1].x - temp[2].x;                  //  将折线延长线上两点加入作为首点和尾点
    temp[0].y = 2 * temp[1].y - temp[2].y;

    temp[num + 1].x = 2 * temp[num].x - temp[num - 1].x;
    temp[num + 1].y = 2 * temp[num].y - temp[num - 1].y;

    std::vector<struct point> curvePoint = std::vector<struct point>(num + insertNumSum);
    int totalnum = 0;

    for (int i = 0; i < num - 1; i++)                          //  每条线段均匀插入点
    {
        NodePt1 = temp[i];
        NodePt2 = temp[i + 1];
        NodePt3 = temp[i + 2];
        NodePt4 = temp[i + 3];

        float dt = 1.0 / (resl[i] + 1);

        for (int j = 0; j < resl[i] + 1; j++)
        {
            t = dt * j;
            curvePoint[totalnum].x = F03(t)*NodePt1.x + F13(t)*NodePt2.x + F23(t)*NodePt3.x + F33(t)*NodePt4.x;
            curvePoint[totalnum].y = F03(t)*NodePt1.y + F13(t)*NodePt2.y + F23(t)*NodePt3.y + F33(t)*NodePt4.y;
            totalnum++;
        }

        if (i == num - 2) {              //  最后一个尾点
            t = 1;
            curvePoint[totalnum].x = F03(t)*NodePt1.x + F13(t)*NodePt2.x + F23(t)*NodePt3.x + F33(t)*NodePt4.x;
            curvePoint[totalnum].y = F03(t)*NodePt1.y + F13(t)*NodePt2.y + F23(t)*NodePt3.y + F33(t)*NodePt4.y;
            totalnum++;
        }
    }

    for (int i = 0; i < num + insertNumSum - 1; ++i)
    {
        DDALine(curvePoint.at(i), curvePoint.at(i + 1), red);
    }
}

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawCurve", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    std::vector<struct point> originPoint;
    originPoint.push_back({ 100, 300 });
    originPoint.push_back({ 120, 200 });
    originPoint.push_back({ 220, 200 });
    originPoint.push_back({ 270, 100 });
    originPoint.push_back({ 370, 100 });
    originPoint.push_back({ 420, 200 });
    originPoint.push_back({ 420, 300 });

    std::vector<int> resl(6, 10);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        BezierCurve(originPoint);

        ThreeOrderBSplineInterpolatePt(originPoint, resl);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
