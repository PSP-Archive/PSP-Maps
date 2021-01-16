PSP-Maps
http://royale.zerezo.com/psp/

Install:
	* Copy PSP-Maps folder to your PSP/GAMExxx folder.
	* For custom firmwares 3.90, please also try PSP/GAME380 folder.

Controls:
	* Digital pad: move the map.
	* Analog pad: move smoothly the map.
	* R button: zoom in.
	* L button: zoom out.
	* >< button: next view.
	* [] button: previous view.
	* /\ button: go to address.
	* () button: show/hide informations.
	* Start: menu.

Offline browsing:
	* First, make sure the disk cache is enabled (default).
	* Then, when you are online, browse the map to fill your cache.
	* When you start PSP-Maps, you can skip the wireless menu.
	* Now, you can browse your cache!

Caching:
	* You can adjust the size of your cache in the menu (you must validate to confirm).
	* The bigger is the better, but it will use some space on your memory stick.
	* The "cache zoom levels" option is helpful to download a big map to your cache.

PC version:
	* If you don't have WiFi, you can use the PC version to build a compatible cache.
	* It can also be used to prepare a large cache (PC is faster than PSP).
	* You will have exactly the same display as the PSP.
	* Arrows to move, page up/down to zoom, F1/F2/F3/F4 for buttons, escape for menu.
	* Copy the cache/ and data/ folders to your PSP.
	* An easier way is to connect the PSP to the PC, browse to the PSP-Maps folder, and launch pspmaps.exe directly from there.

GP2X version:
	* You will have to build a map cache on the PC before.
	* This is just experimental, it may not work in the future...

KML support:
	* Create a kml/ directory in your PSP-Maps folder.
	* Copy the .kml files in this directory.
	* Currently, it only supports "Points" and "Lines".
	* If you create an .png image with the same name as your KML file, it will be used to replace the default marker.
	* You can easily create directions with this link (replace the towns in the URL):
		http://maps.google.com/maps?output=kml&saddr=toulouse&daddr=paris

URL updating:
	* The file "urls.txt" contains the list of addresses used by PSP-Maps to retrieve the images.
	* If Google Maps does not work anymore, try updating this file with the latest version.
	* You can now use Google China, replace the first line by:
		http://mt%d.google.cn/mt?v=cn1.4&x=%d&y=%d&zoom=%d

Motion Kit:
	* Since version 1.3 you can use the PSP Motion Kit with PSP-Maps.
	* Tilting the PSP will allow you to move on the map.
	* Moving the PSP quickly up or down will allow you to zoom.
	* If you experience errors starting PSP-Maps, you can try to remove the motion_driver.prx file.

GPS support:
	* Since version 2.0 you can use the PSP-290 GPS with PSP-Maps.
	* As soon as the GPS gets a signal, it will try to center the map on the current position.
	* You can disable this behavior in the menu if you want to manually move on the map again.
