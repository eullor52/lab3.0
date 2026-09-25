#include "fixtures.hpp"
#include "../headers/shared_ptr.hpp"

TEST_F(SharedPtrFixture, default_constructor) {
    SharedPtr<int> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(p));
    EXPECT_EQ(p.use_count(), 0u);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем конструктор по умолчанию.\n";
        std::cerr << "Ожидался указатель nullptr и use_count()==0, получено get()="
                  << p.get() << ", use_count()=" << p.use_count() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, constructor_from_pointer) {
    EXPECT_EQ(sharedInt.get(), number);
    EXPECT_EQ(*sharedInt, 1);
    EXPECT_EQ(sharedInt.use_count(), 1u);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем конструктор от сырого указателя.\n";
        std::cerr << "sharedInt.get() ожидался " << number << ", получен " << sharedInt.get() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, copy_constructor_shares_ownership) {
    SharedPtr<int> copy(sharedInt);
    EXPECT_EQ(copy.get(), sharedInt.get());
    EXPECT_EQ(copy.use_count(), 2u);
    EXPECT_EQ(sharedInt.use_count(), 2u);
    *copy = 5;
    EXPECT_EQ(*sharedInt, 5);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем копирующий конструктор.\n";
        std::cerr << "Ожидался use_count()==2 у обоих, получено copy=" << copy.use_count()
                  << ", sharedInt=" << sharedInt.use_count() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, move_constructor) {
    SharedPtr<int> moved(std::move(sharedInt));
    EXPECT_EQ(moved.get(), number);
    EXPECT_EQ(moved.use_count(), 1u);
    EXPECT_EQ(sharedInt.get(), nullptr);
    EXPECT_EQ(sharedInt.use_count(), 0u);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем move-конструктор.\n";
        std::cerr << "moved.get() ожидался " << number << ", получен " << moved.get() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, copy_assignment_shares_ownership) {
    SharedPtr<int> other;
    other = sharedInt;
    EXPECT_EQ(other.get(), sharedInt.get());
    EXPECT_EQ(other.use_count(), 2u);
    EXPECT_EQ(sharedInt.use_count(), 2u);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем копирующее присваивание.\n";
        std::cerr << "Ожидался use_count()==2, получено other=" << other.use_count()
                  << ", sharedInt=" << sharedInt.use_count() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, copy_assignment_releases_previous_object) {
    SharedPtr<int> other(new int(999));
    other = sharedInt;
    EXPECT_EQ(other.get(), number);
    EXPECT_EQ(*other, 1);
    EXPECT_EQ(sharedInt.use_count(), 2u);
}

TEST_F(SharedPtrFixture, move_assignment) {
    SharedPtr<int> target;
    target = std::move(sharedInt);
    EXPECT_EQ(target.get(), number);
    EXPECT_EQ(target.use_count(), 1u);
    EXPECT_EQ(sharedInt.get(), nullptr);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем move-присваивание.\n";
        std::cerr << "target.get() ожидался " << number << ", получен " << target.get() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, self_assignment_is_safe) {
    sharedInt = sharedInt;
    EXPECT_EQ(sharedInt.get(), number);
    EXPECT_EQ(sharedInt.use_count(), 1u);
}

TEST_F(SharedPtrFixture, self_move_assignment_is_safe) {
    sharedInt = std::move(sharedInt);
    EXPECT_EQ(sharedInt.get(), number);
    EXPECT_EQ(*sharedInt, 1);
}

TEST_F(SharedPtrFixture, get) {
    EXPECT_EQ(sharedInt.get(), number);
    EXPECT_EQ(sharedArray.get(), array);
}

TEST_F(SharedPtrFixture, use_count_reflects_number_of_owners) {
    EXPECT_EQ(sharedInt.use_count(), 1u);
    SharedPtr<int> copy(sharedInt);
    EXPECT_EQ(sharedInt.use_count(), 2u);
    EXPECT_EQ(copy.use_count(), 2u);
}

