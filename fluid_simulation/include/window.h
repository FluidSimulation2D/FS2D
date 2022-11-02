#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

struct FluidConfig
{
	float velocity_diffusion = 0.f;
	float color_diffusion = 0.f;
	float density_diffusion = 0.f;

	float pressure = 0.f;
	float vorticity = 0.f;

	float force_scale = 0.f;
	int force_radius = 0.f;

	float bloom_intense = 0.f;
	bool b_bloom_enabled = true;

	bool b_colorful = false;

	sf::Color color = { 128, 128, 128 };

	FluidConfig(float _velocity_diffusion = 0.8f, float _color_diffusion = 0.8f, float _density_diffusion = 1.2f,
		float _pressure = 1.5f, float _vorticity = 50.f, float _force_scale = 1000.f, float _force_radius = 100.f,
		float _bloom_intense = 25000.f, bool _bloom_enabled = true, sf::Color _color = { 128, 128, 128 }, bool _colorful = false) :
		velocity_diffusion(_velocity_diffusion), color_diffusion(_color_diffusion), density_diffusion(_density_diffusion),
		pressure(_pressure), vorticity(_vorticity), force_scale(_force_scale), force_radius(_force_radius),
		bloom_intense(_bloom_intense), b_bloom_enabled(_bloom_enabled), color(_color), b_colorful(_colorful) {}
};

struct BaseWindowProperties
{
	sf::Uint16 width;
	sf::Uint16 height;

	sf::Uint8 scale;

	sf::Uint16 field_width;
	sf::Uint16 field_height;

	BaseWindowProperties(sf::Uint16 _width = 1280, sf::Uint16 _height = 720, sf::Uint8 _scale = 2) :
		width(_width), height(_height), scale(_scale)
	{
		field_width = width / scale;
		field_height = height / scale;
	}
};

class MainWindow : public sf::RenderWindow
{
public:
	MainWindow();
	MainWindow(sf::VideoMode _video_mode, const sf::String& _title, sf::Uint32 _style = 7U, const sf::ContextSettings& _ctx_settings = sf::ContextSettings());
	MainWindow(sf::WindowHandle _window_handle, const sf::ContextSettings& _ctx_settings = sf::ContextSettings());

	virtual ~MainWindow();

	void mainLoop();

protected: // Main functions

	void createMenu();

	void updateFluidConfig();

	void computeField();

protected: // Properties

	tgui::Gui m_gui;
	tgui::Group::Ptr m_menu_group;

	sf::Vector2i m_last_mouse_pos = { -1, -1 };
	sf::Vector2i m_cur_mouse_pos = { -1, -1 };

	bool b_is_paused = false;
	bool b_is_influenced = false;

	BaseWindowProperties m_base_properties;

	FluidConfig m_fluid_config;
	bool b_is_fluid_changed = false;

	std::vector<sf::Uint8> m_pixel_buffer;
	sf::Texture m_fluid_texture;
	sf::Sprite m_fluid_sprite;

protected: // SIGNALS

	void OnVelocityDiffusionChanged(const tgui::String& _new_value);

	void OnColorDiffusionChanged(const tgui::String& _new_value);

	void OnDensityDiffusionChanged(const tgui::String& _new_value);

	void OnPressureChanged(const tgui::String& _new_value);

	void OnVorticityChanged(const tgui::String& _new_value);

	void OnForceChanged(const tgui::String& _new_value);

	void OnRadiusChanged(const tgui::String& _new_value);

	void OnBloomIntenseChanged(const tgui::String& _new_value);

	void OnRColorChanged(const tgui::String& _new_value);

	void OnGColorChanged(const tgui::String& _new_value);

	void OnBColorChanged(const tgui::String& _new_value);

	void OnBloomChecked(bool _is_checked);
	void OnColorfulChecked(bool _is_checked);
};
