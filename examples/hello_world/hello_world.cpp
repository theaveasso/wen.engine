#include <wvk/wvk_entry.hpp>

#include "hw_game.hpp"

wvk::Application* CreateApplication()
{
	return new HWGame;
}