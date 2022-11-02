#include <iostream>
#include <chrono>

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "sum.h"
#include "window.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define SCALE 2
#define FIELD_WIDTH WINDOW_WIDTH / SCALE
#define FIELD_HEIGHT WINDOW_HEIGHT / SCALE

int main()
{
	MainWindow window;

	window.mainLoop();

	return 0;
}
