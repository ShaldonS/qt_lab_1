#include "Filter.h"

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

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}

QColor GrayScaleFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue(), 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue(), 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue());
	return color;
}
QColor SepiaFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	float intensity = 0.299f * color.red() + 0.587f * color.green() + 0.114f * color.blue();
	float k = 10;
	int r = (int)clamp(2 * k + intensity, 255.f, 0.f);
	int g = (int)clamp(0.5f * k + intensity, 255.f, 0.f);
	int b = (int)clamp(intensity - 1 * k, 255.f, 0.f);
	color.setRgb(r, g, b);
	return color;
}
QColor BrightFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	int k = 20;
	QColor color = img.pixelColor(x, y);
	int r = (int)clamp(color.red() + k, 255, 0);
	int g = (int)clamp(color.green() + k, 255, 0);
	int b = (int)clamp(color.blue() + k, 255, 0);
	color.setRgb(r, g, b);
	return color;
}