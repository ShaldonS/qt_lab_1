#pragma once
#include <QImage>
#include <vector>
#include <cmath>
#include <iostream>

template <class T>
T tclamp(T value, T max, T min)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;
};

QImage Filter::process(const QImage& img) const
{
	QImage result(img);
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	return result;
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

class MatrixFilter :public Filter
{
protected:
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MatrixFilter(const Kernel& kernel) : mKernel(kernel) {};
	virtual ~MatrixFilter() = default;
};

QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(tclamp(x + j, img.width() - 1, 0), tclamp(y + i, img.height() - 1, 0));
			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	return QColor(tclamp(returnR, 255.f, 0.f), tclamp(returnG, 255.f, 0.f), tclamp(returnB, 255.f, 0.f));
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

class  GaussianFilter : public MatrixFilter
{
public:
	GaussianFilter(std::size_t radius = 3) : MatrixFilter(GaussianKernel(radius)) {}
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

class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(std::size_t radius = 1) : MatrixFilter(BlurKernel(radius)) {}
};

class EmbossmentKernel : public Kernel
{
public:
	using Kernel::Kernel;
	EmbossmentKernel(std::size_t radius = 1) : Kernel(radius)
	{
		data[0] = 0, data[1] = 1, data[2] = 0,
		data[3] = 1, data[4] = 0, data[5] = -1,
		data[6] = 0, data[7] = -1, data[8] = 0;
	}
};

class EmbossmentFilter : public MatrixFilter
{
public:
	EmbossmentFilter(std::size_t radius = 1) : MatrixFilter(EmbossmentKernel(radius)){}
};


class SobelKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SobelKernel( std::size_t radius) : Kernel(radius)
	{
		static const float KERNELDATA[]{
	-1, -2, -1,
	0, 0, 0,
	1, 2, 1
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

class SobelFilter : public MatrixFilter
{
public:
	SobelFilter( std::size_t radius = 1) : MatrixFilter(SobelKernel(radius)) {}
};

class InvertFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}

class GrayScaleFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor GrayScaleFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue(), 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue(), 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue());
	return color;
}

class SepiaFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor SepiaFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	float intensity = 0.299f * color.red() + 0.587f * color.green() + 0.114f * color.blue();
	float k = 10;
	int r = (int)tclamp(2 * k + intensity, 255.f, 0.f);
	int g = (int)tclamp(0.5f * k + intensity, 255.f, 0.f);
	int b = (int)tclamp(intensity - 1 * k, 255.f, 0.f);
	color.setRgb(r, g, b);
	return color;
}

class BrightFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor BrightFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int k = 20;
	QColor color = img.pixelColor(x, y);
	int r = (int)tclamp(color.red() + k, 255, 0);
	int g = (int)tclamp(color.green() + k, 255, 0);
	int b = (int)tclamp(color.blue() + k, 255, 0);
	color.setRgb(r, g, b);
	return color;
}
class СorrectionFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor СorrectionFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);

	int big = 10000;
	int r = tclamp(color.red() * 255 / big, 255, 0);
	int g = tclamp(color.green() * 255 / big, 255, 0);
	int b = tclamp(color.blue() * 255 / big, 255, 0);
	color.setRgb(r, g, b);
	return color;
}

class MotionBlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MotionBlurKernel(int n = 2, std::size_t radius = 1) : Kernel(radius)
	{
		for (std::size_t x = 0; x < getSize(); x++)
		{
			for (std::size_t y = 0; y < getSize(); y++)
			{
				if (x == y)
				{
					data[x * getSize() + y] = (float)1 / n;
				}
				else
				{
					data[x * getSize() + y] = 0;
				}
			}
		}
	}
};

class MotionBlurFilter : public MatrixFilter
{
public:
	MotionBlurFilter(int n = 3, std::size_t radius = 1) : MatrixFilter(MotionBlurKernel(n, radius)) {}
};

class GreyWorldFilter : public Filter
{
protected:
	double Rs, Gs, Bs, AVG;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	QImage process(const QImage& img);
	
};

QColor GreyWorldFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(tclamp((AVG * color.red() / Rs), 255.0, 0.0), tclamp((AVG * color.green() / Gs), 255.0, 0.0), tclamp((AVG * color.blue() / Bs), 255.0, 0.0));
	return color;
}

QImage GreyWorldFilter::process(const QImage& img)
{
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor tmp = img.pixelColor(x, y);
			Rs += tmp.red();
			Gs += tmp.green();
			Bs += tmp.blue();
		}

	Rs /= img.width() * img.height();
	Gs /= img.width() * img.height();
	Bs /= img.width() * img.height();

	AVG = (Rs + Gs + Bs) / 3;

	return Filter::process(img);
}

class SharpnessKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SharpnessKernel(std::size_t radius = 2) : Kernel(radius)
	{
		//for (std::size_t i = 0; i < getLen(); i++)
			//data[i] = -9.0f * getLen();
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;

	data[3] = 0;
	data[4] = 0;
	data[5] = 4;

	data[6] = -1;
	data[7] = -1;
	data[8] = -1;
		/*data[0] = 0;
		data[1] = -1;
		data[2] = 0;
		data[3] = -1;
		data[4] = 5;
		data[5] = -1;
		data[6] = 0;
		data[7] = -1;
		data[8] = 0;*/
	}

};

class SharpnessFilter : public MatrixFilter //резкость
{
public:
	SharpnessFilter (std::size_t radius = 2) : MatrixFilter(SharpnessKernel(radius)) {}
};

class GlassFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor GlassFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int x_1(0), y_1(0);
	x_1 = x - (rand() % 2 - 0.5) * 5;
	y_1 = y - (rand() % 2 - 0.5) * 5;
	QColor color;
	if (x_1 >= 0 && x_1 < img.width() && y_1 >= 0 && y_1 < img.height())
	{
		color = img.pixelColor(x_1, y_1);
		color.setRgb(color.red(), color.green(), color.blue());
		return color;
	}
	else
	{
		color = img.pixelColor(x, y);
		color.setRgb(color.red(), color.green(), color.blue());
		return color;
	}
}

class WavesFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor WavesFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int x_1(0), y_1(0);
	double pi = 3.14;
	x_1 = x + 20 * sin(2 * pi*x / 60);
	y_1 = y;
	QColor color;
	if (x_1 >= 0 && x_1 < img.width() && y_1 >= 0 && y_1 < img.height())
	{
		color = img.pixelColor(x_1, y_1);
		color.setRgb(color.red(), color.green(), color.blue());
		return color;
	}
	else
	{
		color = img.pixelColor(x, y);
		color.setRgb(color.red(), color.green(), color.blue());
		return color;
	}
}

/*class MedianFilter : public Filter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

QColor MedianFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	std::vector<QColor> vec(25, QColor(0, 0, 0));
	int size(0);
	for (int i = -2; i <= 2; ++i)
	{
		for (int j = -2; j <= 2; ++j)
		{
			if (x + i < 0 || x + i > img.width())
				continue;
			if (y + j < 0 || y + j > img.height())
				continue;
			vec[size++] = img.pixelColor(x+i, y+j);
		}
	}
	std::sort(vec.begin(), vec.begin() + size, [](QColor A, QColor B) {return (A.red() + A.green() + A.blue() < (B.red() + B.green() + B.blue())); });
	return vec[3];
}*/

class MedianFilter : public Filter
{
protected:
	int radius;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MedianFilter(int _r) : radius(_r) {}
};

QColor MedianFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int returnR = 0;
	int returnG = 0;
	int returnB = 0;
	short int size = 2 * radius + 1;

	short int** data = new short int* [3];
	for (int i = 0; i < 3; i++)
		data[i] = new short int[size * size];


	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			data[0][idx] = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0), tclamp<float>(y + i, img.height() - 1, 0)).red();
			data[1][idx] = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0), tclamp<float>(y + i, img.height() - 1, 0)).green();
			data[2][idx] = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0), tclamp<float>(y + i, img.height() - 1, 0)).blue();
		}

	std::sort(data[0], data[0] + size * size);
	returnR = data[0][(size * size - 1) / 2];

	std::sort(data[1], data[1] + size * size);
	returnG = data[1][(size * size - 1) / 2];

	std::sort(data[2], data[2] + size * size);
	returnB = data[2][(size * size - 1) / 2];

	return QColor(
		tclamp<float>(returnR, 255, 0),
		tclamp<float>(returnG, 255, 0),
		tclamp<float>(returnB, 255, 0));
}

class MorphoKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MorphoKernel(size_t radius = 1) : Kernel(radius)
	{
		data[0] = 0; data[1] = 1; data[2] = 0;
		data[3] = 1; data[4] = 1; data[5] = 1;
		data[6] = 0; data[7] = 1; data[8] = 0;
	}
};

class DilationFilter : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const;
public:
	DilationFilter(const Kernel& kernel) : MatrixFilter(kernel) {}
	DilationFilter(size_t radius = 1) : MatrixFilter(MorphoKernel(radius)) {}
};

