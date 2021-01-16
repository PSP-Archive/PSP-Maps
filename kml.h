#include <SDL.h>

enum
{
	PLACEMARK_NONE,
	PLACEMARK_POINT,
	PLACEMARK_LINE
};

typedef struct _PlacemarkPoint
{
	float lat;
	float lon;
} PlacemarkPoint;

typedef struct _PlacemarkLine
{
	PlacemarkPoint point;
	struct _PlacemarkLine *next;
} PlacemarkLine;

typedef struct _Placemark
{
	char *name;
	char *description;
	SDL_Surface *marker;
	int type;
	union
	{
		PlacemarkPoint *point;
		char *line;
	};
	struct _Placemark *next;
} Placemark;

void kml_load();
void kml_free();
void kml_display(SDL_Surface *dst, float x, float y, int z);
