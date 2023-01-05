#include "epch.h"
#include "Engine/Renderer/TextureList.h"

namespace Engine
{
	static const std::string skyFaces[6] =
	{
		"assets/textures/skybox/sky/right.jpg",
		"assets/textures/skybox/sky/left.jpg",
		"assets/textures/skybox/sky/top.jpg",
		"assets/textures/skybox/sky/bottom.jpg",
		"assets/textures/skybox/sky/front.jpg",
		"assets/textures/skybox/sky/back.jpg"
	};

	static const std::string forestFaces[6] =
	{
		"assets/textures/skybox/forest/posx.jpg",
		"assets/textures/skybox/forest/negx.jpg",
		"assets/textures/skybox/forest/posy.jpg",
		"assets/textures/skybox/forest/negy.jpg",
		"assets/textures/skybox/forest/posz.jpg",
		"assets/textures/skybox/forest/negz.jpg"
	};

	static const std::string forestFaces2[6] =
	{
		"assets/textures/skybox/forest2/posx.jpg",
		"assets/textures/skybox/forest2/negx.jpg",
		"assets/textures/skybox/forest2/posy.jpg",
		"assets/textures/skybox/forest2/negy.jpg",
		"assets/textures/skybox/forest2/posz.jpg",
		"assets/textures/skybox/forest2/negz.jpg"
	};

	static const std::string forestFaces3[6] =
	{
		"assets/textures/skybox/forest3/posx.jpg",
		"assets/textures/skybox/forest3/negx.jpg",
		"assets/textures/skybox/forest3/posy.jpg",
		"assets/textures/skybox/forest3/negy.jpg",
		"assets/textures/skybox/forest3/posz.jpg",
		"assets/textures/skybox/forest3/negz.jpg"
	};

	void TextureList::CreateTextures(std::shared_ptr<Engine::Scene> scene)
	{
		Engine::Renderer::CreateTexture("Chess", "assets/textures/checkerboard.png", scene);
		Engine::Renderer::CreateTexture("Wolf", "assets/textures/wolf.png", scene);
		Engine::Renderer::CreateTexture("White", "assets/textures/white.png", scene);
		Engine::Renderer::CreateTexture("Tree", "assets/textures/tree.png", scene);
		Engine::Renderer::CreateTexture("Pine", "assets/textures/pine.png", scene);
		Engine::Renderer::CreateTexture("Fir", "assets/textures/fir.png", scene);
		Engine::Renderer::CreateTexture("Cypress", "assets/textures/cypress.png", scene);
		Engine::Renderer::CreateTexture("Corner", "assets/textures/corner.png", scene);
		Engine::Renderer::CreateTexture("Leaf", "assets/textures/leave.jpg", scene);

		Engine::Renderer::CreateSkybox("Sky", skyFaces, scene);
		Engine::Renderer::CreateSkybox("Forest", forestFaces, scene);
		Engine::Renderer::CreateSkybox("Urban forest", forestFaces2, scene);
		Engine::Renderer::CreateSkybox("Forest clearing", forestFaces3, scene);
	}
}