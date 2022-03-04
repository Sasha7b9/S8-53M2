// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Panel/Controls.h"
#include "Menu/MenuItems.h"


namespace Menu
{
    // Возвращает адрес элемента меню, соответствующего данной button.
    void* ItemUnderButton(Key::E);

    // Возвращает true, если элемент меню item затенён (находится не на самом верхнем слое. Как правило, это означает,
    // что раскрыт раскрывающийся элемент меню вроде Choice или Governor.
    bool IsShade(void* item);

    bool IsPressed(void* item);

    void Draw();

    // Функция должна вызываться в главном цикле.
    void UpdateInput();

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

    // Установить время автоматического сокрытия меню в соответствии с установками.
    void SetAutoHide(bool active);
    // Возвращает путь к текущему пункту меню в текстовом виде, готовом к выводу на экран.
    char *StringNavigation(char buffer[100]);

    void OpenItemTime();
    // Вовзращает true, если элемент меню item является ативным, т.е. может быть нажат.
    bool ItemIsActive(void *item);
    // Возвращает тип элемента меню по адресу address.
    TypeItem::E TypeMenuItem(const void *);
    // Возвращает тип открытого элемента меню.
    TypeItem::E TypeOpenedItem();
    // Возвращает адрес текущего элемента меню (текущим, как правило, является элемент, кнопка которого была нажата последней.
    void *CurrentItem();
    // Сделать/разделать текущим пункт страницы.
    void SetCurrentItem(const void *item, bool active);
    // Возвращает адрес элемента, которому принадлежит элемент по адресу item.
    Page* Keeper(const void *item);

    NamePage::E GetNameOpenedPage();

    bool ChangeOpenedItem(void *item, int delta);
    // Уменьшает или увеличивает значение Governor, GovernorColor или Choice по адресу item в зависимости от знака delta
    void ChangeItem(void *item, int delta);
    // Возвращает высоту в пикселях открытого элемента Choice или NamePage по адресу item.
    int HeightOpenedItem(void *item);
    // Возвращает название элемента по адресу item, как оно выглядит на дисплее прибора.
    pchar  TitleItem(void *item);
    // Возвращает true, если button - функциональная клавиша [1..5].
    bool IsFunctionalButton(Key::E);
    // Закрыть открытый элемент меню.
    void CloseOpenedItem();

    void OpenItem(const void *item, bool open);             // Открыть/закрыть элемент меню по адрему item.

    bool ItemIsOpened(const void *item);                    // Возвращает true, если элемент меню по адрему item открыт.

    void ShortPressOnPageItem(Page *page, int numItem);     // Вызывает функцию короткого нажатия кнопки над итемом numItem страницы page;

    Page *PagePointerFromName(NamePage::E namePage);

    void ChangeStateFlashDrive();

    int8 PosActItem(NamePage::E namePage);                  // Возвращает позицию активного пункта на странице namePage.
};
