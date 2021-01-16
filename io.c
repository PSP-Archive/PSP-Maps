/* prints a message using the bitmap font */
void print(SDL_Surface *dst, int x, int y, char *text)
{
	SDL_Rect pos;
	SDL_Surface *src;
	SDL_Color color = {255, 255, 255};
	if (font == NULL) return;
	pos.x = x;
	pos.y = y;
	src = TTF_RenderText_Blended(font, text, color);
	SDL_BlitSurface(src, NULL, dst, &pos);
	SDL_FreeSurface(src);
}

/* input text */
void input(SDL_Surface *dst, int x, int y, char *text, int max)
{
	SDL_Event event;
	int action, active = 0, flip;
	int up = 0, down = 0;
	
	void update()
	{
		SDL_Rect pos;
		int xx, yy;
		char tmp;
		
		TTF_SizeText(font, text, &xx, &yy);
		pos.x = x;
		pos.y = y;
		pos.w = WIDTH - x*2;
		pos.h = yy;
		SDL_FillRect(dst, &pos, BLACK);
		
		print(dst, x, y, text);
		/* display blinking cursor */
		if (flip / 5 % 2)
		{
			tmp = text[active];
			text[active] = '\0';
			TTF_SizeText(font, text, &xx, &yy);
			print(dst, x+xx, y, "_");
			text[active] = tmp;
		}
		flip++;
		SDL_BlitSurface(dst, NULL, screen, NULL);
		SDL_Flip(screen);
	}
	
	#ifdef _PSP_FW_VERSION
	/* danzeff */
	if (config.danzeff)
	{
		strcpy(text, "");
		danzeff_load();
		danzeff_set_screen(dst);
		danzeff_moveTo(165, 110);
		
		for (;;)
		{
			SceCtrlData ctrl = getCtrlFromJoystick(joystick);
			int c = danzeff_readInput(ctrl);
			
			switch (c)
			{
				case 0:
					break;
				case '\10':
				case DANZEFF_LEFT:
					if (active > 0)
					{
						active--;
						text[active] = '\0';
					}
					break;
				case DANZEFF_RIGHT:
					if (active < max)
					{
						text[active] = ' ';
						active++;
						text[active] = '\0';
					}
					break;
				case DANZEFF_SELECT:
				case DANZEFF_START:
					return;
				default:
					if (active < max)
					{
						text[active] = c;
						active++;
						text[active] = '\0';
					}
					break;
			}
			
			danzeff_render();
			update();
			SDL_Delay(50);
			
			/* flush events */
			while (SDL_PollEvent(&event));
		}
	}
	else	
	#endif
	
	/* arcade */
	{
		strcpy(text, " ");
		for (;;)
		{
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_QUIT:
						quit();
						break;
					case SDL_KEYDOWN:
					case SDL_JOYBUTTONDOWN:
						if (event.type == SDL_KEYDOWN)
							action = event.key.keysym.sym;
						else
							action = event.jbutton.button;
						switch (action)
						{
							case SDLK_ESCAPE:
							case SDLK_SPACE:
							case PSP_BUTTON_START:
								return;
							case SDLK_LEFT:
							case PSP_BUTTON_LEFT:
							case PSP_BUTTON_L:
							case PSP_BUTTON_A:
								if (active > 0)
								{
									text[active] = '\0';
									active--;
								}
								break;
							case SDLK_RIGHT:
							case PSP_BUTTON_RIGHT:
							case PSP_BUTTON_R:
							case PSP_BUTTON_B:
							case PSP_BUTTON_Y:
							case PSP_BUTTON_X:
								if (active < max)
								{
									active++;
									text[active] = ' ';
									text[active+1] = '\0';
								}
								break;
							case SDLK_UP:
							case PSP_BUTTON_UP:
								up = 1;
								break;
							case SDLK_DOWN:
							case PSP_BUTTON_DOWN:
								down = 1;
								break;
							default:
								break;
						}
						break;
					case SDL_KEYUP:
					case SDL_JOYBUTTONUP:
						if (event.type == SDL_KEYUP)
							action = event.key.keysym.sym;
						else
							action = event.jbutton.button;
						switch (action)
						{
							case SDLK_UP:
							case PSP_BUTTON_UP:
								up = 0;
								break;
							case SDLK_DOWN:
							case PSP_BUTTON_DOWN:
								down = 0;
								break;
						}
						break;
				}
			}
			if (up == 1 || up > 10)
			{
				if ((text[active] >= 'A' && text[active] < 'Z') || (text[active] >= '0' && text[active] < '9')) text[active]++;
				else if (text[active] == 'Z') text[active] = '0';
				else if (text[active] == '9') text[active] = ' ';
				else if (text[active] == ' ') text[active] = 'A';
			}
			if (up) up++;
			if (down == 1 || down > 10)
			{
				if ((text[active] > 'A' && text[active] <= 'Z') || (text[active] > '0' && text[active] <= '9')) text[active]--;
				else if (text[active] == 'A') text[active] = ' ';
				else if (text[active] == ' ') text[active] = '9';
				else if (text[active] == '0') text[active] = 'Z';
			}
			if (down) down++;
			update();
			SDL_Delay(50);
		}
	}
}
