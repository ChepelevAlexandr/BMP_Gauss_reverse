#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

// ����������� ��������� ��� �������� ��������� BMP-�����
#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature;        // ��������� BMP ����� ('BM')
    uint32_t img_size;         // ������ ����� � ������
    uint32_t reserved;
    uint32_t data_offset;      // �������� �� ������ ������ �����������
    uint32_t header_size;      // ������ ��������� (������� ������ ���������)
    int32_t width;             // ������ ����������� � ��������
    int32_t height;            // ������ ����������� � ��������
    uint16_t color_planes;     // ���������� ���������� (������ ���� 1)
    uint16_t bits_per_pixel;   // ���������� ��� �� �������
    uint32_t compression;      // ��� ���������� (0 - ��� ����������)
    uint32_t image_size;       // ������ ����������� � ������
    int32_t x_pixels_per_meter; // �������������� ����������
    int32_t y_pixels_per_meter; // ������������ ����������
    uint32_t colors_used;      // ���������� ������������ ������
    uint32_t important_colors; // ���������� "������" ������
};
#pragma pack(pop)

// ����������� ��������� ��� �������� ���������� � �������
struct pixel_struct {
    uint8_t red;   // ������� ���������
    uint8_t green; // ������� ���������
    uint8_t blue;  // ����� ���������
};
// ������� ��� �������� ����������� �� 90 �������� ������ ������� �������
void rotate_left(vector<pixel_struct>& pix, BMPHeader& header) {
    vector<pixel_struct> new_pix(pix.size()); // ������� ����� ������ ��� �������� ���������� ��������
    int width = header.width; // �������� ������ ��������� �����������
    int height = header.height; // �������� ������ ��������� �����������

    // ����������� �������� ������ �������
    for (int y = 0; y < height; y++) { // �������� �� ������� ��������� �����������
        for (int x = 0; x < width; x++) { // �������� �� �������� ��������� �����������
            int new_x = height - y - 1; // ����� ���������� X ��� ����������� ������� (90 �������� ������ ������� �������)
            int new_y = x; // ����� ���������� Y ��� ����������� ������� (90 �������� ������ ������� �������)

            int original_index = y * width + x; // ������ �������� ������� � �������� �����������
            int new_index = new_y * height + new_x; // ������ ����������� ������� � ����� �������

            new_pix[new_index] = pix[original_index]; // �������� ������� �� ��������� ������� � ����� ������ � ������ ��������
        }
    }
    // ��������� ��������� ��������� ��� ������ �����
    header.width = height; // ������ ������ ��������� �� ����� ������
    header.height = width; // ������ ������ ��������� �� ����� ������

    // ��������� ������ �������� � ����������� ����������
    pix = new_pix; // ����������� ����� ������ �������� ��������� �������
}

// ������� ��� �������� ����������� �� 90 �������� �� ������� �������
void rotate_right(vector<pixel_struct>& pix, BMPHeader& header) {
    vector<pixel_struct> new_pix(pix.size()); // ������� ����� ������ ��� �������� ���������� ��������
    int width = header.width; // �������� ������ ��������� �����������
    int height = header.height; // �������� ������ ��������� �����������

    // ����������� �������� ������ �������
    for (int y = 0; y < height; y++) { // �������� �� ������� ��������� �����������
        for (int x = 0; x < width; x++) { // �������� �� �������� ��������� �����������
            int new_x = y; // ����� ���������� X ��� ����������� �������
            int new_y = width - x - 1; // ����� ���������� Y ��� ����������� �������

            int original_index = y * width + x; // ������ �������� ������� � �������� �����������
            int new_index = new_y * height + new_x; // ������ ����������� ������� � ����� �������

            new_pix[new_index] = pix[original_index]; // �������� ������� �� ��������� ������� � ����� ������ � ������ ��������
        }
    }
    // ��������� ��������� ��������� ��� ������ �����
    header.width = height; // ������ ������ ��������� �� ����� ������
    header.height = width; // ������ ������ ��������� �� ����� ������

    // ��������� ������ �������� � ����������� ����������
    pix = new_pix; // ����������� ����� ������ �������� ��������� �������
}

