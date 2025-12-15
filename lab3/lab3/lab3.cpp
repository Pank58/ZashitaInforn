#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <stack>


bool is_perfect_square(long long x) {
    if (x < 0) return false;
    long long root = std::sqrt(x);
    return root * root == x;
}


std::vector<long long> trial_division(long long n) {
    std::vector<long long> factors;

    // Обработка отрицательных чисел и нуля
    if (n < 0) {
        factors.push_back(-1);
        n = -n;
    }

    if (n == 0 || n == 1) {
        factors.push_back(n);
        return factors;
    }

    // Извлечение множителей 2
    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    // Перебор нечётных делителей
    long long f = 3;
    while (f * f <= n) {
        if (n % f == 0) {
            factors.push_back(f);
            n /= f;
        }
        else {
            f += 2;
        }
    }

    // Добавление последнего простого числа, если оно больше 1
    if (n > 1) {
        factors.push_back(n);
    }

    return factors;
}


std::vector<long long> fermat_factorization(long long n) {
    std::vector<long long> factors;

    if (n < 0) {
        factors.push_back(-1);
        n = -n;
    }

    if (n == 0 || n == 1) {
        factors.push_back(n);
        return factors;
    }

   
    while (n % 2 == 0) {
        factors.push_back(2);
        n /= 2;
    }

    
    if (n == 1) {
        return factors;
    }

    
    std::stack<long long> numbers;
    numbers.push(n);

    while (!numbers.empty()) {
        long long current = numbers.top();
        numbers.pop();

        // 
        if (current == 1) {
            continue;
        }

        //Разложение методом Ферма
        long long a = static_cast<long long>(std::ceil(std::sqrt(current)));
        long long b2 = a * a - current;

        int iterations = 0;
        int max_iterations = 100000; 

        // Поиск полного квадрата
        bool found = false;
        while (!is_perfect_square(b2) && iterations < max_iterations) {
            a++;
            b2 = a * a - current;
            iterations++;
        }

        if (is_perfect_square(b2)) {
            //current = (a - b) * (a + b)
            long long b = static_cast<long long>(std::sqrt(b2));
            long long factor1 = a - b;
            long long factor2 = a + b;

            // Проверяем, что множители нетривиальные
            if (factor1 > 1 && factor1 < current && factor2 > 1 && factor2 < current) {
                numbers.push(factor1);
                numbers.push(factor2);
            }
            else {
                factors.push_back(current);
            }
        }
        else {
            // Не нашли разложение - считаем число простым
            factors.push_back(current);
        }
    }

    std::sort(factors.begin(), factors.end());
    return factors;
}


void print_factors(long long number, const std::vector<long long>& factors) {
    std::cout << number << " = ";

    if (factors.empty()) {
        std::cout << "1";
        return;
    }

    for (size_t i = 0; i < factors.size(); ++i) {
        std::cout << factors[i];
        if (i < factors.size() - 1) {
            std::cout << " * ";
        }
    }
    std::cout << std::endl;
}


void print_help() {
    std::cout << "Использование:" << std::endl;
    std::cout << "  program --mode <mode> --number <number>" << std::endl;
    std::cout << std::endl;
    std::cout << "Параметры:" << std::endl;
    std::cout << "  --mode     Режим работы: 'trial' (перебор) или 'fermat' (метод Ферма)" << std::endl;
    std::cout << "  --number   Число для факторизации" << std::endl;
    std::cout << "  --help     Показать эту справку" << std::endl;
    std::cout << std::endl;
    std::cout << "Примеры:" << std::endl;
    std::cout << "  program --mode trial --number 123456789" << std::endl;
    std::cout << "  program --mode fermat --number 987654321" << std::endl;
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string mode;
    long long number = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        }
        else if (arg == "--number" && i + 1 < argc) {
            number = std::stoll(argv[++i]);
        }
        else if (arg == "--help") {
            print_help();
            return 0;
        }
    }

    // Проверка корректности аргументов
    if (mode.empty() || number == 0) {
        std::cerr << "Ошибка: Необходимо указать оба параметра --mode и --number" << std::endl;
        std::cerr << "Используйте --help для получения справки" << std::endl;
        return 1;
    }

    if (mode != "trial" && mode != "fermat") {
        std::cerr << "Ошибка: Неверный режим. Допустимые значения: 'trial', 'fermat'" << std::endl;
        return 1;
    }

    std::vector<long long> factors;
    auto start_time = std::chrono::high_resolution_clock::now();

    // Выполнение факторизации в выбранном режиме
    if (mode == "trial") {
        std::cout << "Режим: Перебор возможных делителей" << std::endl;
        factors = trial_division(number);
    }
    else if (mode == "fermat") {
        std::cout << "Режим: Метод факторизации Ферма" << std::endl;
        factors = fermat_factorization(number);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

             
    std::cout << "==========================================" << std::endl;
    print_factors(number, factors);
    std::cout << "Время выполнения: " << duration.count() << " микросекунд ("
        << duration.count() / 1000.0 << " миллисекунд)" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}