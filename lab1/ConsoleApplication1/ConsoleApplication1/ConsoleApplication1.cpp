//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <string>
//#include <unordered_map>
//#include <stdexcept>
//#include <vector>
//#include <set>
//#include <algorithm>
//#include <locale>
//#include <clocale>
//#include <cwctype>
//#include <type_traits>
//#include <limits>
//#include <cstdlib>
//#include <windows.h>
//
//// -----------------------------
//// ProgramOptions
//// -----------------------------
//struct ProgramOptions {
//    std::string mode;        // Encrypt or Decrypt
//    long long offset = 0;
//    std::string text;        // исходная байтовая строка (в системной кодировке, напр. CP1251)
//    std::wstring wtext;      // wide-строка для обработки
//};
//
//// -----------------------------
//// Конвертация через системную локаль (CP1251 на русском Windows)
//// -----------------------------
//static std::wstring mb_to_wide_sys(const std::string& s) {
//    // Устанавливаем локаль по умолчанию (системную).
//    // В Windows это даст преобразование в CP1251 при русской локали.
//    std::setlocale(LC_CTYPE, "");
//    // Сначала определим длину
//    std::mbstate_t state{};
//    const char* src = s.c_str();
//    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
//    if (len == (size_t)-1) {
//        throw std::runtime_error("Ошибка: не удалось преобразовать входную строку в системной кодировке.");
//    }
//    std::vector<wchar_t> buf(len + 1);
//    src = s.c_str();
//    size_t r = std::mbsrtowcs(buf.data(), &src, buf.size(), &state);
//    if (r == (size_t)-1) {
//        throw std::runtime_error("Ошибка: не удалось преобразовать входную строку в системной кодировке.");
//    }
//    return std::wstring(buf.data(), buf.data() + r);
//}
//
//static std::string wide_to_mb_sys(const std::wstring& ws) {
//    std::setlocale(LC_CTYPE, "");
//    std::mbstate_t state{};
//    const wchar_t* src = ws.c_str();
//    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
//    if (len == (size_t)-1) {
//        throw std::runtime_error("Ошибка: не удалось преобразовать вывод в системную кодировку.");
//    }
//    std::vector<char> buf(len + 1);
//    src = ws.c_str();
//    size_t r = std::wcsrtombs(buf.data(), &src, buf.size(), &state);
//    if (r == (size_t)-1) {
//        throw std::runtime_error("Ошибка: не удалось преобразовать вывод в системную кодировку.");
//    }
//    return std::string(buf.data(), buf.data() + r);
//}
//
//// -----------------------------
//// Утилиты
//// -----------------------------
//static std::wstring trim_ws(const std::wstring& s) {
//    size_t i = 0;
//    while (i < s.size() && std::iswspace(s[i])) ++i;
//    size_t j = s.size();
//    while (j > i && std::iswspace(s[j - 1])) --j;
//    return s.substr(i, j - i);
//}
//
//static bool is_russian_lower(wchar_t c) {
//    // Включаем 'ё'
//    return (c >= L'а' && c <= L'я') || (c == L'ё');
//}
//
//// -----------------------------
//// ArgsParser
//// -----------------------------
//class ArgsParser {
//public:
//    static void printHelp() {
//        std::cout << "Использование:\n"
//            "program --mode Encrypt|Decrypt --offset N --text \"ваш текст\"\n\n"
//            "Параметры:\n"
//            "  --mode     Режим работы: Encrypt или Decrypt\n"
//            "  --offset   Сдвиг (целое число, допускаются отрицательные значения)\n"
//            "  --text     Текст для обработки (в кавычках), в системной кодировке (CP1251 на Windows RU)\n"
//            "  --help     Показать эту справку\n\n"
//            "Примечания:\n"
//            "  Сдвигаются только русские строчные буквы (включая 'ё') и цифры; пунктуация и пробелы остаются без изменений.\n";
//    }
//
//    static ProgramOptions parse(int argc, char* argv[]) {
//        if (argc < 2) {
//            throw std::runtime_error("Ошибка: параметры не заданы. Используйте --help для справки.");
//        }
//
//        const std::set<std::string> allowed = { "--mode", "--offset", "--text", "--help" };
//        std::unordered_map<std::string, std::string> args;
//        std::set<std::string> seen;
//
//        for (int i = 1; i < argc; ++i) {
//            std::string token = argv[i];
//
//            if (token == "--help") {
//                printHelp();
//                std::exit(0);
//            }
//
//            if (token.rfind("--", 0) != 0) {
//                // неожиданный аргумент (не ключ)
//                throw std::runtime_error(std::string("Ошибка: неожиданный аргумент '") + token + "'.");
//            }
//
//            // Проверка известного параметра
//            if (allowed.find(token) == allowed.end()) {
//                throw std::runtime_error("Ошибка: неизвестный параметр " + token + ".");
//            }
//
//            // Дублирование параметра
//            if (seen.count(token)) {
//                throw std::runtime_error("Ошибка: дублирование параметра " + token + ".");
//            }
//
//            // Должен быть следующий токен — значение
//            if (i + 1 >= argc) {
//                throw std::runtime_error("Ошибка: параметр " + token + " не имеет значения.");
//            }
//
//            std::string value = argv[i + 1];
//            if (value.rfind("--", 0) == 0) {
//                // следующий токен — другой ключ - значит у текущего параметра нет значения
//                throw std::runtime_error("Ошибка: параметр " + token + " не имеет значения.");
//            }
//
//            args[token] = value;
//            seen.insert(token);
//            ++i;  // пропускаем значение
//        }
//
//        // Проверка обязательных параметров
//        if (args.find("--mode") == args.end())
//            throw std::runtime_error("Ошибка: отсутствует обязательный параметр --mode.");
//        if (args.find("--offset") == args.end())
//            throw std::runtime_error("Ошибка: отсутствует обязательный параметр --offset.");
//        if (args.find("--text") == args.end())
//            throw std::runtime_error("Ошибка: отсутствует обязательный параметр --text.");
//
//        ProgramOptions opts;
//        opts.mode = args["--mode"];
//        if (opts.mode != "Encrypt" && opts.mode != "encrypt" && opts.mode != "Decrypt" && opts.mode != "decrypt") {
//            throw std::runtime_error("Ошибка: параметр --mode должен быть Encrypt или Decrypt.");
//        }
//
//        // Парсим offset (должен поддерживать отрицательные и большие по модулю значения)
//        const std::string& offStr = args["--offset"];
//        try {
//            // Используем stoll для поддержки больших/отрицательных значений
//            size_t pos = 0;
//            long long val = std::stoll(offStr, &pos, 10);
//            if (pos != offStr.size()) {
//                throw std::invalid_argument("bad");
//            }
//            opts.offset = val;
//        }
//        catch (const std::invalid_argument&) {
//            throw std::runtime_error("Ошибка: параметр --offset должен быть целым числом.");
//        }
//        catch (const std::out_of_range&) {
//            throw std::runtime_error("Ошибка: значение параметра --offset выходит за пределы допустимого диапазона.");
//        }
//
//        // Текстовый параметр — оставляем в байтовой форме (системная кодировка)
//        opts.text = args["--text"];
//
//        // Конвертируем в wide строку через системную локаль (CP1251 на русской Windows)
//        opts.wtext = mb_to_wide_sys(opts.text);
//
//        // Проверка пустого ввода (строка из одних пробельных символов считается пустой)
//        std::wstring trimmed = trim_ws(opts.wtext);
//        if (trimmed.empty()) {
//            throw std::runtime_error("Ошибка: пустой ввод текста.");
//        }
//
//        // Проверка на недопустимые символы
//        // Разрешаем русские строчные буквы (включая ё), цифры, пробельные символы, стандартную пунктуацию
//        std::wstring allowedPunct = L" .,!-;'()[]{}@#$%^&+=~`…—–";
//        std::set<wchar_t> badChars;
//        for (wchar_t wc : opts.wtext) {
//            if (is_russian_lower(wc)) continue;
//            if (std::iswdigit(wc)) continue;
//            if (std::iswspace(wc)) continue;
//            if (allowedPunct.find(wc) != std::wstring::npos) continue;
//            badChars.insert(wc);
//        }
//        if (!badChars.empty()) {
//            // Преобразуем каждый проблемный wchar_t в системную мультибайтовую последовательность для читабельности
//            std::string list;
//            bool first = true;
//            for (wchar_t ch : badChars) {
//                std::wstring tmp(1, ch);
//                std::string conv;
//                try {
//                    conv = wide_to_mb_sys(tmp);
//                }
//                catch (...) {
//                    // Если не получилось конвертировать символ, используем hex-представление
//                    char buf[32];
//                    std::snprintf(buf, sizeof(buf), "U+%04X", static_cast<unsigned int>(ch));
//                    conv = std::string(buf);
//                }
//                if (!first) list += ", ";
//                list += "'" + conv + "'";
//                first = false;
//            }
//            throw std::runtime_error("Ошибка: в тексте обнаружены недопустимые символы: " + list + ".");
//        }
//
//        return opts;
//    }
//};
//
//// -----------------------------
//// SubstitutionCipher (шаблон по контейнеру wchar_t)
//// -----------------------------
//template <typename Container>
//class SubstitutionCipher {
//    static_assert(std::is_same<typename Container::value_type, wchar_t>::value,
//        "SubstitutionCipher требует контейнер wchar_t (например std::wstring).");
//
//public:
//    explicit SubstitutionCipher(long long offset) : k(offset) {}
//
//    Container encrypt(const Container& input) const {
//        return transform(input, true);
//    }
//
//    Container decrypt(const Container& input) const {
//        return transform(input, false);
//    }
//
//private:
//    long long k;
//
//    Container transform(const Container& input, bool encrypt) const {
//        static const std::wstring alphabet = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
//        const int N = static_cast<int>(alphabet.size());  // 33
//        const int ND = 10;
//
//        // нормализуем сдвиг
//        int kk_letters = static_cast<int>(((k % N) + N) % N);
//        int kk_digits = static_cast<int>(((k % ND) + ND) % ND);
//
//        Container out;
//        out.reserve(input.size());
//
//        for (wchar_t ch : input) {
//            if (is_russian_lower(ch)) {
//                int pos = static_cast<int>(alphabet.find(ch));
//                if (pos == -1) {
//                    // на всякий случай — если символ не найден, копируем как есть
//                    out.push_back(ch);
//                }
//                else {
//                    int newPos = encrypt ? (pos + kk_letters) % N : (pos - kk_letters + N) % N;
//                    out.push_back(alphabet[newPos]);
//                }
//            }
//            else if (std::iswdigit(ch)) {
//                int pos = static_cast<int>(ch - L'0');
//                int newPos = encrypt ? (pos + kk_digits) % ND : (pos - kk_digits + ND) % ND;
//                out.push_back(static_cast<wchar_t>(L'0' + newPos));
//            }
//            else {
//                // пунктуация/пробелы без изменений
//                out.push_back(ch);
//            }
//        }
//        return out;
//    }
//};
//
//// -----------------------------
//// main
//// -----------------------------
//int main(int argc, char* argv[]) {
//    SetConsoleCP(1251);
//    SetConsoleOutputCP(1251);
//    try {
//        // Парсим аргументы
//        ProgramOptions opts = ArgsParser::parse(argc, argv);
//
//        // Выполняем шифрование/дешифрование на wide-строке
//        SubstitutionCipher<std::wstring> cipher(opts.offset);
//        std::wstring result_w = (opts.mode == "Encrypt" || opts.mode == "encrypt") ? cipher.encrypt(opts.wtext) : cipher.decrypt(opts.wtext);
//
//        // Конвертируем обратно и выводим (в системной кодировке)
//        std::string out = wide_to_mb_sys(result_w);
//        std::cout << out << std::endl;
//        return 0;
//
//    }
//    catch (const std::exception& ex) {
//        std::cerr << ex.what() << std::endl;
//        return 1;
//    }
//}
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <locale>
#include <clocale>
#include <cwctype>
#include <type_traits>
#include <limits>
#include <cstdlib>
#include <windows.h>

