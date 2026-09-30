#include "../headers/uniq_ptr.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

template <typename Func>
double measure_ms(Func&& f) {
    auto start = Clock::now();
    f();
    auto end = Clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

struct BenchResult {
    double create_ms = 0.0;
    double copy_ms = 0.0;
    double delete_ms = 0.0;
    double total_ms() const { return create_ms + copy_ms + delete_ms; }
};

BenchResult bench_raw(size_t n) {
    BenchResult r;
    std::vector<int*> v1;
    r.create_ms = measure_ms([&]() {
        v1.reserve(n);
        for (size_t i = 0; i < n; ++i) v1.push_back(new int(static_cast<int>(i)));
    });

    std::vector<int*> v2;
    r.copy_ms = measure_ms([&]() {
        v2 = v1;
    });

    r.delete_ms = measure_ms([&]() {
        for (auto* p : v1) delete p;
        v1.clear();
        v2.clear();
    });
    return r;
}

BenchResult bench_uniq(size_t n) {
    BenchResult r;
    std::vector<UniqPtr<int>> v1;
    r.create_ms = measure_ms([&]() {
        v1.reserve(n);
        for (size_t i = 0; i < n; ++i) v1.push_back(MakeUnique<int>(static_cast<int>(i)));
    });

    std::vector<UniqPtr<int>> v2;
    r.copy_ms = measure_ms([&]() {
        v2.reserve(n);
        for (auto& p : v1) v2.push_back(std::move(p));
    });

    r.delete_ms = measure_ms([&]() {
        v1.clear();
        v2.clear();
    });
    return r;
}

BenchResult bench_std_uniq(size_t n) {
    BenchResult r;
    std::vector<std::unique_ptr<int>> v1;
    r.create_ms = measure_ms([&]() {
        v1.reserve(n);
        for (size_t i = 0; i < n; ++i) v1.push_back(std::make_unique<int>(static_cast<int>(i)));
    });

    std::vector<std::unique_ptr<int>> v2;
    r.copy_ms = measure_ms([&]() {
        v2.reserve(n);
        for (auto& p : v1) v2.push_back(std::move(p));
    });

    r.delete_ms = measure_ms([&]() {
        v1.clear();
        v2.clear();
    });
    return r;
}

void print_row(std::ostream& os, size_t n, const char* name, const BenchResult& r, long overhead) {
    os << std::left << std::fixed << std::setprecision(2)
       << std::setw(10) << n << ' '
       << std::setw(18) << name << ' '
       << std::setw(14) << r.create_ms << ' '
       << std::setw(16) << r.copy_ms << ' '
       << std::setw(14) << r.delete_ms << ' '
       << std::setw(10) << r.total_ms() << ' '
       << overhead << "\n";
}

// void csv_row(std::ofstream& csv, size_t n, const char* name, const BenchResult& r, long overhead) {
//     csv << n << "," << name << "," << r.create_ms << "," << r.copy_ms << ","
//         << r.delete_ms << "," << r.total_ms() << "," << overhead << "\n";
// }

int main() {
    std::vector<size_t> sizes = {1'000, 10'000, 100'000, 1'000'000};

    // std::ofstream csv("benchmark_uniq_ptr.csv");
    // csv << "n,type,create_ms,copy_ms,delete_ms,total_ms,overhead_bytes\n";
    long overhead_raw = 0;
    long overhead_uniq = static_cast<long>(sizeof(UniqPtr<int>));
    long overhead_std_uniq = static_cast<long>(sizeof(std::unique_ptr<int>));

    auto print_header = [&]() {
        std::cout << std::left
                  << std::setw(10) << "N" << ' '
                  << std::setw(18) << "Тип указателя" << ' '
                  << std::setw(14) << "Создание(мс)" << ' '
                  << std::setw(16) << "Копирование(мс)" << ' '
                  << std::setw(14) << "Удаление(мс)" << ' '
                  << std::setw(10) << "Итого(мс)" << ' '
                  << "Накладные(Б/эл)\n";
        std::cout << std::string(110, '-') << "\n";
    };
    print_header();

    for (size_t n : sizes) {
        BenchResult raw = bench_raw(n);
        BenchResult uniq = bench_uniq(n);
        BenchResult std_uniq = bench_std_uniq(n);

        print_row(std::cout, n, "Сырой указатель", raw, overhead_raw);
        print_row(std::cout, n, "Наш UniquePtr", uniq, overhead_uniq);
        print_row(std::cout, n, "STL unique_ptr", std_uniq, overhead_std_uniq);

        // csv_row(csv, n, "raw", raw, overhead_raw);
        // csv_row(csv, n, "UniqPtr", uniq, overhead_uniq);
        // csv_row(csv, n, "std::unique_ptr", std_uniq, overhead_std_uniq);
    }

    // std::cout << "\nCSV сохранён в benchmark_uniq_ptr.csv (для построения графика).\n";
    std::cout << "Примечание: у UniqPtr/unique_ptr нет копирующего конструктора, поэтому в "
                 "колонке \"Копирование\" измеряется перенос владения (move) в другой вектор.\n";
    return 0;
}