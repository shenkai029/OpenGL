#include "TestTexture2D.h"
#include "Renderer.h"
#include "imgui/imgui.h"

namespace test {

	TestTexture2D::TestTexture2D() : 
        m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0),
        m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))) {

        float positions[] = {
        -50.0f,  -50.0f, 0.0f, 0.0f,// 0
         50.0f,  -50.0f, 1.0f, 0.0f,// 1
         50.0f,   50.0f, 1.0f, 1.0f,// 2
        -50.0f,   50.0f, 0.0f, 1.0f// 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_VAO = std::make_unique<VertexArray>();
        

        // draw triangle
        // generate and bind array buffer, and load postion data to the buffer
        // enable VertexAttribArray to specify layout of positon data
         m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VAO->AddBuffer(*m_VBO, layout);

        // draw rectangle is actually draw 2 triangle using 2 * 3 vertices
        // generate and bind new element array buffer, and load indices data to the buffer
        m_IBO = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

        m_Texture = std::make_unique<Texture>("res/texture/PennState.PNG");
        m_Shader->SetUniform1i("u_Texture", 0);

        m_Shader->Unbind();
	}

	TestTexture2D::~TestTexture2D()
	{
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;
        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA); // model object position
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMAT4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB); // model object position
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMAT4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender()
	{
        ImGui::SliderFloat3("TranslationA", &m_TranslationA.x, 0.0f, 960.0f);      // Edit 1 float using a slider from 0.0f to 1.0f 
        ImGui::SliderFloat3("TranslationB", &m_TranslationB.x, 0.0f, 960.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}