// -----------------------------
// ProgramOptions
// -----------------------------
struct ProgramOptions {
    std::string mode;        // Encrypt or Decrypt
    long long offset = 0;
    std::string text;        // исходная байтовая строка (в системной кодировке, напр. CP1251)
    std::wstring wtext;      // wide-строка для обработки
};

// -----------------------------
// Конвертация через системную локаль (CP1251 на русском Windows)
// -----------------------------
static std::wstring mb_to_wide_sys(const std::string& s) {
    // Устанавливаем локаль по умолчанию (системную).
    // В Windows это даст преобразование в CP1251 при русской локали.
    std::setlocale(LC_CTYPE, "");
    // Сначала определим длину
    std::mbstate_t state{};
    const char* src = s.c_str();
    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (len == (size_t)-1) {
        throw std::runtime_error("Ошибка: не удалось преобразовать входную строку в системной кодировке.");
    }
    std::vector<wchar_t> buf(len + 1);
    src = s.c_str();
    size_t r = std::mbsrtowcs(buf.data(), &src, buf.size(), &state);
    if (r == (size_t)-1) {
        throw std::runtime_error("Ошибка: не удалось преобразовать входную строку в системной кодировке.");
    }
    return std::wstring(buf.data(), buf.data() + r);
}

static std::string wide_to_mb_sys(const std::wstring& ws) {
    std::setlocale(LC_CTYPE, "");
    std::mbstate_t state{};
    const wchar_t* src = ws.c_str();
    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len == (size_t)-1) {
        throw std::runtime_error("Ошибка: не удалось преобразовать вывод в системную кодировку.");
    }
    std::vector<char> buf(len + 1);
    src = ws.c_str();
    size_t r = std::wcsrtombs(buf.data(), &src, buf.size(), &state);
    if (r == (size_t)-1) {
        throw std::runtime_error("Ошибка: не удалось преобразовать вывод в системную кодировку.");
    }
    return std::string(buf.data(), buf.data() + r);
}

