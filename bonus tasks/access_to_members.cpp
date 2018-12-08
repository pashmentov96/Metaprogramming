//
// Created by nikita on 08.12.18.
//

#include <type_traits>
#include <typeinfo>
#include <iostream>
#include "../typelists.h"

template <class TList, template <class> class Unit>
struct GenScatterHierarchy;

template <class T1, class... T2, template <class> class Unit>
struct GenScatterHierarchy<TypeList<T1, T2...>, Unit>: public GenScatterHierarchy<T1, Unit>, public GenScatterHierarchy<TypeList<T2...>, Unit> {
public:
    using TList = TypeList<T1, T2...>;
    using LeftBase = GenScatterHierarchy<T1, Unit>;
    using RightBase = GenScatterHierarchy<TypeList<T2...>, Unit>;
};

template <class AtomicType, template <class> class Unit>
struct GenScatterHierarchy: public Unit<AtomicType> {
public:
    using LeftBase = Unit<AtomicType>;
};

template <template <class> class Unit>
struct GenScatterHierarchy<NullType, Unit> {};

template<class T>
struct Holder {
    T value_;
};

template<class T, template<class> class Unit, class TList>
Unit<T>& Field(GenScatterHierarchy<TList, Unit>& obj) {
    return obj;
}

template<class T, template<class> class Unit, class TList>
const Unit<T>& Field(const GenScatterHierarchy<TList, Unit>& obj) {
    return obj;
}

int main() {
    using typeList = TypeList<int, double, std::string>;
    using Hierarchy = GenScatterHierarchy<typeList, Holder>;
    Hierarchy obj;
    static_cast<Holder<int>&>(obj).value_ = 100;
    std::cout << Field<int>(obj).value_;
    return 0;
}