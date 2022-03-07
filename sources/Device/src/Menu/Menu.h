// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Panel/Controls.h"
#include "Menu/MenuItems.h"


namespace Menu
{
    extern bool showDebugMenu;

    namespace Handlers
    {
        // Функция обработки короткого нажатия кнопки (менее 0.5 сек.).
        void ShortPressureButton(Key::E);

        // Функция обработки длинного нажатия кнопки (более 0.5 сек.).
        void LongPressureButton(Key::E);

        // Функция вызывается, когда кнопка переходит из отжатого в нажатое положение.
        void PressButton(Key::E);

        // Функция вызывается, когда кнопка переходит из нажатого в отжатое положение.
        void ReleaseButton(Key::E);

        // Функция обработки поворота ручки УСТАНОВКА вправо.
        void RotateRegSetRight();

        // Функция обработки поворота ручки УСТАНОВКА влево.
        void RotateRegSetLeft();
    }

    void Draw();

    // Функция должна вызываться в главном цикле.
    void UpdateInput();

    // Отобразить/скрыть меню.
    void Show(bool);

    // Установить время автоматического сокрытия меню в соответствии с установками.
    void SetAutoHide(bool active);

    // Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.
    char *StringNavigation(char buffer[100]);

    void ChangeStateFlashDrive();

    // Отображено ли меню на экране.
    bool IsShown();

    // Если true - меню находится в дополнительном режиме.
    bool IsMinimize();
};
