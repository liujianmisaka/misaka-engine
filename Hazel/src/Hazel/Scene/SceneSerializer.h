#pragma once

#include "Hazel/Scene/Scene.h"

namespace Hazel {
    class SceneSerializer {
    public:
        SceneSerializer(const Ref<Scene>& scene);

        void Serializer(const std::string& filepath);
        void SerializerRuntime(const std::string& filepath);

        bool Deserializer(const std::string& filepath);
        bool DeserializerRuntime(const std::string& filepath);
    private:
        Ref<Scene> m_Scene;
    };
}
