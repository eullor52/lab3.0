#include "fixtures.hpp"
#include "../headers/uniq_ptr.hpp"
#include <type_traits>
#include <utility>

TEST_F(UniqPtrFixture, default_constructor) {
    UniqPtr<int> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_FALSE(p);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем конструктор по умолчанию.\n";
        std::cerr << "Ожидаемый указатель: nullptr\n";
        std::cerr << "Полученный указатель: " << p.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, constructor_from_pointer) {
    EXPECT_EQ(uniqInt.get(), number);
    EXPECT_EQ(*uniqInt, 1);
    EXPECT_EQ(uniqChar.get(), character);
    EXPECT_EQ(*uniqChar, '1');
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем конструктор от сырого указателя.\n";
        std::cerr << "uniqInt.get() ожидался " << number << ", получен " << uniqInt.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, move_constructor) {
    UniqPtr<int> moved(std::move(uniqInt));
    EXPECT_EQ(moved.get(), number);
    EXPECT_EQ(*moved, 1);
    EXPECT_EQ(uniqInt.get(), nullptr);
    EXPECT_FALSE(uniqInt);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем move-конструктор.\n";
        std::cerr << "moved.get() ожидался " << number << ", получен " << moved.get() << "\n";
        std::cerr << "uniqInt.get() после перемещения ожидался nullptr, получен " << uniqInt.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, not_copy_constructible) {
    static_assert(!std::is_copy_constructible<UniqPtr<int>>::value,
                  "UniqPtr<T> не должен быть копируемым");
    static_assert(!std::is_copy_constructible<UniqPtr<int[]>>::value,
                  "UniqPtr<T[]> не должен быть копируемым");
    SUCCEED();
}

TEST_F(UniqPtrFixture, not_copy_assignable) {
    static_assert(!std::is_copy_assignable<UniqPtr<int>>::value,
                  "UniqPtr<T> не должен поддерживать копирующее присваивание");
    static_assert(!std::is_copy_assignable<UniqPtr<int[]>>::value,
                  "UniqPtr<T[]> не должен поддерживать копирующее присваивание");
    SUCCEED();
}

TEST_F(UniqPtrFixture, move_assignment) {
    UniqPtr<int> target;
    target = std::move(uniqInt);
    EXPECT_EQ(target.get(), number);
    EXPECT_EQ(*target, 1);
    EXPECT_EQ(uniqInt.get(), nullptr);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем move-присваивание.\n";
        std::cerr << "target.get() ожидался " << number << ", получен " << target.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, self_move_assignment_is_safe) {
    uniqInt = std::move(uniqInt);
    EXPECT_EQ(uniqInt.get(), number);
    EXPECT_EQ(*uniqInt, 1);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем самоприсваивание при перемещении.\n";
        std::cerr << "Указатель после self-move: " << uniqInt.get() << " (ожидался " << number << ")\n\n";
    }
}

TEST_F(UniqPtrFixture, get) {
    EXPECT_EQ(uniqInt.get(), number);
    EXPECT_EQ(uniqArray.get(), array);
}

