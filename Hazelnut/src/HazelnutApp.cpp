#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Hazel {

    class Hazelnut : public Application {
    public:
        Hazelnut()
            : Application("Hazelnut")
        {
		    PushLayer(new EditorLayer());
	    }

        ~Hazelnut() override = default;
    };

    Application* CreateApplication() {
	    return new Hazelnut();
    }
}