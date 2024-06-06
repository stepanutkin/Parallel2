#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
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
    // Получение количества доступных ядер
    setlocale(LC_ALL, "Russian");
    int num_threads = std::thread::hardware_concurrency();
    std::cout << "Доступное количество аппаратных ядер: " << num_threads << std::endl;

    std::vector<int> sizes = { 1000, 10000, 100000, 1000000 };

    for (int size : sizes) {
        std::vector<int> vec1(size);
        std::vector<int> vec2(size);
        std::vector<int> result(size);

        fillVector(vec1);
        fillVector(vec2);

        std::vector<std::thread> threads;

        for (int num_threads = 2; num_threads <= 16; num_threads *= 2) {
            std::vector<std::chrono::steady_clock::time_point> start_times(num_threads);
            std::vector<std::chrono::steady_clock::time_point> end_times(num_threads);
            std::vector<long long> durations(num_threads);

            for (int i = 0; i < num_threads; ++i) {
                start_times[i] = std::chrono::steady_clock::now();
            }

            int chunk_size = std::ceil(static_cast<double>(size) / num_threads);
            for (int i = 0; i < num_threads; ++i) {
                int start = i * chunk_size;
                int end = std::min((i + 1) * chunk_size, size);
                threads.emplace_back(sumVectors, std::ref(vec1), std::ref(vec2), std::ref(result), start, end);
            }

            for (int i = 0; i < num_threads; ++i) {
                threads[i].join();
                end_times[i] = std::chrono::steady_clock::now();
                durations[i] = std::chrono::duration_cast<std::chrono::microseconds>(end_times[i] - start_times[i]).count();
            }

            long long average_duration = std::accumulate(durations.begin(), durations.end(), 0LL) / num_threads;

            std::cout << "Результаты для " << num_threads << " потоков и вектора размером " << size << " элементов: " << average_duration << " мкс" << std::endl;

            threads.clear();
        }
    }

    return 0;
}