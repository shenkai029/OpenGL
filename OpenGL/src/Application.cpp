#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    // use another scope to make sure vb/ib object can be delete from stack before calling glfwTerminate()
    {
        float positions[] = {
         -50.0f,  -50.0f, 0.0f, 0.0f,// 0
          50.0f,  -50.0f, 1.0f, 0.0f,// 1
          50.0f,   50.0f, 1.0f, 1.0f,// 2
         -50.0f,   50.0f, 0.0f, 1.0f,// 3

          150.0f,  -50.0f, 0.0f, 0.0f,// 0
          250.0f,  -50.0f, 1.0f, 0.0f,// 1
          250.0f,   50.0f, 1.0f, 1.0f,// 2
          150.0f,   50.0f, 0.0f, 1.0f// 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
            4, 5, 6,
            6, 7, 4
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // draw triangle
        // generate and bind array buffer, and load postion data to the buffer
        // enable VertexAttribArray to specify layout of positon data
        VertexArray va;
        VertexBuffer vb(positions, 8 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        // draw rectangle is actually draw 2 triangle using 2 * 3 vertices
        // generate and bind new element array buffer, and load indices data to the buffer
        IndexBuffer ib(indices, 12);

        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f); // vertices projection position 
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // camera view position

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/texture/PennState.PNG");
        texture.Bind();
        shader.SetUniform4i("u_Texture", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        glm::vec3 translationA(200, 200, 0);
        //glm::vec3 translationB(400, 200, 0);

        float r_ch = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplGlfwGL3_NewFrame();

            shader.Bind();

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA); // model object position
                glm::mat4 mvp = proj * view * model;
                shader.SetUniformMAT4f("u_MVP", mvp);
                renderer.Draw(va, ib, shader);
            }

            //{
            //    glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB); // model object position
            //    glm::mat4 mvp = proj * view * model;
            //    shader.SetUniformMAT4f("u_MVP", mvp);
            //    renderer.Draw(va, ib, shader);
            //}

            shader.SetUniform4f("u_Color", r_ch, 0.3f, 0.8f, 1.0f);


            if (r_ch > 1.0f) increment = -0.05f;
            else if (r_ch < 0.0f) increment = 0.05f;
            r_ch += increment;

            {
                ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f 
                //ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 960.0f);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }    

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}