class ErosionFilter : public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const;
public:
	ErosionFilter(const Kernel& kernel) : MatrixFilter(kernel) {}
	ErosionFilter(size_t radius = 1) : MatrixFilter(MorphoKernel(radius)) {}
};

class OpeningFilter
{
public:
	QImage process(const QImage& img);
};

class ClosingFilter
{
public:
	QImage process(const QImage& img);
};

class GradFilter
{
public:
	QImage process(const QImage& img);
};
class TopHatFilter
{
public:
	QImage process(const QImage& img);
};
class BlackHatFilter
{
public:
	QImage process(const QImage& img);
};


QColor DilationFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnB = 0;
	float returnG = 0;

	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0),
				tclamp<float>(y + i, img.height() - 1, 0));

			if (mKernel[idx])
			{
				if (color.red() > returnR)
					returnR = color.red();
				if (color.green() > returnG)
					returnG = color.green();
				if (color.blue() > returnB)
					returnB = color.blue();
			}

		}
	return QColor(returnR, returnG, returnB);
}

QColor ErosionFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 255;
	float returnB = 255;
	float returnG = 255;

	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;
			QColor color = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0),
				tclamp<float>(y + i, img.height() - 1, 0));

			if (mKernel[idx])
			{
				if (color.red() < returnR)
					returnR = color.red();
				if (color.green() < returnG)
					returnG = color.green();
				if (color.blue() < returnB)
					returnB = color.blue();
			}

		}
	return QColor(returnR, returnG, returnB);
}

QImage OpeningFilter::process(const QImage& img)
{
	ErosionFilter erode;
	QImage imgA = erode.process(img);

	DilationFilter dilate;
	QImage imgB = dilate.process(imgA);

	return imgB;
}

QImage ClosingFilter::process(const QImage& img)
{
	DilationFilter dilate;
	QImage imgA = dilate.process(img);

	ErosionFilter erode;
	QImage imgB = erode.process(imgA);

	return imgB;
}

QImage GradFilter::process(const QImage& img)
{
	QImage result(img);

	DilationFilter dilate;
	QImage imgA = dilate.process(img);

	ErosionFilter erode;
	QImage imgB = erode.process(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor colorA = imgA.pixelColor(x, y);
			QColor colorB = imgB.pixelColor(x, y);

			int returnR = tclamp<float>(colorA.red() - colorB.red(), 255, 0);
			int returnG = tclamp<float>(colorA.green() - colorB.green(), 255, 0);
			int returnB = tclamp<float>(colorA.blue() - colorB.blue(), 255, 0);

			result.setPixelColor(x, y, QColor(returnR, returnG, returnB));
		}
	return result;
}

QImage TopHatFilter::process(const QImage& img)
{
	QImage imgA(img);

	OpeningFilter open;
	QImage imgB = open.process(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor colorA = imgA.pixelColor(x, y);
			QColor colorB = imgB.pixelColor(x, y);

			int returnR = tclamp<float>(colorA.red() - colorB.red(), 255, 0);
			int returnG = tclamp<float>(colorA.green() - colorB.green(), 255, 0);
			int returnB = tclamp<float>(colorA.blue() - colorB.blue(), 255, 0);

			imgB.setPixelColor(x, y, QColor(returnR, returnG, returnB));
		}
	return imgB;
}

QImage BlackHatFilter::process(const QImage& img)
{
	QImage imgB(img);

	ClosingFilter close;
	QImage imgA = close.process(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor colorA = imgA.pixelColor(x, y);
			QColor colorB = imgB.pixelColor(x, y);

			int returnR = tclamp<float>(colorA.red() - colorB.red(), 255, 0);
			int returnG = tclamp<float>(colorA.green() - colorB.green(), 255, 0);
			int returnB = tclamp<float>(colorA.blue() - colorB.blue(), 255, 0);

			imgB.setPixelColor(x, y, QColor(returnR, returnG, returnB));
		}
	return imgB;
}

