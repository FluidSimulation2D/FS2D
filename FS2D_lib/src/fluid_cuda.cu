#include "fluid.h"

#include "cuda_runtime.h"
#include "cuda_runtime_api.h"
#include "cudart_platform.h"
#include "device_launch_parameters.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#define sci(x) static_cast<int>(x)
#define scu(x) static_cast<uint8_t>(x)

struct Coord2D
{
	float x = 0.0f, y = 0.0f;

	__device__ Coord2D operator-(const Coord2D& _other);
	__device__ Coord2D operator-();
	__device__ Coord2D operator+(const Coord2D& _other);
	__device__ Coord2D operator*(float _number);

	__device__ Coord2D operator-() const;
	__device__ Coord2D operator*(float _number) const;
};

struct Coord2DI
{
	int x = 0, y = 0;
};

struct RGBColor
{
	float R = 0.0f, G = 0.0f, B = 0.0f;

	RGBColor() = default;
	__host__ __device__ RGBColor(float _R, float _G, float _B);

	__host__ __device__ RGBColor operator+(const RGBColor& _other);
	__host__ __device__ RGBColor operator*(float _num);

	__host__ __device__ RGBColor operator*(float _num) const;
};

struct Particle
{
	Coord2D u;
	RGBColor color;
};

static Config globalConfig;

static const int globalColorArraySize = 7;
static RGBColor globalColorArray[globalColorArraySize], globalCurrentColor;
static uint8_t* globalColorField;

static Particle *globalCurrField, *globalPrevField;
static unsigned int globalXSize, globalYSize;

static float *globalPrevPressure, *globalCurrPressure, *globalVorticityField;
static float elapsedTime = 0.0f, timeSincePress = 0.0f;

__device__ Coord2D Coord2D::operator-(const Coord2D& _other)
{
	return { x - _other.x, y - _other.y };
}

__device__ Coord2D Coord2D::operator-()
{
	return { -x, -y };
}

__device__ Coord2D Coord2D::operator+(const Coord2D& _other)
{
	return { x + _other.x, y + _other.y };
}

__device__ Coord2D Coord2D::operator*(float _num)
{
	return { x * _num, y * _num };
}

__device__ Coord2D Coord2D::operator-() const
{
	return { -x, -y };
}

__device__ Coord2D Coord2D::operator*(float _num) const
{
	return { x * _num, y * _num };
}

__host__ __device__ RGBColor::RGBColor(float _R, float _G, float _B)
	: R(min(255.0f, max(0.0f, _R))), G(min(255.0f, max(0.0f, _G))), B(min(255.0f, max(0.0f, _B))) {}

__host__ __device__ RGBColor RGBColor::operator+(const RGBColor& _other)
{
	return { R + _other.R, G + _other.G, B + _other.B };
}

__host__ __device__ RGBColor RGBColor::operator*(float _num)
{
	return { R * _num, G * _num, B * _num };
}

__host__ __device__ RGBColor RGBColor::operator*(float _num) const
{
	return { R * _num, G * _num, B * _num };
}

__device__ Coord2D computeU(Particle* _particleField, size_t _xSize, size_t _ySize, const Coord2DI& _coordV, const Coord2D& _coordB, float _alpha, float _beta)
{
	int posX = _coordV.x, posY = _coordV.y;
	auto f = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _particleField[_y * _xSize + _x].u : -_coordB; };
	return (f(posX, posY - 1) + f(posX, posY + 1) + f(posX + 1, posY) + f(posX - 1, posY) + _coordB * _alpha) * (1.0f / _beta);
}

__device__ float computeP(float* _pressureField, size_t _xSize, size_t _ySize, const Coord2DI& _coord, float _B, float _alpha, float _beta)
{
	int posX = _coord.x, posY = _coord.y;
	auto f = [&](int x, int y) { return (x < _xSize && x >= 0 && y < _ySize && y >= 0) ? _pressureField[y * _xSize + x] : -_pressureField[posY * _xSize + posX]; };
	return (f(posX, posY - 1) + f(posX, posY + 1) + f(posX - 1, posY) + f(posX + 1, posY) + _alpha * _B) * (1.0f / _beta);
}

