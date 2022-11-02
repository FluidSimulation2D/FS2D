#include "window.h"

MainWindow::MainWindow() : sf::RenderWindow({ 1280, 720 }, "Fluid Simuation 2D") // { m_base_properties.width, m_base_properties.height }, "")
{
	m_base_properties = BaseWindowProperties();
	m_pixel_buffer.resize(m_base_properties.field_width * m_base_properties.field_height * 4);
	m_fluid_texture.create(m_base_properties.field_width, m_base_properties.field_height);

	m_gui.setWindow(*this);
}

MainWindow::MainWindow(sf::VideoMode _video_mode, const sf::String& _title, sf::Uint32 _style, const sf::ContextSettings& _ctx_settings) :
	sf::RenderWindow(_video_mode, _title, _style, _ctx_settings),
	m_base_properties(_video_mode.width, _video_mode.height)
{
	m_pixel_buffer.resize(m_base_properties.field_width * m_base_properties.field_height * 4);
	m_fluid_texture.create(m_base_properties.field_width, m_base_properties.field_height);

	m_gui.setWindow(*this);
}

MainWindow::MainWindow(sf::WindowHandle _window_handle, const sf::ContextSettings& _ctx_settings) :
	sf::RenderWindow(_window_handle, _ctx_settings)
{
	m_base_properties = BaseWindowProperties(getSize().x, getSize().y);

	m_pixel_buffer.resize(m_base_properties.field_width * m_base_properties.field_height * 4);
	m_fluid_texture.create(m_base_properties.field_width, m_base_properties.field_height);

	m_gui.setWindow(*this);
}

MainWindow::~MainWindow() {}

void MainWindow::mainLoop()
{
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();

	createMenu();
	m_fluid_sprite.setPosition({ m_menu_group->getSize().x, 0});
	auto menu_rect = sf::RectangleShape(m_menu_group->getSize());
	menu_rect.setFillColor({ 200, 200, 200 });
	menu_rect.setOutlineColor({ 180, 180, 180 });
	menu_rect.setOutlineThickness(2.5);

	while (isOpen())
	{
		clear(sf::Color::White);

		end = std::chrono::system_clock::now();
		std::chrono::duration<float> diff = end - start;
		// setTitle("Fluid Simulation: " + std::to_string(int(1.0f / diff.count())) + " FPS");
		start = end;

		sf::Event event;
		while (pollEvent(event))
		{
			m_gui.handleEvent(event);

			if (event.type == sf::Event::Closed)
			{
				close();
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					m_last_mouse_pos = { event.mouseButton.x, event.mouseButton.y };
					m_last_mouse_pos /= (int)m_base_properties.scale;

					b_is_influenced = true;
				}
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				b_is_influenced = false;
			}

			if (event.type == sf::Event::MouseMoved)
			{
				std::swap(m_cur_mouse_pos, m_last_mouse_pos);
				m_cur_mouse_pos = { event.mouseButton.x, event.mouseButton.y };
				m_cur_mouse_pos /= (int)m_base_properties.scale;
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					b_is_paused = !b_is_paused;
				}
			}
		}

		float dt = 0.02f;

		if (!b_is_paused)
		{
			computeField();
		}

		m_fluid_texture.update(m_pixel_buffer.data());
		m_fluid_sprite.setTexture(m_fluid_texture);
		m_fluid_sprite.setScale({ (float)m_base_properties.scale, (float)m_base_properties.scale });

		draw(menu_rect);
		m_gui.draw();
		draw(m_fluid_sprite);
		display();
	}
}

void MainWindow::computeField() {}

void MainWindow::updateFluidConfig() {}

