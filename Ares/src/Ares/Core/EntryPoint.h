#pragma once

#include "Ares/Core/Core.h"

#ifdef ARES_PLATFORM_WINDOWS

// external function that will be defined in the client
extern Ares::Application* Ares::CreateApplication();

int main(int argc, char** argv) {

	Ares::Log::Init();

	auto app = Ares::CreateApplication();	
	app->Run();
	delete app;
}

#endif