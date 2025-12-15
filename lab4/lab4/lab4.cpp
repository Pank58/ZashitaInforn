#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <Windows.h>

//Решето Эратосфена 
std::vector<int> sieve_of_eratosthenes(int limit) {
    if (limit < 2) {
        return std::vector<int>();
    }

    std::vector<bool> is_prime(limit + 1, true);
    is_prime[0] = is_prime[1] = false;

    for (int p = 2; p * p <= limit; ++p) {
        if (is_prime[p]) {
            for (int i = p * p; i <= limit; i += p) {
                is_prime[i] = false;
            }
        }
    }

    std::vector<int> primes;
    for (int i = 2; i <= limit; ++i) {
        if (is_prime[i]) {
            primes.push_back(i);
        }
    }

    return primes;
}

//поверка на совешенност числа
bool is_perfect_number(long long number) {
    if (number < 1) return false;

    long long sum = 1; 
    long long sqrt_num = static_cast<long long>(std::sqrt(number));

    // Находим все делители и суммируем их
    for (long long i = 2; i <= sqrt_num; ++i) {
        if (number % i == 0) {
            sum += i;
            if (i != number / i) {
                sum += number / i;
            }
        }
    }

    return sum == number && number != 1;
}

//поиск собственных делителей
std::vector<long long> get_proper_divisors(long long number) {
    std::vector<long long> divisors;
    if (number <= 1) return divisors;

    divisors.push_back(1); 

    long long sqrt_num = static_cast<long long>(std::sqrt(number));
    for (long long i = 2; i <= sqrt_num; ++i) {
        if (number % i == 0) {
            divisors.push_back(i);
            if (i != number / i) {
                divisors.push_back(number / i);
            }
        }
    }

    std::sort(divisors.begin(), divisors.end());
    return divisors;
}

template<typename T>
T custom_min(T a, T b) {
    return (a < b) ? a : b;
}


void print_sieve_results(int limit, const std::vector<int>& primes, long long duration) {
    std::cout << "==========================================" << std::endl;
    std::cout << "РЕШЕТО ЭРАТОСФЕНА" << std::endl;
    std::cout << "Верхняя граница: " << limit << std::endl;
    std::cout << "Найдено простых чисел: " << primes.size() << std::endl;

    if (!primes.empty()) {
        std::cout << "Первые 20 простых чисел: ";
        int show_count = custom_min(20, static_cast<int>(primes.size()));
        for (int i = 0; i < show_count; ++i) {
            std::cout << primes[i];
            if (i < show_count - 1) std::cout << ", ";
        }
        std::cout << std::endl;

        if (primes.size() > 20) {
            std::cout << "... и еще " << primes.size() - 20 << " чисел" << std::endl;
        }

        std::cout << "Последние 5 простых чисел: ";
        int last_count = custom_min(5, static_cast<int>(primes.size()));
        for (int i = primes.size() - last_count; i < primes.size(); ++i) {
            std::cout << primes[i];
            if (i < primes.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }

    std::cout << "Время выполнения: " << duration << " микросекунд ("
        << duration / 1000.0 << " миллисекунд)" << std::endl;
    std::cout << "==========================================" << std::endl;
}

void print_perfect_results(long long number, bool is_perfect,
    const std::vector<long long>& divisors,
    long long duration) {
    std::cout << "==========================================" << std::endl;
    std::cout << "ПРОВЕРКА СОВЕРШЕННОГО ЧИСЛА" << std::endl;
    std::cout << "Число: " << number << std::endl;
    std::cout << "Результат: " << (is_perfect ? "СОВЕРШЕННОЕ" : "НЕ СОВЕРШЕННОЕ") << std::endl;

    if (!is_perfect && !divisors.empty()) {
        std::cout << "Собственные делители: ";
        for (size_t i = 0; i < divisors.size(); ++i) {
            std::cout << divisors[i];
            if (i < divisors.size() - 1) std::cout << " + ";
        }

        long long sum = 0;
        for (long long div : divisors) sum += div;
        std::cout << " = " << sum << std::endl;
    }

    if (is_perfect) {
        std::cout << "Собственные делители: ";
        for (size_t i = 0; i < divisors.size(); ++i) {
            std::cout << divisors[i];
            if (i < divisors.size() - 1) std::cout << " + ";
        }
        long long sum = 0;
        for (long long div : divisors) sum += div;
        std::cout << " = " << sum << std::endl;
    }

    std::cout << "Время выполнения: " << duration << " микросекунд ("
        << duration / 1000.0 << " миллисекунд)" << std::endl;
    std::cout << "==========================================" << std::endl;
}


void print_help() {
    std::cout << "Использование:" << std::endl;
    std::cout << "  program --mode <mode> [--limit <limit> | --number <number>]" << std::endl;
    std::cout << std::endl;
    std::cout << "Параметры:" << std::endl;
    std::cout << "  --mode     Режим работы: 'sieve' (Решето Эратосфена) или 'perfect' (проверка совершенных чисел)" << std::endl;
    std::cout << "  --limit    Для режима 'sieve': верхняя граница поиска простых чисел" << std::endl;
    std::cout << "  --number   Для режима 'perfect': число для проверки на совершенность" << std::endl;
    std::cout << "  --help     Показать эту справку" << std::endl;
    std::cout << std::endl;
    std::cout << "Примеры:" << std::endl;
    std::cout << "  program --mode sieve --limit 1000" << std::endl;
    std::cout << "  program --mode perfect --number 28" << std::endl;
    std::cout << "  program --mode perfect --number 496" << std::endl;
    std::cout << std::endl;
    std::cout << "Известные совершенные числа: 6, 28, 496, 8128, 33550336, 8589869056" << std::endl;
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string mode;
    int limit = 0;
    long long number = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        }
        else if (arg == "--limit" && i + 1 < argc) {
            limit = std::stoi(argv[++i]);
        }
        else if (arg == "--number" && i + 1 < argc) {
            number = std::stoll(argv[++i]);
        }
        else if (arg == "--help") {
            print_help();
            return 0;
        }
    }

    if (mode.empty()) {
        std::cerr << "Ошибка: Необходимо указать параметр --mode" << std::endl;
        std::cerr << "Используйте --help для получения справки" << std::endl;
        return 1;
    }

    if (mode == "sieve") {
        if (limit == 0) {
            std::cerr << "Ошибка: Для режима 'sieve' необходимо указать --limit" << std::endl;
            return 1;
        }
        if (limit < 2) {
            std::cerr << "Ошибка: Предел должен быть не менее 2" << std::endl;
            return 1;
        }
    }
    else if (mode == "perfect") {
        if (number == 0) {
            std::cerr << "Ошибка: Для режима 'perfect' необходимо указать --number" << std::endl;
            return 1;
        }
        if (number < 1) {
            std::cerr << "Ошибка: Число должно быть положительным" << std::endl;
            return 1;
        }
    }
    else {
        std::cerr << "Ошибка: Неверный режим. Допустимые значения: 'sieve', 'perfect'" << std::endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    if (mode == "sieve") {
        std::cout << "Режим: Решето Эратосфена" << std::endl;
        std::vector<int> primes = sieve_of_eratosthenes(limit);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        print_sieve_results(limit, primes, duration.count());
    }
    else if (mode == "perfect") {
        std::cout << "Режим: Проверка совершенных чисел" << std::endl;
        bool perfect = is_perfect_number(number);
        std::vector<long long> divisors;

        divisors = get_proper_divisors(number);
      

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        print_perfect_results(number, perfect, divisors, duration.count());
    }

    return 0;
}