// -----------------------------
// Утилиты
// -----------------------------
static std::wstring trim_ws(const std::wstring& s) {
    size_t i = 0;
    while (i < s.size() && std::iswspace(s[i])) ++i;
    size_t j = s.size();
    while (j > i && std::iswspace(s[j - 1])) --j;
    return s.substr(i, j - i);
}

static bool is_russian_lower(wchar_t c) {
    // Включаем 'ё'
    return (c >= L'а' && c <= L'я') || (c == L'ё');
}

// -----------------------------
// ArgsParser
// -----------------------------
class ArgsParser {
public:
    static void printHelp() {
        std::cout << "Использование:\n"
            "program --mode Encrypt|Decrypt --offset N --text \"ваш текст\"\n\n"
            "Параметры:\n"
            "  --mode     Режим работы: Encrypt или Decrypt\n"
            "  --offset   Сдвиг (целое число, допускаются отрицательные значения)\n"
            "  --text     Текст для обработки (в кавычках), в системной кодировке (CP1251 на Windows RU)\n"
            "  --help     Показать эту справку\n\n"
            "Примечания:\n"
            "  Сдвигаются только русские строчные буквы (включая 'ё') и цифры; пунктуация и пробелы остаются без изменений.\n";
    }

    static ProgramOptions parse(int argc, char* argv[]) {
        if (argc < 2) {
            throw std::runtime_error("Ошибка: параметры не заданы. Используйте --help для справки.");
        }

        const std::set<std::string> allowed = { "--mode", "--offset", "--text", "--help" };
        std::unordered_map<std::string, std::string> args;
        std::set<std::string> seen;

        for (int i = 1; i < argc; ++i) {
            std::string token = argv[i];

            if (token == "--help") {
                printHelp();
                std::exit(0);
            }

            if (token.rfind("--", 0) != 0) {
                // неожиданный аргумент (не ключ)
                throw std::runtime_error(std::string("Ошибка: неожиданный аргумент '") + token + "'.");
            }

            // Проверка известного параметра
            if (allowed.find(token) == allowed.end()) {
                throw std::runtime_error("Ошибка: неизвестный параметр " + token + ".");
            }

            // Дублирование параметра
            if (seen.count(token)) {
                throw std::runtime_error("Ошибка: дублирование параметра " + token + ".");
            }

            // Должен быть следующий токен — значение
            if (i + 1 >= argc) {
                throw std::runtime_error("Ошибка: параметр " + token + " не имеет значения.");
            }

            std::string value = argv[i + 1];
            if (value.rfind("--", 0) == 0) {
                // следующий токен — другой ключ - значит у текущего параметра нет значения
                throw std::runtime_error("Ошибка: параметр " + token + " не имеет значения.");
            }

            args[token] = value;
            seen.insert(token);
            ++i;  // пропускаем значение
        }

        // Проверка обязательных параметров
        if (args.find("--mode") == args.end())
            throw std::runtime_error("Ошибка: отсутствует обязательный параметр --mode.");
        if (args.find("--offset") == args.end())
            throw std::runtime_error("Ошибка: отсутствует обязательный параметр --offset.");
        if (args.find("--text") == args.end())
            throw std::runtime_error("Ошибка: отсутствует обязательный параметр --text.");

        ProgramOptions opts;
        opts.mode = args["--mode"];
        if (opts.mode != "Encrypt" && opts.mode != "encrypt" && opts.mode != "Decrypt" && opts.mode != "decrypt") {
            throw std::runtime_error("Ошибка: параметр --mode должен быть Encrypt или Decrypt.");
        }

        // Парсим offset (должен поддерживать отрицательные и большие по модулю значения)
        const std::string& offStr = args["--offset"];
        try {
            // Используем stoll для поддержки больших/отрицательных значений
            size_t pos = 0;
            long long val = std::stoll(offStr, &pos, 10);
            if (pos != offStr.size()) {
                throw std::invalid_argument("bad");
            }
            opts.offset = val;
        }
        catch (const std::invalid_argument&) {
            throw std::runtime_error("Ошибка: параметр --offset должен быть целым числом.");
        }
        catch (const std::out_of_range&) {
            throw std::runtime_error("Ошибка: значение параметра --offset выходит за пределы допустимого диапазона.");
        }

        // Текстовый параметр — оставляем в байтовой форме (системная кодировка)
        opts.text = args["--text"];

        // Конвертируем в wide строку через системную локаль (CP1251 на русской Windows)
        opts.wtext = mb_to_wide_sys(opts.text);

        // Проверка пустого ввода (строка из одних пробельных символов считается пустой)
        std::wstring trimmed = trim_ws(opts.wtext);
        if (trimmed.empty()) {
            throw std::runtime_error("Ошибка: пустой ввод текста.");
        }

        // Проверка на недопустимые символы
        // Разрешаем русские строчные буквы (включая ё), цифры, пробельные символы, стандартную пунктуацию
        std::wstring allowedPunct = L" .,!-;'()[]{}@#$%^&+=~`…—–";
        std::set<wchar_t> badChars;
        for (wchar_t wc : opts.wtext) {
            if (is_russian_lower(wc)) continue;
            if (std::iswdigit(wc)) continue;
            if (std::iswspace(wc)) continue;
            if (allowedPunct.find(wc) != std::wstring::npos) continue;
            badChars.insert(wc);
        }
        if (!badChars.empty()) {
            // Преобразуем каждый проблемный wchar_t в системную мультибайтовую последовательность для читабельности
            std::string list;
            bool first = true;
            for (wchar_t ch : badChars) {
                std::wstring tmp(1, ch);
                std::string conv;
                try {
                    conv = wide_to_mb_sys(tmp);
                }
                catch (...) {
                    // Если не получилось конвертировать символ, используем hex-представление
                    char buf[32];
                    std::snprintf(buf, sizeof(buf), "U+%04X", static_cast<unsigned int>(ch));
                    conv = std::string(buf);
                }
                if (!first) list += ", ";
                list += "'" + conv + "'";
                first = false;
            }
            throw std::runtime_error("Ошибка: в тексте обнаружены недопустимые символы: " + list + ".");
        }

        return opts;
    }
};

