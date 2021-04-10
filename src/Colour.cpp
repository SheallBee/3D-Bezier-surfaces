#include "Colour.h"

namespace Colours
{
	Colour::Colour() : Red(0.0f), Green(0.0f), Blue(0.0f), Alpha(0.0f) { }
	Colour::Colour(float r, float g, float b, float a) : Red(r), Green(g), Blue(b), Alpha(a) { }
}