__device__ RGBColor computeC(Particle* _colorField, size_t _xSize, size_t _ySize, const Coord2DI& _coordPos, const RGBColor& _colorB, float _alpha, float _beta)
{
	int x = _coordPos.x, y = _coordPos.y;
	auto f = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _colorField[_y * _xSize + _x].color : RGBColor(); };
	return (f(x, y - 1) + f(x, y + 1) + f(x - 1, y) + f(x + 1, y) + _colorB * _alpha) * (1.0f / _beta);
}

__device__ Particle interpolate(const Coord2D& _pos, Particle* _particleField, size_t _xSize, size_t _ySize)
{
	int x1 = sci(_pos.x), y1 = sci(_pos.y),
		x2 = sci(_pos.x) + 1, y2 = sci(_pos.y) + 1;

	auto clamp = [](int _val, int _maxVal) { return min(_maxVal - 1, max(0, _val)); };

	Particle q1 = _particleField[clamp(y1, _ySize) * _xSize + clamp(x1, _xSize)],
		 q2 = _particleField[clamp(y2, _ySize) * _xSize + clamp(x1, _xSize)],
		 q3 = _particleField[clamp(y1, _ySize) * _xSize + clamp(x2, _xSize)],
		 q4 = _particleField[clamp(y2, _ySize) * _xSize + clamp(x2, _xSize)];

	float t1 = (x2 - _pos.x) / (x2 - x1), t2 = (_pos.x - x1) / (x2 - x1),
		  t3 = (y2 - _pos.y) / (y2 - y1), t4 = (_pos.y - y1) / (y2 - y1);

	return { (q1.u * t1 + q3.u * t2) * t3 + (q2.u * t1 + q4.u * t2) * t4,
		 (q2.color * t1 + q4.color * t2) * t3 + (q2.color * t1 + q4.color * t2) * t4 };
}

__device__ float divergency(Particle* _particleField, size_t _xSize, size_t _ySize, const Coord2DI& _coordPos)
{
	int posX = _coordPos.x, posY = _coordPos.y;
	Particle& C = _particleField[posY * _xSize + posX];
	auto f = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _particleField[_y * _xSize + _x].u.x : -C.u.x; };
	auto g = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _particleField[_y * _xSize + _x].u.y : -C.u.y; };
	return (f(posX + 1, posY) - f(posX - 1, posY) + g(posX, posY + 1) - g(posX, posY - 1)) / 2.0f;
}

__device__ Coord2D gradient(float* _field, size_t _xSize, size_t _ySize, const Coord2DI& _coordPos)
{
	int posX = _coordPos.x, posY = _coordPos.y;
	auto f = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _field[_y * _xSize + _x] : _field[posY * _xSize + posX]; };
	return { (f(posX + 1, posY) - f(posX - 1, posY)) * 0.5f, (f(posX, posY + 1) - f(posX, posY - 1)) / 2.0f };
}

__device__ Coord2D absGradient(float* _field, size_t _xSize, size_t _ySize, const Coord2DI& _coordPos)
{
	int posX = _coordPos.x, posY = _coordPos.y;
	auto f = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _field[_y * _xSize + _x] : _field[posY * _xSize + posX]; };
	return { (abs(f(posX + 1, posY)) - abs(f(posX - 1, posY))) / 2.0f, (abs(f(posX, posY + 1)) - abs(f(posX, posY - 1))) / 2.0f };
}

__device__ float curl(Particle* _particleField, size_t _xSize, size_t _ySize, const Coord2DI& _coordPos)
{
	int posX = _coordPos.x, posY = _coordPos.y;
	auto f = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _particleField[_y * _xSize + _x].u.x : -_particleField[posY * _xSize + posX].u.x; };
	auto g = [&](int _x, int _y) { return (_x < _xSize && _x >= 0 && _y < _ySize && _y >= 0) ? _particleField[_y * _xSize + _x].u.y : -_particleField[posY * _xSize + posX].u.y; };
	return ((g(posX, posY + 1) - g(posX, posY - 1)) - (f(posX + 1, posY) - f(posX - 1, posY))) / 2.0f;
}

