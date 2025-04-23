#pragma once

#include <glm/vec2.hpp>

#include "wvk/common/wvk_pch.hpp"
#include "wvk/gfx/wvk_gfx.hpp"

struct GLFWwindow;

namespace wvk
{
class WVK_API Application
{
  public:
	virtual ~Application() = default;

	struct AppConfig
	{
		glm::ivec2 windowSize = {1280, 720};
		glm::ivec2 renderArea = {1280, 720};

		std::string appName   = "Wen Moon";
		std::string windowTitle;

		std::string sourceSubDirName;
	};

	void init(const AppConfig& config);
	void run();
	void cleanup();

	virtual void on_init()           = 0;
	virtual void on_update(float dt) = 0;
	virtual void on_draw()           = 0;
	virtual void on_cleanup()        = 0;
	virtual void on_window_resize() {};

	inline const AppConfig& get_config() { return _app_config; }
  protected:
	virtual void load_app_settings() {};
	virtual void load_dev_settings(const std::filesystem::path &configPath);

	GLFWwindow *_window = nullptr;

	gfx::Instance _instance;

	AppConfig _app_config;
	bool      _vsync   = true;

	bool _is_running   = false;
	bool _is_pause     = false;
	bool _is_prod      = false;
	bool _is_staging   = true;

	bool  _frame_limit = true;
	float _frame_time  = 0.0f;
	float _average_fps = 0.0f;

  private:
	void handle_base_dev_input();
};

//Application *CreateApplication();
}        // namespace wvk