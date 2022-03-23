// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "defines.h"
#include "Display/Text.h"


bool    IntInRange                              // Возвращает true, если value входит в диапазон [min; max].
                   (int value,                  // проверяемое значение.
                   int min,                     // нижняя граница диапазона.
                   int max                      // верхняя граница диапазона.
                   );
float   MaxFloat(float val1, float val2, float val3);                   // Возвращает максимальное значение из трёх.
int8    CircleIncreaseInt8(int8 *val, int8 min, int8 max);              // Увеличивает значение по адресу val на 1. Затем, если результат превышает max, приравинвает его к min.
int16   CircleIncreaseInt16(int16 *val, int16 min, int16 max);
int8    CircleDecreaseInt8(int8 *val, int8 min, int8 max);              // Умеьшает значение по адресу val на 1. Затем, если результат меньше min, приравнивает его max.
int16   CircleDecreaseInt16(int16 *val, int16 min, int16 max);
int     CircleIncreaseInt(int *val, int min, int max);
int     CircleDecreaseInt(int *val, int min, int max);
float   CircleAddFloat(float *val, float delta, float min, float max);  // Увелечивает значение по адресу val на delta. Затем, если результат больше max, приравнивает его min.
float   CircleSubFloat(float *val, float delta, float min, float max);  // Уменьшает значение по адресу val на delta. Затем, елси результат меньше min, приравнивает его masx.
void    SwapInt(int *value0, int *value1);                      // Обменивает местами содержимое памяти по адресам value0 и value1.
void    SortInt(int *value0, int *value1);                      // Размещает переменные value0 и value1 в порядке возрастания.
char    GetSymbolForGovernor(int value);                        // Возвращает изображение регулятора, соответствующее его текущему положению.
void    EmptyFuncVV();                                      // Функция-заглушка. Ничего не делает.
void    EmptyFuncVI(int);
void    EmptyFuncVpV(void *);                                   // Функция-заглушка. Ничего не делает.
void    EmptyFuncpVII(void *, int, int);                        // Функция-заглушка. Ничего не делает.
void    EmptyFuncVI16(int16);                                   // Функция-заглушка. Ничего не делает.
void    EmptyFuncVB(bool);                                      // Функция-заглушка. Ничего не делает.
bool    EmptyFuncBV();
void    IntToStrCat(char *_buffer, int _value);
