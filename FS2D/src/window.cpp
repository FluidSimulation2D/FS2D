#include "window.h"

MainWindow::MainWindow() : sf::RenderWindow({ WINDOW_WIDTH + MENU_WIDTH, WINDOW_HEIGHT }, "Fluid Simuation 2D")
{
	mBaseProperties = BaseWindowProperties(WINDOW_WIDTH + MENU_WIDTH, WINDOW_HEIGHT);
	mPixelBuffer.resize(static_cast<size_t>(mBaseProperties.fieldWidth * mBaseProperties.fieldHeight * 4));
	mFluidTexture.create(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);

	mGui.setWindow(*this);
}

MainWindow::MainWindow(sf::VideoMode _videoMode, const sf::String& _title, sf::Uint32 _style, const sf::ContextSettings& _ctxSettings) :
	sf::RenderWindow(_videoMode, _title, _style, _ctxSettings),
	mBaseProperties(_videoMode.width, _videoMode.height)
{
	mPixelBuffer.resize(static_cast<size_t>(mBaseProperties.fieldWidth * mBaseProperties.fieldHeight * 4));
	mFluidTexture.create(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);

	mGui.setWindow(*this);
}

MainWindow::MainWindow(sf::WindowHandle _windowHandle, const sf::ContextSettings& _ctxSettings) :
	sf::RenderWindow(_windowHandle, _ctxSettings)
{
	mBaseProperties = BaseWindowProperties(getSize().x, getSize().y);

	mPixelBuffer.resize(static_cast<size_t>(mBaseProperties.fieldWidth * mBaseProperties.fieldHeight * 4));
	mFluidTexture.create(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);

	mGui.setWindow(*this);
}

MainWindow::~MainWindow() {}

void MainWindow::mainLoop()
{
	initialization(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);
	createMenu();

	auto menu_rect = sf::RectangleShape(mMenuGroup->getSize());
	menu_rect.setFillColor({ 200, 200, 200 });
	menu_rect.setOutlineColor({ 180, 180, 180 });
	menu_rect.setOutlineThickness(2.5);

	int offset = mMenuGroup->getSize().x;

	mFluidSprite.setPosition(mMenuGroup->getSize().x, 0);
	mFluidSprite.setScale({ static_cast<float>(mBaseProperties.scale), static_cast<float>(mBaseProperties.scale) });

	while (isOpen())
	{
		clear(sf::Color::White);

		sf::Event event;
		while (pollEvent(event))
		{
			mGui.handleEvent(event);

			if (event.type == sf::Event::Closed)
			{
				close();
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					mLastMousePos = { event.mouseButton.x - offset, event.mouseButton.y };
					mLastMousePos /= static_cast<int>(mBaseProperties.scale);

					bIsInfluenced = true;
				}
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				bIsInfluenced = false;
			}

			if (event.type == sf::Event::MouseMoved)
			{
				std::swap(mCurMousePos, mLastMousePos);

				mCurMousePos = { event.mouseMove.x - offset, event.mouseMove.y };
				mCurMousePos /= static_cast<int>(mBaseProperties.scale);
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					bIsPaused = !bIsPaused;
				}
			}
		}

		if (!bIsPaused)
		{
			if (bFluidChanged)
			{
				updateFluidConfig();
				bFluidChanged = false;
			}

			compute(mPixelBuffer.data(), mLastMousePos.x, mLastMousePos.y, mCurMousePos.x, mCurMousePos.y, bIsInfluenced);
		}

		mFluidTexture.update(mPixelBuffer.data());
		mFluidSprite.setTexture(mFluidTexture, false);
		
		draw(mFluidSprite);
		mGui.draw();

		display();
	}

	finalization();
}

