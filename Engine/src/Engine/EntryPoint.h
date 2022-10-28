#pragma once

#ifdef E_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	printf("Engine printout\n");
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}


#endif