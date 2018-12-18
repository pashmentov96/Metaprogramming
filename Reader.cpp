//
// Created by nikita on 18.12.18.
//

#include <iostream>
#include <fstream>
#include "typelists.h"

template <class TList>
class LineReader {};

template <class T1, class... T2>
class LineReader <TypeList<T1, T2...> > {
private:
    using Next = LineReader<TypeList<T2...> >;
public:
    T1 elem;
    enum {size = sizeof(T1) + Next::size};

    void read(void * ptr, std::fstream &in) {
        in >> elem;
        T1 * tmp = static_cast<T1*>(ptr);
        *tmp = elem;
        Next next;
        next.read(tmp + 1, in);
    }
};

template<>
class LineReader <TypeList<> > {
public:
    enum {size = 0};

    void read(void * ptr, std::fstream &in) {

    }
};


template<class TList>
class Reader {
private:
    void readNextLine() {}
};

template<class... T1>
class Reader <TypeList<T1...> > {
private:
    std::fstream in;

public:

    enum {result = 0};

    Reader() {
        in.open("input.txt", std::fstream::in);
    }

    void * readNextLine() {

        using NextLine = LineReader<TypeList<T1...> >;
        NextLine nextLine;
        void * ptr = malloc(sizeOf<TypeList<T1...> >::value);
        nextLine.read(ptr, in);
        return ptr;
    }

    ~Reader() {
        in.close();
    }
};

template <class T>
void add(void * &ptr, T elem) {
    T * new_ptr = static_cast<T*>(ptr);
    *new_ptr = elem;
    ptr = new_ptr + 1;
}

int main() {


    using typeList = TypeList<int, int, char>;
    std::cout << "sizeof(typeList) = " << sizeOf<typeList>::value << std::endl;

    using MyReader = Reader<typeList>;
    MyReader reader;

    for (int i = 0; i < 3; ++i) {
        void *ptr = reader.readNextLine();

        int *ptr1 = static_cast<int *>(ptr);
        ptr = ptr1 + 1;

        int *ptr2 = static_cast<int *>(ptr);
        ptr = ptr2 + 1;

        char *ptr3 = static_cast<char *>(ptr);

        std::cout << *ptr1 << " " << *ptr2 << " " << *ptr3 << std::endl;
    }

    return 0;
}