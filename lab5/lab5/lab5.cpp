#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <windows.h>

using namespace std;

/**
 * @brief Вычисляет наибольший общий делитель (НОД) двух чисел
 * @param a Первое число
 * @param b Второе число
 * @return НОД(a, b) (неотрицательное число)
 */
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return abs(a);
}

/**
 * @brief Разложение числа на простые множители
 * @param n Число для разложения (должно быть > 0)
 * @return Вектор пар (простой_множитель, степень)
 */
vector<pair<int, int>> factorize(int n) {
    vector<pair<int, int>> factors;
    int temp = n;

    for (int i = 2; i * i <= temp; ++i) {
        if (temp % i == 0) {
            int count = 0;
            while (temp % i == 0) {
                temp /= i;
                count++;
            }
            factors.push_back({ i, count });
        }
    }

    if (temp > 1) {
        factors.push_back({ temp, 1 });
    }

    return factors;
}

/**
 * @brief Функция Эйлера φ(n) - количество чисел, взаимно простых с n
 * @param n Натуральное число (> 0)
 * @return Значение функции Эйлера φ(n)
 */
int eulerPhi(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;

    auto factors = factorize(n);
    int result = n;

    for (const auto& factor : factors) {
        int prime = factor.first;
        result = result / prime * (prime - 1);
    }

    return result;
}

/**
 * @brief Расширенный алгоритм Евклида
 * @param a Первое число
 * @param b Второе число
 * @return Кортеж (gcd, x, y), где gcd = НОД(a, b) и a*x + b*y = gcd
 */
tuple<int, int, int> extendedGCD(int a, int b) {
    if (b == 0) {
        return make_tuple(abs(a), (a >= 0) ? 1 : -1, 0);
    }

    int gcd, x1, y1;
    tie(gcd, x1, y1) = extendedGCD(b, a % b);

    // Обновляем x и y
    int x = y1;
    int y = x1 - (a / b) * y1;

    return make_tuple(gcd, x, y);
}

/**
 * @brief Находит обратный элемент a по модулю m: a⁻¹ mod m
 * @param a Число
 * @param m Модуль
 * @return Обратный элемент, или -1 если обратного не существует (НОД(a,m) ≠ 1)
 */
int modInverse(int a, int m) {
    int gcd, x, y;
    tie(gcd, x, y) = extendedGCD(a, m);
    if (gcd != 1) return -1;
    return (x % m + m) % m;  
}

/**
 * @brief Китайская теорема об остатках
 * @param a Вектор остатков
 * @param m Вектор модулей (должны быть попарно взаимно просты)
 * @return Наименьшее неотрицательное решение x, или -1 при ошибке
 */
int chineseRemainderTheorem(const vector<int>& a, const vector<int>& m) {
    size_t k = a.size();
    if (k != m.size() || k == 0) return -1;

    for (int mi : m) {
        if (mi <= 0) return -1;
    }

    // Проверка попарной взаимной простоты модулей
    for (size_t i = 0; i < k; ++i) {
        for (size_t j = i + 1; j < k; ++j) {
            if (gcd(m[i], m[j]) != 1) {
                cerr << "Ошибка: модули " << m[i] << " и " << m[j]
                    << " не взаимно просты!" << endl;
                return -1;
            }
        }
    }

    // Вычисляем общий модуль M = m₁·m₂·…·mₖ
    long long M = 1;
    for (int mi : m) M *= mi;

    long long x = 0;  // искомое решение

    // Для каждого сравнения: x ≡ aᵢ (mod mᵢ)
    for (size_t i = 0; i < k; ++i) {
        long long Mi = M / m[i];           // Mᵢ = M / mᵢ
        int inv = modInverse(Mi, m[i]);    // обратный к Mᵢ по модулю mᵢ

        if (inv == -1) {
            cerr << "Не удалось найти обратный для " << Mi
                << " по модулю " << m[i] << endl;
            return -1;
        }

        // Вклад i-го сравнения: aᵢ · Mᵢ · inv (mod M)
        x = (x + static_cast<long long>(a[i]) * Mi % M * inv % M) % M;
    }

    return static_cast<int>(x);
}


//Функции вывода

/**
 * @brief Выводит результат вычисления функции Эйлера
 * @param n Входное число
 * @param phi Результат φ(n)
 * @param duration Время выполнения в микросекундах
 */
