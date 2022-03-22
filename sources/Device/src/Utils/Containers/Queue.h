// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


template<typename T>
class Queue
{
public:
    Queue();

    ~Queue();

    void Push(T elem)
    {
        if (pointer == nullptr)
        {
            pointer = new T[1];
            *pointer = elem;
            iFront = 0;
            iBack = 1;
        }
        else
        {
            T *temp = pointer;
            int num = iBack - iFront + 1;
            pointer = new T[(uint)(num)];
            for (int i = 0; i < num - 1; i++)
            {
                pointer[i] = temp[i + iFront];
            }
            pointer[num - 1] = elem;
            iFront = 0;
            iBack = num;
            delete[] temp;
        }
    }

    T Front();

    T Back();

    void Clear();

    int Size() const
    {
        return (iBack - iFront);
    }

    bool Empty() const;

    T &operator[](int n);

    T *Data();

private:

    void Destroy();
    
    T *pointer;         // Указатель на массив элементов
    int iFront;         // Индекс первого элемента
    int iBack;          // Индекс элемента за последним
};
