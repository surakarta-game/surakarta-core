#include <cstdlib>
#include <iostream>
#include <memory>
#include "hello_cpp_defs.h"
#include "surakarta/surakarta_common.h"

namespace hello_cpp {

// learn google test
int Add(int a, int b) {
    return a + b;
}

// learn how to use std::unique_ptr and std::vector

// using type = std::vector<SurakartaPosition>;   //* SurakartaPosition类名  using type起别名
std::unique_ptr<std::vector<SurakartaPosition>> GenerateAllPositions();  //*声明
//* 智能指针unique_ptr，可以自动释放内存
std::unique_ptr<std::vector<SurakartaPosition>> GenerateAllPositions() {
    // TODO: generate {0, 0}, {0, 1}, ..., {5, 5} in a vector, and return it.
    auto pVector = std::make_unique<std::vector<SurakartaPosition>>();
    for (int i = 0; i <= 5; i++)
        for (int j = 0; j <= 5; j++) {
            auto position = SurakartaPosition(i, j);
            pVector->push_back(position);
        }
    return pVector;
    //* nullptr = NULL
}

//////////////////////////////////////////////////////////////////////////////////////////

// learn how to use std::shared_ptr, std::pair and class

class SmartPointerContainer : public Base {  // don't mind the "public Base" for now
   public:
    // pairs
    std::shared_ptr<std::pair<int, double>> ptr1_;
    std::shared_ptr<std::pair<int, double>> ptr2_;
    std::shared_ptr<std::pair<int, double>> ptr3_;

    SmartPointerContainer() = default;
    //* make_shared 模板函数
    //* pair具体化这个模板函数，使之有一个类型pair
    SmartPointerContainer(int a, double b, int c, double d, int e, double f)
        : ptr1_(std::make_shared<std::pair<int, double>>(a, b)),  //* 赋值方式 = 与 () 有细微区别
          ptr2_(std::make_shared<std::pair<int, double>>(c, d)),
          ptr3_(std::make_shared<std::pair<int, double>>(e, f)) {
        // TODO: Initialize ptr1_, ptr2_, ptr3_ with the given values. {a, b}, {c, d}, {e, f}
    }

    // TODO: Create a member function named "DoSomething" that returns the sum of the pairs' first elements.
    int DoSomething() {
        return ptr1_->first + ptr2_->first + ptr3_->first;
    }
};

int* DoSomething1(int* ptr) {
    if (rand() % 2 == 0) {  // We still use rand here but it's not recommended from C++11.
        *ptr = *ptr + 1;
    } else {
        *ptr = *ptr - 1;
    }
    return ptr;
}

std::unique_ptr<int> DoSomething2(std::unique_ptr<int> ptr) {
    if (rand() % 2 == 0) {  // We still use rand here but it's not recommended from C++11.
        *ptr = *ptr + 1;
    } else {
        *ptr = *ptr - 1;
    }
    return ptr;
}

int* LearningRawPtr() {
    int* raw_ptr = new int(1);  //* 先malloc,然后初始化为1
    DoSomething1(raw_ptr);
    std::cout << *raw_ptr << std::endl;
    return raw_ptr;  // If caller forgets to delete, memory leak occurs.
}

std::unique_ptr<int> LearningUniquePtr() {
    // TODO: Do the same thing as LearningRawPtr, but with std::unique_ptr.
    // Here is an implementation, please modify it to achieve the goal. (Fisrtly, you need to uncomment the code below.)
    std::unique_ptr<int> u_ptr = std::make_unique<int>(1);
    auto u_ptr2 = DoSomething2(std::move(u_ptr));
    // move: （类型转换）强制转换成右值————将死之人   move之后不能再用这个变量！
    std::cout << *u_ptr2 << std::endl;
    return u_ptr2;  // No memory leak will occur.
}

}  // namespace hello_cpp