void print_euler_results(int n, int phi, long long duration) {
    cout << "==========================================" << endl;
    cout << "ФУНКЦИЯ ЭЙЛЕРА" << endl;
    cout << "Число: " << n << endl;
    cout << "φ(" << n << ") = " << phi << endl;

    
    cout << "Числа от 1 до " << n << ", взаимно простые с " << n << ": ";
    vector<int> coprime_numbers;
    for (int i = 1; i <= n; ++i) {
        if (gcd(i, n) == 1) {
            coprime_numbers.push_back(i);
        }
    }

    for (size_t i = 0; i < coprime_numbers.size(); ++i) {
        cout << coprime_numbers[i];
        if (i < coprime_numbers.size() - 1) cout << ", ";
    }
    cout << endl;

    cout << "Время выполнения: " << duration << " микросекунд ("
        << duration / 1000.0 << " миллисекунд)" << endl;
    cout << "==========================================" << endl;
}

/**
 * @brief Выводит результат расширенного алгоритма Евклида
 * @param a Первое число
 * @param b Второе число
 * @param gcd_result НОД(a, b)
 * @param x Коэффициент x
 * @param y Коэффициент y
 * @param duration Время выполнения в микросекундах
 */
void print_extended_gcd_results(int a, int b, int gcd_result, int x, int y, long long duration) {
    cout << "==========================================" << endl;
    cout << "РАСШИРЕННЫЙ АЛГОРИТМ ЕВКЛИДА" << endl;
    cout << "Числа: a = " << a << ", b = " << b << endl;
    cout << "НОД(" << a << ", " << b << ") = " << gcd_result << endl;
    cout << "Коэффициенты: x = " << x << ", y = " << y << endl;
    cout << "Проверка: " << a << " * " << x << " + " << b << " * " << y
        << " = " << (a * x + b * y) << endl;

    
    if (a * x + b * y == gcd_result) {
        cout << "Решение верно!" << endl;
    }
    else {
        cout << "Ошибка: решение не удовлетворяет уравнению!" << endl;
    }

    cout << "Время выполнения: " << duration << " микросекунд ("
        << duration / 1000.0 << " миллисекунд)" << endl;
    cout << "==========================================" << endl;
}

/**
 * @brief Выводит результат китайской теоремы об остатках
 * @param a Вектор остатков
 * @param m Вектор модулей
 * @param solution Найденное решение
 * @param duration Время выполнения в микросекундах
 */
void print_chinese_remainder_results(const vector<int>& a, const vector<int>& m,
    int solution, long long duration) {
    cout << "==========================================" << endl;
    cout << "КИТАЙСКАЯ ТЕОРЕМА ОБ ОСТАТКАХ" << endl;

    
    cout << "Система сравнений:" << endl;
    for (size_t i = 0; i < a.size(); ++i) {
        cout << "x ≡ " << a[i] << " (mod " << m[i] << ")" << endl;
    }

    if (solution == -1) {
        cout << "Решение не найдено (ошибка в данных)" << endl;
    }
    else {
        long long M = 1;
        for (int mi : m) M *= mi;

        cout << "Решение системы:" << endl;
        cout << "x ≡ " << solution << " (mod " << M << ")" << endl;

        cout << "Проверка:" << endl;
        bool allOK = true;
        for (size_t i = 0; i < a.size(); ++i) {
            int rem = (solution % m[i] + m[i]) % m[i]; 
            cout << "x mod " << m[i] << " = " << rem
                << " (должно быть " << a[i] << ")";
            if (rem == a[i]) {
                cout << "верно";
            }
            else {
                cout << " НЕВЕРНО!";
                allOK = false;
            }
            cout << endl;
        }

        if (allOK) {
            cout << "Все проверки пройдены успешно!" << endl;
        }
        else {
            cout << "Ошибка: решение не удовлетворяет системе!" << endl;
        }
    }

    cout << "Время выполнения: " << duration << " микросекунд ("
        << duration / 1000.0 << " миллисекунд)" << endl;
    cout << "==========================================" << endl;
}

/**
 * @brief Выводит справку по использованию программы
 */