// -----------------------------
// InteractiveParser
// -----------------------------
class InteractiveParser {
public:
    static ProgramOptions parseInteractive() {
        ProgramOptions opts;

        while (true) {
            try {
                std::cout << "Введите параметры в формате: --mode Encrypt --offset 3 --text \"пример\"\n> ";

                std::string line;
                std::getline(std::cin, line);

                if (line.empty()) {
                    std::cout << "Ошибка: пустой ввод. Попробуйте снова.\n";
                    continue;
                }

                // Разбиваем строку на аргументы
                std::vector<std::string> args = parseCommandLine(line);

                // Добавляем имя программы как первый аргумент
                args.insert(args.begin(), "program");

                // Преобразуем в argc/argv формат
                std::vector<char*> argv;
                for (auto& arg : args) {
                    argv.push_back(&arg[0]);
                }

                // Используем существующий парсер
                opts = ArgsParser::parse(argv.size(), argv.data());
                break;
            }
            catch (const std::exception& ex) {
                std::cout << ex.what() << "\nПопробуйте снова.\n";
            }
        }

        return opts;
    }

private:
    static std::vector<std::string> parseCommandLine(const std::string& line) {
        std::vector<std::string> args;
        std::string current;
        bool inQuotes = false;
        bool inArg = false;

        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];

