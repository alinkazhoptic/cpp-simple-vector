// вставьте сюда ваш код для класса ArrayPtr
// внесите в него изменения, 
// которые позволят реализовать move-семантику
#pragma once

/*
Урок 8 Создаем RAII обертку над массивом в динамической памяти
Задание
Разработайте шаблонный класс ArrayPtr, играющий роль умного указателя на массив в динамической памяти. 
У него должен быть такой функционал:
- удаление массива при разрушении умного указателя;
- конструкторы: по умолчанию, из указателя на существующий массив, создающий новый массив заданного размера;
- доступ к элементу массива по индексу;
- запрет операций копирования и присваивания;
- метод swap для обмена содержимым с другим объектом ArrayPtr;
- метод Release, прекращающий владение массивом и возвращающий значение сырого указателя.

Ограничения
Сохраните сигнатуры всех публичных методов класса ArrayPtr неизменными, чтобы код скомпилировался без ошибок.

*/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if (size == 0) {
            raw_ptr_ = nullptr;
            return;
        }
        raw_ptr_ = new Type[size]{};

    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr() {
        delete[] raw_ptr_;
        raw_ptr_ = nullptr;
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;



    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* res_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return res_ptr;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        return *std::next(raw_ptr_, index);
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        const Type* ptr_const = std::next(raw_ptr_, index);
        return *ptr_const;
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        return (raw_ptr_ != nullptr);
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        return raw_ptr_;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        Type* ptr_tmp = raw_ptr_;
        raw_ptr_ = other.Get();
        other.raw_ptr_= ptr_tmp;
    }

private:
    Type* raw_ptr_ = nullptr;
};



