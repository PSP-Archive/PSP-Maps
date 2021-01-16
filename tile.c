/* returns in buffer "b" the name of the Google Maps tile for location (x,y,z) */
char *GGtile(int x, int y, int z)
{
	static char b[99];
	int c = 18 - z;
	b[c] = '\0';
	while (z++ < 17)
	{
		c--;
		if (x % 2)
		{
			if (y % 2)
				b[c] = 's';
			else
				b[c] = 'r';
		}
		else
		{
			if (y % 2)
				b[c] = 't';
			else
				b[c] = 'q';
		}
		x/=2;
		y/=2;
	}
	b[0] = 't';
	return b;
}

/* returns in buffer "b" the name of the Virtual Earth tile for location (x,y,z) */
char *VEtile(int x, int y, int z)
{
	static char b[99];
	int c = 17 - z;
	b[c] = '\0';
	while (z++ < 17)
	{
		c--;
		if (x % 2)
		{
			if (y % 2)
				b[c] = '3';
			else
				b[c] = '1';
		}
		else
		{
			if (y % 2)
				b[c] = '2';
			else
				b[c] = '0';
		}
		x/=2;
		y/=2;
	}
	return b;
}

/* save tile in memory cache */
void savememory(int x, int y, int z, int s, SDL_Surface *tile)
{
	DEBUG("savememory(%d, %d, %d, %d)\n", x, y, z, s);
	SDL_FreeSurface(memory[memory_idx].tile);
	memory[memory_idx].x = x;
	memory[memory_idx].y = y;
	memory[memory_idx].z = z;
	memory[memory_idx].s = s;
	memory[memory_idx].tile = tile;
	memory_idx = (memory_idx + 1) % MEMORY_CACHE_SIZE;
}

/* return the disk file name for cache entry
 * maximum of 1000 entries per folder to improve access speed */
void diskname(char *buf, int n)
{
	/* create folders if needed */
	sprintf(buf, "cache/%.3d", n/1000);
	mkdir(buf, 0755);
	/* return the full file name */
	sprintf(buf, "cache/%.3d/%.3d.dat", n/1000, n%1000);
}

/* save tile in disk cache */
void savedisk(int x, int y, int z, int s, SDL_RWops *rw, int n)
{
	FILE *f;
	char name[50];
	char buffer[BUFFER_SIZE];
	
	if (!config.cache_size) return;
	
	DEBUG("savedisk(%d, %d, %d, %d)\n", x, y, z, s);
	
	if (rw == NULL)
	{
		printf("warning: savedisk(NULL)!\n");
		return;
	}
	
	disk[disk_idx].x = x;
	disk[disk_idx].y = y;
	disk[disk_idx].z = z;
	disk[disk_idx].s = s;
	
	SDL_RWseek(rw, 0, SEEK_SET);
	diskname(name, disk_idx);
	if ((f = fopen(name, "wb")) != NULL)
	{
		SDL_RWread(rw, buffer, 1, n);
		fwrite(buffer, 1, n, f);
		fclose(f);
	}
	
	disk_idx = (disk_idx + 1) % config.cache_size;
}

/* curl callback to save in memory */
size_t curl_write(void *ptr, size_t size, size_t nb, void *stream)
{
	SDL_RWops *rw = stream;
	int t = nb * size;
	rw->write(rw, ptr, size, nb);
	return t;
}

