#pragma once

/**
 * @file vector.h 
 * @brief Definition of the Vector class for dynamic arrays with automatic resizing.
 */

#include <memory>
#include <cassert>
#include <algorithm>
#include <new>
#include <utility>
#include <iterator>
#include <cstdlib>

/**
 * @brief The RawMemory class represents a block of raw memory.
 * It provides low-level memory management for the Vector class.
 */
template <typename T>
class RawMemory {
    public:
        
        RawMemory() = default;

        /**
         * @brief Constructor.
         * 
         * @param capacity The capacity of the memory block.
         */
        explicit RawMemory(size_t capacity) : buffer_(Allocate(capacity)), capacity_(capacity) {}

        ~RawMemory() { 
            Deallocate(buffer_); 
        }

        RawMemory(const RawMemory&) = delete;
        RawMemory& operator=(const RawMemory& rhs) = delete;

        /**
         * @brief Move constructor.
         * 
         * @param other The other RawMemory object to move from.
         */
        RawMemory(RawMemory&& other) noexcept : buffer_(std::exchange(other.buffer_, nullptr)), capacity_(std::exchange(other.capacity_, 0)) {}

        /**
         * @brief Move assignment operator.
         * 
         * @param other The other RawMemory object to move from.
         * 
         * @return A reference to the current RawMemory object.
         */
        RawMemory& operator=(RawMemory&& rhs) noexcept {

            if (this != &rhs) {
                buffer_ = std::move(rhs.buffer_);
                capacity_ = std::move(rhs.capacity_);
                rhs.buffer_ = nullptr;
                rhs.capacity_ = 0;
            }

            return *this;
        }
        
        /**
         * @brief Overloaded operator for element access and arithmetic operations.
         * @param offset The offset of the element.
         * @return A pointer to the element at the specified offset.
         */
        T* operator+(size_t offset) noexcept { 
            assert(offset <= capacity_); 
            return buffer_ + offset; 
        }

        const T* operator+(size_t offset) const noexcept { 
            return const_cast<RawMemory&>(*this) + offset; 
        }

        /**
         * @brief Overloaded operator for element access.
         * @param index The index of the element.
         * @return A reference to the element at the specified index.
         */
        const T& operator[](size_t index) const noexcept { 
            return const_cast<RawMemory&>(*this)[index]; 
        }

        T& operator[](size_t index) noexcept { 
            assert(index < capacity_); 
            return buffer_[index]; 
        }

        /**
         * @brief Swaps the content of two RawMemory objects.
         * @param other The other RawMemory object.
         */
        void Swap(RawMemory& other) noexcept { 
            std::swap(buffer_, other.buffer_); 
            std::swap(capacity_, other.capacity_); 
        }

        /**
         * @brief Gets the address of the memory block.
         * @return A pointer to the memory block.
         */
        const T* GetAddress() const noexcept { 
            return buffer_; 
        }

        T* GetAddress() noexcept { 
            return buffer_; 
        }

        /**
         * @brief Gets the capacity of the memory block.
         * @return The capacity of the memory block.
         */
        size_t Capacity() const { 
            return capacity_; 
        }

    private:
        T* buffer_ = nullptr;   /*< The pointer to the memory block. */
        size_t capacity_ = 0;   /*< The capacity of the memory block. */

        /**
         * @brief Allocates memory for the specified number of elements.
         * @param n The number of elements.
         * @return A pointer to the allocated memory block.
         */
        static T* Allocate(size_t n) { 
            return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr; 
        }
        
        /**
         * @brief Deallocates the memory block.
         * @param buf The pointer to the memory block to deallocate.
         */
        static void Deallocate(T* buf) noexcept { 
            operator delete(buf); 
        }
};

/**
 * @brief The Vector class implements a dynamically resizable array.
 */
template <typename T>
class Vector {
    public:

        using iterator = T*; /*< Iterator type for iterating over the elements of the Vector. */
        using const_iterator = const T*; /*< Const iterator type for iterating over the elements of the Vector. */


        Vector() = default;

        /**
         * @brief Constructor.
         * @param size The initial size of the Vector.
         */
        explicit Vector(size_t size) : data_(size), size_(size) {
            std::uninitialized_value_construct_n(data_.GetAddress(), size);
        }

