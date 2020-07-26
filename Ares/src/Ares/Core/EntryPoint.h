#pragma once

#ifdef ARES_PLATFORM_WINDOWS

// external function that will be defined in the client
extern Ares::Application* Ares::CreateApplication();

int main(int argc, char** argv) {

	Ares::InitializeCore();
	ARES_CORE_LOG("Initializing");


	ARES_CORE_LOG("Creating APP");
	ARES_PROFILE_BEGIN_SESSION("Startup", "AresProfile-Startup.json");
	auto app = Ares::CreateApplication();
	ARES_PROFILE_END_SESSION();

	ARES_CORE_LOG("Creatied APP");

	ARES_PROFILE_BEGIN_SESSION("Runtime", "AresProfile-Runtime.json");
	app->Run();
	ARES_PROFILE_END_SESSION();

	ARES_PROFILE_BEGIN_SESSION("Shutdown", "AresProfile-Shutdown.json");
	delete app;
	ARES_PROFILE_END_SESSION();

	Ares::ShutdownCore();
}

#endif