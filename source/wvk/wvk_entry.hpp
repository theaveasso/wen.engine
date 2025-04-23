#pragma once

#include "wvk_wen.hpp"

extern wvk::Application* CreateApplication();

int main()
{
	wvk::Application::AppConfig config;
	auto game = CreateApplication();
	game->init(game->get_config());

	game->run();

	delete game;
}