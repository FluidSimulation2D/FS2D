#include <stdint.h>

// Export struct
struct Config
{
	float velocityDiffusion, colorDiffusion, densityDiffusion;
	float pressure, vorticity, forceScale, bloomIntense, dt;
	int radius, velocityIterations, pressureIterations, xThreads, yThreads;
	bool bloomEnabled, bColorful;
};

// Export functions
void setConfig(float _velocityDiffusion = 0.8f, float _colorDiffusion = 0.8f, float _densityDiffusion = 1.2f, float _pressure = 1.5f, float _vorticity = 50.0f,
	float _forceScale = 5000.0f, float _bloomIntense = 0.1f, float _dt = 0.02, int _radius = 400, int _velocityIterations = 20,
	int _pressureIterations = 40, int _xThreads = 80, int _yThreads = 1, bool _bloomEnabled = true, bool _colorful = true);

void initialization(int _x, int _y);

void compute(uint8_t* _resultField, int _prevX, int _prevY, int _currX, int _currY, bool _isPressed);

void finalization();