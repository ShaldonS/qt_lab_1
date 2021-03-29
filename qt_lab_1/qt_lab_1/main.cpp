#include "Filter.h"
#include "MatrixFilter.h"
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
	img.save("giraffe.jpg");
	////////////////////////////////////
	InvertFilter invert;
	invert.process(img).save("invert.png");
    /////////////////////////////////////
	BlurFilter blur;
	blur.process(img).save("blur.png");
	/////////////////////////////////////
	GaussianFilter gaus;
	gaus.process(img).save("gaus.png");
	///////////////////////////////////
	GrayScaleFilter gray;
	gray.process(img).save("gray.png");
	///////////////////////////////////
	SepiaFilter sepia;
	sepia.process(img).save("sepia.png");
	///////////////////////////////////
	BrightFilter bright;
	bright.process(img).save("bright.jpg");
}