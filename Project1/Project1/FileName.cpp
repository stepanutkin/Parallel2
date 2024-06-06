#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <numeric>
#include <cmath>
#include <algorithm>

// Функция для заполнения вектора случайными числами
void fillVector(std::vector<int>& vec) {
    for (int i = 0; i < vec.size(); ++i) {
        vec[i] = rand() % 100; // заполнение случайными числами от 0 до 99
    }
}

// Функция для суммирования двух векторов в интервале [start, end)
void sumVectors(const std::vector<int>& v1, const std::vector<int>& v2, std::vector<int>& result, int start, int end) {
    for (int i = start; i < end; ++i) {
        result[i] = v1[i] + v2[i];
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    // Получаем количество доступных аппаратных ядер
    int num_threads = std::thread::hardware_concurrency();
    std::cout << "Доступное количество аппаратных ядер: " << num_threads << std::endl;

    std::vector<int> sizes = { 1000, 10000, 100000, 1000000 };

    // Выводим заголовок таблицы
    std::cout << "Потоки\t1000\t10000\t100000\t1000000" << std::endl;

    for (int num_threads = 1; num_threads <= 16; num_threads *= 2) {
        std::vector<long long> durations;

        for (int size : sizes) {
            std::vector<int> vec1(size);
            std::vector<int> vec2(size);
            std::vector<int> result(size);

            fillVector(vec1);
            fillVector(vec2);

            std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

            int chunk_size = std::ceil(static_cast<double>(size) / num_threads);
            std::vector<std::thread> threads;

            for (int i = 0; i < num_threads; ++i) {
                int start = i * chunk_size;
                int end = std::min((i + 1) * chunk_size, size);
                threads.emplace_back(sumVectors, std::ref(vec1), std::ref(vec2), std::ref(result), start, end);
            }

            for (int i = 0; i < num_threads; ++i) {
                threads[i].join();
            }

            std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
            long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

            durations.push_back(duration);

            threads.clear();
        }

        // Выводим результаты в виде строки
        std::cout << num_threads;
        for (long long duration : durations) {
            std::cout << "\t" << duration << +"s";
        }
        std::cout << std::endl;
    }

    return 0;
}