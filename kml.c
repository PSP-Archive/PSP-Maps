#include "global.h"
#include "kml.h"

#include <math.h>
#include <string.h>
#include <dirent.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

SDL_Surface *marker;
Placemark *places = NULL;

void placemark_parse(xmlNode *node, Placemark *place)
{
	xmlNode *cur, *cur2;
	
	for (cur = node->children; cur; cur = cur->next)
	{
		if (strcmp((char *) cur->name, "name") == 0)
			place->name = strdup((char *) cur->children->content);
		if (strcmp((char *) cur->name, "description") == 0)
			place->description = strdup((char *) cur->children->content);
		if (strcmp((char *) cur->name, "Point") == 0)
		{
			/* parse point */
			place->type = PLACEMARK_POINT;
			place->point = malloc(sizeof(PlacemarkPoint));
			place->marker = marker;
			/* scan for coordinates */
			for (cur2 = cur->children; cur2; cur2 = cur2->next)
			{
				if (strcmp((char *) cur2->name, "coordinates") == 0)
					sscanf((char *) cur2->children->content, "%f,%f,", &place->point->lon, &place->point->lat);
			}
		}
		if (strcmp((char *) cur->name, "GeometryCollection") == 0)
		{
			/* parse line */
			place->type = PLACEMARK_LINE;
			place->line = strdup("");
			/* scan for lines */
			for (cur2 = cur->children; cur2; cur2 = cur2->next)
				if (strcmp((char *) cur2->name, "LineString") == 0)
				{
					place->line = realloc(place->line, strlen(place->line) + strlen((char *) cur2->children->children->content) + 1);
					strcat(place->line, (char *) cur2->children->children->content);
				}
		}
	}
}

void kml_parse(char *file)
{
	xmlDoc *doc = NULL;
	xmlNode *node, *cur;

	doc = xmlReadFile(file, NULL, 0);
	if (doc == NULL)
	{
		DEBUG("KML error: no document!\n");
		return;
	}

	node = xmlDocGetRootElement(doc);
	if (node == NULL)
	{
		DEBUG("KML error: no root element!\n");
		return;
	}
	
	/* check the XML document starts with the KML node */
	if (strcmp((char *) node->name, "kml"))
	{
		DEBUG("KML error: no kml root element!\n");
		return;
	}
	node = node->children;
	
	/* skip Document and Folder nodes */
	while (node && (strcmp((char *) node->name, "Document") == 0 || strcmp((char *) node->name, "Folder") == 0))
		node = node->children;
	
	/* scan Placemarks */
	for (cur = node; cur; cur = cur->next)
		if (strcmp((char *) cur->name, "Placemark") == 0)
		{
			Placemark *tmp = malloc(sizeof(Placemark));
			placemark_parse(cur, tmp);
			tmp->next = places;
			places = tmp;
		}
	
	xmlFreeDoc(doc);
}

void kml_load()
{
	DIR *directory;
	struct dirent *entry;
	char file[100];
	SDL_Surface *default_marker;
	
	LIBXML_TEST_VERSION
	xmlKeepBlanksDefault(0);
	
	/* default marker */
	default_marker = IMG_Load("data/marker.png");
	
	/* parse the kml directory */
	if ((directory = opendir("kml/")) != NULL)
		while ((entry = readdir(directory)) != NULL)
			/* load only the .kml files */
			if (entry->d_name[0] != '.' && strcasecmp(strchr(entry->d_name, '.'), ".kml") == 0)
			{
				/* remove .kml suffix */
				strchr(entry->d_name, '.')[0] = '\0';
				/* try to load .png image */
				sprintf(file, "kml/%s.png", entry->d_name);
				DEBUG("kml_parse(\"%s\")\n", file);
				marker = IMG_Load(file);
				/* if not available, default marker */
				if (marker == NULL) marker = default_marker;
				/* load KML file */
				sprintf(file, "kml/%s.kml", entry->d_name);
				kml_parse(file);
			}
	
	xmlCleanupParser();
}

void kml_free()
{
	Placemark *tmp = places;
	while (places)
	{
		tmp = places->next;
		free(places);
		places = tmp;
	}
}

void kml_display(SDL_Surface *dst, float x, float y, int z)
{
	Placemark *place = places;
	float nx, ny;
	while (place)
	{
		switch (place->type)
		{
			case PLACEMARK_POINT:
			{
				latlon2xy(place->point->lat, place->point->lon, &nx, &ny, z);
				SDL_Rect pos;
				pos.x = (nx-x)*256+WIDTH/2 - place->marker->w/2;
				pos.y = (ny-y)*256+HEIGHT/2 - place->marker->h/2;
				SDL_BlitSurface(place->marker, NULL, dst, &pos);
				break;
			}
			case PLACEMARK_LINE:
			{
				int init = 0;
				float ox = 0, oy = 0;
				char *copy = strdup(place->line);
				char *tmp;
				tmp = strtok(copy, " ");
				while (tmp)
				{
					float lat, lon;
					sscanf(tmp, "%f,%f,", &lon, &lat);
					latlon2xy(lat, lon, &nx, &ny, z);
					if (init)
						lineColor(dst, (ox-x)*256+WIDTH/2, (oy-y)*256+HEIGHT/2, (nx-x)*256+WIDTH/2, (ny-y)*256+HEIGHT/2, 0x0000ffaa);
					ox = nx;
					oy = ny;
					init = 1;
					tmp = strtok(NULL, " ");
				}
				free(copy);
				break;
			}
		}
		place = place->next;
	}
}
