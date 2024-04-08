/*
Урок 11. Разработка контейнера SimpleVector часть 2 
Задание
Реализуйте в классе SimpleVector следующие операции:
- Конструктор копирования. Копия вектора должна иметь вместимость, достаточную для хранения копии элементов исходного вектора.
- Оператор присваивания. Должен обеспечивать строгую гарантию безопасности исключений.
- Метод PushBack, добавляющий элемент в конец вектора. Должен обеспечивать строгую гарантию безопасности исключений.
- Метод PopBack, удаляющий последний элемент вектора. Не выбрасывает исключений.
- Метод Insert, вставляющий элемент в произвольное место контейнера. Обеспечивает базовую гарантию безопасности исключений.
- Метод Erase, удаляющий элемент в произвольной позиции вектора. Обеспечивает базовую гарантию безопасности исключений.
- Метод swap, обменивающий содержимое вектора с другим вектором. Не выбрасывает исключений, имеет время выполнения O(1).
- Операторы == и !=. Два вектора равны, если их размеры равны и содержат равные элементы. Равенство вместимости не требуется.
- Операторы <, >, <=, >=, выполняющие лексикографическое сравнение содержимого двух векторов.

Ограничения
Недопустимо передавать в методы Insert и Erase невалидные итераторы и итераторы, полученные у другого вектора. 
Нельзя передавать в метод Erase и end-итератор текущего вектора. Вызывать PopBack у пустого вектора тоже нельзя.
Гарантируется, что тренажёр не будет передавать недопустимые значения итераторов в методы Insert и Erase 
и вызывать PopBack у пустого вектора. Поэтому не усложняйте реализацию этих методов.

Урок 12. Добавление резервирования
Задание
Сигнатура метода Reserve:
void Reserve(size_t new_capacity); 
Reserve задает ёмкость вектора. Этот метод повышает эффективность кода в случае, 
когда пользователь заранее знает хотя бы приблизительное количество элементов в векторе. 
Reserve сразу выделяет нужное количество памяти. При добавлении новых элементов в вектор 
копирование будет происходить или значительно реже или совсем не будет.
Если new_capacity больше текущей capacity, память должна быть перевыделена, 
а элементы вектора скопированы в новый отрезок памяти.
Кроме этого реализуйте возможность вызвать конструктор SimpleVector, 
который будет сразу резервировать нужное количество памяти. Пример вызова ниже.

Урок 7 Move-семантика
Задание
Это задание — итоговый проект спринта. Не забудьте сохранить верное решение.
Улучшите свой вектор, добавив возможность перемещения. 
Используйте предлагаемые тесты и допишите свои, чтобы удостовериться, 
что все нужные методы поддерживают не только копирование, но и перемещение. 
Используйте знания, полученные в последних уроках, и разберитесь, какие методы добавить в уже существующие классы, 
чтобы поддержать возможность перемещения.

*/

#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <utility>

#include "array_ptr.h"


// Класс-обертка, чтобы хранить требуемую вместимость вектора для конструктора SimpleVector
class ReserveProxyObj {
public: 
    ReserveProxyObj(size_t need_capacity)
    : capacity_to_reserve_(need_capacity) {}

    size_t capacity_to_reserve_;
};

