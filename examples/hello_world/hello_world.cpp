#include <wvk/wvk_entry.hpp>

#include "game.h"

wvk::Application *CreateApplication()
{
	return new HWGame;
}