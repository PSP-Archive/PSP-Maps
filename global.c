#include <math.h>

void latlon2xy(float lat, float lon, float *x, float *y, int z)
{
	float e = sin(lat * M_PI / 180);
	if (e > 0.9999) e = 0.9999;
	if (e < -0.9999) e = -0.9999;
	*x = pow(2, 17-z) * (lon + 180) / 360;
	*y = pow(2, 16-z) * (1 - log((1 + e)/(1 - e)) / 2 / M_PI);
}