__global__ void advect(Particle* _particleCurrField, Particle* _particlePrevField, size_t _xSize, size_t _ySize, float _densityDiffusion, float _dt)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	float decay = 1.0f / (1.0f + _densityDiffusion * _dt);

	Coord2D pos = { static_cast<float>(x), static_cast<float>(y) };
	Particle &prevP = _particlePrevField[y * _xSize + x],
			 p = interpolate(pos - prevP.u * _dt, _particlePrevField, _xSize, _ySize);

	_particleCurrField[y * _xSize + x] = { p.u * decay, { pow(p.color.R, 1.005f) * decay, pow(p.color.G, 1.005f) * decay, pow(p.color.B, 1.005f) * decay } };
}

__global__ void paint(Particle* _particleField, uint8_t* _colorField, size_t _xSize, size_t _ySize)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	float R = _particleField[y * _xSize + x].color.R, G = _particleField[y * _xSize + x].color.G, B = _particleField[y * _xSize + x].color.B;

	_colorField[4 * (y * _xSize + x) + 0] = scu(min(255.0f, 255.0f * R));
	_colorField[4 * (y * _xSize + x) + 1] = scu(min(255.0f, 255.0f * G));
	_colorField[4 * (y * _xSize + x) + 2] = scu(min(255.0f, 255.0f * B));
	_colorField[4 * (y * _xSize + x) + 3] = scu(255.0f);
}

__global__ void project(Particle* _particleField, size_t _xSize, size_t _ySize, float* _fieldP)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	Coord2D& u = _particleField[y * _xSize + x].u;
	u = u - gradient(_fieldP, _xSize, _ySize, { x, y });
}

__global__ void computeVelocity(Particle* _particleCurrField, Particle* _particlePrevField, size_t _xSize, size_t _ySize, float _velocityDiffusion, float _dt)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	float alpha = _velocityDiffusion * _velocityDiffusion / _dt;
	_particleCurrField[y * _xSize + x].u = computeU(_particlePrevField, _xSize, _ySize, { x, y }, _particlePrevField[y * _xSize + x].u, alpha, 4.0f + alpha);
}

__global__ void computePressure(Particle* _particleCurrField, size_t _xSize, size_t _ySize, float* _currP, float* _prevP, float _pressure)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	_currP[y * _xSize + x] = computeP(_prevP, _xSize, _ySize, { x, y }, divergency(_particleCurrField, _xSize, _ySize, { x, y }), -_pressure * _pressure, 4.0f);
}

__global__ void computeColor(Particle* _particleCurrField, Particle* _particlePrevField, size_t _xSize, size_t _ySize, float _colorDiffusion, float _dt)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	float alpha = _colorDiffusion * _colorDiffusion / _dt;
	_particleCurrField[y * _xSize + x].color = computeC(_particlePrevField, _xSize, _ySize, { x, y }, _particlePrevField[y * _xSize + x].color, alpha, 4.0f + alpha);
}

__global__ void computeVorticity(Particle* _particleField, float* _fieldV, size_t _xSize, size_t _ySize)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	_fieldV[y * _xSize + x] = curl(_particleField, _xSize, _ySize, { x, y });
}

__global__ void applyForce(Particle* _particleField, size_t _xSize, RGBColor _color, Coord2D _coordF, Coord2DI _pos, int _r, float _dt)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;
	float e = expf(-((x - _pos.x) * (x - _pos.x) + (y - _pos.y) * (y - _pos.y)) / _r);
	Particle& p = _particleField[y * _xSize + x];

	_color = _color * e + p.color;
	p = { p.u + _coordF * _dt * e, { _color.R, _color.G, _color.B } };
}

__global__ void applyVorticity(Particle* _particleCurrField, Particle* _particlePrevField, float* _fieldV, size_t _xSize, size_t _ySize, float _vorticity, float _dt)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y;

	Particle &pOld = _particlePrevField[y * _xSize + x], &pNew = _particleCurrField[y * _xSize + x];

	Coord2D v = absGradient(_fieldV, _xSize, _ySize, { x, y });
	v.y *= -1.0f;
	Coord2D vNorm = v * (1.0f / (sqrtf(v.x * v.x + v.y * v.y) + 1e-5f));

	pNew = pOld;
	pNew.u = pNew.u + (vNorm * _fieldV[y * _xSize + x] * _vorticity) * _dt;
}

