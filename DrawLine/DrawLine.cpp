#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

void plot(int x, int y)
{
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(float(x) / (float(SCR_WIDTH) / 2), float(y) / (float(SCR_HEIGHT) / 2));
}

void DDALine(int x0, int y0, int x1, int y1)
{
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
        plot(x + 0.5f, y + 0.5f);
        x += xIncre;
        y += yIncre;
    }
    glEnd();
}

// 不考虑斜率为0或无穷大的情况
void MPLine(int x0, int y0, int x1, int y1)
{
    int x, y, d0, d1, d2, a, b;
    if (x1 < x0)
    {
        x = x1;
        x1 = x0;
        x0 = x;
        y = y1;
        y1 = y0;
        y0 = y;
    }
    else
    {
        x = x0;
    }

    y = y0;
    a = y0 - y1;
    b = x1 - x0;
    float k = -a * 1.0 / b;

    glPointSize(2);
    glBegin(GL_POINTS);

    if (k >= 0 && k <= 1)
    {
        d0 = 2 * a + b;
        d1 = 2 * a;
        d2 = 2 * (a + b);
        for (x = x0; x <= x1; x++) {
            plot(x, y);
            if (d0 < 0) {
                y++;
                d0 += d2;
            }
            else {
                d0 += d1;
            }
        }
    }
    else if (k > 1)
    {
        d0 = a + 2 * b;
        d1 = 2 * (a + b);
        d2 = 2 * b;
        for (y = y0; y <= y1; y++) {
            plot(x, y);
            if (d0 < 0) {
                d0 += d2;
            }
            else {
                x++;
                d0 += d1;
            }
        }
    }
    else if (k >= -1 && k < 0)
    {
        d0 = 2 * a - b;
        d1 = 2 * (a - b);
        d2 = 2 * a;
        for (x = x0; x <= x1; x++) {
            plot(x, y);
            if (d0 < 0) {
                d0 += d2;
            }
            else {
                y--;
                d0 += d1;
            }
        }
    }
    else if (k < -1)
    {
        d0 = a - 2 * b;
        d1 = -2 * b;
        d2 = 2 * (a - b);
        for (y = y0; y >= y1; y--) {
            plot(x, y);
            if (d0 < 0) {
                x++;
                d0 += d2;
            }
            else {
                d0 += d1;
            }
        }
    }
        
    glEnd();
}

// 不考虑斜率为0或无穷大的情况
void BresenhamLine(int x0, int y0, int x1, int y1)
{
    int x, y, dx, dy, d;
    if (x1 < x0)
    {
        x = x1;
        x1 = x0;
        x0 = x;
        y = y1;
        y1 = y0;
        y0 = y;
    }
    else
    {
        x = x0;
    }
    y = y0;
    dx = x1 - x0;
    dy = y1 - y0;
    float k = dy * 1.0 / dx;

    glPointSize(2);
    glBegin(GL_POINTS);

    if (k < 1 && k >= 0)
    {
        d = 2 * dy - dx;
        for (x = x0; x <= x1; x++) {
            plot(x, y);
            if (d < 0) {
                d += 2 * dy;
            }
            else {
                y++;
                d += 2 * dy - 2 * dx;
            }
        }
    }
    else if (k > -1 && k < 0)
    {
        d = 2 * dy + dx;
        for (x = x0; x <= x1; x++) {
            plot(x, y);
            if (d >= 0) {
                d += 2 * dy;
            }
            else {
                y--;
                d += 2 * dx + 2 * dy;
            }
        }
    }
    else if (k >= 1)
    {
        d = dy;
        for (y = y0; y <= y1; y++) {
            plot(x, y);
            if (d < 0) {
                d += 2 * dx;
            }
            else {
                x++;
                d += 2 * dx - 2 * dy;
            }
        }
    }
    else if (k <= -1)
    {
        d = 2 * dx - dy;
        for (y = y0; y >= y1; y--) {
            plot(x, y);
            if (d >= 0) {
                d -= 2 * dx;
            }
            else {
                x++;
                d -= 2 * dx + 2 * dy;
            }
        }
    }
    
    glEnd();
}

void CirclePlot(int x, int y, int p, int q)
{
    plot(x + p, y + q);
    plot(x - p, y + q);
    plot(x + p, y - q);
    plot(x - p, y - q);
    plot(x + q, y + p);
    plot(x - q, y + p);
    plot(x + q, y - p);
    plot(x - q, y - p);
}

void MPCircle(int x0, int y0, int r)
{
    int x, y;
    float d;

    x = 0;
    y = r;
    d = 1.25 - r;

    glPointSize(2);
    glBegin(GL_POINTS);

    CirclePlot(x0, y0, x, y);

    while (x < y)
    {
        if (d < 0)
        {
            d += 2 * x + 3;
        }
        else
        {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
        CirclePlot(x0, y0, x, y);
    }

    glEnd();
}

void BresenhamCircle(int x0, int y0, int r)
{
    int x, y, d;

    x = 0;
    y = r;
    d = 3 - 2 * r;

    glPointSize(2);
    glBegin(GL_POINTS);

    CirclePlot(x0, y0, x, y);

    while (x < y)
    {
        if (d < 0)
        {
            d += 4 * x + 6;
        }
        else
        {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
        CirclePlot(x0, y0, x, y);
    }

    glEnd();
}

void EllipsePlot(int x0, int y0, int x, int y)
{
    plot(x0 + x, y0 + y);
    plot(x0 + x, y0 - y);
    plot(x0 - x, y0 + y);
    plot(x0 - x, y0 - y);
}

void MPEllipse(int x0, int y0, int a, int b)
{
    int x = 0, y = b;
    int aa = a * a, bb = b * b;
    int d = (int)(bb + aa * (-b + 0.25) + 0.5);

    glPointSize(2);
    glBegin(GL_POINTS);

    EllipsePlot(x0, y0, x, y);

    while (bb * x < aa * y)
    {
        if (d < 0) {
            d += bb * (2 * x + 3);
            x++;
        }
        else {

            d += bb * (2 * x + 3) + aa * (-2 * y + 2);
            y--;
            x++;
        }
        EllipsePlot(x0, y0, x, y);
    }

    d = (int)(bb * (x + 0.5) * (x + 0.5) + aa * (y - 1) * (y - 1) - aa * bb + 0.5);

    while (y > 0) {
        if (d > 0) {
            d += aa * (-2 * y + 3);
            y--;
        }
        else {
            d += bb * (2 * x + 2) + aa * (-2 * y + 3);
            x++;
            y--;
        }
        EllipsePlot(x0, y0, x, y);
    }

    glEnd();
}

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawLine", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        DDALine(-300, -200, 200, 400);

        MPLine(-200, 300, 200, -400);

        BresenhamLine(-300, 100, 400, -360);

        MPCircle(0, 0, 200);

        BresenhamCircle(200, 80, 160);

        MPEllipse(-100, -250, 160, 100);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
