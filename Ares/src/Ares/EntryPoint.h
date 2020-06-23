#pragma once

#ifdef ARES_PLATFORM_WINDOWS

// external function that will be defined in the client
extern Ares::Application* Ares::CreateApplication();

int main(int argc, char** argv) {

	Ares::Log::Init();

	// core engine loggers
	ARES_CORE_LOG("Log");
	ARES_CORE_INFO("Info");
	ARES_CORE_WARN("Warn");
	ARES_CORE_ERROR("Error");
	ARES_CORE_FATAL("Fatal");

	// client loggers
	ARES_LOG("Log");
	ARES_INFO("Info");
	ARES_WARN("Warn");
	ARES_ERROR("Error");
	ARES_FATAL("Fatal");

	//printf("Ares Engine Started!");

	auto app = Ares::CreateApplication();
	
	app->Run();
	
	delete app;

}


#endif