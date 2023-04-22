#pragma once

#include <Hazel.h>

#include "Hazel/Renderer/Renderer2D.h"

struct ParticleProps {
    glm::vec2 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;
};

class ParticleSystem {
public:
    ParticleSystem(uint32_t maxParticles = 100000);

    void OnUpdate(Hazel::Timestep ts);
    void OnRender(Hazel::OrthographicCamera& camera);

    void Emit(const ParticleProps& particleProps);

private:
    struct Particle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;

        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;

        bool Active = false;
    };

    std::vector<Particle> m_ParticlePool{};
    uint32_t m_PoolIndex;

    unsigned int m_QuadVertexArray = 0;
    std::unique_ptr<Hazel::Shader> m_ParticleShader = nullptr;
    unsigned int m_ParticleShaderViewProj = 0, m_ParticleShaderTransform = 0, m_ParticleShaderColor = 0;
};