__global__ void applyBloom(uint8_t* _colorField, size_t _xSize, Coord2DI _pos, int _radius, float _bloomIntense)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x, y = blockIdx.y * blockDim.y + threadIdx.y, pos = 4 * (y * _xSize + x);
	float e = _bloomIntense * expf(-((x - _pos.x) * (x - _pos.x) + (y - _pos.y) * (y - _pos.y) + 1.0f) / (_radius * _radius));
	uint8_t R = _colorField[pos + 0], G = _colorField[pos + 1], B = _colorField[pos + 2], maxval = max(R, max(G, B));

	_colorField[pos + 0] = min(255.0f, R + maxval * e);
	_colorField[pos + 1] = min(255.0f, G + maxval * e);
	_colorField[pos + 2] = min(255.0f, B + maxval * e);
}

// Export functions
void setConfig(float _velocityDiffusion, float _colorDiffusion, float _densityDiffusion, float _pressure, float _vorticity,
			   float _forceScale, float _bloomIntense, float _dt, int _radius, int _velocityIterations, int _pressureIterations,
			   int _xThreads, int _yThreads, bool _bloomEnabled, bool _colorful, bool _parallel, unsigned char _r, unsigned char _g, unsigned char _b)
{
	globalConfig.velocityDiffusion = _velocityDiffusion;
	globalConfig.colorDiffusion = _colorDiffusion;
	globalConfig.densityDiffusion = _densityDiffusion;
	globalConfig.pressure = _pressure;
	globalConfig.vorticity = _vorticity;
	globalConfig.forceScale = _forceScale;
	globalConfig.bloomIntense = _bloomIntense;
	globalConfig.dt = _dt;
	globalConfig.radius = _radius;
	globalConfig.velocityIterations = _velocityIterations;
	globalConfig.pressureIterations = _pressureIterations;
	globalConfig.xThreads = _xThreads;
	globalConfig.yThreads = _yThreads;
	globalConfig.bloomEnabled = _bloomEnabled;
	globalConfig.bColorful = _colorful;
	globalConfig.bParallel = _parallel;
	globalConfig.color[0] = _r;
	globalConfig.color[1] = _g;
	globalConfig.color[2] = _b;

	globalColorArray[0] = globalConfig.bColorful ? RGBColor({ 1.0f, 0.0f, 0.0f }) : RGBColor({ globalConfig.color[0] / 255.f, globalConfig.color[1] / 255.f, globalConfig.color[2] / 255.f });
}

void initialization(int _x, int _y)
{
	setConfig();

	globalColorArray[0] = globalConfig.bColorful ? RGBColor({ 1.0f, 0.0f, 0.0f }) : RGBColor({ globalConfig.color[0] / 255.f, globalConfig.color[1] / 255.f, globalConfig.color[2] / 255.f });
	globalColorArray[1] = { 0.0f, 1.0f, 0.0f };
	globalColorArray[2] = { 1.0f, 0.0f, 1.0f };
	globalColorArray[3] = { 1.0f, 1.0f, 0.0f };
	globalColorArray[4] = { 0.0f, 1.0f, 1.0f };
	globalColorArray[5] = { 1.0f, 0.0f, 1.0f };
	globalColorArray[6] = { 1.0f, 0.5f, 0.3f };

	globalCurrentColor = globalConfig.bColorful ? globalColorArray[rand() % globalColorArraySize] : globalColorArray[0];

	globalXSize = _x, globalYSize = _y;

	cudaSetDevice(0);
	cudaError_t error = cudaGetLastError();
	if (error != cudaSuccess)
	{
		std::cout << cudaGetErrorName(error) << std::endl;
		std::abort();
	}

	size_t size = globalXSize * globalYSize;

	cudaMalloc(&globalColorField, 4 * size * sizeof(uint8_t));
	cudaMalloc(&globalPrevField, size * sizeof(Particle));
	cudaMalloc(&globalCurrField, size * sizeof(Particle));
	cudaMalloc(&globalPrevPressure, size * sizeof(float));
	cudaMalloc(&globalCurrPressure, size * sizeof(float));
	cudaMalloc(&globalVorticityField, size * sizeof(float));
}