/*
class NewMorphoKernel
{
protected:

	QColor calcNewPixelColor(const QImage& img, int x, int y) const;

public:

	int h;
	int w;
	double** arr;

	NewMorphoKernel() = default;
	NewMorphoKernel(int _h, int _w, double** _arr)
	{
		h = _h;
		w = _w;
		arr = new double* [h];

		for (int i = 0; i < h; i++)
		{
			arr[i] = new double[w];

			for (int j = 0; j < w; j++)
			{
				arr[i][j] = _arr[i][j];
			}
		}
	}

	QImage process(const QImage& img) const
	{
		QImage result(img);

		for (int x = 0; x < img.width(); x++)
			for (int y = 0; y < img.height(); y++)
			{
				QColor color = calcNewPixelColor(img, x, y);
				result.setPixelColor(x, y, color);
			}
		return result;
	}

	int getH() { return h; }
	int getW() { return w; }
};
/////
/// Dilation
/////
class Dilation : public NewMorphoKernel
{
protected:

	QColor calcNewPixelColor(const QImage& img, int x, int y);

public:

	Dilation()
	{
		w = 3;
		h = 3;
		arr = new double* [h];

		arr[0] = new double[w] {0, 1, 0};
		arr[1] = new double[w] {1, 1, 1};
		arr[2] = new double[w] {0, 1, 0};
	}

	Dilation(NewMorphoKernel tmp) : NewMorphoKernel(tmp) {};
};

QColor Dilation::calcNewPixelColor(const QImage& img, int x, int y)
{
	float returnR = 0;
	float returnB = 0;
	float returnG = 0;

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			QColor color = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0),
				tclamp<float>(y + i, img.height() - 1, 0));

			if (arr[i][j])
			{
				if (color.red() > returnR)
					returnR = color.red();
				if (color.green() > returnG)
					returnG = color.green();
				if (color.blue() > returnB)
					returnB = color.blue();
			}

		}
	return QColor(returnR, returnG, returnB);
}
/////
/// Erosion
/////
class Erosion : public NewMorphoKernel
{
	Erosion()
	{
		w = 3;
		h = 3;
		arr = new double* [h];

		arr[0] = new double[w] {0, 1, 0};
		arr[1] = new double[w] {1, 1, 1};
		arr[2] = new double[w] {0, 1, 0};
	}

	Erosion(NewMorphoKernel tmp) : NewMorphoKernel(tmp) {};
	QColor calcNewPixelColor(const QImage& img, int x, int y) const;

};

QColor Erosion::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnB = 0;
	float returnG = 0;

	for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++)
		{
			QColor color = img.pixelColor(tclamp<float>(x + j, img.width() - 1, 0),
				tclamp<float>(y + i, img.height() - 1, 0));

			if (arr[i][j])
			{
				if (color.red() < returnR)
					returnR = color.red();
				if (color.green() < returnG)
					returnG = color.green();
				if (color.blue() < returnB)
					returnB = color.blue();
			}

		}
	return QColor(returnR, returnG, returnB);
}
*/
//histogram Brightness = 0,3 * Red + 0,59 * Green + 0,11 * Blue.

class HistogrammFilter : public Filter
{
	float intensity_max, intensity_min;
public:
	HistogrammFilter()
	{
		intensity_max = 1, intensity_min = 1;
	}
	void intensities_range_calc(const QImage& img);
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
	QImage process(const QImage& img);
};


QImage HistogrammFilter::process(const QImage& img)
{
	intensities_range_calc(img);
	QImage result(img);//создаём переменную-картинку-результат
	//проходим каждый пикслей в цикле 
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			//создаём "переменную-результат"-"color" работы функции обработки цвета текущего пикселя
			//(в новой картинке)
			QColor color = calcNewPixelColor(img, x, y);
			//в результирующей картинке устанавливаем этот пиксель(x,y) в новый цвет color
			result.setPixelColor(x, y, color);
		}
	}
	//возвращаем полученное изображение
	return result;
}

QColor HistogrammFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	//берём значения цвета текущего пикселя
	QColor color = img.pixelColor(x, y);

	//устанавливаем во все каналы полученное значение
	float intensity = 0, intensity_tmp = 0;
	intensity_tmp  = 0.3 * color.red() + 0.59 * color.green() + 0.51 * color.blue();
	intensity = (intensity_tmp - intensity_min) * (255 - 0) / (intensity_max - intensity_min);

	color.setRgb(tclamp<float>(intensity, 255.f, 0.f), tclamp<float>(intensity, 255.f, 0.f), tclamp<float>(intensity, 255.f, 0.f));
	return color;
}


void HistogrammFilter::intensities_range_calc(const QImage& img)
{
	float min_int = 0, max_int = 0;
	float tmp_intens;
	for (int x = 0; x < img.width(); x++)
	{
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = img.pixelColor(tclamp<float>(x, img.width() - 1, 0), tclamp<float>(y, img.height() - 1, 0));
			tmp_intens = 0.3 * color.red() + 0.59 * color.green() + 0.11 * color.blue();
			if (tmp_intens > max_int)
			{
				max_int = tmp_intens;
			}
			if (tmp_intens < min_int)
			{
				min_int = tmp_intens;
			}
		}
	}
	intensity_max = max_int;
	intensity_min = min_int;

}
