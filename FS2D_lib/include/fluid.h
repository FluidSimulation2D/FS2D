#include <stdint.h>

// Export struct
struct Config
{
	float velocityDiffusion, colorDiffusion, densityDiffusion;
	float pressure, vorticity, forceScale, bloomIntense, dt;
	int radius, velocityIterations, pressureIterations, xThreads, yThreads;
	bool bloomEnabled, bColorful, bParallel;

	unsigned char color[3] { 255, 0, 0 };

	Config(float _velocityDiffusion = 0.8f, float _colorDiffusion = 0.8f, float _densityDiffusion = 1.2f, float _pressure = 1.5f, float _vorticity = 50.0f,
		float _forceScale = 5000.0f, float _bloomIntense = 0.1f, float _dt = 0.02, int _radius = 400, int _velocityIterations = 20,
		int _pressureIterations = 40, int _xThreads = 80, int _yThreads = 1, bool _bloomEnabled = true, bool _colorful = true, bool _parallel = true) :
		velocityDiffusion(_velocityDiffusion), colorDiffusion(_colorDiffusion), densityDiffusion(_densityDiffusion), pressure(_pressure), vorticity(_vorticity),
		forceScale(_forceScale), bloomIntense(_bloomIntense), dt(_dt), radius(_radius), velocityIterations(_velocityIterations),
		pressureIterations(_pressureIterations), xThreads(_xThreads), yThreads(_yThreads), bloomEnabled(_bloomEnabled), bColorful(_colorful), bParallel(_parallel) {}
};

// Export functions
void setConfig(float _velocityDiffusion = 0.8f, float _colorDiffusion = 0.8f, float _densityDiffusion = 1.2f, float _pressure = 1.5f, float _vorticity = 50.0f,
	float _forceScale = 5000.0f, float _bloomIntense = 0.1f, float _dt = 0.02, int _radius = 400, int _velocityIterations = 20,
	int _pressureIterations = 40, int _xThreads = 80, int _yThreads = 1, bool _bloomEnabled = true, bool _colorful = true, bool _parallel = true,
	unsigned char _r = 255, unsigned char _g = 0, unsigned char _b = 0);

void initialization(int _x, int _y);

void compute(uint8_t* _resultField, int _prevX, int _prevY, int _currX, int _currY, bool _isPressed);

void finalization();