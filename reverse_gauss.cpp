#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

// Определение структуры для хранения заголовка BMP-файла
#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature;        // Сигнатура BMP файла ('BM')
    uint32_t img_size;         // Размер файла в байтах
    uint32_t reserved;
    uint32_t data_offset;      // Смещение до начала данных изображения
    uint32_t header_size;      // Размер заголовка (включая данный структуру)
    int32_t width;             // Ширина изображения в пикселях
    int32_t height;            // Высота изображения в пикселях
    uint16_t color_planes;     // Количество плоскостей (должно быть 1)
    uint16_t bits_per_pixel;   // Количество бит на пиксель
    uint32_t compression;      // Тип компрессии (0 - без компрессии)
    uint32_t image_size;       // Размер изображения в байтах
    int32_t x_pixels_per_meter; // Горизонтальное разрешение
    int32_t y_pixels_per_meter; // Вертикальное разрешение
    uint32_t colors_used;      // Количество используемых цветов
    uint32_t important_colors; // Количество "важных" цветов
};
#pragma pack(pop)

// Определение структуры для хранения информации о пикселе
struct pixel_struct {
    uint8_t red;   // Красный компонент
    uint8_t green; // Зеленый компонент
    uint8_t blue;  // Синий компонент
};
// Функция для поворота изображения на 90 градусов против часовой стрелки
void rotate_left(vector<pixel_struct>& pix, BMPHeader& header) {
    vector<pixel_struct> new_pix(pix.size()); // Создаем новый вектор для хранения повернутых пикселей
    int width = header.width; // Получаем ширину исходного изображения
    int height = header.height; // Получаем высоту исходного изображения

    // Симметрично отражаем каждый пиксель
    for (int y = 0; y < height; y++) { // Проходим по строкам исходного изображения
        for (int x = 0; x < width; x++) { // Проходим по столбцам исходного изображения
            int new_x = height - y - 1; // Новая координата X для повернутого пикселя (90 градусов против часовой стрелки)
            int new_y = x; // Новая координата Y для повернутого пикселя (90 градусов против часовой стрелки)

            int original_index = y * width + x; // Индекс текущего пикселя в исходном изображении
            int new_index = new_y * height + new_x; // Индекс повернутого пикселя в новом векторе

            new_pix[new_index] = pix[original_index]; // Копируем пиксель из исходного вектора в новый вектор с учетом поворота
        }
    }
    // Обновляем параметры заголовка для нового файла
    header.width = height; // Меняем ширину заголовка на новую высоту
    header.height = width; // Меняем высоту заголовка на новую ширину

    // Обновляем массив пикселей с повернутыми значениями
    pix = new_pix; // Присваиваем новый вектор пикселей исходному вектору
}

// Функция для поворота изображения на 90 градусов по часовой стрелке
void rotate_right(vector<pixel_struct>& pix, BMPHeader& header) {
    vector<pixel_struct> new_pix(pix.size()); // Создаем новый вектор для хранения повернутых пикселей
    int width = header.width; // Получаем ширину исходного изображения
    int height = header.height; // Получаем высоту исходного изображения

    // Симметрично отражаем каждый пиксель
    for (int y = 0; y < height; y++) { // Проходим по строкам исходного изображения
        for (int x = 0; x < width; x++) { // Проходим по столбцам исходного изображения
            int new_x = y; // Новая координата X для повернутого пикселя
            int new_y = width - x - 1; // Новая координата Y для повернутого пикселя

            int original_index = y * width + x; // Индекс текущего пикселя в исходном изображении
            int new_index = new_y * height + new_x; // Индекс повернутого пикселя в новом векторе

            new_pix[new_index] = pix[original_index]; // Копируем пиксель из исходного вектора в новый вектор с учетом поворота
        }
    }
    // Обновляем параметры заголовка для нового файла
    header.width = height; // Меняем ширину заголовка на новую высоту
    header.height = width; // Меняем высоту заголовка на новую ширину

    // Обновляем массив пикселей с повернутыми значениями
    pix = new_pix; // Присваиваем новый вектор пикселей исходному вектору
}

