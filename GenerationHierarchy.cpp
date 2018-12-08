#include <type_traits>
#include <typeinfo>
#include <iostream>
#include "typelists.h"

// Вычисляем рекурсивно число Фибоначчи
template <int n>
struct Fibonacci {
    enum {value = Fibonacci<n - 1>::value + Fibonacci<n - 2>::value};
};

template <>
struct Fibonacci<0> {
    enum {value = 1};
};

template <>
struct Fibonacci<1> {
    enum {value = 1};
};


// GenScatterHierarchy по мотивам книги Александреску
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


// GenLinearHierarchy по мотивам книги Александреску
template <class TList, template <class AtomicType, class Base> class Unit, class Root = NullType>
class GenLinearHierarchy;

template <class T1, class... T2, template <class, class> class Unit, class Root>
class GenLinearHierarchy<TypeList<T1, T2...>, Unit, Root>: public Unit<T1, GenLinearHierarchy<TypeList<T2...>, Unit, Root> > {
public:
    using ChildBase = Unit<T1, GenLinearHierarchy<TypeList<T2...>, Unit, Root> >;
    using Child = GenLinearHierarchy<TypeList<T2...>, Unit, Root>;
};

template <class T, template <class, class> class Unit, class Root>
class GenLinearHierarchy<TypeList<T>, Unit, Root>: public Unit<T, Root> {
public:
    using ChildBase = Unit<T, Root>;
};

// Unit для Linear
template <class T, class Base>
class EventHandler: public Base {
public:
    virtual void OnEvent(T &obj, int eventId);
};

// Unit для Scatter
template <class T>
struct Holder {
    T value;
};


// Напишем такой класс, который будет исходный TypeList разбивать на ветки нужной длины,
// каждую ветку записывать в TypeList, а весь результат - это TypeList из TypeList'ов(TypeList веток)
template <class TList, int is_start, int num_branch, int index_branch>
struct GenFibonacciTypeList {};

// Случай, когда мы получаем исходный TypeList
template <class... T1>
struct GenFibonacciTypeList<TypeList<T1...>, -1, -1, 0> {
public:
    using Result = typename GenFibonacciTypeList<TypeList<T1...>, 1, 0, Fibonacci<0>::value>::Result;
};

// случай, когда мы идем по ветке
// тут же рассматриваю случай, когда эта ветка заканчивается -- не смог вынести в отдельный случай, потому что был конфликт
template <class T1, class... T2, int num_branch, int index_branch>
struct GenFibonacciTypeList<TypeList<T1, T2...>, 0, num_branch, index_branch> {
public:
    using branch = typename std::conditional<index_branch == 0, TypeList<T1>, typename Append<T1, typename GenFibonacciTypeList<TypeList<T2...>, 0, num_branch, index_branch - 1>::branch>::Result>::type;
    using Result = typename std::conditional<index_branch == 0, typename GenFibonacciTypeList <TypeList<T2...>, 1, num_branch + 1, Fibonacci<num_branch + 1>::value>::Result, typename GenFibonacciTypeList<TypeList<T2...>, 0, num_branch, index_branch - 1>::Result>::type;
};

// случай, когда мы находимся в первой вершине ветки
template <class T1, class... T2, int num_branch, int index_branch>
struct GenFibonacciTypeList<TypeList<T1, T2...>, 1, num_branch, index_branch> {
private:
    using next = typename GenFibonacciTypeList<TypeList<T2...>, 0, num_branch, index_branch - 1>::branch;
public:
    using branch = typename Append<T1, next>::Result;
    using Result = typename Add <branch, typename GenFibonacciTypeList<TypeList<T2...>, 0, num_branch, index_branch - 1>::Result>::Result;
};

// случай, когда TypeList закончился
template <int index_list, int num_branch, int index_branch>
struct GenFibonacciTypeList<TypeList<>, index_list, num_branch, index_branch> {
public:
    using branch = TypeList<>;
    using Result = TypeList<>;
};


// теперь можно вызывать FibonacciTypeList<TypeList> и это будет наш искомый список веток
// работу проверял в main при помощи At
template <class TList>
using FibonacciTypeList = typename ReverseTypeList<typename GenFibonacciTypeList<TList, -1, -1, 0>::Result>::Result;


// теперь надо создать все ветки, чтобы потом можно было от них унаследоваться с помощью Scatter
template <template <class, class> class Unit, class Root, class... TLists>
class GenBranches;

template <template <class, class> class Unit, class Root, class... TLists>
class GenBranches<Unit, Root, TypeList<TLists...> > {
public:
    using Result = TypeList<GenLinearHierarchy<TLists, Unit, Root>...>;
};


// генерируем требуемую иерархию с помощью Scatter и веток
template <template <class> class ScatterUnit, template <class, class> class LinearUnit, class Root, class TList>
using GenFibonacciHierarchy = GenScatterHierarchy<typename GenBranches<LinearUnit, Root, FibonacciTypeList<TList> >::Result, ScatterUnit>;


int main() {
    std::cout << Fibonacci<5>::value << std::endl;
    using typeList = TypeList<int, int, double, int, char, int, int, int>;
    std::cout << Length<typeList>::value << std::endl;
    using fibonacciList = FibonacciTypeList<typeList>;
    std::cout << Length<fibonacciList>::value << std::endl;
    //using l = TypeAt<1, fibonacciList>::Result;
    //using ll = Length<l>::value;
    using Hierarchy = GenFibonacciHierarchy <Holder, EventHandler, NullType, typeList>;
    Hierarchy hierarchy;
    return 0;
}