/* get the image on internet and return a buffer */
SDL_RWops *getnet(int x, int y, int z, int s)
{
	char request[1024];
	SDL_RWops *rw;
	
	DEBUG("getnet(%d, %d, %d, %d)\n", x, y, z, s);
	
	switch (s)
	{
		case GG_MAP:
			sprintf(request, _url[s], ++balancing%4, x, y, z);
			break;
		case GG_SATELLITE:
			sprintf(request, _url[s], ++balancing%4, GGtile(x, y, z));
			break;
		case GG_HYBRID:
		case GG_TERRAIN:
			sprintf(request, _url[s], ++balancing%4, x, y, z);
			break;
		case VE_ROAD:
		case VE_AERIAL:
		case VE_HYBRID:
		case VE_HILL:
			sprintf(request, _url[s], VEtile(x, y, z));
			break;
		case YH_MAP:
		case YH_SATELLITE:
		case YH_HYBRID:
			sprintf(request, _url[s], x, (int) pow(2, 16-z)-y-1, z+1);
			break;
		case OS_MAPNIK:
		case OS_OSMARENDER:
			sprintf(request, _url[s], 17-z, x, y);
			break;
		case GG_MOON_APOLLO:
		case GG_MOON_ELEVATION:
			sprintf(request, _url[s], 17-z, x, (int) pow(2, 17-z)-y-1);
			break;
		case GG_MARS_ELEVATION:
		case GG_MARS_VISIBLE:
		case GG_MARS_INFRARED:
			sprintf(request, _url[s], GGtile(x, y, z));
			break;
		case GG_SKY_VISIBLE:
			sprintf(request, _url[s], x, y, 17-z);
			break;
		case GG_SKY_INFRARED:
		case GG_SKY_MICROWAVE:
		case GG_SKY_HISTORICAL:
			sprintf(request, _url[s], 17-z, x, y);
			break;
	}
	
	rw = SDL_RWFromMem(response, BUFFER_SIZE);
	
	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "PSP-Maps " VERSION);
	curl_easy_setopt(curl, CURLOPT_URL, request);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, rw);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	
	if (curl_easy_perform(curl) != 0)
		/* if there was a network error, invalidate previous buffer */
		response[0] = '\0';
	
	return rw;
}

/* return the tile from disk if available, or NULL */
SDL_Surface *getdisk(int x, int y, int z, int s)
{
	int i;
	char name[50];
	DEBUG("getdisk(%d, %d, %d, %d)\n", x, y, z, s);
	for (i = 0; i < config.cache_size; i++)
		if (disk[i].x == x && disk[i].y == y && disk[i].z == z && disk[i].s == s)
		{
			diskname(name, i);
			return IMG_Load(name);
		}
	return NULL;
}

/* return the tile from memory if available, or NULL */
SDL_Surface *getmemory(int x, int y, int z, int s)
{
	int i;
	DEBUG("getmemory(%d, %d, %d, %d)\n", x, y, z, s);
	for (i = 0; i < MEMORY_CACHE_SIZE; i++)
		if (memory[i].tile && memory[i].x == x && memory[i].y == y && memory[i].z == z && memory[i].s == s)
			return memory[i].tile;
	return NULL;
}

/* downloads the image from Google for location (x,y,z) with mode (s) */
SDL_Surface* gettile(int x, int y, int z, int s)
{
	SDL_RWops *rw;
	SDL_Surface *tile;
	int n;
	
	/* try memory cache */
	if ((tile = getmemory(x, y, z, s)) != NULL)
		return tile;
	
	/* try disk cache */
	if ((tile = getdisk(x, y, z, s)) != NULL)
	{
		savememory(x, y, z, s, tile);
		return tile;
	}
	
	/* try internet */
	rw = getnet(x, y, z, s);
	
	/* load the image */
	n = SDL_RWtell(rw);
	SDL_RWseek(rw, 0, SEEK_SET);
	tile = IMG_Load_RW(rw, 0);
	SDL_RWseek(rw, 0, SEEK_SET);
	
	/* if there is no tile, copy the n/a image
	 * I use a dummy call to zoomSurface to copy the surface
	 * because I had issues with SDL_DisplayFormat() on PSP */
	if (tile == NULL)
		tile = zoomSurface(na, 1, 1, 0);
	/* only save on disk if not n/a
	 * to avoid filling the cache with wrong images
	 * when we are offline */
	else
		savedisk(x, y, z, s, rw, n);
	savememory(x, y, z, s, tile);
	
	SDL_RWclose(rw);
	
	return tile;
}