// Функция для применения размытия по Гауссу
void gauss(vector<pixel_struct>& pix, int width, int height, int radius) {
    vector<pixel_struct> blur(width * height); // Создаем новый вектор для хранения размытых пикселей

    // Вычисляем Гауссово ядро
    vector<double> kernel(2 * radius + 1); // Создаем вектор для Гауссова ядра
    double sigma = static_cast<double>(radius) / 3.0; // Вычисляем сигму для Гауссова фильтра
    double sum = 0.0; // Инициализируем сумму для нормализации ядра

    for (int x = -radius; x <= radius; ++x) { // Проходим по координатам ядра
        double value = exp(-0.5 * (x * x) / (sigma * sigma)); // Вычисляем значение Гауссова ядра
        kernel[x + radius] = value; // Сохраняем значение в векторе ядра
        sum += value; // Суммируем значения для нормализации
    }

    for (int i = 0; i < 2 * radius + 1; ++i) { // Нормализуем Гауссово ядро
        kernel[i] /= sum;
    }

    // Применяем размытие
    for (int y = 0; y < height; ++y) { // Проходим по строкам изображения
        for (int x = 0; x < width; ++x) { // Проходим по столбцам изображения
            double r = 0.0, g = 0.0, b = 0.0; // Инициализируем компоненты цвета для нового пикселя
            int index = y * width + x; // Индекс текущего пикселя

            for (int i = -radius; i <= radius; ++i) { // Проходим по координатам Гауссова ядра
                int neighborX = x + i; // Вычисляем координату соседнего пикселя по X
                if (neighborX >= 0 && neighborX < width) { // Проверяем, что соседний пиксель в пределах изображения
                    int neighborIndex = y * width + neighborX; // Индекс соседнего пикселя
                    double weight = kernel[i + radius]; // Вес соседнего пикселя по Гауссову ядру

                    r += pix[neighborIndex].red * weight; // Применяем размытие к компоненте красного цвета
                    g += pix[neighborIndex].green * weight; // Применяем размытие к компоненте зеленого цвета
                    b += pix[neighborIndex].blue * weight; // Применяем размытие к компоненте синего цвета
                }
            }

            blur[index].red = static_cast<unsigned char>(r); // Сохраняем размытую компоненту красного цвета
            blur[index].green = static_cast<unsigned char>(g); // Сохраняем размытую компоненту зеленого цвета
            blur[index].blue = static_cast<unsigned char>(b); // Сохраняем размытую компоненту синего цвета
        }
    }
    // Обновляем массив пикселей с размытыми значениями
    pix = blur; // Присваиваем новый вектор пикселей исходному вектору
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Инициализация файлов
    cout << "Введите название исходного файла (без .bmp): ";
    string file_name;
    cin >> file_name;
    file_name = file_name + ".bmp";

    ifstream img(file_name, ios::binary);
    ofstream newbmp("newbmp.bmp", ios::binary | ios::trunc);

    // Проверка на корректность открытия файла
    if (!img.is_open()) {
        cout << "Не удалось открыть файл!" << endl;
        return 0;
    }

    // Чтение заголовка BMP-файла
    BMPHeader header;
    img.read(reinterpret_cast<char*>(&header), sizeof(header));

    int width = header.width;
    int height = header.height;
    char* garbage = new char[header.data_offset - 54];

    // Создание вектора для хранения пикселей
    vector<pixel_struct> pix(width * height);

    img.read(reinterpret_cast<char*>(pix.data()), pix.size() * 3);
    img.read(reinterpret_cast<char*>(garbage), header.data_offset - 54);

    // Логика интерфейса пользователя
    char command1;
    cout << "Введите команду для поворота " << endl;
    cout << "1 - поворот на 90 градусов вправо, 2 - поворот на 90 градусов влево, 0 - без поворота ";
    cin >> command1;

    if (command1 == '1')
        rotate_right(pix, header); // Вызываем функцию поворота вправо, если выбрано соответствующее действие
    else if (command1 == '2')
        rotate_left(pix, header); // Вызываем функцию поворота влево, если выбрано соответствующее действие

    int radius;
    cout << "Введите силу размытия по Гауссу " << endl;
    cout << "Введите целое число или ноль если не нужно ";
    cin >> radius;

    if (radius != 0)
        gauss(pix, width, height, radius); // Вызываем функцию размытия по Гауссу, если указан радиус размытия

    // Запись данных в новый файл
    newbmp.write(reinterpret_cast<char*>(&header), sizeof(header)); // Записываем заголовок BMP-файла
    newbmp.write(reinterpret_cast<char*>(garbage), header.data_offset - 54); // Записываем пустое пространство между заголовком и данными
    newbmp.write(reinterpret_cast<char*>(pix.data()), pix.size() * 3); // Записываем массив пикселей (каждый пиксель занимает 3 байта)

    cout << "Изображение записано в файл 'newbmp.bmp'." << endl;

    img.close(); // Закрываем исходный файл
    newbmp.close(); // Закрываем новый файл

    return 0;
}