// 2022/2/11 19:49:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SCALE_FFT                   (set.math.scaleFFT)
#define SCALE_FFT_IS_LOG            (SCALE_FFT == ScaleFFT::Log)
#define SCALE_FFT_IS_LINEAR         (SCALE_FFT == ScaleFFT::Linear)

#define SOURCE_FFT                  (set.math.sourceFFT)
#define SOURCE_FFT_IS_A             (SOURCE_FFT == SourceFFT::A)
#define SOURCE_FFT_IS_B             (SOURCE_FFT == SourceFFT::B)

#define WINDOW_FFT                  (set.math.windowFFT)
#define WINDOW_FFT_IS_HAMMING       (WINDOW_FFT == WindowFFT::Hamming)
#define WINDOW_FFT_IS_BLACKMAN      (WINDOW_FFT == WindowFFT::Blackman)
#define WINDOW_FFT_IS_HANN          (WINDOW_FFT == WindowFFT::Hann)

#define FFT_MAX_DB                  (set.math.fftMaxDB)

#define MATH_FUNC                   (set.math.func)
#define MATH_FUNC_IS_SUM            (MATH_FUNC == Function_Sum)
#define MATH_FUNC_IS_MUL            (MATH_FUNC == Function_Mul)

#define FFT_CUR_CURSOR              (set.math.currentCursor)
#define FFT_CUR_CURSOR_IS_0         (FFT_CUR_CURSOR == 0)

#define FFT_POS_CURSOR(num)         (set.math.posCur[num])
#define FFT_POS_CURSOR_0            (FFT_POS_CURSOR(0))
#define FFT_POS_CURSOR_1            (FFT_POS_CURSOR(1))

#define ENABLED_FFT                 (set.math.enableFFT)

#define MODE_DRAW_MATH              (set.math.modeDraw)
#define DISABLED_DRAW_MATH          (MODE_DRAW_MATH == ModeDrawMath_Disable)
#define MODE_DRAW_MATH_IS_TOGETHER  (MODE_DRAW_MATH == ModeDrawMath_Together)
#define MODE_DRAW_MATH_IS_SEPARATE  (MODE_DRAW_MATH == ModeDrawMath_Separate)

#define MATH_MODE_REG_SET           (set.math.modeRegSet)
#define MATH_MODE_REG_SET_IS_RSHIFT (MATH_MODE_REG_SET == ModeRegSet_RShift)
#define MATH_MODE_REG_SET_IS_RANGE  (MATH_MODE_REG_SET == ModeRegSet_Range)

#define MATH_MULTIPLIER             (set.math.multiplier)




// Масштаб шкалы для отображения спектра.
struct ScaleFFT
{
    enum E
    {
        Log,           // Это значение означает логарифмическую шкалу вывода спектра.
        Linear         // Это значение означает линейную шкалу вывода спектра.
    };
};

// Источинк сигнала для отображения спектра.
struct SourceFFT
{
    enum E
    {
        A,            // Расчёт и отображение спектра первого канала.
        B,            // Расчёт и отображение спектра второго канала.
        Both          // Расчёт и отображение спектров обоих каналов.
    };
};

// Вид окна для умножения сигнала перед расчётом спектра.
struct WindowFFT
{
    enum E
    {
        Rectangle,    // Перед расчётом спектра сигнал не умножается на окно.
        Hamming,      // Перед расчётом спектра сигнал умножается на окно Хэмминга.
        Blackman,     // Перед расчётом спектра сигнал умножается на окно Блэкмана.
        Hann          // Перед расчётом спектра сигнал умножается на окно Ханна.
    };
};

// Минимальное значение на вертикальной координате графика спектра.
struct FFTmaxDB
{
    enum E
    {
        _40,            // Вертикальная координата до -40Дб
        _60,            // Вертикальная координата до -60Дб
        _80             // Вертикальная координата до -80Дб
    };

    static float Abs();
};

// Какую функцию рассчитывать.
enum Function
{
    Function_Sum,           // Сумма двух каналов.
    Function_Mul            // Произведение двух каналов.
};


// Где отрисовывать математический сигнал.
enum ModeDrawMath
{
    ModeDrawMath_Disable,   // Отображение математического сигнала выключено.
    ModeDrawMath_Separate,  // Математический сигнал отображается на отдельном экране.
    ModeDrawMath_Together   // Математический сигнал отображается поверх текущего сигнала.
};


// Чем управляет ручка УСТАНОВКА, когда включена математическая функция.
enum ModeRegSet
{
    ModeRegSet_Range,       // Ручка УСТАНОВКА управляет масштабом по напряжению.
    ModeRegSet_RShift       // Ручка УСТАНОВКА управляет смещением по напряжению.
};


struct SettingsMath
{
    ScaleFFT::E     scaleFFT;       // Масштаб шкалы для отображения спектра.
    SourceFFT::E    sourceFFT;      // Источник сигнала для расчёта и отображения спектра.
    WindowFFT::E    windowFFT;      // Задаёт вид окна, на которое умножается сигнал перед расчётом спектра.
    FFTmaxDB::E     fftMaxDB;       // Минимальное значение на вертикальной координате графика спектра.
    Function        func;           // Включённая математическая функция.
    uint8           currentCursor;  // Определяет, каким курсором спектра управляет ручка УСТАНОВКА.
    uint8           posCur[2];      // Позиция курсора спектра. Изменятеся 0...256.
    int8            koeff1add;      // Коэффициент при первом слагаемом для сложения.
    int8            koeff2add;
    int8            koeff1mul;
    int8            koeff2mul;
    bool            enableFFT;      // \c true, если включён спектр.
    ModeDrawMath    modeDraw;       // Раздельный или общий дисплей в режиме математической функции.
    ModeRegSet      modeRegSet;     // Функция ручки УСТАНОВКА - масштаб по времени или смещение по вертикали.
    Range::E        range;          // Масштаб по напряжению математического сигнала.
    Divider::E      multiplier;     // Множитель масштаба математического сигнала.
    RShift          rShift;         // Смещение по вертикальной оси математического сигнала.
};
