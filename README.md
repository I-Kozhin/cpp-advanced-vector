# Vector Class
The Vector class is a C++ implementation of a dynamic array with automatic resizing. It provides functionality for managing a resizable array and offers various methods for element manipulation and memory management.

## Features
*  Automatic resizing: The **Vector** class automatically resizes the underlying array when necessary, allowing for dynamic storage of elements.
*  Iterator support: It provides iterator-based access to the elements of the **Vector** using **begin()**, **end()**, **cbegin()**, and **cend()** methods.
*  Memory management: The **Vector** class manages memory using the **RawMemory** class, which provides low-level memory allocation and deallocation capabilities.
*  Element access and modification: The **Vector** class allows accessing and modifying elements using the **[]** operator.
*  Size and capacity management: It offers methods to get the current size and capacity of the **Vector**.
*  Resizing and capacity management: The **Vector** class provides methods to resize the array and reserve capacity for future elements.
*  Element insertion and erasure: It supports element insertion and erasure at specific positions within the **Vector**.
*  Move and copy semantics: The **Vector** class supports move and copy operations, including move and copy constructors and assignment operators.

##  Usage
To use the Vector class in your C++ program, follow these steps:

1. Include the vector.h header file in your source file:
  ```cpp
  #include "vector.h"
  ```
2. Create an instance of the Vector class:
  ```cpp
  Vector<int> myVector;
```
3. Use the available methods to manipulate the Vector as needed. For example, you can add elements using PushBack():
  ```cpp
  myVector.PushBack(42);
  ```
   You can also access and modify elements using the [] operator:
  ```cpp
  int element = myVector[0];
  myVector[0] = 10;
  ```
4. Remember to manage memory appropriately when using the Vector class. It automatically handles resizing, but you can also manually resize or reserve capacity using the Resize() and Reserve() methods.

## Example
Here's a simple example that demonstrates the usage of the Vector class:
```cpp
#include <iostream>
#include "vector.h"

int main() {
    Vector<int> numbers;
    numbers.PushBack(10);
    numbers.PushBack(20);
    numbers.PushBack(30);

    for (const auto& number : numbers) {
        std::cout << number << " ";
    }

    return 0;
}
```
Output:
```
10 20 30
```
## Project Plans

The project aims to finalize the implementation of the **Vector** class with the following plans:

*  Optimize performance: Improve the performance of the Vector class by optimizing memory allocation and element management.
*  Bug fixes: Address any reported bugs or issues that may arise during usage.
*  Refactoring: Continuously refactor the codebase to improve code quality, maintainability, and readability.

## License

The Vector class is provided under the MIT License.

Feel free to customize the README according to your project's needs, providing additional details and usage examples.