void print_help() {
    cout << "Использование:" << endl;
    cout << "  program --mode <mode> [параметры]" << endl;
    cout << endl;
    cout << "Параметры:" << endl;
    cout << "  --mode     Режим работы:" << endl;
    cout << "             'euler'    - Функция Эйлера" << endl;
    cout << "             'euclid'   - Расширенный алгоритм Евклида" << endl;
    cout << "             'chinese'  - Китайская теорема об остатках" << endl;
    cout << "  --number   Для режима 'euler': число для вычисления φ(n)" << endl;
    cout << "  --a, --b   Для режима 'euclid': два числа для алгоритма" << endl;
    cout << "  --residues Для режима 'chinese': остатки (через запятую)" << endl;
    cout << "  --moduli   Для режима 'chinese': модули (через запятую)" << endl;
    cout << "  --help     Показать эту справку" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  lab5.exe --mode euler --number 100" << endl;
    cout << "  lab5.exe --mode euclid --a 35 --b 15" << endl;
    cout << "  lab5.exe --mode chinese --residues 2,3,2 --moduli 3,5,7" << endl;
    cout << "  lab5.exe --help" << endl;
}

/**
 * @brief Разбирает строку с числами, разделенными запятыми
 * @param str Строка для разбора
 * @return Вектор чисел
 */
vector<int> parse_numbers(const string& str) {
    vector<int> result;
    string current;

    for (char c : str) {
        if (c == ',') {
            if (!current.empty()) {
                result.push_back(stoi(current));
                current.clear();
            }
        }
        else if (c != ' ') {
            current += c;
        }
    }

    if (!current.empty()) {
        result.push_back(stoi(current));
    }

    return result;
}


int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string mode;
    int number = 0;
    int a = 0, b = 0;
    string residues_str, moduli_str;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--mode" && i + 1 < argc) {
            mode = argv[++i];
        }
        else if (arg == "--number" && i + 1 < argc) {
            number = stoi(argv[++i]);
        }
        else if (arg == "--a" && i + 1 < argc) {
            a = stoi(argv[++i]);
        }
        else if (arg == "--b" && i + 1 < argc) {
            b = stoi(argv[++i]);
        }
        else if (arg == "--residues" && i + 1 < argc) {
            residues_str = argv[++i];
        }
        else if (arg == "--moduli" && i + 1 < argc) {
            moduli_str = argv[++i];
        }
        else if (arg == "--help") {
            print_help();
            return 0;
        }
    }

    if (mode.empty()) {
        cerr << "Ошибка: Необходимо указать параметр --mode" << endl;
        cerr << "Используйте --help для получения справки" << endl;
        return 1;
    }

    auto start_time = chrono::high_resolution_clock::now();

    if (mode == "euler") {
        if (number == 0) {
            cerr << "Ошибка: Для режима 'euler' необходимо указать --number" << endl;
            return 1;
        }
        if (number < 1) {
            cerr << "Ошибка: Число должно быть натуральным (≥1)" << endl;
            return 1;
        }

        cout << "Режим: Функция Эйлера" << endl;
        int phi = eulerPhi(number);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        print_euler_results(number, phi, duration.count());

    }
    else if (mode == "euclid") {
        if (a == 0 && b == 0) {
            cerr << "Ошибка: Для режима 'euclid' необходимо указать --a и --b" << endl;
            return 1;
        }

        cout << "Режим: Расширенный алгоритм Евклида" << endl;
        int gcd_result, x, y;
        tie(gcd_result, x, y) = extendedGCD(a, b);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        print_extended_gcd_results(a, b, gcd_result, x, y, duration.count());

    }
    else if (mode == "chinese") {
        if (residues_str.empty() || moduli_str.empty()) {
            cerr << "Ошибка: Для режима 'chinese' необходимо указать --residues и --moduli" << endl;
            return 1;
        }

        vector<int> residues = parse_numbers(residues_str);
        vector<int> moduli = parse_numbers(moduli_str);

        if (residues.size() != moduli.size()) {
            cerr << "Ошибка: Количество остатков и модулей должно совпадать" << endl;
            return 1;
        }

        cout << "Режим: Китайская теорема об остатках" << endl;
        int solution = chineseRemainderTheorem(residues, moduli);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

        print_chinese_remainder_results(residues, moduli, solution, duration.count());

    }
    else {
        cerr << "Ошибка: Неверный режим. Допустимые значения: 'euler', 'euclid', 'chinese'" << endl;
        return 1;
    }

    return 0;
}