#include "Engine.h"
#include <Engine/Core/EntryPoint.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.inl>

#include "App2D.h"

class ExampleLayer : public Engine::Layer
{
public:
    ExampleLayer()
        : Layer("Example"), m_CameraController(1280.0f / 720.0f)
    {
        m_VertexArray = Engine::VertexArray::Create();

        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };
        Engine::Ref<Engine::VertexBuffer> vertexbuffer = Engine::VertexBuffer::Create(vertices, sizeof(vertices));

        Engine::BufferLayout layout = {
            { Engine::ShaderDataType::Float3, "a_Position"},
            { Engine::ShaderDataType::Float4, "a_Color"}
        };
        vertexbuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexbuffer);

        uint32_t indices[] = {
            0, 1, 2
        };
        Engine::Ref<Engine::IndexBuffer> indexbuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexbuffer);

        m_SquareVA = Engine::VertexArray::Create();

        float squarevertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.2f, 0.3f, 0.8f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.2f, 0.3f, 0.8f, 1.0f
        };

        Engine::Ref<Engine::VertexBuffer> SquareVB = Engine::VertexBuffer::Create(squarevertices, sizeof(squarevertices));
        SquareVB->SetLayout({
            { Engine::ShaderDataType::Float3, "a_Position"},
            { Engine::ShaderDataType::Float2, "a_TextCoord"},
            { Engine::ShaderDataType::Float4, "a_Color"}
        });
        m_SquareVA->AddVertexBuffer(SquareVB);

        uint32_t squareindices[] = {
            0, 1, 2, 2, 3 ,0
        };
        Engine::Ref<Engine::IndexBuffer> SquareIB = Engine::IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(uint32_t));
        m_SquareVA->SetIndexBuffer(SquareIB);

        m_Shader = Engine::Shader::Create("TriangleShader", Engine::Log::GetShadersDir() + "VertexSrc.txt", Engine::Log::GetShadersDir() + "FragmentSrc.txt");
        m_BlueShader = Engine::Shader::Create("BlueShader", Engine::Log::GetShadersDir() + "VertexSrc.txt", Engine::Log::GetShadersDir() + "FragmentSrc.txt");
        auto textureshader = m_ShaderLibrary.Load("TextureShader", Engine::Log::GetShadersDir() + "TextureVertex.txt", Engine::Log::GetShadersDir() + "TextureFragment.txt");

        m_Texture = Engine::Texture2D::Create(Engine::Log::GetAssetsDir() + "Texture/Checkerboard.png");
        m_LogoTexture = Engine::Texture2D::Create(Engine::Log::GetAssetsDir() + "Texture/ChernoLogo.png");

        std::dynamic_pointer_cast<Engine::OpenGLShader>(textureshader)->Use();
        std::dynamic_pointer_cast<Engine::OpenGLShader>(textureshader)->UploadInt("u_Texture", 0);
    }
    
    void OnUpdate(Engine::DeltaTime ts) override
    {
        // Update
        m_CameraController.OnUpdate(ts);

        Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        Engine::RenderCommand::Clear();

        Engine::Renderer::BeginScene(m_CameraController.GetCamera());

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        std::dynamic_pointer_cast<Engine::OpenGLShader>(m_BlueShader)->Use();
        std::dynamic_pointer_cast<Engine::OpenGLShader>(m_BlueShader)->UploadVec3("u_Color", m_SquareColor);

        for(int i = 0; i < 20; i++)
        {
            for(int j = 0; j < 20; j++)
            {
                glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                Engine::Renderer::SubmitGeometry(m_BlueShader, m_SquareVA, transform);
            }
        }

        m_Texture->Bind();
        auto textureshader = m_ShaderLibrary.Get("TextureShader");
        Engine::Renderer::SubmitGeometry(textureshader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        m_LogoTexture->Bind();
        Engine::Renderer::SubmitGeometry(textureshader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        
        //Triangle
        //Engine::Renderer::SubmitGeometry(m_Shader, m_VertexArray);

        Engine::Renderer::EndScene();
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }
    void OnEvent(Engine::Event& e) override
    {
        m_CameraController.OnEvent(e);
    }
private:
    Engine::ShaderLibrary m_ShaderLibrary;
    Engine::Ref<Engine::Shader> m_Shader;
    Engine::Ref<Engine::VertexArray> m_VertexArray;

    Engine::Ref<Engine::Shader> m_BlueShader;
    Engine::Ref<Engine::VertexArray> m_SquareVA;

    Engine::Ref<Engine::Texture2D> m_Texture, m_LogoTexture;

    Engine::CameraController m_CameraController;

    glm::vec3 m_SquarePosition;
    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
    };

class App : public Engine::Application
{
public:
    App()
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new App2D());
    }

    virtual ~App()
    {

    }
};

Engine::Application* Engine::CreateApplication()
{
    return new App();
}