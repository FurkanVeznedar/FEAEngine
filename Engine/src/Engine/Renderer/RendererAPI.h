#ifndef _RENDERERAPI_H_
#define _RENDERERAPI_H_

#include <glm/glm.hpp>

#include "Engine/Renderer/VertexArray.h"

namespace Engine {

    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0, OpenGL = 1
        };
    public:
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexarray) = 0;

        inline static API GetAPI() { return s_API; }
    private:
        static API s_API;

    };
}

#endif // _RENDERERAPI_H_