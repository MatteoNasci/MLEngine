#include <engine.h>

#include <GLFW/glfw3.h>
using namespace mle;

Engine::Engine() : m_mainTimeManager(), m_mainConsole("Console.txt", timerManager()){

}
Console& Engine::console(){
    return m_mainConsole;
}
TimerManager& Engine::timerManager(){
    return m_mainTimeManager;
}
bool Engine::init(){
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return false;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return true;
}

Engine& Engine::instance(){
    static Engine engine;
    return engine;
}