        /**
         * @brief Copy constructor.
         * @param other The other Vector object to copy from.
         */
        Vector(const Vector& other) : data_(other.size_), size_(other.size_) {
            std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
        }

        /**
         * @brief Move constructor.
         * @param other The other Vector object to move from.
         */
        Vector(Vector&& other) noexcept : data_(std::move(other.data_)), size_(std::exchange(other.size_, 0)) {}

        ~Vector() { 
            std::destroy_n(data_.GetAddress(), size_); 
        }

        iterator begin() noexcept { 
            return data_.GetAddress();
        }

        iterator end() noexcept { 
            return size_ + data_.GetAddress(); 
        }

        const_iterator begin() const noexcept { 
            return cbegin(); 
        }

        const_iterator end() const noexcept { 
            return cend(); 
        }

        const_iterator cbegin() const noexcept { 
            return data_.GetAddress(); 
        }

        const_iterator cend() const noexcept { 
            return size_ + data_.GetAddress(); 
        }


        size_t Size() const noexcept { 
            return size_; 
        }

        size_t Capacity() const noexcept { 
            return data_.Capacity(); 
        }

        /**
         * @brief Reserves capacity for the Vector.
         * @param new_capacity The new capacity to reserve.
         */
        void Reserve(size_t new_capacity) {

            if (new_capacity <= data_.Capacity()) {
                return;
            }

            RawMemory<T> new_data(new_capacity);

            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }

            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        }

        /**
         * @brief Resizes the Vector.
         * @param new_size The new size of the Vector.
         */
        void Resize(size_t new_size) {

            if (new_size < size_) {
                std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);

            }
            else {

                if (new_size > data_.Capacity()) {
                    const size_t new_capacity = std::max(data_.Capacity() * 2, new_size);

                    Reserve(new_capacity);
                }

                std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
            }

            size_ = new_size;
        }

        /**
         * @brief Swaps the content of two Vector objects.
         * @param other The other Vector object.
         */
        void Swap(Vector& other) noexcept { 
            data_.Swap(other.data_), std::swap(size_, other.size_); 
        }

        /**
         * @brief Emplaces a new element at the end of the Vector.
         * @tparam Args The types of the arguments to forward.
         * @param args The arguments to forward.
         * @return A reference to the emplaced element.
         */
        template <typename... Args>
        T& EmplaceBack(Args&&... args);

        /**
         * @brief Emplaces a new element at the specified position in the Vector.
         * @tparam Args The types of the arguments to forward.
         * @param pos The position at which to emplace the element.
         * @param args The arguments to forward.
         * @return An iterator pointing to the emplaced element.
         */
        template <typename... Args>
        iterator Emplace(const_iterator pos, Args&&... args);

        /**
         * @brief Erases an element at the specified position from the Vector.
         * @param pos The position of the element to erase.
         * @return An iterator pointing to the element following the erased element.
         */
        iterator Erase(const_iterator pos) {

            assert(pos >= begin() && pos < end());
            size_t indx = pos - begin();
            std::move(begin() + indx + 1, end(), begin() + indx);
            std::destroy_at(end() - 1);
            size_ -= 1;
            return (begin() + indx);
        }

        /**
         * @brief Inserts a new element at the specified position in the Vector.
         * @param pos The position at which to insert the element.
         * @param item The item to insert.
         * @return An iterator pointing to the inserted element.
         */
        iterator Insert(const_iterator pos, const T& item) { 
            return Emplace(pos, item); 
        }
        
        /**
         * @brief Inserts a new element at the specified position in the Vector.
         * @param pos The position at which to insert the element.
         * @param item The item to insert.
         * @return An iterator pointing to the inserted element.
         */
        iterator Insert(const_iterator pos, T&& item) { 
            return Emplace(pos, std::move(item)); 
        }

        /**
         * @brief Removes the last element from the Vector.
         */
        void PopBack() {
            assert(size_);
            std::destroy_at(data_.GetAddress() + size_ - 1);
            --size_;
        }

        /**
         * @brief Inserts a new element at the end of the Vector.
         * @tparam Type The type of the element to emplace.
         * @param value The value of the element to emplace.
        */
        template <typename Type>
        void PushBack(Type&& value);

        /**
         * @brief Copy assignment operator.
         * @param other The other Vector object to copy from.
         * @return A reference to the current Vector object.
         */
        Vector& operator=(const Vector& other) {
            if (this != &other) {
                if (other.size_ <= data_.Capacity()) {
                    if (size_ <= other.size_) {
                        std::copy(other.data_.GetAddress(),
                            other.data_.GetAddress() + size_,
                            data_.GetAddress());

                        std::uninitialized_copy_n(other.data_.GetAddress() + size_,
                            other.size_ - size_,
                            data_.GetAddress() + size_);
                    }
                    else {

                        std::copy(other.data_.GetAddress(),
                            other.data_.GetAddress() + other.size_,
                            data_.GetAddress());
                        std::destroy_n(data_.GetAddress() + other.size_,
                            size_ - other.size_);
                    }
                    size_ = other.size_;
                }
                else {
                    Vector other_copy(other);
                    Swap(other_copy);
                }
            }

            return *this;
        }

        const T& operator[](size_t index) const noexcept { 
            return const_cast<Vector&>(*this)[index]; 
        }

        T& operator[](size_t index) noexcept { 
            assert(index < size_); 
            return data_[index]; 
        }

        Vector& operator=(Vector&& other) noexcept { 
            Swap(other); 
            return *this; 
        }

    private:
        RawMemory<T> data_; /*< The RawMemory object for storing the elements. */
        size_t size_ = 0; /*< The size of the Vector. */
};

