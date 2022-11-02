#include "window.h"

MainWindow::MainWindow() : sf::RenderWindow({ 1280, 720 }, "Fluid Simuation 2D") // { mBaseProperties.width, mBaseProperties.height }, "")
{
	mBaseProperties = BaseWindowProperties();
	mPixelBuffer.resize(mBaseProperties.fieldWidth * mBaseProperties.fieldHeight * 4);
	mFluidTexture.create(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);

	mGui.setWindow(*this);
}

MainWindow::MainWindow(sf::VideoMode _videoMode, const sf::String& _title, sf::Uint32 _style, const sf::ContextSettings& _ctxSettings) :
	sf::RenderWindow(_videoMode, _title, _style, _ctxSettings),
	mBaseProperties(_videoMode.width, _videoMode.height)
{
	mPixelBuffer.resize(mBaseProperties.fieldWidth * mBaseProperties.fieldHeight * 4);
	mFluidTexture.create(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);

	mGui.setWindow(*this);
}

MainWindow::MainWindow(sf::WindowHandle _windowHandle, const sf::ContextSettings& _ctxSettings) :
	sf::RenderWindow(_windowHandle, _ctxSettings)
{
	mBaseProperties = BaseWindowProperties(getSize().x, getSize().y);

	mPixelBuffer.resize(mBaseProperties.fieldWidth * mBaseProperties.fieldHeight * 4);
	mFluidTexture.create(mBaseProperties.fieldWidth, mBaseProperties.fieldHeight);

	mGui.setWindow(*this);
}

MainWindow::~MainWindow() {}

void MainWindow::mainLoop()
{
	createMenu();
	mFluidSprite.setPosition({ mMenuGroup->getSize().x, 0});
	auto menu_rect = sf::RectangleShape(mMenuGroup->getSize());
	menu_rect.setFillColor({ 200, 200, 200 });
	menu_rect.setOutlineColor({ 180, 180, 180 });
	menu_rect.setOutlineThickness(2.5);

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
					mLastMousePos = { event.mouseButton.x, event.mouseButton.y };
					mLastMousePos /= (int)mBaseProperties.scale;

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
				mCurMousePos = { event.mouseButton.x, event.mouseButton.y };
				mCurMousePos /= (int)mBaseProperties.scale;
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
			computeField();
		}

		mFluidTexture.update(mPixelBuffer.data());
		mFluidSprite.setTexture(mFluidTexture);
		mFluidSprite.setScale({ (float)mBaseProperties.scale, (float)mBaseProperties.scale });

		draw(menu_rect);
		mGui.draw();
		draw(mFluidSprite);
		display();
	}
}

void MainWindow::computeField() {}

void MainWindow::updateFluidConfig() {}

void MainWindow::createMenu()
{
	mMenuGroup = tgui::Group::create({ "17%", "100%" });
	setSize({ sf::Uint16(getSize().x * 1.2f), getSize().y });

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
	radiusEdit->setText(std::to_string(mFluidConfig.forceRadius));
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
	bloomEnableCheckbox->setChecked(true);
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
	colorfulCheckbox->setChecked(false);
	colorfulCheckbox->onChange(&MainWindow::OnColorfulChecked, this);

	colorGrid->addWidget(colorfulTitle, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorfulCheckbox, 0, 1);

	auto colorTitle = tgui::Label::copy(bloomIntenseTitle);
	colorTitle->setText("Edit color:");
	auto colorREdit = tgui::EditBox::copy(bloomIntenseEdit);
	colorREdit->setMaximumCharacters(3U);
	colorREdit->setText(std::to_string(128));
	colorREdit->onReturnKeyPress(&MainWindow::OnColorRChanged, this);
	auto colorGEdit = tgui::EditBox::copy(colorREdit);
	colorGEdit->onReturnKeyPress(&MainWindow::OnColorGChanged, this);
	auto colorBEdit = tgui::EditBox::copy(colorREdit);
	colorBEdit->onReturnKeyPress(&MainWindow::OnColorBChanged, this);

	colorGrid->addWidget(colorTitle, 2, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorREdit, 1, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorGEdit, 2, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	colorGrid->addWidget(colorBEdit, 3, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));

	colorGroup->add(colorGrid);
	colorGroup->setPosition(0, mainPropsGrid->getSize().y + bloomGroup->getSize().y + 5 * 20);
	mMenuGroup->add(colorGroup);

	mMenuGroup->setPosition(10, 10);
	mGui.add(mMenuGroup);
}

void MainWindow::OnVelocityDiffusionChanged(const tgui::String& _newValue)
{
	mFluidConfig.velocityDiffusion = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnColorDiffusionChanged(const tgui::String& _newValue)
{
	mFluidConfig.colorDiffusion = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnDensityDiffusionChanged(const tgui::String& _newValue)
{
	mFluidConfig.densityDiffusion = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnPressureChanged(const tgui::String& _newValue)
{
	mFluidConfig.velocityDiffusion = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnVorticityChanged(const tgui::String& _newValue)
{
	mFluidConfig.vorticity = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnForceChanged(const tgui::String& _newValue)
{
	mFluidConfig.forceScale = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnRadiusChanged(const tgui::String& _newValue)
{
	mFluidConfig.forceRadius = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnBloomIntenseChanged(const tgui::String& _newValue)
{
	mFluidConfig.bloomIntense = _newValue.toFloat();
	updateFluidConfig();
}

void MainWindow::OnColorRChanged(const tgui::String& _newValue)
{
	mFluidConfig.color.r = _newValue.toInt();
	updateFluidConfig();
}

void MainWindow::OnColorGChanged(const tgui::String& _newValue)
{
	mFluidConfig.color.g = _newValue.toInt();
	updateFluidConfig();
}

void MainWindow::OnColorBChanged(const tgui::String& _newValue)
{
	mFluidConfig.color.b = _newValue.toInt();
	updateFluidConfig();
}

void MainWindow::OnBloomChecked(bool _isChecked)
{
	mFluidConfig.bBloomEnabled = _isChecked;
	updateFluidConfig();
}

void MainWindow::OnColorfulChecked(bool _isChecked)
{
	mFluidConfig.bColorful = _isChecked;
	updateFluidConfig();
}
