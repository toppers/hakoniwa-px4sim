#ifndef _CSV_DATA_HPP_
#define _CSV_DATA_HPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class CsvData {
private:
    std::ofstream csv_file;
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

    // デストラクタでファイルを閉じる
    ~CsvData() {
        if (csv_file.is_open()) {
            csv_file.close();
        }
    }
};




#endif /* _CSV_DATA_HPP_ */