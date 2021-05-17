#include "Filter.h"
#include <iostream>

using namespace std;
struct pixel
{
	int x;
	int y;
	int rgb;
};

template <class T>
T cclamp(T value, T max, T min)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

void Dilation(const QImage& source, QImage& result)
{
	int MW, MH;
	bool* mask;
	cout << "Enter width: " << endl;
	cin >> MW;
	MH = MW;
	system("cls");
	mask = new bool[MW * MH];
	for (int i = 0; i < MW * MH; i++)
	{
		mask[i] = 0;
	}

	char elemOfMask;
	for (int i = 0; i < MW * MH; i++)
	{
		cout << "Enter 1 or 0" << endl;
		for (int j = 0; j < MW * MH; j++)
		{
			if (j == i)
			{
				cout << "[" << mask[j] << "]" << " ";
			}
			else
			{
				cout << mask[j] << " ";
			}
			if (j % MW == MW - 1 && j > 0)
			{
				cout << endl;
			}
		}
		cin >> elemOfMask;
		if (elemOfMask == '0' || elemOfMask == '1')
		{
			mask[i] = elemOfMask - '0';
		}
		system("cls");

	}

	result = source;

	if (MH * MW % 2 == 1)
	{
		for (int y = MH / 2; y < source.height() - MH / 2; y++)
		{
			for (int x = MW / 2; x < source.width() - MW / 2; x++)
			{
				QColor max;
				max.setRgb(0, 0, 0);
				for (int j = -MH / 2, jj = 0; j <= MH / 2; j++, jj++)
				{
					for (int i = -MW / 2, ii = 0; i <= MW / 2; i++, ii++)
					{
						if ((mask[ii + MH * jj]) && (source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red() > max.red() || source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green() > max.green() || source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue() > max.blue()))
						{
							max.setRgb(source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue());
						}
					}
				}

				if (max.red() > 0 || max.green() > 0 || max.blue() > 0)
				{
					result.setPixelColor(x, y, max);
				}
			}
		}
	}
	else
	{
		for (int y = MH / 2; y < source.height() - MH / 2; y++)
		{
			for (int x = MW / 2; x < source.width() - MW / 2; x++)
			{
				QColor max;
				max.setRgb(0, 0, 0);
				for (int j = -MH / 2, jj = 0; j < MH / 2; j++, jj++)
				{
					for (int i = -MW / 2, ii = 0; i < MW / 2; i++, ii++)
					{
						if ((mask[ii + MH * jj]) && (source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red() + source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green() + source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue() > max.red() + max.green() + max.blue()))
						{
							max.setRgb(source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue());
						}
					}
				}

				if (max.red() > 0 || max.green() > 0 || max.blue() > 0)
				{
					result.setPixelColor(x, y, max);
				}
			}
		}
	}
}

