// csvreader_gtest.cpp
// Тесты Google Test для программы csvreader

#include <gtest/gtest.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <fstream>

// Вспомогательная функция: запускает csvreader и возвращает его вывод
std::string RunCsvReader(const std::string& input_csv) {
    // Записываем input_csv во временный файл
    std::string tmp_filename = "test_input.csv";
    {
        std::ofstream ofs(tmp_filename);
        ofs << input_csv;
    }
    // Формируем команду
    std::string cmd = "./csvreader " + tmp_filename;
    std::array<char, 128> buffer;
    std::string result;
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    // удаляем временный файл
    std::remove(tmp_filename.c_str());
    return result;
}

// Тест 1: базовый пример из ТЗ
TEST(CsvReaderTest, BasicExample) {
    std::string input =
        ",A,B,Cell\n"
        "1,1,0,1\n"
        "2,2,=A1+Cell30,0\n"
        "30,0,=B1+A1,5\n";

    std::string expected =
        ",A,B,Cell\n"
        "1,1,0,1\n"
        "2,2,6,0\n"
        "30,0,1,5\n";

    EXPECT_EQ(RunCsvReader(input), expected);
}

// Тест 2: деление на ноль
TEST(CsvReaderTest, DivisionByZero) {
    std::string input =
        ",X,Y\n"
        "1,10,=X1-10\n"
        "2,5,=X1-10/0\n"; // второй рядка имеет деление на ноль

    std::string output = RunCsvReader(input);
    // в ячейке с делением на ноль ожидаем ERROR
    EXPECT_NE(output.find("ERROR"), std::string::npos);
}

// Тест 3: неверная ссылка на столбец
TEST(CsvReaderTest, InvalidColumnReference) {
    std::string input =
        ",A,B\n"
        "1,3,=C1+1\n"; // ссылка на C1 несуществует

    std::string output = RunCsvReader(input);
    EXPECT_NE(output.find("ERROR"), std::string::npos);
}

// Тест 4: вложенные формулы
TEST(CsvReaderTest, NestedFormulas) {
    std::string input =
        ",M,N\n"
        "1,2,=M1+1\n"
        "2,=M1+N1,=N2*2\n";

    std::string expected =
        ",M,N\n"
        "1,2,3\n"
        "2,5,6\n";

    EXPECT_EQ(RunCsvReader(input), expected);
}

// Тест 5: отрицательные числа и пробелы
TEST(CsvReaderTest, NegativeNumbersAndSpaces) {
    std::string input =
        ",A,B\n"
        "1, -5 , = A1 * -2 \n";

    std::string expected =
        ",A,B\n"
        "1,-5,10\n";

    EXPECT_EQ(RunCsvReader(input), expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
