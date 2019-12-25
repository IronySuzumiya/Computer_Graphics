#include <GLFW/glfw3.h>
#include <iostream>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

GLfloat ctrlPoints[3][3][3] = {
    {
        { -4.0f, 0.0f, 4.0f },
        { -2.0f, 4.0f, 4.0f },
        {  4.0f, 0.0f, 4.0f }
    },
    {
        { -4.0f, 0.0f, 0.0f },
        { -2.0f, 4.0f, 0.0f },
        {  4.0f, 0.0f, 0.0f }
    },
    {
        { -4.0f, 0.0f, -4.0f },
        { -2.0f, 4.0f, -4.0f },
        {  4.0f, 0.0f, -4.0f }
    }
};

void DrawPoints()
{
    int i, j;

    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(5.0f);

    glBegin(GL_POINTS);
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            glVertex3fv(ctrlPoints[i][j]);
    glEnd();
}

void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(60.0f, 1.0f, 0.0f, 0.0f);


    glColor3f(0.0f, 0.0f, 1.0f);
    glMap2f(GL_MAP2_VERTEX_3, //生成的数据类型  
        0.0f, // u的下界 
        10.0f, //u的上界  
        3, //数据中点的间隔  
        3, //u方向上的阶  
        0.0f, //v的下界  
        10.0f, //v的上界  
        9, // 控制点之间的间隔  
        3, // v方向上的阶  
        &ctrlPoints[0][0][0]); //控制点数组 
        //启用求值器  
    glEnable(GL_MAP2_VERTEX_3);
    //从0到10映射一个包含10个点的网格  
    glMapGrid2f(10, 0.0f, 10.0f, 10, 0.0f, 10.0f);
    // 计算网格  
    glEvalMesh2(GL_LINE, 0, 10, 0, 10);
    //画控制点
    DrawPoints();

    glPopMatrix();
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawSurface", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        RenderScene();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
