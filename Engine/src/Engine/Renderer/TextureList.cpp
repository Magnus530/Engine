#include "epch.h"
#include "Engine/Renderer/TextureList.h"

namespace Engine
{
	static const std::string skyFaces[6] =
	{
		"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"
	};

	static const std::string forestFaces[6] =
	{
		"assets/textures/skybox/forestright.jpg",
		"assets/textures/skybox/forestleft.jpg",
		"assets/textures/skybox/foresttop.jpg",
		"assets/textures/skybox/forestbottom.jpg",
		"assets/textures/skybox/forestfront.jpg",
		"assets/textures/skybox/forestback.jpg"
	};

	void TextureList::CreateTextures(std::shared_ptr<Engine::Scene> scene)
	{
		Engine::Renderer::CreateTexture("Chess", "assets/textures/checkerboard.png", scene);
		Engine::Renderer::CreateTexture("Wolf", "assets/textures/wolf.png", scene);
		Engine::Renderer::CreateTexture("White", "assets/textures/white.png", scene);

		Engine::Renderer::CreateSkybox("Sky", skyFaces, scene);
		Engine::Renderer::CreateSkybox("Forest", forestFaces, scene);
	}
}