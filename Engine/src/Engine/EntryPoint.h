#pragma once

#ifdef E_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();
	E_CORE_WARN("Initialized Core Log.");
	int a = 5;
	E_INFO("ClientLogger. Var={0}", a);

	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}

#endif