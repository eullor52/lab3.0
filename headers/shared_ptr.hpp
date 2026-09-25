#pragma once
#include "exceptions.hpp"
#include <cstddef>
#include <utility>
#include <type_traits>

template <typename T>
class SharedPtr {
private:
	T* ptr;
	size_t* ref_count;
public:
	SharedPtr();
	SharedPtr(T* ptr);
	SharedPtr(const SharedPtr<T>& other);
	SharedPtr(SharedPtr<T>&& other) noexcept;
	~SharedPtr();

	void release();
	void reset(T* ptr = nullptr);
	void swap(SharedPtr<T>& other);

	T* get() const;
	size_t use_count() const;

	SharedPtr<T>& operator=(const SharedPtr<T>& other);
	SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept;

	T& operator*() const;
	T* operator->() const;
	operator bool() const;
};

template <typename T>
SharedPtr<T>::SharedPtr() : ptr(nullptr), ref_count(nullptr) {}

template <typename T>
SharedPtr<T>::SharedPtr(T* p) : ptr(p), ref_count(p ? new size_t(1) : nullptr) {}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr<T>& other) : ptr(other.ptr), ref_count(other.ref_count) {
	if (ref_count) ++(*ref_count);
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr<T>&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
	other.ptr = nullptr;
	other.ref_count = nullptr;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
	release();
}

template <typename T>
void SharedPtr<T>::release() {
	if (ref_count) {
		if (--(*ref_count) == 0) {
			delete ptr;
			delete ref_count;
		}
		ptr = nullptr;
		ref_count = nullptr;
	}
}

template <typename T>
void SharedPtr<T>::reset(T* p) {
	if (p != ptr) {
		release();
		if (p) {
			ptr = p;
			ref_count = new size_t(1);
		}
	}
}

template <typename T>
void SharedPtr<T>::swap(SharedPtr<T>& other) {
	if (this != &other) {
		T* tmp_ptr = ptr;
		size_t* tmp_count = ref_count;
		ptr = other.ptr;
		ref_count = other.ref_count;
		other.ptr = tmp_ptr;
		other.ref_count = tmp_count;
	}
}

template <typename T>
T* SharedPtr<T>::get() const {
	return ptr;
}

template <typename T>
size_t SharedPtr<T>::use_count() const {
	return ref_count ? *ref_count : 0;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& other) {
	if (this != &other) {
		release();
		ptr = other.ptr;
		ref_count = other.ref_count;
		if (ref_count) ++(*ref_count);
	}
	return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& other) noexcept {
	if (this != &other) {
		release();
		ptr = other.ptr;
		ref_count = other.ref_count;
		other.ptr = nullptr;
		other.ref_count = nullptr;
	}
	return *this;
}

template <typename T>
T& SharedPtr<T>::operator*() const {
	if (ptr != nullptr) {
		return *ptr;
	}
	else throw NullPtrException("Указатель не на что не указывает.");
}

template <typename T>
T* SharedPtr<T>::operator->() const {
	return ptr;
}

template <typename T>
SharedPtr<T>::operator bool() const {
	return ptr != nullptr;
}


// Массивы
template <typename T>
class SharedPtr<T[]> {
private:
	T* ptr;
	size_t* ref_count;
public:
	SharedPtr();
	SharedPtr(T* ptr);
	SharedPtr(const SharedPtr<T[]>& other);
	SharedPtr(SharedPtr<T[]>&& other) noexcept;
	~SharedPtr();

	void release();
	void reset(T* ptr = nullptr);
	void swap(SharedPtr<T[]>& other);

	T* get() const;
	size_t use_count() const;

	SharedPtr<T[]>& operator=(const SharedPtr<T[]>& other);
	SharedPtr<T[]>& operator=(SharedPtr<T[]>&& other) noexcept;

	T& operator*() const;
	T* operator->() const;
	T& operator[](size_t index) const;
	explicit operator bool() const;
};

template <typename T>
SharedPtr<T[]>::SharedPtr() : ptr(nullptr), ref_count(nullptr) {}

template <typename T>
SharedPtr<T[]>::SharedPtr(T* p) : ptr(p), ref_count(p ? new size_t(1) : nullptr) {}

template <typename T>
SharedPtr<T[]>::SharedPtr(const SharedPtr<T[]>& other) : ptr(other.ptr), ref_count(other.ref_count) {
	if (ref_count) ++(*ref_count);
}

template <typename T>
SharedPtr<T[]>::SharedPtr(SharedPtr<T[]>&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
	other.ptr = nullptr;
	other.ref_count = nullptr;
}

template <typename T>
SharedPtr<T[]>::~SharedPtr() {
	release();
}

template <typename T>
void SharedPtr<T[]>::release() {
	if (ref_count) {
		if (--(*ref_count) == 0) {
			delete[] ptr;
			delete ref_count;
		}
		ptr = nullptr;
		ref_count = nullptr;
	}
}

template <typename T>
void SharedPtr<T[]>::reset(T* p) {
	if (p != ptr) {
		release();
		if (p) {
			ptr = p;
			ref_count = new size_t(1);
		}
	}
}

template <typename T>
void SharedPtr<T[]>::swap(SharedPtr<T[]>& other) {
	if (this != &other) {
		T* tmp_ptr = ptr;
		size_t* tmp_count = ref_count;
		ptr = other.ptr;
		ref_count = other.ref_count;
		other.ptr = tmp_ptr;
		other.ref_count = tmp_count;
	}
}

template <typename T>
T* SharedPtr<T[]>::get() const {
	return ptr;
}

template <typename T>
size_t SharedPtr<T[]>::use_count() const {
	return ref_count ? *ref_count : 0;
}

template <typename T>
SharedPtr<T[]>& SharedPtr<T[]>::operator=(const SharedPtr<T[]>& other) {
	if (this != &other) {
		release();
		ptr = other.ptr;
		ref_count = other.ref_count;
		if (ref_count) ++(*ref_count);
	}
	return *this;
}

template <typename T>
SharedPtr<T[]>& SharedPtr<T[]>::operator=(SharedPtr<T[]>&& other) noexcept {
	if (this != &other) {
		release();
		ptr = other.ptr;
		ref_count = other.ref_count;
		other.ptr = nullptr;
		other.ref_count = nullptr;
	}
	return *this;
}

template <typename T>
T& SharedPtr<T[]>::operator*() const {
	if (ptr != nullptr) {
		return *ptr;
	}
	else throw NullPtrException("Указатель не на что не указывает.");
}

template <typename T>
T* SharedPtr<T[]>::operator->() const {
	return ptr;
}

template <typename T>
T& SharedPtr<T[]>::operator[](size_t index) const {
	return ptr[index];
}

template <typename T>
SharedPtr<T[]>::operator bool() const {
	return ptr != nullptr;
}

template <typename T>
struct _MakeShared {
	typedef SharedPtr<T> SingleObj;
};

template <typename T>
struct _MakeShared<T[]> {
	typedef SharedPtr<T[]> Array;
};

template <typename T, size_t Bound>
struct _MakeShared<T[Bound]> {
	struct InvalidType {};
};

template <typename T, typename ... Args>
inline typename _MakeShared<T>::SingleObj MakeShared(Args&& ... _args) {
	return SharedPtr<T>(new T(std::forward<Args>(_args) ...));
}

template <typename T>
inline typename _MakeShared<T>::Array MakeShared(size_t count) {
	return SharedPtr<T>(new std::remove_extent_t<T>[count]());
}

template <typename T, typename ... Args>
inline typename _MakeShared<T>::InvalidType MakeShared(Args&& ...) = delete;