TEST_F(UniqPtrFixture, release_transfers_ownership) {
    int* released = uniqInt.release();
    EXPECT_EQ(released, number);
    EXPECT_EQ(uniqInt.get(), nullptr);
    EXPECT_FALSE(uniqInt);
    delete released;
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем release().\n";
        std::cerr << "Возвращённый указатель ожидался " << number << ", получен " << released << "\n";
        std::cerr << "uniqInt.get() после release() ожидался nullptr, получен " << uniqInt.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, reset_replaces_pointer) {
    int* fresh = new int(99);
    uniqInt.reset(fresh);
    EXPECT_EQ(uniqInt.get(), fresh);
    EXPECT_EQ(*uniqInt, 99);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем reset(новый указатель).\n";
        std::cerr << "Ожидался указатель " << fresh << ", получен " << uniqInt.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, reset_default_argument_clears_pointer) {
    uniqInt.reset();
    EXPECT_EQ(uniqInt.get(), nullptr);
    EXPECT_FALSE(uniqInt);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем reset() без аргументов.\n";
        std::cerr << "Ожидался nullptr, получен " << uniqInt.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, reset_with_same_pointer_is_noop) {
    uniqInt.reset(number);
    EXPECT_EQ(uniqInt.get(), number);
    EXPECT_EQ(*uniqInt, 1);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем reset(тот же указатель).\n";
        std::cerr << "Указатель не должен был измениться: ожидался " << number << ", получен " << uniqInt.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, swap) {
    UniqPtr<int> other(new int(77));
    int* otherPtr = other.get();
    uniqInt.swap(other);
    EXPECT_EQ(uniqInt.get(), otherPtr);
    EXPECT_EQ(*uniqInt, 77);
    EXPECT_EQ(other.get(), number);
    EXPECT_EQ(*other, 1);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем swap().\n";
        std::cerr << "uniqInt.get() ожидался " << otherPtr << ", получен " << uniqInt.get() << "\n";
        std::cerr << "other.get() ожидался " << number << ", получен " << other.get() << "\n\n";
    }
}

TEST_F(UniqPtrFixture, swap_with_self_is_noop) {
    uniqInt.swap(uniqInt);
    EXPECT_EQ(uniqInt.get(), number);
    EXPECT_EQ(*uniqInt, 1);
}

TEST_F(UniqPtrFixture, operator_bool) {
    EXPECT_TRUE(static_cast<bool>(uniqInt));
    UniqPtr<int> empty;
    EXPECT_FALSE(static_cast<bool>(empty));
}

TEST_F(UniqPtrFixture, operator_star_valid) {
    EXPECT_EQ(*uniqInt, 1);
    EXPECT_EQ(*uniqChar, '1');
}

TEST_F(UniqPtrFixture, operator_star_on_empty_throws) {
    UniqPtr<int> empty;
    EXPECT_THROW(*empty, NullPtrException);
    try {
        *empty;
        FAIL() << "Ожидалось исключение NullPtrException";
    } catch (const NullPtrException& e) {
        EXPECT_STREQ(e.what(), "Указатель не на что не указывает.");
    }
}

TEST_F(UniqPtrFixture, operator_arrow) {
    UniqPtr<Point> p(new Point(3, 4));
    EXPECT_EQ(p->x, 3);
    EXPECT_EQ(p->y, 4);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем operator->().\n";
        std::cerr << "Ожидалось x=3,y=4, получено x=" << p->x << ",y=" << p->y << "\n\n";
    }
}

TEST_F(UniqPtrFixture, array_get) {
    EXPECT_EQ(uniqArray.get(), array);
}

TEST_F(UniqPtrFixture, array_operator_brackets) {
    EXPECT_EQ(uniqArray[0], 1);
    EXPECT_EQ(uniqArray[1], 2);
    EXPECT_EQ(uniqArray[2], 3);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем operator[] для UniqPtr<int[]>.\n";
        std::cerr << "Ожидалось 1 2 3, получено "
                  << uniqArray[0] << " " << uniqArray[1] << " " << uniqArray[2] << "\n\n";
    }
}

TEST_F(UniqPtrFixture, array_operator_star_returns_first_element) {
    EXPECT_EQ(*uniqArray, array[0]);
}

TEST_F(UniqPtrFixture, array_operator_bool) {
    EXPECT_TRUE(static_cast<bool>(uniqArray));
    UniqPtr<int[]> empty;
    EXPECT_FALSE(static_cast<bool>(empty));
}

TEST_F(UniqPtrFixture, array_release_transfers_ownership) {
    int* released = uniqArray.release();
    EXPECT_EQ(released, array);
    EXPECT_EQ(uniqArray.get(), nullptr);
    delete[] released;
}

TEST_F(UniqPtrFixture, array_reset_replaces_pointer) {
    int* fresh = new int[2]{5, 6};
    uniqArray.reset(fresh);
    EXPECT_EQ(uniqArray.get(), fresh);
    EXPECT_EQ(uniqArray[0], 5);
    EXPECT_EQ(uniqArray[1], 6);
}