TEST_F(SharedPtrFixture, reset_replaces_pointer_and_resets_count) {
    int* fresh = new int(99);
    sharedInt.reset(fresh);
    EXPECT_EQ(sharedInt.get(), fresh);
    EXPECT_EQ(*sharedInt, 99);
    EXPECT_EQ(sharedInt.use_count(), 1u);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем reset(новый указатель).\n";
        std::cerr << "Ожидался указатель " << fresh << ", получен " << sharedInt.get() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, reset_default_argument_clears_pointer) {
    sharedInt.reset();
    EXPECT_EQ(sharedInt.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(sharedInt));
    EXPECT_EQ(sharedInt.use_count(), 0u);
}

TEST_F(SharedPtrFixture, reset_with_same_pointer_is_noop) {
    sharedInt.reset(number);
    EXPECT_EQ(sharedInt.get(), number);
    EXPECT_EQ(sharedInt.use_count(), 1u);
    EXPECT_EQ(*sharedInt, 1);
}

TEST_F(SharedPtrLeakFixture, release_deletes_object_when_last_owner) {
    SharedPtr<TrackedObject> p(new TrackedObject(1));
    p.release();
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p.use_count(), 0u);
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(SharedPtrLeakFixture, release_keeps_object_alive_if_shared) {
    SharedPtr<TrackedObject> a(new TrackedObject(1));
    SharedPtr<TrackedObject> b(a);
    a.release();
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(TrackedObject::instances, 1);
    EXPECT_EQ(b.use_count(), 1u);
    EXPECT_EQ(b->value, 1);
}

TEST_F(SharedPtrFixture, release_on_empty_pointer_is_safe) {
    SharedPtr<int> empty;
    empty.release();
    EXPECT_EQ(empty.get(), nullptr);
    EXPECT_EQ(empty.use_count(), 0u);
}

TEST_F(SharedPtrFixture, swap) {
    SharedPtr<int> other(new int(77));
    int* otherPtr = other.get();
    sharedInt.swap(other);
    EXPECT_EQ(sharedInt.get(), otherPtr);
    EXPECT_EQ(*sharedInt, 77);
    EXPECT_EQ(other.get(), number);
    EXPECT_EQ(*other, 1);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем swap().\n";
        std::cerr << "sharedInt.get() ожидался " << otherPtr << ", получен " << sharedInt.get() << "\n\n";
    }
}

TEST_F(SharedPtrFixture, swap_with_self_is_noop) {
    sharedInt.swap(sharedInt);
    EXPECT_EQ(sharedInt.get(), number);
    EXPECT_EQ(*sharedInt, 1);
}

TEST_F(SharedPtrFixture, operator_bool) {
    EXPECT_TRUE(static_cast<bool>(sharedInt));
    SharedPtr<int> empty;
    EXPECT_FALSE(static_cast<bool>(empty));
}

TEST_F(SharedPtrFixture, operator_star_valid) {
    EXPECT_EQ(*sharedInt, 1);
    EXPECT_EQ(*sharedChar, '1');
}

TEST_F(SharedPtrFixture, operator_star_on_empty_throws) {
    SharedPtr<int> empty;
    EXPECT_THROW(*empty, NullPtrException);
    try {
        *empty;
        FAIL() << "Ожидалось исключение NullPtrException";
    } catch (const NullPtrException& e) {
        EXPECT_STREQ(e.what(), "Указатель не на что не указывает.");
    }
}

TEST_F(SharedPtrFixture, operator_arrow) {
    SharedPtr<Point> p(new Point(3, 4));
    EXPECT_EQ(p->x, 3);
    EXPECT_EQ(p->y, 4);
}

TEST_F(SharedPtrFixture, array_get) {
    EXPECT_EQ(sharedArray.get(), array);
}

TEST_F(SharedPtrFixture, array_operator_brackets) {
    EXPECT_EQ(sharedArray[0], 1);
    EXPECT_EQ(sharedArray[1], 2);
    EXPECT_EQ(sharedArray[2], 3);
    if (testing::Test::HasFailure()) {
        std::cerr << "\nТестируем operator[] для SharedPtr<int[]>.\n";
        std::cerr << "Ожидалось 1 2 3, получено "
                  << sharedArray[0] << " " << sharedArray[1] << " " << sharedArray[2] << "\n\n";
    }
}

TEST_F(SharedPtrFixture, array_copy_shares_ownership) {
    SharedPtr<int[]> copy(sharedArray);
    EXPECT_EQ(copy.use_count(), 2u);
    copy[0] = 100;
    EXPECT_EQ(sharedArray[0], 100);
}