/**
 * @brief Pushes a new element to the end of the Vector.
 * @tparam Type The type of the element to push.
 * @param value The value of the element to push.
 */
template <typename T>
template <typename Type>
void Vector<T>::PushBack(Type&& value) {

    if (data_.Capacity() <= size_) {

        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);

        new (new_data.GetAddress() + size_) T(std::forward<Type>(value));

        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
        else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        }

        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);

    }
    else {
        new (data_.GetAddress() + size_) T(std::forward<Type>(value));
    }
    size_++;
}

/**
 * @brief Emplaces a new element to the end of the Vector.
 * @tparam Args The types of the arguments to forward.
 * @param args The arguments to forward.
 * @return A reference to the emplaced element.
 */
template <typename T>
template <typename... Args>
T& Vector<T>::EmplaceBack(Args&&... args) {
    if (data_.Capacity() <= size_) {
        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
        new (new_data.GetAddress() + size_) T(std::forward<Args>(args)...);
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
        else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);

    }
    else {
        new (data_.GetAddress() + size_) T(std::forward<Args>(args)...);
    }

    return data_[size_++];
}

/**
 * @brief Emplaces a new element at the specified position in the Vector.
 * @tparam Args The types of the arguments to forward.
 * @param pos The position at which to emplace the element.
 * @param args The arguments to forward.
 * @return An iterator pointing to the emplaced element.
 */
template <typename T>
template <typename... Args>
typename Vector<T>::iterator Vector<T>::Emplace(const_iterator pos, Args&&... args) {
    assert(pos >= begin() && pos <= end());
    size_t indx = pos - begin();

    if (data_.Capacity() <= size_) {

        RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);

        new (new_data.GetAddress() + indx) T(std::forward<Args>(args)...);
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), indx, new_data.GetAddress());
            std::uninitialized_move_n(data_.GetAddress() + indx, size_ - indx, new_data.GetAddress() + indx + 1);
        }
        else {
            std::uninitialized_copy_n(data_.GetAddress(), indx, new_data.GetAddress());
            std::uninitialized_copy_n(data_.GetAddress() + indx, size_ - indx, new_data.GetAddress() + indx + 1);
        }

        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }
    else {
        try {
            if (pos != end()) {

                T new_s(std::forward<Args>(args)...);
                new (end()) T(std::forward<T>(data_[size_ - 1]));

                std::move_backward(begin() + indx, end() - 1, end());
                *(begin() + indx) = std::forward<T>(new_s);

            }
            else {
                new (end()) T(std::forward<Args>(args)...);
            }

        }
        catch (...) {
            operator delete (end());
            throw;
        }
    }
    size_++;
    return begin() + indx;
}