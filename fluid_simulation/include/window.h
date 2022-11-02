#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

struct FluidConfig
{
	float velocityDiffusion = 0.f;
	float colorDiffusion = 0.f;
	float densityDiffusion = 0.f;

	float pressure = 0.f;
	float vorticity = 0.f;

	float forceScale = 0.f;
	int forceRadius = 0.f;

	float bloomIntense = 0.f;
	bool bBloomEnabled = true;

	bool bColorful = false;

	sf::Color color = { 128, 128, 128 };

	FluidConfig(float _velocityDiffusion = 0.8f, float _colorDiffusion = 0.8f, float _densityDiffusion = 1.2f,
		float _pressure = 1.5f, float _vorticity = 50.f, float _forceScale = 1000.f, float _forceRadius = 100.f,
		float _bloomIntense = 25000.f, bool _bloomEnabled = true, sf::Color _color = { 128, 128, 128 }, bool _colorful = false) :
		velocityDiffusion(_velocityDiffusion), colorDiffusion(_colorDiffusion), densityDiffusion(_densityDiffusion),
		pressure(_pressure), vorticity(_vorticity), forceScale(_forceScale), forceRadius(_forceRadius),
		bloomIntense(_bloomIntense), bBloomEnabled(_bloomEnabled), color(_color), bColorful(_colorful) {}
};

struct BaseWindowProperties
{
	sf::Uint16 width;
	sf::Uint16 height;

	sf::Uint8 scale;

	sf::Uint16 fieldWidth;
	sf::Uint16 fieldHeight;

	BaseWindowProperties(sf::Uint16 _width = 1280, sf::Uint16 _height = 720, sf::Uint8 _scale = 2) :
		width(_width), height(_height), scale(_scale)
	{
		fieldWidth = width / scale;
		fieldHeight = height / scale;
	}
};

class MainWindow : public sf::RenderWindow
{
public:
	MainWindow();
	MainWindow(sf::VideoMode _videoMode, const sf::String& _title, sf::Uint32 _style = 7U, const sf::ContextSettings& _ctxSettings = sf::ContextSettings());
	MainWindow(sf::WindowHandle _windowHandle, const sf::ContextSettings& _ctxSettings = sf::ContextSettings());

	virtual ~MainWindow();

	void mainLoop();

protected: // Main functions

	void createMenu();

	void updateFluidConfig();

	void computeField();

protected: // Properties

	tgui::Gui mGui;
	tgui::Group::Ptr mMenuGroup;

	sf::Vector2i mLastMousePos = { -1, -1 };
	sf::Vector2i mCurMousePos = { -1, -1 };

	bool bIsPaused = false;
	bool bIsInfluenced = false;

	BaseWindowProperties mBaseProperties;

	FluidConfig mFluidConfig;
	bool bFluidChanged = false;

	std::vector<sf::Uint8> mPixelBuffer;
	sf::Texture mFluidTexture;
	sf::Sprite mFluidSprite;

protected: // SIGNALS

	void OnVelocityDiffusionChanged(const tgui::String& _newValue);

	void OnColorDiffusionChanged(const tgui::String& _newValue);

	void OnDensityDiffusionChanged(const tgui::String& _newValue);

	void OnPressureChanged(const tgui::String& _newValue);

	void OnVorticityChanged(const tgui::String& _newValue);

	void OnForceChanged(const tgui::String& _newValue);

	void OnRadiusChanged(const tgui::String& _newValue);

	void OnBloomIntenseChanged(const tgui::String& _newValue);

	void OnColorRChanged(const tgui::String& _newValue);

	void OnColorGChanged(const tgui::String& _newValue);

	void OnColorBChanged(const tgui::String& _newValue);

	void OnBloomChecked(bool _isChecked);
	void OnColorfulChecked(bool _isChecked);
};