            if (c == '"') {
                inQuotes = !inQuotes;
                if (!inQuotes && !current.empty()) {
                    args.push_back(current);
                    current.clear();
                    inArg = false;
                }
                continue;
            }

            if (inQuotes) {
                current += c;
                continue;
            }

            if (std::isspace(c)) {
                if (inArg) {
                    args.push_back(current);
                    current.clear();
                    inArg = false;
                }
                continue;
            }

            current += c;
            inArg = true;
        }

        if (!current.empty()) {
            args.push_back(current);
        }

        return args;
    }
};

// -----------------------------
// SubstitutionCipher (шаблон по контейнеру wchar_t)
// -----------------------------
template <typename Container>
class SubstitutionCipher {
    static_assert(std::is_same<typename Container::value_type, wchar_t>::value,
        "SubstitutionCipher требует контейнер wchar_t (например std::wstring).");

public:
    explicit SubstitutionCipher(long long offset) : k(offset) {}

    Container encrypt(const Container& input) const {
        return transform(input, true);
    }

    Container decrypt(const Container& input) const {
        return transform(input, false);
    }

private:
    long long k;

    Container transform(const Container& input, bool encrypt) const {
        static const std::wstring alphabet = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
        const int N = static_cast<int>(alphabet.size());  // 33
        const int ND = 10;

        // нормализуем сдвиг
        int kk_letters = static_cast<int>(((k % N) + N) % N);
        int kk_digits = static_cast<int>(((k % ND) + ND) % ND);

        Container out;
        out.reserve(input.size());

        for (wchar_t ch : input) {
            if (is_russian_lower(ch)) {
                int pos = static_cast<int>(alphabet.find(ch));
                if (pos == -1) {
                    // на всякий случай — если символ не найден, копируем как есть
                    out.push_back(ch);
                }
                else {
                    int newPos = encrypt ? (pos + kk_letters) % N : (pos - kk_letters + N) % N;
                    out.push_back(alphabet[newPos]);
                }
            }
            else if (std::iswdigit(ch)) {
                int pos = static_cast<int>(ch - L'0');
                int newPos = encrypt ? (pos + kk_digits) % ND : (pos - kk_digits + ND) % ND;
                out.push_back(static_cast<wchar_t>(L'0' + newPos));
            }
            else {
                // пунктуация/пробелы без изменений
                out.push_back(ch);
            }
        }
        return out;
    }
};