void Erosion(const QImage& source, QImage& result)
{
	int MW, MH;
	bool* mask;
	cout << "Enter width: " << endl;
	cin >> MW;
	MH = MW;
	system("cls");
	mask = new bool[MW * MH];
	for (int i = 0; i < MW * MH; i++)
	{
		mask[i] = 0;
	}

	char elemOfMask;
	for (int i = 0; i < MW * MH; i++)
	{
		cout << "Enter 1 or 0" << endl;
		for (int j = 0; j < MW * MH; j++)
		{
			if (j == i)
			{
				cout << "[" << mask[j] << "]" << " ";
			}
			else
			{
				cout << mask[j] << " ";
			}
			if (j % MW == MW - 1 && j > 0)
			{
				cout << endl;
			}
		}
		cin >> elemOfMask;
		if (elemOfMask == '0' || elemOfMask == '1')
		{
			mask[i] = elemOfMask - '0';
		}
		system("cls");

	}

	result = source;

	if (MH * MW % 2 == 1) {
		for (int y = MH / 2; y < source.height() - MH / 2; y++)
		{
			for (int x = MW / 2; x < source.width() - MW / 2; x++)
			{
				QColor min;
				min.setRgb(255, 255, 255);
				for (int j = -MH / 2, jj = 0; j <= MH / 2; j++, jj++)
				{
					for (int i = -MW / 2, ii = 0; i <= MW / 2; i++, ii++)
					{
						if ((mask[ii + MH * jj]) && (source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red() + source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green() + source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue() < min.red() + min.green() + min.blue()))
						{
							min.setRgb(source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue());
						}
					}
				}

				if (min.red() < 255 || min.green() < 255 || min.blue() < 255)
				{
					result.setPixelColor(x, y, min);
				}
			}
		}
	}
	else
	{
		for (int y = MH / 2; y < source.height() - MH / 2; y++)
		{
			for (int x = MW / 2; x < source.width() - MW / 2; x++)
			{
				QColor min;
				min.setRgb(255, 255, 255);
				for (int j = -MH / 2, jj = 0; j < MH / 2; j++, jj++)
				{
					for (int i = -MW / 2, ii = 0; i < MW / 2; i++, ii++)
					{
						if ((mask[ii + MH * jj]) && (source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red() < min.red() || source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green() < min.green() || source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue() < min.blue()))
						{
							min.setRgb(source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).red(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).green(), source.pixelColor(cclamp(x + i, source.width() - 1, 0), cclamp(y - j, source.height() - 1, 0)).blue());
						}
					}
				}

				if (min.red() < 255 || min.green() < 255 || min.blue() < 255)
				{
					result.setPixelColor(x, y, min);
				}
			}
		}
	}
}

void Opening(const QImage& source, QImage& result)
{
	QImage helper;
	Erosion(source, helper);

	Dilation(helper, result);
}

void Closing(const QImage& source, QImage& result)
{
	QImage helper;
	Dilation(source, helper);

	Erosion(helper, result);
}

void Grad(const QImage& source, QImage& result)
{
	result = source;

	QImage helper1;
	QImage helper2;
	QColor color;

	Dilation(source, helper1);
	Erosion(source, helper2);

	for (int y = 0; y < helper1.height(); y++)
	{
		for (int x = 0; x < helper1.width(); x++)
		{
			int red, green, blue;
			red = cclamp(helper1.pixelColor(x, y).red() - helper2.pixelColor(x, y).red(), 255, 0);
			green = cclamp(helper1.pixelColor(x, y).green() - helper2.pixelColor(x, y).green(), 255, 0);
			blue = cclamp(helper1.pixelColor(x, y).blue() - helper2.pixelColor(x, y).blue(), 255, 0);

			color.setRgb(red, green, blue);

			result.setPixelColor(x, y, color);
		}
	}
}

void TopHat(const QImage& source, QImage& result)
{
	result = source;

	QImage helper;
	QColor color;
	Closing(source, helper);

	for (int y = 0; y < helper.height(); y++)
	{
		for (int x = 0; x < helper.width(); x++)
		{
			int red, green, blue;
			red = cclamp(source.pixelColor(x, y).red() - helper.pixelColor(x, y).red(), 255, 0);
			green = cclamp(source.pixelColor(x, y).green() - helper.pixelColor(x, y).green(), 255, 0);
			blue = cclamp(source.pixelColor(x, y).blue() - helper.pixelColor(x, y).blue(), 255, 0);

			color.setRgb(red, green, blue);

			result.setPixelColor(x, y, color);
		}
	}
}

void BlackHat(const QImage& source, QImage& result)
{
	result = source;

	QImage helper;
	QColor color;
	Closing(source, helper);

	for (int y = 0; y < helper.height(); y++)
	{
		for (int x = 0; x < helper.width(); x++)
		{
			int red, green, blue;
			red = cclamp(helper.pixelColor(x, y).red() - source.pixelColor(x, y).red(), 255, 0);
			green = cclamp(helper.pixelColor(x, y).green() - source.pixelColor(x, y).green(), 255, 0);
			blue = cclamp(helper.pixelColor(x, y).blue() - source.pixelColor(x, y).blue(), 255, 0);

			color.setRgb(red, green, blue);

			result.setPixelColor(x, y, color);
		}
	}
}

