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
    glMap2f(GL_MAP2_VERTEX_3, //���ɵ���������  
        0.0f, // u���½� 
        10.0f, //u���Ͻ�  
        3, //�����е�ļ��  
        3, //u�����ϵĽ�  
        0.0f, //v���½�  
        10.0f, //v���Ͻ�  
        9, // ���Ƶ�֮��ļ��  
        3, // v�����ϵĽ�  
        &ctrlPoints[0][0][0]); //���Ƶ����� 
        //������ֵ��  
    glEnable(GL_MAP2_VERTEX_3);
    //��0��10ӳ��һ������10���������  
    glMapGrid2f(10, 0.0f, 10.0f, 10, 0.0f, 10.0f);
    // ��������  
    glEvalMesh2(GL_LINE, 0, 10, 0, 10);
    //�����Ƶ�
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
