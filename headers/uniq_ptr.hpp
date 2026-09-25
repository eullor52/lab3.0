#pragma once
#include "exceptions.hpp"
#include <utility>
#include <type_traits>
#include <cstddef>

template <typename T>
class UniqPtr {
private:
	T* ptr;
public:
	UniqPtr();
	UniqPtr(T* ptr);
	UniqPtr(UniqPtr<T>&& other) noexcept;
	UniqPtr(const UniqPtr<T>&) = delete;
	~UniqPtr();
	
	T* release();
	void reset(T* ptr = nullptr);
	void swap(UniqPtr<T>& other);

	T* get() const;
	
	UniqPtr<T>& operator=(UniqPtr<T>&& other) noexcept;
	UniqPtr<T>& operator=(const UniqPtr<T>&) = delete;
	
	T& operator*() const;
	T* operator->() const;
	operator bool() const;
};

template <typename T>
UniqPtr<T>::UniqPtr() : ptr(nullptr) {}

template <typename T>
UniqPtr<T>::UniqPtr(T* p) : ptr(p) {}

template <typename T>
UniqPtr<T>::UniqPtr(UniqPtr<T>&& other) noexcept : ptr(other.ptr) {
	other.ptr = nullptr;
}

template <typename T>
UniqPtr<T>::~UniqPtr() {
	delete ptr;
}

template <typename T>
T* UniqPtr<T>::release() {
	T* tmp = ptr;
	ptr = nullptr;
	return tmp;
}

template <typename T>
void UniqPtr<T>::reset(T* p) {
	if (p != ptr) {
		if (ptr) delete ptr;
		ptr = p;
	}
}

template <typename T>
void UniqPtr<T>::swap(UniqPtr<T>& other) {
	if (this != &other) {
		T* tmp = ptr;
		ptr = other.ptr;
		other.ptr = tmp;
	}
}

template <typename T>
T* UniqPtr<T>::get() const {
	return ptr;
}

template <typename T>
UniqPtr<T>& UniqPtr<T>::operator=(UniqPtr<T>&& other) noexcept {
	if (this != &other) {
		delete ptr;
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return *this;
}

template <typename T>
T& UniqPtr<T>::operator*() const {
	if (ptr != nullptr) {
		return *ptr;
	}
	else throw NullPtrException("Указатель не на что не указывает.");
}

template <typename T>
T* UniqPtr<T>::operator->() const {
	return ptr;
}

template <typename T>
UniqPtr<T>::operator bool() const {
	return ptr != nullptr;
}


// Массивы

template <typename T>
class UniqPtr<T[]> {
private:
	T* ptr;
public:
	UniqPtr();
	UniqPtr(T* ptr);
	UniqPtr(UniqPtr<T[]>&& other) noexcept;
	UniqPtr(const UniqPtr<T[]>&) = delete;
	~UniqPtr();
	
	T* release();
	void reset(T* ptr = nullptr);
	void swap(UniqPtr<T[]>& other);

	T* get() const;
	
	UniqPtr<T[]>& operator=(UniqPtr<T[]>&& other) noexcept;
	UniqPtr<T[]>& operator=(const UniqPtr<T[]>&) = delete;
	
	T& operator*() const;
	T* operator->() const;
	T& operator[](size_t index) const;
	operator bool() const;
};

template <typename T>
UniqPtr<T[]>::UniqPtr() : ptr(nullptr) {}

template <typename T>
UniqPtr<T[]>::UniqPtr(T* p) : ptr(p) {}

template <typename T>
UniqPtr<T[]>::UniqPtr(UniqPtr<T[]>&& other) noexcept : ptr(other.ptr) {
	other.ptr = nullptr;
}

template <typename T>
UniqPtr<T[]>::~UniqPtr() {
	delete[] ptr;
}

template <typename T>
T* UniqPtr<T[]>::release() {
	T* tmp = ptr;
	ptr = nullptr;
	return tmp;
}

template <typename T>
void UniqPtr<T[]>::reset(T* p) {
	if (p != ptr) {
		if (ptr) delete[] ptr;
		ptr = p;
	}
}

template <typename T>
void UniqPtr<T[]>::swap(UniqPtr<T[]>& other) {
	if (this != &other) {
		T* tmp = ptr;
		ptr = other.ptr;
		other.ptr = tmp;
	}
}

template <typename T>
T* UniqPtr<T[]>::get() const {
	return ptr;
}

template <typename T>
UniqPtr<T[]>& UniqPtr<T[]>::operator=(UniqPtr<T[]>&& other) noexcept {
	if (this != &other) {
		delete[] ptr;
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return *this;
}

template <typename T>
T& UniqPtr<T[]>::operator*() const {
	if (ptr != nullptr) {
		return *ptr;
	}
	else throw NullPtrException("Указатель не на что не указывает.");
}

template <typename T>
T* UniqPtr<T[]>::operator->() const {
	return ptr;
}

template <typename T>
T& UniqPtr<T[]>::operator[](size_t index) const {
	return ptr[index];
}

template <typename T>
UniqPtr<T[]>::operator bool() const {
	return ptr != nullptr;
}


template <typename T>
struct _MakeUniq {
	typedef UniqPtr<T> SingleObj;
};

template <typename T>
struct _MakeUniq<T[]> {
	typedef UniqPtr<T[]> Array;
};

template <typename T, size_t Bound>
struct _MakeUniq<T[Bound]> {
	struct InvalidType {};
};

template <typename T, typename ... Args>
inline typename _MakeUniq<T>::SingleObj MakeUnique(Args&& ... _args) {
	return UniqPtr<T>(new T(std::forward<Args>(_args) ...));
}

template <typename T>
inline typename _MakeUniq<T>::Array MakeUnique(size_t count) {
	return UniqPtr<T>(new std::remove_extent_t<T>[count]());
}

template <typename T, typename ... Args>
inline typename _MakeUniq<T>::InvalidType MakeUnique(Args&& ...) = delete;