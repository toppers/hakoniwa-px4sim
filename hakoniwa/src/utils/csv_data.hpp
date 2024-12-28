#ifndef _CSV_DATA_HPP_
#define _CSV_DATA_HPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

class CsvData {
private:
    std::ofstream csv_file;
    std::ifstream csv_read_file;
    std::vector<std::vector<std::string>> data; // 全データを保持するためのベクタ
    size_t current_index = 0; // 現在の読み込み位置
public:
    // コンストラクタ
    CsvData(const std::string& file_name, const std::vector<std::string>& header) {
        csv_file.open(file_name, std::ios::out);
        if (!csv_file.is_open()) {
            std::cerr << "ファイルを開けません: " << file_name << std::endl;
            exit(1);
        }
        write(header);
    }

    // 読み込み用コンストラクタ
    CsvData(const std::string& file_name) {
        csv_read_file.open(file_name, std::ios::in);
        if (!csv_read_file.is_open()) {
            std::cerr << "ファイルを開けません: " << file_name << std::endl;
            exit(1);
        }
        // 全行を読み込む
        std::string line;
        while (std::getline(csv_read_file, line)) {
            std::vector<std::string> row;
            std::string cell;
            std::istringstream lineStream(line);
            while (std::getline(lineStream, cell, ',')) {
                row.push_back(cell);
            }
            data.push_back(row);
        }
        csv_read_file.close();
    }

    // データを書き込むメソッド
    void write(const std::vector<std::string>& value) {
        for (size_t i = 0; i < value.size(); ++i) {
            csv_file << value[i];
            if (i < value.size() - 1)
                csv_file << ",";
        }
        csv_file << "\n";
    }

    // ファイルのフラッシュ
    void flush() {
        csv_file.flush();
    }

    // 1行ずつ読み込むメソッド
    bool read(std::vector<std::string>& value) {
        if (current_index < data.size()) {
            value = data[current_index++];
            return true;
        }
        return false;
    }
    void remove_last() {
        if (!data.empty()) {
            data.pop_back();
        }
    }
    // デストラクタでファイルを閉じる
    ~CsvData() {
        if (csv_file.is_open()) {
            csv_file.close();
        }
        if (csv_read_file.is_open()) {
            csv_read_file.close();
        }
    }
};

#endif /* _CSV_DATA_HPP_ */
