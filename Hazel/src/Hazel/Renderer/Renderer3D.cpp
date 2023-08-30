#include "hzpch.h"
#include "Hazel/Renderer/Renderer3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/UniformBuffer.h"

namespace Hazel {

    struct SphereVertex {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        float Radius;

        // Editor only
        int EntityId;
    };

    struct Renderer2DData {
        static constexpr uint32_t MaxQuads = 20000;
        static constexpr uint32_t MaxVertices = MaxQuads * 4;
        static constexpr uint32_t MaxIndices = MaxQuads * 6;
        static constexpr uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

        Ref<VertexArray> SphereVertexArray;
        Ref<VertexBuffer> SphereVertexBuffer;
        Ref<Shader> SphereShader;
        uint32_t SphereIndexCount = 0;
        SphereVertex* SphereVertexBufferBase = nullptr;
        SphereVertex* SphereVertexBufferPtr = nullptr;

        Ref<Texture2D> WhiteTexture;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        Renderer3D::Statistics Stats;

        struct CameraData {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;
    };

    static Renderer2DData s_Data;

    void Renderer3D::Init() {
        HZ_PROFILE_FUNCTION();

        // Circle
        s_Data.SphereVertexArray = VertexArray::Create();

        s_Data.SphereVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(SphereVertex));
        s_Data.SphereVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_WorldPosition" },
            { ShaderDataType::Float3, "a_LocalPosition" },
            { ShaderDataType::Float4, "a_Color"         },
            { ShaderDataType::Float,  "a_Radius"        },
            { ShaderDataType::Int,    "a_EntityId"      }
                                             });
        s_Data.SphereVertexArray->AddVertexBuffer(s_Data.SphereVertexBuffer);
        s_Data.SphereVertexBufferBase = new SphereVertex[Renderer2DData::MaxVertices];


        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        s_Data.SphereShader = Shader::Create("assets/shaders/Renderer3D_Sphere.glsl");

        // Set first texture slot to 0
        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
    }

    void Renderer3D::Shutdown() {
        HZ_PROFILE_FUNCTION();

        delete[] s_Data.SphereVertexBufferBase;
    }

    void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform) {
        HZ_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        StartBatch();
    }

    void Renderer3D::BeginScene(const EditorCamera& camera) {
        HZ_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        StartBatch();
    }

    void Renderer3D::BeginScene(const OrthographicCamera& camera) {
        HZ_PROFILE_FUNCTION();

        s_Data.CameraBuffer.ViewProjection = camera.GetProjectionMatrix();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        StartBatch();
    }

    void Renderer3D::EndScene() {
        HZ_PROFILE_FUNCTION();

        Flush();
    }

    void Renderer3D::StartBatch() {
        s_Data.SphereIndexCount = 0;
        s_Data.SphereVertexBufferPtr = s_Data.SphereVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer3D::Flush() {
        if (s_Data.SphereIndexCount) {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.SphereVertexBufferPtr - (uint8_t*)s_Data.SphereVertexBufferBase);
            s_Data.SphereVertexBuffer->SetData(s_Data.SphereVertexBufferPtr, dataSize);

            s_Data.SphereShader->Bind();
            RenderCommand::DrawIndexed(s_Data.SphereVertexArray, s_Data.SphereIndexCount);
            s_Data.Stats.DrawCalls++;
        }
    }

    void Renderer3D::NextBatch() {
        Flush();
        StartBatch();
    }

    void Renderer3D::ResetStats() {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer3D::Statistics Renderer3D::GetStats() {
        return s_Data.Stats;
    }

}