void MainWindow::createMenu()
{
	m_menu_group = tgui::Group::create({ "17%", "100%" });
	setSize({ sf::Uint16(getSize().x * 1.2f), getSize().y });

	/// MAIN PROPERTIES GROUP
	auto main_props_group = tgui::Group::create();
	auto main_props_grid = tgui::Grid::create();

	// velocity diffusion
	auto v_diff_title = tgui::Label::create();
	v_diff_title->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
	v_diff_title->setHeight(18U);
	v_diff_title->setTextSize(14U);
	v_diff_title->setWidth(150);
	v_diff_title->setText("Velocity diffusion:");

	auto v_diff_edit = tgui::EditBox::create();
	v_diff_edit->setHeight(18U);
	v_diff_edit->setMaximumCharacters(6U);
	v_diff_edit->setTextSize(14U);
	v_diff_edit->setWidth(60);
	v_diff_edit->setText(std::to_string(m_fluid_config.velocity_diffusion));
	v_diff_edit->onReturnKeyPress(&MainWindow::OnVelocityDiffusionChanged, this);

	main_props_grid->addWidget(v_diff_title, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(v_diff_edit, 0, 1);

	// color diffusion
	auto c_diff_title = tgui::Label::copy(v_diff_title);
	c_diff_title->setText("Color diffusion:");
	tgui::EditBox::Ptr c_diff_edit = tgui::EditBox::copy(v_diff_edit);
	c_diff_edit->setText(std::to_string(m_fluid_config.color_diffusion));
	c_diff_edit->onReturnKeyPress(&MainWindow::OnColorDiffusionChanged, this);

	main_props_grid->addWidget(c_diff_title, 1, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(c_diff_edit, 1, 1);

	// density diffusion
	auto d_diff_title = tgui::Label::copy(v_diff_title);
	d_diff_title->setText("Density diffusion:");
	tgui::EditBox::Ptr d_diff_edit = tgui::EditBox::copy(v_diff_edit);
	d_diff_edit->setText(std::to_string(m_fluid_config.density_diffusion));
	d_diff_edit->onReturnKeyPress(&MainWindow::OnDensityDiffusionChanged, this);

	main_props_grid->addWidget(d_diff_title, 2, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(d_diff_edit, 2, 1);

	// pressure
	auto pressure_title = tgui::Label::copy(v_diff_title);
	pressure_title->setText("Pressure:");
	tgui::EditBox::Ptr pressure_edit = tgui::EditBox::copy(v_diff_edit);
	pressure_edit->setText(std::to_string(m_fluid_config.pressure));
	pressure_edit->onReturnKeyPress(&MainWindow::OnPressureChanged, this);

	main_props_grid->addWidget(pressure_title, 3, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(pressure_edit, 3, 1);

	// vorticity
	auto vorticity_title = tgui::Label::copy(v_diff_title);
	vorticity_title->setText("Vorticity:");
	tgui::EditBox::Ptr vorticity_edit = tgui::EditBox::copy(v_diff_edit);
	vorticity_edit->setText(std::to_string(m_fluid_config.vorticity));
	vorticity_edit->onReturnKeyPress(&MainWindow::OnVorticityChanged, this);

	main_props_grid->addWidget(vorticity_title, 4, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(vorticity_edit, 4, 1);

	// force scale
	auto force_title = tgui::Label::copy(v_diff_title);
	force_title->setText("Force scale:");
	tgui::EditBox::Ptr force_edit = tgui::EditBox::copy(v_diff_edit);
	force_edit->setText(std::to_string(m_fluid_config.force_scale));
	force_edit->onReturnKeyPress(&MainWindow::OnForceChanged, this);

	main_props_grid->addWidget(force_title, 5, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(force_edit, 5, 1);

	// radius
	auto radius_title = tgui::Label::copy(v_diff_title);
	radius_title->setText("Radius:");
	tgui::EditBox::Ptr radius_edit = tgui::EditBox::copy(v_diff_edit);
	radius_edit->setText(std::to_string(m_fluid_config.force_radius));
	radius_edit->onReturnKeyPress(&MainWindow::OnRadiusChanged, this);

	main_props_grid->addWidget(radius_title, 6, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	main_props_grid->addWidget(radius_edit, 6, 1);

	main_props_grid->setAutoSize(true);

	main_props_group->add(main_props_grid);
	m_menu_group->add(main_props_group);

	/// BLOOM GROUP
	auto bloom_group = tgui::Group::create();
	auto bloom_grid = tgui::Grid::create();

	// bloom enabled
	auto bloom_enable_title = tgui::Label::copy(v_diff_title);
	bloom_enable_title->setText("Bloom:");
	auto bloom_enable_checkbox = tgui::CheckBox::create();
	bloom_enable_checkbox->setChecked(true);
	bloom_enable_checkbox->onChange(&MainWindow::OnBloomChecked, this);

	bloom_grid->addWidget(bloom_enable_title, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	bloom_grid->addWidget(bloom_enable_checkbox, 0, 1);

	// bloom intense
	auto bloom_intense_title = tgui::Label::copy(v_diff_title);
	bloom_intense_title->setText("Bloom intense:");
	auto bloom_intense_edit = tgui::EditBox::copy(v_diff_edit);
	bloom_intense_edit->setText(std::to_string(m_fluid_config.bloom_intense));
	bloom_intense_edit->onReturnKeyPress(&MainWindow::OnBloomIntenseChanged, this);

	bloom_grid->addWidget(bloom_intense_title, 1, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	bloom_grid->addWidget(bloom_intense_edit, 1, 1);

	bloom_grid->setAutoSize(true);

	bloom_group->add(bloom_grid);
	bloom_group->setPosition(0, main_props_grid->getSize().y + 20);
	m_menu_group->add(bloom_group);

	/// COLOR GROUP
	auto color_group = tgui::Group::create();
	auto color_grid = tgui::Grid::create();

	// color enabled
	auto colorful_title = tgui::Label::copy(v_diff_title);
	colorful_title->setText("Colorful:");
	auto colorful_checkbox = tgui::CheckBox::create();
	colorful_checkbox->setChecked(false);
	colorful_checkbox->onChange(&MainWindow::OnColorfulChecked, this);

	color_grid->addWidget(colorful_title, 0, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	color_grid->addWidget(colorful_checkbox, 0, 1);

	auto color_title = tgui::Label::copy(bloom_intense_title);
	color_title->setText("Edit color:");
	auto rcolor_edit = tgui::EditBox::copy(bloom_intense_edit);
	rcolor_edit->setMaximumCharacters(3U);
	rcolor_edit->setText(std::to_string(128));
	rcolor_edit->onReturnKeyPress(&MainWindow::OnRColorChanged, this);
	auto gcolor_edit = tgui::EditBox::copy(rcolor_edit);
	gcolor_edit->onReturnKeyPress(&MainWindow::OnGColorChanged, this);
	auto bcolor_edit = tgui::EditBox::copy(rcolor_edit);
	bcolor_edit->onReturnKeyPress(&MainWindow::OnBColorChanged, this);

	color_grid->addWidget(color_title, 2, 0, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	color_grid->addWidget(rcolor_edit, 1, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	color_grid->addWidget(gcolor_edit, 2, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));
	color_grid->addWidget(bcolor_edit, 3, 1, tgui::Grid::Alignment::Left, tgui::Padding(0, 4, 0, 4));

	color_group->add(color_grid);
	color_group->setPosition(0, main_props_grid->getSize().y + bloom_group->getSize().y + 5 * 20);
	m_menu_group->add(color_group);

	m_menu_group->setPosition(10, 10);
	m_gui.add(m_menu_group);
}

void MainWindow::OnVelocityDiffusionChanged(const tgui::String& _new_value)
{
	m_fluid_config.velocity_diffusion = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnColorDiffusionChanged(const tgui::String& _new_value)
{
	m_fluid_config.color_diffusion = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnDensityDiffusionChanged(const tgui::String& _new_value)
{
	m_fluid_config.density_diffusion = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnPressureChanged(const tgui::String& _new_value)
{
	m_fluid_config.velocity_diffusion = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnVorticityChanged(const tgui::String& _new_value)
{
	m_fluid_config.vorticity = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnForceChanged(const tgui::String& _new_value)
{
	m_fluid_config.force_scale = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnRadiusChanged(const tgui::String& _new_value)
{
	m_fluid_config.force_radius = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnBloomIntenseChanged(const tgui::String& _new_value)
{
	m_fluid_config.velocity_diffusion = _new_value.toFloat();
	updateFluidConfig();
}

void MainWindow::OnRColorChanged(const tgui::String& _new_value)
{
	m_fluid_config.color.r = _new_value.toInt();
	updateFluidConfig();
}

void MainWindow::OnGColorChanged(const tgui::String& _new_value)
{
	m_fluid_config.color.g = _new_value.toInt();
	updateFluidConfig();
}

void MainWindow::OnBColorChanged(const tgui::String& _new_value)
{
	m_fluid_config.color.b = _new_value.toInt();
	updateFluidConfig();
}

void MainWindow::OnBloomChecked(bool _is_checked)
{
	m_fluid_config.b_bloom_enabled = _is_checked;
	updateFluidConfig();
}

void MainWindow::OnColorfulChecked(bool _is_checked)
{
	m_fluid_config.b_colorful = _is_checked;
	updateFluidConfig();
}
