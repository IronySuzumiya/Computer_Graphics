#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <vector>

#define LEFT 1
#define RIGHT 2
#define BOTTOM 4
#define TOP 8

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

struct rectangle
{
    int XL, XR, YB, YT;
};

struct line
{
    int x0, y0, x1, y1;
};

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

int judge(float p, float q, float &u1, float &u2)
{
    float u;

    if (p > 0.0)
    {
        u = q / p;
        if (u < u1)
            return 0;
        else if (u < u2)
            u2 = u;
    }
    else if (p < 0.0)
    {
        u = q / p;
        if (u > u2)
            return 0;
        else if (u > u1)
            u1 = u;
    }
    else if (q < 0.0)
    {
        return 0;
    }

    return 1;
}


void LYDBarskeyLineClipping(struct rectangle rec, struct line ln)
{
    int x0 = ln.x0, x1 = ln.x1, y0 = ln.y0, y1 = ln.y1;
    int dx = x1 - x0, dy = y1 - y0;
    float u1 = 0.0f, u2 = 1.0f;

    DDALine(rec.XL, rec.YB, rec.XL, rec.YT);
    DDALine(rec.XR, rec.YB, rec.XR, rec.YT);
    DDALine(rec.XL, rec.YB, rec.XR, rec.YB);
    DDALine(rec.XL, rec.YT, rec.XR, rec.YT);

    if (judge(-dx, x0 - rec.XL, u1, u2))
    {
        if (judge(dx, rec.XR - x0, u1, u2))
        {
            if (judge(-dy, y0 - rec.YB, u1, u2))
            {
                if (judge(dy, rec.YT - y0, u1, u2))
                {
                    if (u2 < 1.0)
                    {
                        x1 = x0 + u2 * dx;
                        y1 = y0 + u2 * dy;
                    }
                    if (u1 > 0.0)
                    {
                        x0 = x0 + u1 * dx;
                        y0 = y0 + u1 * dy;
                    }
                    DDALine(x0, y0, x1, y1);
                }
            }
        }
    }
}

int encode(int x, int y, struct rectangle rec)
{
    int c = 0;
    if (x < rec.XL)
        c = c | LEFT;
    else if (x > rec.XR)
        c = c | RIGHT;
    if (y < rec.YB)
        c = c | BOTTOM;
    else if (y > rec.YT)
        c = c | TOP;
    return c;
}

void CSLineClipping(struct rectangle rec, struct line ln)
{
    int x0 = ln.x0, y0 = ln.y0, x1 = ln.x1, y1 = ln.y1;
    int dx, dy, code1, code2, code;
    float x, y;
    code1 = encode(x0, y0, rec);
    code2 = encode(x1, y1, rec);
    dy = y1 - y0;
    dx = x1 - x0;
    while ((code1 != 0) || (code2 != 0))
    {
        if ((code1 & code2) != 0)
            return;
        code = code1;
        if (code1 == 0)
            code = code2;
        if ((LEFT & code) != 0)
        {
            x = rec.XL;
            y = y0 + dy * (rec.XL - x0) / dx;
        }
        else if ((RIGHT & code) != 0)
        {
            x = rec.XR;
            y = y0 + dy * (rec.XR - x0) / dx;
        }
        else if ((BOTTOM & code) != 0)
        {
            y = rec.YB;
            x = x0 + dx * (rec.YB - y0) / dy;
        }
        else if ((TOP & code) != 0)
        {
            y = rec.YT;
            x = x0 + dx * (rec.YT - y0) / dy;
        }
        if (code == code1)
        {
            x0 = x;
            y0 = y;
            code1 = encode(x, y, rec);
        }
        else
        {
            x1 = x;
            y1 = y;
            code2 = encode(x, y, rec);
        }
    }
    DDALine(rec.XL, rec.YB, rec.XL, rec.YT);
    DDALine(rec.XR, rec.YB, rec.XR, rec.YT);
    DDALine(rec.XL, rec.YB, rec.XR, rec.YB);
    DDALine(rec.XL, rec.YT, rec.XR, rec.YT);
    DDALine(x0, y0, x1, y1);
}

struct point
{
    int x, y;
};

void DrawPolygon(std::vector<struct point>& polygon)
{
    int size = polygon.size();
    struct point p0, p1;
    int x0, y0, x1, y1;
    for (int i = 0; i < size - 1; ++i)
    {
        p0 = polygon.at(i);
        p1 = polygon.at(i + 1);
        x0 = p0.x;
        y0 = p0.y;
        x1 = p1.x;
        y1 = p1.y;
        DDALine(x0, y0, x1, y1);
    }
    p0 = polygon.at(size - 1);
    p1 = polygon.at(0);
    x0 = p0.x;
    y0 = p0.y;
    x1 = p1.x;
    y1 = p1.y;
    DDALine(x0, y0, x1, y1);
}

struct point Intersect(struct point S, struct point P, struct line cut)
{
    struct point res;
    if (cut.y0 == cut.y1)
    {
        res.y = cut.y0;
        res.x = S.x + float((cut.y0 - S.y) * (P.x - S.x)) / (P.y - S.y) + 0.5f;
    }
    else
    {
        res.x = cut.x0;
        res.y = S.y + float((cut.x0 - S.x) * (P.y - S.y)) / (P.x - S.x) + 0.5f;
    }
    return res;
}

bool Inside(struct point v, struct line cut)
{
    return (cut.x1 > cut.x0 && v.y >= cut.y0)
        || (cut.x1 < cut.x0 && v.y <= cut.y0)
        || (cut.y1 > cut.y0 && v.x <= cut.x0)
        || (cut.y1 < cut.y0 && v.x >= cut.x0);
}

std::vector<struct point> SutherlandHodgmanPolygonClipping(std::vector<struct point>& polygon, struct line cut)
{
    int size = polygon.size();
    struct point S, P, I;
    std::vector<struct point> output;

    S = polygon.at(size - 1);
    for (int i = 0; i < size; ++i)
    {
        P = polygon.at(i);
        if (Inside(P, cut))
        {
            if (Inside(S, cut))
            {
                output.push_back(P);
            }
            else
            {
                I = Intersect(S, P, cut);
                output.push_back(I);
                output.push_back(P);
            }
        }
        else if (Inside(S, cut))
        {
            I = Intersect(S, P, cut);
            output.push_back(I);
        }
        S = P;
    }

    return output;
}

int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Clipping", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    std::vector<struct point> polygon;
    polygon.push_back({ -100, -100 });
    polygon.push_back({ 0, 50 });
    polygon.push_back({ 50, 30 });
    polygon.push_back({ 100, 100 });
    polygon.push_back({ -50, 250 });
    polygon.push_back({ 200, 350 });
    polygon.push_back({ 400, 120 });
    polygon.push_back({ 220, -200 });

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        CSLineClipping({ 0, 300, -300, 0 }, { -400, 300, 400, -400 });

        LYDBarskeyLineClipping({ -300, 0, 0, 300 }, { -400, 0, 400, 400 });

        polygon = SutherlandHodgmanPolygonClipping(polygon, { 0, 0, 300, 0 });
        polygon = SutherlandHodgmanPolygonClipping(polygon, { 300, 0, 300, 300 });
        polygon = SutherlandHodgmanPolygonClipping(polygon, { 300, 300, 0, 300 });
        polygon = SutherlandHodgmanPolygonClipping(polygon, { 0, 300, 0, 0 });

        DrawPolygon(polygon);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}