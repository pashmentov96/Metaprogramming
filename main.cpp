#include <cstdio>
#include <type_traits>
#include <typeinfo>
#include <iostream>

class NullType {};

template <typename T = NullType, typename ... U>
struct TypeList {
    using head = T;
    using tail = TypeList <U ...>;
};

template <typename T>
struct TypeList<T> {
    using head = T;
    using tail = NullType;
};


//Length
template <typename TypeList>
struct Length {
    enum {value = std::is_same<typename TypeList::head, NullType>::value ? 0 : Length<typename TypeList::tail>::value + 1};
};

template <>
struct Length <NullType> {
    enum {value = 0};
};


//At
template <unsigned int N, typename TypeList>
struct TypeAt {
};

template <typename ...Args>
struct TypeAt <0, TypeList <Args...>> {
    using Result = typename TypeList <Args...>::head;
};

template <unsigned int N, typename ...Args>
struct TypeAt <N, TypeList <Args...>> {
    using Result = typename TypeAt <N - 1, typename TypeList <Args...>::tail>::Result;
};

//Find
template <class T, typename TypeList>
struct IndexOf {
};

template <class T>
struct IndexOf <T, NullType> {
    enum {value = -1};
};

template <class T, typename ...Args>
struct IndexOf <T, TypeList<T, Args...> > {
    enum {value = 0};
};

template <class T, typename ...Args>
struct IndexOf <T, TypeList <Args...> > {
    enum {value = IndexOf <T, typename TypeList <Args...>::tail>::value + 1};
};


//append
template <typename TypeList1, typename TypeList2>
struct Append {
};

template <class T, typename ...Args>
struct Append <T, TypeList <Args...> > {
    using Result = TypeList<T, Args...>;
};

template <typename ...Args1, typename ...Args2>
struct Append <TypeList <Args1...>, TypeList <Args2...> > {
    using Result = TypeList <Args1..., Args2...>;
};


//erase
template <class T, typename TypeList>
struct Erase {
};

template <class T>
struct Erase <T, NullType> {
    using Result = NullType;
};

template <class T, typename ...Args>
struct Erase <T, TypeList <T, Args...> > {
    using Result = TypeList <Args...>;
};

template <class T, typename ...Args>
struct Erase <T, TypeList <Args...> > {
    using Result = typename Append <typename TypeList<Args...>::head, typename Erase <T, typename TypeList <Args...>::tail >::Result >::Result ;
};

int main() {
    using list = TypeList <int, char, double>;
    using emptyList = TypeList<>;
    std::cout << Length<list>::value << " " << Length<emptyList>::value << std::endl;
    std::cout << std::is_same<TypeAt <0, list>::Result, int>::value << std::endl;
    std::cout << IndexOf <double, list>::value << std::endl;
    using newList = Append <int, list>::Result;
    std::cout << Length<newList>::value << std::endl;
    std::cout << std::is_same<TypeAt<3, newList>::Result, int>::value << std::endl;
    using listAfterErase = Erase <char, list>::Result;
    std::cout << Length <listAfterErase>::value << std::endl;
    std::cout << std::is_same<TypeAt <0, listAfterErase>::Result, int>::value << " " << std::is_same<TypeAt <1, listAfterErase>::Result, double>::value << std::endl;
    return 0;
}