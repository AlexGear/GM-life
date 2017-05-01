#ifndef LOGIC_H
#define LOGIC_H

#include "struct.h"

void startGame(const gameSettings settings, gameField &field);
void logic(const gameSettings settings, gameField &oldField, gameField &newField);
int neighborsAlive(const gameField field, const int h, const int w);    // ������� ����� �������
void copyField(const gameSettings settings,                      // �������� �������� ��
               gameField &oldField, const gameField &newField);  // newField � oldField

#endif // LOGIC_H