TEST_F(UniqPtrFixture, array_swap) {
    UniqPtr<int[]> other(new int[2]{5, 6});
    int* otherPtr = other.get();
    uniqArray.swap(other);
    EXPECT_EQ(uniqArray.get(), otherPtr);
    EXPECT_EQ(other.get(), array);
}

TEST_F(UniqPtrLeakFixture, destructor_frees_managed_object) {
    {
        UniqPtr<TrackedObject> p(new TrackedObject(1));
        EXPECT_EQ(TrackedObject::instances, 1);
    }
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(UniqPtrLeakFixture, reset_frees_previous_object) {
    UniqPtr<TrackedObject> p(new TrackedObject(1));
    EXPECT_EQ(TrackedObject::instances, 1);
    p.reset(new TrackedObject(2));
    EXPECT_EQ(TrackedObject::instances, 1);
    EXPECT_EQ(p->value, 2);
}

TEST_F(UniqPtrLeakFixture, reset_with_same_pointer_does_not_double_free) {
    TrackedObject* raw = new TrackedObject(1);
    UniqPtr<TrackedObject> p(raw);
    p.reset(raw);
    EXPECT_EQ(TrackedObject::instances, 1);
}

TEST_F(UniqPtrLeakFixture, move_assignment_frees_target_previous_object) {
    UniqPtr<TrackedObject> a(new TrackedObject(1));
    UniqPtr<TrackedObject> b(new TrackedObject(2));
    EXPECT_EQ(TrackedObject::instances, 2);
    b = std::move(a);
    EXPECT_EQ(TrackedObject::instances, 1);
    EXPECT_EQ(b->value, 1);
}

TEST_F(UniqPtrLeakFixture, release_does_not_destroy_object) {
    UniqPtr<TrackedObject> p(new TrackedObject(1));
    TrackedObject* released = p.release();
    EXPECT_EQ(TrackedObject::instances, 1);
    delete released;
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(UniqPtrLeakFixture, array_destructor_frees_all_elements) {
    {
        UniqPtr<TrackedObject[]> p(new TrackedObject[3]);
        EXPECT_EQ(TrackedObject::instances, 3);
    }
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(UniqPtrFixture, make_unique_single_default) {
    auto p = MakeUnique<int>();
    ASSERT_TRUE(p);
    EXPECT_EQ(*p, 0);
}

TEST_F(UniqPtrFixture, make_unique_single_with_value) {
    auto p = MakeUnique<int>(42);
    ASSERT_TRUE(p);
    EXPECT_EQ(*p, 42);
}

TEST_F(UniqPtrFixture, make_unique_with_constructor_args) {
    auto p = MakeUnique<Point>(3, 4);
    ASSERT_TRUE(p);
    EXPECT_EQ(p->x, 3);
    EXPECT_EQ(p->y, 4);
}

TEST_F(UniqPtrFixture, make_unique_array_is_value_initialized) {
    auto p = MakeUnique<int[]>(5);
    ASSERT_TRUE(p);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(p[i], 0);
    }
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем MakeUnique<T[]>(count) — значения по умолчанию.\n";
        std::cerr << "Ожидались все нули, получено: ";
        for (size_t i = 0; i < 5; ++i) std::cerr << p[i] << " ";
        std::cerr << "\n\n";
    }
}

TEST_F(UniqPtrFixture, make_unique_array_elements_are_writable) {
    auto p = MakeUnique<int[]>(3);
    p[0] = 10;
    p[1] = 20;
    p[2] = 30;
    EXPECT_EQ(p[0], 10);
    EXPECT_EQ(p[1], 20);
    EXPECT_EQ(p[2], 30);
}

TEST_F(UniqPtrLeakFixture, make_unique_array_frees_all_elements) {
    {
        auto p = MakeUnique<TrackedObject[]>(4);
        EXPECT_EQ(TrackedObject::instances, 4);
    }
    EXPECT_EQ(TrackedObject::instances, 0);
}