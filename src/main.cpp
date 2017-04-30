#include <iostream>
#include <stdio.h>
#include "conio.h"
#include "graphics.h"

#include "struct.h"
#include "settings.h"
#include "menu.h"
#include "logic.h"
#include "render.h"

using namespace std;

int main() {
	setlocale(0, "RUS");
	const char settingsFilename[] = "settings";		// дефолтное имя файла настроек
	gameSettings settings;

	switch(initSettings(settingsFilename, settings)) {
	case 1:
		cout << "Не удалось ни загрузить старый файл настроек, ни создать новый.\n";
		return 1;
	case 2:
		cout << "Был создан новый файл настроек, так как старый не удалось загрузить.\n";
		fflush(stdout);
	}

	initwindow(settings.windowW, settings.windowH);
	while(menu(settingsFilename, settings)) {		// menu возвратит false, если игрок решил выйти, либо true, если решил начать игру
		gameField field;
		startGame(settings, field); 	// инициализируем поле
		do {
			logic(settings, field);		// обрабатываем логику игры
			render(settings, field);	// и рисуем поле на экране,
			delay(1000);
		} while (!kbhit() || getch() != 27);	// пока не нажали Escape
	}
	return 0;
}
