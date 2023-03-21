#pragma once

#include "RendererAPI.h"

namespace Hazel {

	class RendererCommand {
	public:
		inline static void SetClearColor(const glm::vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexd(const std::shared_ptr<VertexArray>& vertexArray) {
			s_RendererAPI->DrawIndexd(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}