// -----------------------------
// main
// -----------------------------
int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    try {
        ProgramOptions opts;

        // Если параметров нет - интерактивный режим
        if (argc == 1) {
            opts = InteractiveParser::parseInteractive();
        }
        else {
            // Иначе парсим аргументы командной строки
            opts = ArgsParser::parse(argc, argv);
        }

        // Выполняем шифрование/дешифрование на wide-строке
        SubstitutionCipher<std::wstring> cipher(opts.offset);
        std::wstring result_w = (opts.mode == "Encrypt" || opts.mode == "encrypt") ? cipher.encrypt(opts.wtext) : cipher.decrypt(opts.wtext);

        // Конвертируем обратно и выводим (в системной кодировке)
        std::string out = wide_to_mb_sys(result_w);
        std::cout << out << std::endl;
        return 0;

    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;

        // Если была ошибка при парсинге аргументов, предлагаем интерактивный режим
        if (argc > 1) {
            std::cout << "Хотите ввести параметры в интерактивном режиме? (y/n): ";
            std::string answer;
            std::getline(std::cin, answer);
            if (answer == "y" || answer == "Y" || answer == "д" || answer == "Д") {
                try {
                    ProgramOptions opts = InteractiveParser::parseInteractive();
                    SubstitutionCipher<std::wstring> cipher(opts.offset);
                    std::wstring result_w = (opts.mode == "Encrypt" || opts.mode == "encrypt") ? cipher.encrypt(opts.wtext) : cipher.decrypt(opts.wtext);
                    std::string out = wide_to_mb_sys(result_w);
                    std::cout << out << std::endl;
                    return 0;
                }
                catch (const std::exception& ex2) {
                    std::cerr << ex2.what() << std::endl;
                    return 1;
                }
            }
        }
        return 1;
    }
}