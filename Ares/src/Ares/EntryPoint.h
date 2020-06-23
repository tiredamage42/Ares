#pragma once

#ifdef ARES_PLATFORM_WINDOWS

// external function that will be defined in the client
extern Ares::Application* Ares::CreateApplication();

int main(int argc, char** argv) {
	printf("Ares Engine Started!");

	auto app = Ares::CreateApplication();
	
	app->Run();
	
	delete app;

}


#endif