// Функция, которая возвращает объект, хранящий необходимую ёмкость
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        // Для нулевого вектора память не выделяется
        if (size == 0) {
            return;
        }

        // выделяем память заданного размера и заполняем значениями по умолчанию (это делается внутри конструктора ArrayPtr)
        ArrayPtr<Type> a_ptr_tmp(size);
        array_ptr_.swap(a_ptr_tmp);
        
        size_ = size;
        capacity_ = size_; 
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        // Для нулевого вектора память не выделяется
        if (size == 0) {
            return;
        }
        // выделяем память заданного размера и заполняем значениями по умолчанию (это делается внутри конструктора ArrayPtr)
        ArrayPtr<Type> a_ptr_tmp(size);
        array_ptr_.swap(a_ptr_tmp);

        // обновляем размеры вектора
        size_ = size;
        capacity_ = size_;

        // заполняем значением value
        std::fill(begin(), end(), value );
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        if (init.size() == 0) {
            SimpleVector();
        }
        else {
            // Выделяем память под вектор-копию
            ArrayPtr<Type> array_ptr_copy(init.size()); 
            // Копируем в буфер
            std::copy(init.begin(), init.end(), array_ptr_copy.Get());
            // Swap - меняем местами с текущим массивом
            array_ptr_.swap(array_ptr_copy);
            size_ = init.size();
            capacity_ = init.size();
        }
    }

    // Конструктор копирования
    SimpleVector(const SimpleVector& other) {
        // Выделяем память под вектор-копию (вместимость будет равна размеру)
        ArrayPtr<Type> array_ptr_copy(other.GetSize()); 
        // Копируем в буфер
        std::copy(other.begin(), other.end(), array_ptr_copy.Get());
        // Swap - меняем местами с текущим массивом
        array_ptr_.swap(array_ptr_copy);
        size_ = other.GetSize();
        capacity_ = size_;
    }

    // Конструктор перемещения
    SimpleVector(SimpleVector&& other) {
        // Определеяем новые параметры
        size_t capacity_new = other.GetCapacity();
        size_t size_new = other.GetSize();

        // Создаем умный указатель из указателя на перемещаемый массив 
        // При этом указатель у перемещаемого массива после Release будет nullptr
        // ArrayPtr<Type> array_ptr_copy(other.array_ptr_.Release()); 
        ArrayPtr<Type> array_ptr_copy;
        array_ptr_copy.swap(other.array_ptr_);
        
        // Обнуляем параметры перемещенного вектора
        other.size_ = 0;
        other.capacity_ = 0;
        
        // Swap - меняем местами с текущим массивом
        array_ptr_.swap(array_ptr_copy);

        // Актуализируем размеры
        size_ = size_new;
        capacity_ = capacity_new;

    }

    // Конструктор с резервированием
    SimpleVector(ReserveProxyObj reserved) {
        Reserve(reserved.capacity_to_reserve_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
           return; 
        }
        // 1 - выделяем новую память размером new_capacity
        // +2 - заполняем значениями по умолчанию
        ArrayPtr<Type> array_ptr_copy(new_capacity); 
        
        // 3 - копируем в неё исходный вектор, в результате в начале вектора-копии - копия исходного вектора, а затем - нули
        std::copy(begin(), end(), array_ptr_copy.Get());
        
        // 4 - перекидываем итератор начала на новый созданный вектор
        array_ptr_.swap(array_ptr_copy);

        // 5 - обновляем вместимость
        capacity_ = new_capacity;
        // 6 - удаляем исходный вектор: сам удалится, так как применили swap с копией, а время жизни копии = эта функция
    }


    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Возвращает ссылку на элемент с индексом index, 
    // index не должен выходить за пределы вектора
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        Type* ptr = &array_ptr_[index];
        return *ptr;
    }

    // Возвращает константную ссылку на элемент с индексом index, 
    // index не должен выходить за пределы вектора
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        const Type* ptr_const = &array_ptr_[index];
        return *ptr_const;
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range ("Required index exceeds the size of array");
        }
        return array_ptr_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range ("Required index exceeds the size of array");
        }
        const Type* ptr_const = &array_ptr_[index];
        return *ptr_const;
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // A - Уменьшение размера 
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        // B - Увеличение размера без увеличения вместимости (без выделения доп. памяти)
        else if (new_size <= capacity_) {
            auto it_begin_extra = this->end();  // конец исходного диапазона (нужен для заполнения дефолтными значениями)
            // Обновляем размер, теперь end() будет указывать на новый конец
            size_ = new_size;
            
            // Заполняем дефолтными значениями
            for (auto it = it_begin_extra; it != this->end(); it++) {
                *it = std::move(Type{});
            }
            
            return;
        }
        else{
            // C - Увеличение размера с выделением доп. памяти
            // 0 - определяем новую вместимость
            size_t new_capacity_ = std::max(new_size, capacity_*2);

            // 1 - выделяем новую память размером new_capacity или 2*capacity_
            // +2 - заполняем значениями по умолчанию
            ArrayPtr<Type> array_ptr_copy(new_capacity_); 
            
            // 3 - копируем в неё исходный вектор, в результате в начале вектора-копии - копия исходного вектора, а затем - нули
            std::move(begin(), end(), array_ptr_copy.Get());
            
            // 4 - перекидываем итератор начала на новый созданный вектор
            array_ptr_.swap(array_ptr_copy);

            // 5 - меняем размер на new_size, а емкость на новую емкость (2*capacity или new_capacity в зависимости от того, что выбрано)
            size_ = new_size;
            capacity_ = new_capacity_;
            // 6 - удаляем исходный вектор: сам удалится, так как применили swap с копией, а время жизни копии = эта функция
            return;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return array_ptr_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return std::next(array_ptr_.Get(), static_cast<int>(size_)); // может быть, надо сдвинуть на (size_ + 1)
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return const_cast<const Type*>(array_ptr_.Get());

    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return const_cast<Type*>(std::next(begin(), static_cast<int>(size_)));
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return const_cast<Type*>(array_ptr_.Get());
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return const_cast<Type*>(std::next(cbegin(), size_));
    }

    // Присваивание
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this == &rhs) {
            throw std::invalid_argument("You can't assign the same object");
        }
        // Выделяем память под вектор-копию
        ArrayPtr<Type> array_ptr_copy(rhs.GetSize()); 
        // Копируем в буфер
        std::copy(rhs.begin(), rhs.end(), array_ptr_copy.Get());
        // Swap - меняем местами с текущим массивом
        array_ptr_.swap(array_ptr_copy);
        size_ = rhs.GetSize();
        capacity_ = size_;
        return *this;
    }

    
    // Оператор перемещения
    SimpleVector& operator=(SimpleVector&& rhs)
    {
        if (this == &rhs) {
            throw std::invalid_argument("You can't assign the same object");
        }
        // Определеяем новые параметры
        size_t capacity_new = rhs.GetCapacity();
        size_t size_new = rhs.GetSize();

        //
        ArrayPtr<Type> array_ptr_copy(rhs.array_ptr_.Release());
        // Обнуляем параметры перемещенного вектора
        rhs.size_ = 0;
        rhs.capacity_ = 0;
        // Swap - меняем местами с текущим массивом
        array_ptr_.swap(array_ptr_copy);
        size_ = size_new;
        capacity_ = capacity_new;

        return *this;
    }
    

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        // Случай 1 - Вместимости хватает на доп элемент 
        if (size_ < capacity_) {
            array_ptr_[size_] = item;
            ++size_;
        }
        // Случай 2 - Вместимости не хватает
        else {
            // 0. Вычисляем новую вместимость для последующего выделения памяти
            size_t new_capacity = std::max(capacity_*2, size_t(1));
            // 1. Выделяем новую память вдвое бОльшую
            ArrayPtr<Type> array_ptr_copy(new_capacity);            
            // 2. Копируем элементы исходного массива
            std::copy(begin(), end(), array_ptr_copy.Get());
            // 3. Добавляем новый элемент в конец
            array_ptr_copy[size_] = item;
            // 4. Меняем местами копию и текущий экземпляр
            array_ptr_.swap(array_ptr_copy);
            // 5. Актуализируем размер (+1) и вместимость (в 2 раза больше)
            ++size_;
            capacity_ = new_capacity;
        }

    }
    
    // Перемещает элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(Type&& item) {
        // Случай 1 - Вместимости хватает на доп элемент 
        if (size_ < capacity_) {
            //std::exchange(array_ptr_[size_], item);
            array_ptr_[size_] = std::move(item);
            item = Type{};
            ++size_;
        }
        // Случай 2 - Вместимости не хватает
        else {
            // 0. Вычисляем новую вместимость для последующего выделения памяти
            size_t new_capacity = std::max(capacity_*2, size_t(1));
            // 1. Выделяем новую память вдвое бОльшую
            ArrayPtr<Type> array_ptr_copy(new_capacity);            
            // 2. Копируем элементы исходного массива
            std::move(begin(), end(), array_ptr_copy.Get());
            // 3. Добавляем новый элемент в конец
            array_ptr_copy[size_] = std::move(item);
            item = Type{};
            // 4. Меняем местами копию и текущий экземпляр
            array_ptr_.swap(array_ptr_copy);
            // 5. Актуализируем размер (+1) и вместимость (в 2 раза больше)
            ++size_;
            capacity_ = new_capacity;
        }

    }
    



    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert((pos >= begin()) && (pos <= end()));
        // Вычисляем номер позиции, куда будем вставлять
        Iterator pos_var = const_cast<Iterator>(pos);
        size_t position_num = static_cast<size_t>( std::distance(begin(), pos_var) );
        // Случай 1 - Вместимости хватает на доп элемент 
        if (size_ < capacity_) {
            // Копируем со сдвигом начиная с позиции вставки
            std::copy_backward(pos_var, end(), std::next(end()));
            // Добавляем новый элемент
            array_ptr_[position_num] = value;
        }
        // Случай 2 - Вместимости не хватает
        else {
            // 0. Вычисляем новую вместимость для последующего выделения памяти
            size_t new_capacity = std::max(capacity_*2, size_t (1));

            // 1. Выделяем новую память вдвое бОльшую
            ArrayPtr<Type> array_ptr_copy(new_capacity);
            // и заполняем её дефолтными значениями
            std::fill(array_ptr_copy.Get(), std::next(array_ptr_copy.Get(), static_cast<int>(new_capacity)), Type{});            
            // 2. Копируем элементы исходного массива
            std::copy(begin(), pos_var, array_ptr_copy.Get());
            // 3. Добавляем новый элемент
            array_ptr_copy[position_num] = value;
            // 4. Копируем остальное:
            std::copy(pos_var, end(), std::next(array_ptr_copy.Get(), static_cast<int>(position_num + 1)));
            // 5. Меняем местами копию и текущий экземпляр
            array_ptr_.swap(array_ptr_copy);
            capacity_ = new_capacity;            
        }
        // 6. Актуализируем размер (+1) и вместимость (в 2 раза больше)
        ++size_;
        // 7. Обновляем итератор, т.к. из-за вставки он либо сместился, либо инвалидировался (в случае перевыделения памяти) 
        pos_var = std::next(begin(), static_cast<int>(position_num));

        return pos_var;
    }


    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, Type&& value) {
        assert((pos >= begin()) && (pos <= end()));
        // Вычисляем номер позиции, куда будем вставлять
        Iterator pos_var = const_cast<Iterator>(pos);
        size_t position_num = static_cast<size_t>( std::distance(begin(), pos_var) );
        // Случай 1 - Вместимости хватает на доп элемент 
        if (size_ < capacity_) {
            // Перемещаем (НЕ КОПИРУЕМ) со сдвигом на один элемент
            std::move(pos_var, end(), std::next(pos_var));
            // Добавляем новый элемент
            array_ptr_[position_num] = std::move(value);
        }
        // Случай 2 - Вместимости не хватает
        else {
            // 0. Вычисляем новую вместимость для последующего выделения памяти
            size_t new_capacity = std::max(capacity_*2, size_t (1));

            // 1. Выделяем новую память вдвое бОльшую
            ArrayPtr<Type> array_ptr_copy(new_capacity);

            // и заполняем её дефолтными значениями
            for(size_t i = 0; i < new_capacity; i++) {
                array_ptr_copy[i] = std::move(Type{});
            }

            // 2. Копируем элементы исходного массива
            std::move(begin(), pos_var, array_ptr_copy.Get());
            // 3. Добавляем новый элемент
            array_ptr_copy[position_num] = std::move(value);
            // 4. Копируем остальное:
            std::move(pos_var, end(), std::next(array_ptr_copy.Get(), static_cast<int>(position_num + 1)));
            // 5. Меняем местами копию и текущий экземпляр
            array_ptr_.swap(array_ptr_copy);
            capacity_ = new_capacity;            
        }
        // 6. Актуализируем размер (+1) и вместимость (в 2 раза больше)
        ++size_;
        // 7. Обновляем итератор, т.к. из-за вставки он либо сместился, либо инвалидировался (в случае перевыделения памяти) 
        pos_var = std::next(begin(), static_cast<int>(position_num));

        return pos_var;
    }




    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(size_ != 0);
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert((pos >= begin()) && (pos < end()));
        Iterator pos_var = const_cast<Iterator>(pos);
        int position_num = static_cast<int>(std::distance(begin(), pos_var));
        // Перезапись элементов, начиная со следующей позиции
        if (size_ != 0) {
            std::move(std::next(pos_var), end(), pos_var);
            // Актуализируем размер после "удаления" элемента
            --size_;
        }
        
        return std::next(begin(), position_num);
    }

    

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        // Обмен указателями
        array_ptr_.swap(other.array_ptr_);

        // Обмен размерами
        std::swap(size_, other.size_);
        
        // Обмен вместимостями
        std::swap(capacity_, other.capacity_); 
        
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> array_ptr_;

};


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs) ;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
} 




