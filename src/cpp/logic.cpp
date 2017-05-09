#include "const.h"
#include "logic.h"
#include "graphics.h"
#include <ctime>
#include <stdlib.h>

void startGame(const gameSettings settings, gameField &field) {
	int w = settings.fieldW;
	int h = settings.fieldH;
	for(int i = 0; i < h+2; i++) {
		field[i][0].isAlive = field[i][w+1].isAlive = false;	// заполняем клетки по бокам поля
		for(int j = 1; j <= w; j++)
            if(rand() % 100 < settings.population)
                firstBorn(field, i, j);
            else field[i][j].isAlive = false;
	}
	for(int j = 0; j < w+2; j++) {
		field[0][j].isAlive = field[h+1][j].isAlive = false;	// заполняем клетки сверху и снизу поля
	}
	cleardevice();
}

int neighborsAlive(const gameField field, const int h, const int w) {    // Считает живых соседей
    int neighbors = 0;
    if (field[h - 1][w - 1].isAlive) neighbors++;    //
    if (field[h - 1][  w  ].isAlive) neighbors++;    //
    if (field[h - 1][w + 1].isAlive) neighbors++;    //
    if (field[  h  ][w - 1].isAlive) neighbors++;    // Проверяем все
    if (field[  h  ][w + 1].isAlive) neighbors++;    // соседние клетки
    if (field[h + 1][w - 1].isAlive) neighbors++;    //
    if (field[h + 1][  w  ].isAlive) neighbors++;    //
    if (field[h + 1][w + 1].isAlive) neighbors++;    //
    return neighbors;
}

// Копирует поле из src в dest
void copyField(const gameSettings settings, gameField &dest, const gameField src) {
    for(int i = 1; i <= settings.fieldH; i++) {
        size_t size = sizeof(cell) * (settings.fieldW + 2); // вычисляем, сколько байт скопировать
        memcpy(dest[i], src[i], size);  // копируем сразу целую строку
    }
}

void firstBorn(gameField &field, const int h, const int w) {
    for (int i = 0; i < 9; i++) {
        field[h][w].socialGene[i] = rand() % 2;
    }

    field[h][w].isAlive = true;
    field[h][w].health = 1;
}

struct aliveCell {
    int h;
    int w;
};

void identifyParent(const gameField field, aliveCell(&aliveCell)[8],
                    const int h, const int w, int neighbors) {
    if (field[h][w].isAlive) {
        aliveCell[neighbors].h = h;
        aliveCell[neighbors].w = w;
        neighbors++;
    }
}

void bornCell(const gameField oldField, gameField &newField, const int h, const int w) {

    aliveCell aliveCell[8];

    int neighbors = 0;
    identifyParent(oldField, aliveCell, h - 1, w - 1, neighbors);
    identifyParent(oldField, aliveCell, h - 1, w    , neighbors);
    identifyParent(oldField, aliveCell, h - 1, w + 1, neighbors);
    identifyParent(oldField, aliveCell, h    , w - 1, neighbors);
    identifyParent(oldField, aliveCell, h    , w + 1, neighbors);
    identifyParent(oldField, aliveCell, h + 1, w - 1, neighbors);
    identifyParent(oldField, aliveCell, h + 1, w    , neighbors);
    identifyParent(oldField, aliveCell, h + 1, w + 1, neighbors);

    if (neighbors) {
        int parent = rand() % neighbors;
        for (int i = 0; i < 8; i++) {
            newField[h][w].socialGene[i] =
                    oldField[aliveCell[parent].h][aliveCell[parent].w].socialGene[i];
        }
    }

    newField[h][w].isAlive = true;
    newField[h][w].health = 1;
}

void harmCell(gameField &field, const int h, const int w) {
    if(--field[h][w].health <= 0)
        field[h][w].isAlive = false;
}

struct comfortCell {
    int h;
    int w;
};

void checkPlace(const gameField oldField, const gameField newField,
                const bool socialGene[], comfortCell(&comfortCell)[9],
                const int h, const int w, int &places) {

    if (oldField[h][w].isAlive == false && newField[h][w].isAlive == false) {
        for (int i = 0; i < 9; i++) {
            if (socialGene[i] && neighborsAlive(oldField, h, w) == i) {
                comfortCell[places].h = h;
                comfortCell[places].w = w;
                places++;
                return;    // Место подходит
            }
        }
    }
    return;   // Место не подходит
}

void moveCell(const gameSettings settings, const gameField oldField,
              gameField &newField, const int h, const int w) {

    comfortCell comfortCell[9];
    bool social[8];
    for (int i = 0; i < 9; i++)
        social[i] = oldField[h][w].socialGene[i];

    int places = 0;
    checkPlace(oldField, newField, social, comfortCell, h, w, places);
    if (h > 1 && w > 1)
        checkPlace(oldField, newField, social, comfortCell, h - 1, w - 1, places);
    if (h > 1)
        checkPlace(oldField, newField, social, comfortCell, h - 1, w    , places);
    if (h > 1 && w < settings.fieldW)
        checkPlace(oldField, newField, social, comfortCell, h - 1, w + 1, places);
    if (w > 1)
        checkPlace(oldField, newField, social, comfortCell, h    , w - 1, places);
    if (h > 1 && w < settings.fieldW)
        checkPlace(oldField, newField, social, comfortCell, h    , w + 1, places);
    if (h < settings.fieldH && w > 1)
        checkPlace(oldField, newField, social, comfortCell, h + 1, w - 1, places);
    if (h < settings.fieldH)
        checkPlace(oldField, newField, social, comfortCell, h + 1, w    , places);
    if (h < settings.fieldH && w < settings.fieldW)
        checkPlace(oldField, newField, social, comfortCell, h + 1, w + 1, places);

    if (places) {
        int pick = rand() % places;
        newField[comfortCell[pick].h][comfortCell[pick].w] = oldField[h][w];
    }
}

void logic(const gameSettings settings, gameField &oldField) {
    gameField newField;
    copyField(settings, newField, oldField);
    for(int i = 1; i <= settings.fieldH; i++) {
        for(int j = 1; j <= settings.fieldW; j++) {
            // Перед тем, как будет рассчитано новое поколение,
            // клетки мигрируют в комфортные условия
            if (settings.socialGene && oldField[i][j].isAlive)
                moveCell(settings, oldField, newField, i, j);
        }
	}
	copyField(settings, oldField, newField);
    for(int i = 1; i <= settings.fieldH; i++) {
        for(int j = 1; j <= settings.fieldW; j++) {
            int neighbors = neighborsAlive(oldField, i, j);
            if (oldField[i][j].isAlive == false) { // Рождается, если 3 живых соседа, иначе остаётся мертвой:
                if(neighbors == 3) bornCell(oldField, newField, i, j);
            } else {    // Теряет здоровье, когда меньше 2 или больше 3 соседей:
                if(neighbors < 2 || neighbors > 3) harmCell(newField, i, j);
            }
        }
	}
	copyField(settings, oldField, newField);
}