void MainWindow::updateFluidConfig()
{
	setConfig(mFluidConfig.velocityDiffusion, mFluidConfig.colorDiffusion, mFluidConfig.densityDiffusion, mFluidConfig.pressure, mFluidConfig.vorticity,
		mFluidConfig.forceScale, mFluidConfig.bloomIntense, mFluidConfig.dt, mFluidConfig.radius, mFluidConfig.velocityIterations, mFluidConfig.pressureIterations,
		mFluidConfig.xThreads, mFluidConfig.yThreads, mFluidConfig.bloomEnabled, mFluidConfig.bColorful, mFluidConfig.bParallel, 
		mFluidConfig.color[0], mFluidConfig.color[1], mFluidConfig.color[2]);
}

void MainWindow::createMenu()
{
	// menu width = 1/5 of fluid draw window width -> 1/6 of application window width (in %)
	mMenuGroup = tgui::Group::create({ "16.666666%", "100%" });

	// MAIN PROPERTIES GROUP
	auto mainPropsGroup = tgui::Group::create();
	auto mainPropsGrid = tgui::Grid::create();

	// velocity diffusion
	auto velocityDiffTitle = tgui::Label::create();
	velocityDiffTitle->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
	velocityDiffTitle->setHeight(18U);
	velocityDiffTitle->setTextSize(14U);
	velocityDiffTitle->setWidth(150);
	velocityDiffTitle->setText("Velocity diffusion:");

	auto velocityDiffEdit = tgui::EditBox::create();
	velocityDiffEdit->setHeight(18U);
	velocityDiffEdit->setMaximumCharacters(6U);
	velocityDiffEdit->setTextSize(14U);
	velocityDiffEdit->setWidth(60);
	velocityDiffEdit->setText(std::to_string(mFluidConfig.velocityDiffusion));
	velocityDiffEdit->onReturnKeyPress(&MainWindow::OnVelocityDiffusionChanged, this);

	mainPropsGrid->addWidget(velocityDiffTitle, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(velocityDiffEdit, 0, 1);

	// color diffusion
	auto colorDiffTitle = tgui::Label::copy(velocityDiffTitle);
	colorDiffTitle->setText("Color diffusion:");
	tgui::EditBox::Ptr colorDiffEdit = tgui::EditBox::copy(velocityDiffEdit);
	colorDiffEdit->setText(std::to_string(mFluidConfig.colorDiffusion));
	colorDiffEdit->onReturnKeyPress(&MainWindow::OnColorDiffusionChanged, this);

	mainPropsGrid->addWidget(colorDiffTitle, 1, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(colorDiffEdit, 1, 1);

	// density diffusion
	auto densityDiffTitle = tgui::Label::copy(velocityDiffTitle);
	densityDiffTitle->setText("Density diffusion:");
	tgui::EditBox::Ptr densityDiffEdit = tgui::EditBox::copy(velocityDiffEdit);
	densityDiffEdit->setText(std::to_string(mFluidConfig.densityDiffusion));
	densityDiffEdit->onReturnKeyPress(&MainWindow::OnDensityDiffusionChanged, this);

	mainPropsGrid->addWidget(densityDiffTitle, 2, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(densityDiffEdit, 2, 1);

	// pressure
	auto pressureTitle = tgui::Label::copy(velocityDiffTitle);
	pressureTitle->setText("Pressure:");
	tgui::EditBox::Ptr pressureEdit = tgui::EditBox::copy(velocityDiffEdit);
	pressureEdit->setText(std::to_string(mFluidConfig.pressure));
	pressureEdit->onReturnKeyPress(&MainWindow::OnPressureChanged, this);

	mainPropsGrid->addWidget(pressureTitle, 3, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(pressureEdit, 3, 1);

	// vorticity
	auto vorticityTitle = tgui::Label::copy(velocityDiffTitle);
	vorticityTitle->setText("Vorticity:");
	tgui::EditBox::Ptr vorticityEdit = tgui::EditBox::copy(velocityDiffEdit);
	vorticityEdit->setText(std::to_string(mFluidConfig.vorticity));
	vorticityEdit->onReturnKeyPress(&MainWindow::OnVorticityChanged, this);

	mainPropsGrid->addWidget(vorticityTitle, 4, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(vorticityEdit, 4, 1);

	// force scale
	auto forceTitle = tgui::Label::copy(velocityDiffTitle);
	forceTitle->setText("Force scale:");
	tgui::EditBox::Ptr forceEdit = tgui::EditBox::copy(velocityDiffEdit);
	forceEdit->setText(std::to_string(mFluidConfig.forceScale));
	forceEdit->onReturnKeyPress(&MainWindow::OnForceChanged, this);

	mainPropsGrid->addWidget(forceTitle, 5, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(forceEdit, 5, 1);

	// radius
	auto radiusTitle = tgui::Label::copy(velocityDiffTitle);
	radiusTitle->setText("Radius:");
	tgui::EditBox::Ptr radiusEdit = tgui::EditBox::copy(velocityDiffEdit);
	radiusEdit->setText(std::to_string(mFluidConfig.radius));
	radiusEdit->onReturnKeyPress(&MainWindow::OnRadiusChanged, this);

	mainPropsGrid->addWidget(radiusTitle, 6, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	mainPropsGrid->addWidget(radiusEdit, 6, 1);

	mainPropsGrid->setAutoSize(true);

	mainPropsGroup->add(mainPropsGrid);
	mMenuGroup->add(mainPropsGroup);

	// BLOOM GROUP
	auto bloomGroup = tgui::Group::create();
	auto bloomGrid = tgui::Grid::create();

	// bloom enabled
	auto bloomEnableTitle = tgui::Label::copy(velocityDiffTitle);
	bloomEnableTitle->setText("Bloom:");
	auto bloomEnableCheckbox = tgui::CheckBox::create();
	bloomEnableCheckbox->setChecked(mFluidConfig.bloomEnabled);
	bloomEnableCheckbox->onChange(&MainWindow::OnBloomChecked, this);

	bloomGrid->addWidget(bloomEnableTitle, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	bloomGrid->addWidget(bloomEnableCheckbox, 0, 1);

	// bloom intense
	auto bloomIntenseTitle = tgui::Label::copy(velocityDiffTitle);
	bloomIntenseTitle->setText("Bloom intense:");
	auto bloomIntenseEdit = tgui::EditBox::copy(velocityDiffEdit);
	bloomIntenseEdit->setText(std::to_string(mFluidConfig.bloomIntense));
	bloomIntenseEdit->onReturnKeyPress(&MainWindow::OnBloomIntenseChanged, this);

	bloomGrid->addWidget(bloomIntenseTitle, 1, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	bloomGrid->addWidget(bloomIntenseEdit, 1, 1);

	bloomGrid->setAutoSize(true);

	bloomGroup->add(bloomGrid);
	bloomGroup->setPosition(0, mainPropsGrid->getSize().y + 20);
	mMenuGroup->add(bloomGroup);

	// COLOR GROUP
	auto colorGroup = tgui::Group::create();
	auto colorGrid = tgui::Grid::create();

	// color enabled
	auto colorfulTitle = tgui::Label::copy(velocityDiffTitle);
	colorfulTitle->setText("Colorful:");
	auto colorfulCheckbox = tgui::CheckBox::create();
	colorfulCheckbox->setChecked(mFluidConfig.bColorful);
	colorfulCheckbox->onChange(&MainWindow::OnColorfulChecked, this);

	colorGrid->addWidget(colorfulTitle, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorfulCheckbox, 0, 1);

	auto colorTitle = tgui::Label::copy(bloomIntenseTitle);
	colorTitle->setText("Edit color:");
	auto colorREdit = tgui::EditBox::copy(bloomIntenseEdit);
	colorREdit->setMaximumCharacters(3U);
	colorREdit->setText(std::to_string(mFluidConfig.color[0]));
	colorREdit->onReturnKeyPress(&MainWindow::OnColorRChanged, this);
	auto colorGEdit = tgui::EditBox::copy(colorREdit);
	colorGEdit->setText(std::to_string(mFluidConfig.color[1]));
	colorGEdit->onReturnKeyPress(&MainWindow::OnColorGChanged, this);
	auto colorBEdit = tgui::EditBox::copy(colorREdit);
	colorBEdit->setText(std::to_string(mFluidConfig.color[2]));
	colorBEdit->onReturnKeyPress(&MainWindow::OnColorBChanged, this);

	colorGrid->addWidget(colorTitle, 2, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorREdit, 1, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorGEdit, 2, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorBEdit, 3, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));

	colorGroup->add(colorGrid);
	colorGroup->setPosition(0, mainPropsGrid->getSize().y + bloomGroup->getSize().y + 5 * 20);
	mMenuGroup->add(colorGroup);

	// MODE GROUP
	auto modeGroup = tgui::Group::create();
	auto modeGrid = tgui::Grid::create();

	// parallel mode
	auto modeTitle = tgui::Label::copy(velocityDiffTitle);
	modeTitle->setText("Parallel mode:");
	auto modeCheckbox = tgui::CheckBox::create();
	modeCheckbox->setChecked(mFluidConfig.bParallel);
	modeCheckbox->onChange(&MainWindow::OnModeChecked, this);

	modeGrid->addWidget(modeTitle, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	modeGrid->addWidget(modeCheckbox, 0, 1, tgui::Grid::Alignment::Center, tgui::Padding(20, 4, 0, 4));

	modeGroup->add(modeGrid);
	modeGroup->setPosition(0, mainPropsGrid->getSize().y + bloomGroup->getSize().y + colorGroup->getSize().y + 225);
	mMenuGroup->add(modeGroup);

	mMenuGroup->setPosition(10, 10);
	mGui.add(mMenuGroup);
}

void MainWindow::OnVelocityDiffusionChanged(const tgui::String& _newValue)
{
	mFluidConfig.velocityDiffusion = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnColorDiffusionChanged(const tgui::String& _newValue)
{
	mFluidConfig.colorDiffusion = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnDensityDiffusionChanged(const tgui::String& _newValue)
{
	mFluidConfig.densityDiffusion = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnPressureChanged(const tgui::String& _newValue)
{
	mFluidConfig.velocityDiffusion = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnVorticityChanged(const tgui::String& _newValue)
{
	mFluidConfig.vorticity = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnForceChanged(const tgui::String& _newValue)
{
	mFluidConfig.forceScale = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnRadiusChanged(const tgui::String& _newValue)
{
	mFluidConfig.radius = _newValue.toInt();
	bFluidChanged = true;
}

void MainWindow::OnBloomIntenseChanged(const tgui::String& _newValue)
{
	mFluidConfig.bloomIntense = _newValue.toFloat();
	bFluidChanged = true;
}

void MainWindow::OnColorRChanged(const tgui::String& _newValue)
{
	mFluidConfig.color[0] = _newValue.toInt();
	bFluidChanged = true;
}

void MainWindow::OnColorGChanged(const tgui::String& _newValue)
{
	mFluidConfig.color[1] = _newValue.toInt();
	bFluidChanged = true;
}

void MainWindow::OnColorBChanged(const tgui::String& _newValue)
{
	mFluidConfig.color[2] = _newValue.toInt();
	bFluidChanged = true;
}

void MainWindow::OnBloomChecked(bool _isChecked)
{
	mFluidConfig.bloomEnabled = _isChecked;
	bFluidChanged = true;
}

void MainWindow::OnColorfulChecked(bool _isChecked)
{
	mFluidConfig.bColorful = _isChecked;
	bFluidChanged = true;
}

void MainWindow::OnModeChecked(bool _isChecked)
{
	mFluidConfig.bParallel = _isChecked;
	bFluidChanged = true;
}