void main(int argc, char* argv[])
{
	std::string s;
	QImage img;

	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "-p") && (i + 1 < argc))
		{
			s = argv[i + 1];
		}
	}

	img.load(QString(s.c_str()));
	img.save("img/giraffe.png");

	/*GlassFilter glass;
	glass.process(img).save("img/glass.png");
	///////////////////////////////////
	WavesFilter waves;
	waves.process(img).save("img/waves.png");
	///////////////////////////////////
	SharpnessFilter sharpness;
	sharpness.process(img).save("img/sharpness.png");
	///////////////////////////////////
	MedianFilter median(2);
	median.process(img).save("img/median.png");
	////////////////////////////////////
	InvertFilter invert;
	invert.process(img).save("img/invert.png");
    /////////////////////////////////////
	BlurFilter blur;
	blur.process(img).save("img/blur.png");
	/////////////////////////////////////
	GaussianFilter gaus;
	gaus.process(img).save("img/gaus.png");
	///////////////////////////////////
	GrayScaleFilter gray;
	gray.process(img).save("img/grayscale.png");
	///////////////////////////////////
	SepiaFilter sepia;
	sepia.process(img).save("img/sepia.png");
	///////////////////////////////////
	BrightFilter bright;
	bright.process(img).save("img/bright.jpg");
	///////////////////////////////////
	SobelFilter sobel;
	sobel.process(img).save("img/sobel.png");
	///////////////////////////////////
	СorrectionFilter correction;
	correction.process(img).save("img/correction.png");
	///////////////////////////////////
	EmbossmentFilter emboss;
	emboss.process(img).save("img/emboss.png");
	///////////////////////////////////
	MotionBlurFilter motionblur;
	motionblur.process(img).save("img/motionblur.png");
	///////////////////////////////////
	GreyWorldFilter grey_world;
	grey_world.process(img).save("img/grey_world.png");
	///////////////////////////////////
	HistugrammFilter hust;
	hust.process(img).save("img/histogram.png");*/

	/*
	QImage result;
	Dilation(img, result);
	result.save("img/dilation.png");

	Erosion(img, result);
	result.save("img/erosion.png");

	Opening(img, result);
	result.save("img/open.png");

	Closing(img, result);
	result.save("img/close.png");

	Grad(img, result);
	result.save("img/grad.png");

	TopHat(img, result);
	result.save("img/tophat.png");

	BlackHat(img, result);
	result.save("img/blackhat.png");

	*/
	argv[2] = "img/grey_world1.jpg";
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "-p") && (i + 1 < argc))
		{
			s = argv[i + 1];
		}
	}
	img.load(QString(s.c_str()));
	//perfect reflection
}

/* 
	argv[2] = "img/black.jpg";
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "-p") && (i + 1 < argc))
		{
			s = argv[i + 1];
		}
	}
	img.load(QString(s.c_str()));
	img.save("img/black.png");
	MedianFilter median;
	median.process(img).save("img/median1.png");



	if (argc == 1) { // если в аргументах только имя программы
		std::cout << "no arguments!" << "\n"; // выводим, что нет аргументов
	}
	else {
		// иначе выводим все аргументы, которые переданы
		for (int i = 1; i < argc; i++) {
			std::cout << "argv[" << i << "] - " << argv[i] << "\n";
		}
	}

 	SepiaFilter sepia;
	sepia.process(img).save("img/sepia.png");

	InvertFilter invert1;
	invert1.process(img).save("img/invert.png");

	argv[2] = "C:/Users/eemee/source/repos/qt_lab_1/qt_lab_1/img/sepia.png";
	for (int i = 0; i < argc; i++)
	{
		if (!strcmp(argv[i], "-p") && (i + 1 < argc))
		{
			s = argv[i + 1];
		}
	}
	img.load(QString(s.c_str()));
	img.save("img/sepia.png");

	InvertFilter invert2;
	invert2.process(img).save("img/invert_sepia.png");
*/