// ������� ��� ���������� �������� �� ������
void gauss(vector<pixel_struct>& pix, int width, int height, int radius) {
    vector<pixel_struct> blur(width * height); // ������� ����� ������ ��� �������� �������� ��������

    // ��������� �������� ����
    vector<double> kernel(2 * radius + 1); // ������� ������ ��� �������� ����
    double sigma = static_cast<double>(radius) / 3.0; // ��������� ����� ��� �������� �������
    double sum = 0.0; // �������������� ����� ��� ������������ ����

    for (int x = -radius; x <= radius; ++x) { // �������� �� ����������� ����
        double value = exp(-0.5 * (x * x) / (sigma * sigma)); // ��������� �������� �������� ����
        kernel[x + radius] = value; // ��������� �������� � ������� ����
        sum += value; // ��������� �������� ��� ������������
    }

    for (int i = 0; i < 2 * radius + 1; ++i) { // ����������� �������� ����
        kernel[i] /= sum;
    }

    // ��������� ��������
    for (int y = 0; y < height; ++y) { // �������� �� ������� �����������
        for (int x = 0; x < width; ++x) { // �������� �� �������� �����������
            double r = 0.0, g = 0.0, b = 0.0; // �������������� ���������� ����� ��� ������ �������
            int index = y * width + x; // ������ �������� �������

            for (int i = -radius; i <= radius; ++i) { // �������� �� ����������� �������� ����
                int neighborX = x + i; // ��������� ���������� ��������� ������� �� X
                if (neighborX >= 0 && neighborX < width) { // ���������, ��� �������� ������� � �������� �����������
                    int neighborIndex = y * width + neighborX; // ������ ��������� �������
                    double weight = kernel[i + radius]; // ��� ��������� ������� �� �������� ����

                    r += pix[neighborIndex].red * weight; // ��������� �������� � ���������� �������� �����
                    g += pix[neighborIndex].green * weight; // ��������� �������� � ���������� �������� �����
                    b += pix[neighborIndex].blue * weight; // ��������� �������� � ���������� ������ �����
                }
            }

            blur[index].red = static_cast<unsigned char>(r); // ��������� �������� ���������� �������� �����
            blur[index].green = static_cast<unsigned char>(g); // ��������� �������� ���������� �������� �����
            blur[index].blue = static_cast<unsigned char>(b); // ��������� �������� ���������� ������ �����
        }
    }
    // ��������� ������ �������� � ��������� ����������
    pix = blur; // ����������� ����� ������ �������� ��������� �������
}

int main() {
    setlocale(LC_ALL, "Russian");

    // ������������� ������
    cout << "������� �������� ��������� ����� (��� .bmp): ";
    string file_name;
    cin >> file_name;
    file_name = file_name + ".bmp";

    ifstream img(file_name, ios::binary);
    ofstream newbmp("newbmp.bmp", ios::binary | ios::trunc);

    // �������� �� ������������ �������� �����
    if (!img.is_open()) {
        cout << "�� ������� ������� ����!" << endl;
        return 0;
    }

    // ������ ��������� BMP-�����
    BMPHeader header;
    img.read(reinterpret_cast<char*>(&header), sizeof(header));

    int width = header.width;
    int height = header.height;
    char* garbage = new char[header.data_offset - 54];

    // �������� ������� ��� �������� ��������
    vector<pixel_struct> pix(width * height);

    img.read(reinterpret_cast<char*>(pix.data()), pix.size() * 3);
    img.read(reinterpret_cast<char*>(garbage), header.data_offset - 54);

    // ������ ���������� ������������
    char command1;
    cout << "������� ������� ��� �������� " << endl;
    cout << "1 - ������� �� 90 �������� ������, 2 - ������� �� 90 �������� �����, 0 - ��� �������� ";
    cin >> command1;

    if (command1 == '1')
        rotate_right(pix, header); // �������� ������� �������� ������, ���� ������� ��������������� ��������
    else if (command1 == '2')
        rotate_left(pix, header); // �������� ������� �������� �����, ���� ������� ��������������� ��������

    int radius;
    cout << "������� ���� �������� �� ������ " << endl;
    cout << "������� ����� ����� ��� ���� ���� �� ����� ";
    cin >> radius;

    if (radius != 0)
        gauss(pix, width, height, radius); // �������� ������� �������� �� ������, ���� ������ ������ ��������

    // ������ ������ � ����� ����
    newbmp.write(reinterpret_cast<char*>(&header), sizeof(header)); // ���������� ��������� BMP-�����
    newbmp.write(reinterpret_cast<char*>(garbage), header.data_offset - 54); // ���������� ������ ������������ ����� ���������� � �������
    newbmp.write(reinterpret_cast<char*>(pix.data()), pix.size() * 3); // ���������� ������ �������� (������ ������� �������� 3 �����)

    cout << "����������� �������� � ���� 'newbmp.bmp'." << endl;

    img.close(); // ��������� �������� ����
    newbmp.close(); // ��������� ����� ����

    return 0;
}