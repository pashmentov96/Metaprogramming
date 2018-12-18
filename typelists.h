#include <cstdio>
#include <type_traits>
#include <typeinfo>
#include <iostream>

class NullType {};

template <typename...>
class TypeList;

template <>
class TypeList<> {
public:
    using head = NullType;
    using tail = NullType;
};

template <typename T, typename... U>
class TypeList<T, U...> {
public:
    using head = T;
    using tail = TypeList<U...>;
};

//Length
template <typename TypeList>
struct Length {
    enum {value = Length<typename TypeList::tail>::value + 1};
};

template <>
struct Length <NullType> {
    enum {value = 0};
};

template <>
struct Length <TypeList<> > {
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


//Добавляем один TypeList в другой, но как элемент
template<typename T, typename TL>
struct Add {
};

template<typename T, typename ...Args>
struct Add<T, TypeList<Args...> > {
    using Result = TypeList<Args..., T>;
};


// Разворачиваем TypeList
template <class TList>
struct ReverseTypeList {
};

template <>
struct ReverseTypeList<TypeList<> > {
    using Result = TypeList<>;
};

template <class T1, class... T2>
struct ReverseTypeList<TypeList<T1, T2...> > {
    using Result = typename Append<typename ReverseTypeList<TypeList<T2...> >::Result, TypeList<T1> >::Result;
};

template <class TList>
class sizeOf;

template <class T1, class... T2>
class sizeOf <TypeList<T1, T2...> > {
private:
    using tail = sizeOf<TypeList<T2...> >;
public:
    enum {value = sizeof(T1) + tail::value};
};

template <>
class sizeOf <TypeList<> > {
public:
    enum {value = 0};
};

/*int main() {
    using list = TypeList <int, char, double>;
    using emptyList = TypeList<>;
    std::cout << Length<list>::value << " " << Length<emptyList>::value << std::endl;
    std::cout << std::is_same<TypeAt <0, list>::Result, int>::value << std::endl;
    std::cout << IndexOf <double, list>::value << std::endl;
    using newList = Append <int, TypeList<int> >::Result;
    std::cout << Length<newList>::value << std::endl;
    std::cout << std::is_same<TypeAt<3, newList>::Result, int>::value << std::endl;

    using ll = Add <list, TypeList<> >::Result;
    using ll2 = Add <list, ll>::Result;
    std::cout << "kek " << Length <ll2>::value << std::endl;

    using listAfterErase = Erase <char, list>::Result;
    std::cout << Length <listAfterErase>::value << std::endl;
    std::cout << std::is_same<TypeAt <0, listAfterErase>::Result, int>::value << " " << std::is_same<TypeAt <1, listAfterErase>::Result, double>::value << std::endl;
    return 0;
}*/