TEST_F(SharedPtrFixture, array_operator_bool) {
    EXPECT_TRUE(static_cast<bool>(sharedArray));
    SharedPtr<int[]> empty;
    EXPECT_FALSE(static_cast<bool>(empty));
}

TEST_F(SharedPtrFixture, array_release_deletes_when_last_owner) {
    sharedArray.release();
    EXPECT_EQ(sharedArray.get(), nullptr);
}

TEST_F(SharedPtrFixture, array_swap) {
    SharedPtr<int[]> other(new int[2]{5, 6});
    int* otherPtr = other.get();
    sharedArray.swap(other);
    EXPECT_EQ(sharedArray.get(), otherPtr);
    EXPECT_EQ(other.get(), array);
}

TEST_F(SharedPtrLeakFixture, destructor_frees_object_when_last_owner_destroyed) {
    {
        SharedPtr<TrackedObject> p(new TrackedObject(1));
        EXPECT_EQ(TrackedObject::instances, 1);
    }
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(SharedPtrLeakFixture, object_survives_while_any_copy_alive) {
    SharedPtr<TrackedObject> a(new TrackedObject(1));
    {
        SharedPtr<TrackedObject> b(a);
        EXPECT_EQ(TrackedObject::instances, 1);
        EXPECT_EQ(a.use_count(), 2u);
    }
    EXPECT_EQ(TrackedObject::instances, 1);
    EXPECT_EQ(a.use_count(), 1u);
}

TEST_F(SharedPtrLeakFixture, object_freed_only_after_last_copy_destroyed) {
    SharedPtr<TrackedObject>* a = new SharedPtr<TrackedObject>(new TrackedObject(1));
    SharedPtr<TrackedObject>* b = new SharedPtr<TrackedObject>(*a);
    EXPECT_EQ(TrackedObject::instances, 1);
    delete a;
    EXPECT_EQ(TrackedObject::instances, 1);
    delete b;
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(SharedPtrLeakFixture, reset_releases_previous_object) {
    SharedPtr<TrackedObject> p(new TrackedObject(1));
    EXPECT_EQ(TrackedObject::instances, 1);
    p.reset(new TrackedObject(2));
    EXPECT_EQ(TrackedObject::instances, 1);
    EXPECT_EQ(p->value, 2);
}

TEST_F(SharedPtrLeakFixture, move_assignment_frees_target_previous_object) {
    SharedPtr<TrackedObject> a(new TrackedObject(1));
    SharedPtr<TrackedObject> b(new TrackedObject(2));
    EXPECT_EQ(TrackedObject::instances, 2);
    b = std::move(a);
    EXPECT_EQ(TrackedObject::instances, 1);
    EXPECT_EQ(b->value, 1);
}

TEST_F(SharedPtrLeakFixture, array_destructor_frees_all_elements) {
    {
        SharedPtr<TrackedObject[]> p(new TrackedObject[3]);
        EXPECT_EQ(TrackedObject::instances, 3);
    }
    EXPECT_EQ(TrackedObject::instances, 0);
}

TEST_F(SharedPtrFixture, make_shared_single_default) {
    auto p = MakeShared<int>();
    ASSERT_TRUE(static_cast<bool>(p));
    EXPECT_EQ(*p, 0);
    EXPECT_EQ(p.use_count(), 1u);
}

TEST_F(SharedPtrFixture, make_shared_single_with_value) {
    auto p = MakeShared<int>(42);
    ASSERT_TRUE(static_cast<bool>(p));
    EXPECT_EQ(*p, 42);
}

TEST_F(SharedPtrFixture, make_shared_with_constructor_args) {
    auto p = MakeShared<Point>(3, 4);
    ASSERT_TRUE(static_cast<bool>(p));
    EXPECT_EQ(p->x, 3);
    EXPECT_EQ(p->y, 4);
}

TEST_F(SharedPtrFixture, make_shared_array_is_value_initialized) {
    auto p = MakeShared<int[]>(5);
    ASSERT_TRUE(static_cast<bool>(p));
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(p[i], 0);
    }
}

TEST_F(SharedPtrLeakFixture, make_shared_array_frees_all_elements) {
    {
        auto p = MakeShared<TrackedObject[]>(4);
        EXPECT_EQ(TrackedObject::instances, 4);
    }
    EXPECT_EQ(TrackedObject::instances, 0);
}