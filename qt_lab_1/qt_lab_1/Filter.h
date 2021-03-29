#pragma once
#include <QImage>
template <class T>
T clamp(T value, T max, T min)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

class Kernel
{
protected:
	std::unique_ptr<float[]> data;
	std::size_t radius;
	std::size_t getLen() const { return getSize() * getSize(); }
public:
	Kernel(std::size_t radius) : radius(radius)
	{
		data = std::make_unique<float[]>(getLen());
	}
	Kernel(const Kernel& other) : Kernel(other.radius)
	{
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}
	std::size_t getRadius() const { return radius; }
	std::size_t getSize() const { return 2 * radius + 1; }
	float operator [] (std::size_t id) const { return data[id]; }
	float& operator [] (std::size_t id) { return data[id]; }
};
class GaussianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	GaussianKernel(std::size_t radius = 7, float sigma = 2.f) : Kernel(radius)
	{
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		for (int x = -signed_radius; x < signed_radius; x++)
			for (int y = -signed_radius; y < signed_radius; y++)
			{
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
			}
		for (std::size_t i = 0; i < getLen(); i++)
		{
			data[i] /= norm;
		}
	}
};
class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	BlurKernel(std::size_t radius = 2) : Kernel(radius)
	{
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = 1.0f / getLen();
	}
};
class SobelKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SobelKernel(std::size_t radius = 2) : Kernel(radius)
	{
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = 1.0f / getLen();
	}
};
class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;
};

class InvertFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};
class GrayScaleFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};
class SepiaFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};
class BrightFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};
