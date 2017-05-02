#ifndef STRUCT_H
#define STRUCT_H

struct gameSettings {
	int fieldW, fieldH;
	int windowW, windowH;
	int period;		// Интервал между ходами (в миллисекундах)
	int population;    // Численность популяции (в процентах)
};

struct cell {
	bool isAlive;
};

#include "const.h"

typedef cell gameField[MAX_FIELD_H+2][MAX_FIELD_W+2];	// +2 строки и столбца вокруг поля

#endif // STRUCT_H
