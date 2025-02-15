void TestInsert() {
    Vector<int> v(6);
    for (unsigned long i = 0; i < v.Size(); ++i){
        v[i] = i + 1;
    }
    v.Reserve(v.Size() + 1);
    size_t index = 2;
    auto ref = v[index];
    v.Insert(v.begin(), v[index]);

    assert(ref == v[0]);

    for (unsigned long int i = 1; i < v.Size(); ++i){
        assert((unsigned long int) v[i] == i);
    }
}
//Для вставки объекта в вектор без реаллокации и НЕ в конец:
//```
// создаём новый объект Т(форвард аргументов) - сам по себе, без отдельного RawMemory
// размещающий new на конец диапазона с перемещением элемента в конце диапазона
// move_backwards
// удалять ничего не надо, вроде и так всё перемещено, никаких висящих объектов нет
// *индекс_вставки = std::move(копия из первой строки)

if (size_ == index){
    new(begin()+index) T(std::forward<Args>(args)...);
} else {
    T tmp(std::forward<Args>(args)...);
    new (end()) T(std::forward<T>(*(end()-1)));
    std::move_backward(begin()+index, end()-1, end());
    *(begin() + index) = std::move(tmp);
}
/* {
            if (size_ > 0 && index < size_) {
                T new_obj(std::forward<Args>(args)...);
                new (end()) T(std::move(*(end() - 1))); // передвигаем последний элемент вправо, если есть, что передвигать
                try {
                    std::move_backward(nonconst_pos, end() - 1, end());
                }
                catch (...) {
                    std::destroy_n(end(), 1);
                    throw;
                }
                //std::destroy_at(nonconst_pos);
            }
            //new (data_.GetAddress() + index) T(std::forward<Args>(args)...);
            data_[index] = std::move(new_obj);
            //new (data_.GetAddress() + index) T(std::move(new_obj));
        } */