CFLAGS = -O2 -g
SDLCONFIG = sdl-config
CURLCONFIG = curl-config
XMLCONFIG = xml2-config
LIBS = -lSDL_image -lSDL_gfx -lSDL_ttf -lSDL_mixer

pspmaps: pspmaps.c $(ICON) global.o kml.o tile.c io.c
	$(CC) $(CFLAGS) -o pspmaps$(EXT) pspmaps.c $(ICON) global.o kml.o $(LIBS) `$(CURLCONFIG) --cflags --libs` `$(SDLCONFIG) --cflags --libs` `$(XMLCONFIG) --libs`

global.o: global.c global.h
	$(CC) $(CFLAGS) -c global.c

kml.o: kml.c kml.h
	$(CC) $(CFLAGS) -c kml.c `$(SDLCONFIG) --cflags` `$(XMLCONFIG) --cflags`

icon.o: icon.rc
	$(WINDRES) -i icon.rc -o icon.o

clean:
	rm -rf pspmaps pspmaps.exe *.o PSP-Maps.prx PSP-Maps.elf PARAM.SFO EBOOT.PBP pspmaps.gpu cache/ data/*.dat kml/