void finalization()
{
	cudaFree(globalColorField);
	cudaFree(globalPrevField);
	cudaFree(globalCurrField);
	cudaFree(globalPrevPressure);
	cudaFree(globalCurrPressure);
	cudaFree(globalVorticityField);
}

void compute(uint8_t* _resultField, int _prevX, int _prevY, int _currX, int _currY, bool _isPressed)
{
	dim3 threadsPerBlock(globalConfig.bParallel ? globalConfig.xThreads : 1, globalConfig.bParallel ? globalConfig.yThreads : 1);
	dim3 numBlocks(globalXSize / threadsPerBlock.x, globalYSize / threadsPerBlock.y);


	computeVorticity<<<numBlocks, threadsPerBlock>>>(globalPrevField, globalVorticityField, globalXSize, globalYSize);
	applyVorticity<<<numBlocks, threadsPerBlock>>>(globalCurrField, globalPrevField, globalVorticityField, globalXSize, globalYSize, globalConfig.vorticity, globalConfig.dt);
	std::swap(globalPrevField, globalCurrField);

	for (int i = 0; i < globalConfig.velocityIterations; ++i)
	{
		computeVelocity<<<numBlocks, threadsPerBlock>>>(globalCurrField, globalPrevField, globalXSize, globalYSize, globalConfig.velocityDiffusion, globalConfig.dt);
		computeColor<<<numBlocks, threadsPerBlock>>>(globalCurrField, globalPrevField, globalXSize, globalYSize, globalConfig.colorDiffusion, globalConfig.dt);
		std::swap(globalCurrField, globalPrevField);
	}

	if (_isPressed)
	{
		timeSincePress = 0.0f;
		elapsedTime += globalConfig.dt;

		float w = elapsedTime - sci(elapsedTime);
		globalCurrentColor = globalConfig.bColorful ? globalColorArray[sci(elapsedTime) % globalColorArraySize] * (1 - w) + globalColorArray[sci((elapsedTime) + 1) % globalColorArraySize] * w : globalColorArray[0];
		applyForce<<<numBlocks, threadsPerBlock>>>(globalPrevField, globalXSize, globalCurrentColor,
							   { (_currX - _prevX) * globalConfig.forceScale, (_currY - _prevY) * globalConfig.forceScale }, { _currX, _currY }, globalConfig.radius, globalConfig.dt);
	}
	else
	{
		timeSincePress += globalConfig.dt;
	}

	for (int i = 0; i < globalConfig.pressureIterations; ++i)
	{
		computePressure<<<numBlocks, threadsPerBlock>>>(globalPrevField, globalXSize, globalYSize, globalCurrPressure, globalPrevPressure, globalConfig.pressure);
		std::swap(globalPrevPressure, globalCurrPressure);
	}

	project<<<numBlocks, threadsPerBlock>>>(globalPrevField, globalXSize, globalYSize, globalPrevPressure);
	cudaMemset(globalPrevPressure, 0, globalXSize * globalYSize * sizeof(float));

	advect<<<numBlocks, threadsPerBlock>>>(globalCurrField, globalPrevField, globalXSize, globalYSize, globalConfig.densityDiffusion, globalConfig.dt);
	std::swap(globalCurrField, globalPrevField);

	paint<<<numBlocks, threadsPerBlock>>>(globalPrevField, globalColorField, globalXSize, globalYSize);

	if (globalConfig.bloomEnabled && timeSincePress < 5.0f)
	{
		applyBloom<<<numBlocks, threadsPerBlock>>>(globalColorField, globalXSize, { _currX, _currY }, globalConfig.radius, globalConfig.bloomIntense);
	}

	cudaMemcpy(_resultField, globalColorField, globalXSize * globalYSize * 4 * sizeof(uint8_t), cudaMemcpyDeviceToHost);

	cudaError_t error = cudaGetLastError();
	if (error != cudaSuccess)
	{
		std::cout << cudaGetErrorName(error) << std::endl;
	}
}