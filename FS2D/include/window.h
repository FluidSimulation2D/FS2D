#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "fluid.h"

#define WINDOW_WIDTH 1280
#define MENU_WIDTH 256
#define WINDOW_HEIGHT 720

struct BaseWindowProperties
{
	sf::Uint16 width;
	sf::Uint16 height;

	sf::Uint16 scale;

	sf::Uint16 fieldWidth;
	sf::Uint16 fieldHeight;

	BaseWindowProperties(sf::Uint16 _width = WINDOW_WIDTH, sf::Uint16 _height = WINDOW_HEIGHT, sf::Uint16 _scale = 2) :
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

	// creating menu with fluid configurations
	void createMenu();

	// updating fluid configuration for future calculations
	void updateFluidConfig();

protected: // Properties

	tgui::Gui mGui;
	tgui::Group::Ptr mMenuGroup;

	sf::Vector2i mLastMousePos = { -1, -1 };
	sf::Vector2i mCurMousePos = { -1, -1 };

	bool bIsPaused = false;
	bool bIsInfluenced = false;

	BaseWindowProperties mBaseProperties;

	Config mFluidConfig;
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

	void OnModeChecked(bool _isChecked);
};
