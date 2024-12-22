#include "logger/impl/csv_log_file.hpp"
#include "logger/impl/hako_logger.hpp"
#include "logger/ilog.hpp"
#include <gtest/gtest.h>
#include <filesystem> // For std::filesystem::remove
#include <vector>
#include <string>

const std::string TEMP_FILE = "test_hako_logger.csv";
const std::string TEMP_FILE1 = "test_hako_logger1.csv";
const std::string TEMP_FILE2 = "test_hako_logger2.csv";

// テスト用ヘッダーとデータ
std::vector<hako::logger::LogHeaderType> header = {
    {"Time", hako::logger::LogType::LOG_TYPE_UINT64},
    {"Value", hako::logger::LogType::LOG_TYPE_FLOAT},
    {"Status", hako::logger::LogType::LOG_TYPE_STRING}
};

std::vector<hako::logger::LogDataType> sample_data1 = {
    123456789ULL, 1.23f, std::string("OK")
};

std::vector<hako::logger::LogDataType> sample_data2 = {
    987654321ULL, 4.56f, std::string("NG")
};

// モッククラス
class MockLog : public hako::logger::ILog {
public:
    explicit MockLog(std::vector<hako::logger::LogDataType> data) : data_(data) {}

    std::vector<hako::logger::LogHeaderType>& log_head() override {
        return header;
    }

    std::vector<hako::logger::LogDataType>& log_data() override {
        return data_;
    }

private:
    std::vector<hako::logger::LogDataType> data_;
};

// テストケース: 書き込みと実行の確認
TEST(HakoLoggerTest, AddEntryAndRun) {
    hako::logger::HakoLogger logger;

    // モックデータを準備
    MockLog mock_log1(sample_data1);
    MockLog mock_log2(sample_data2);

    // CsvLogFileを生成してエントリに追加
    auto csv_file1 = std::make_unique<hako::logger::CsvLogFile>(TEMP_FILE1, header);
    logger.add_entry(mock_log1, std::move(csv_file1));
    auto csv_file2 = std::make_unique<hako::logger::CsvLogFile>(TEMP_FILE2, header);
    logger.add_entry(mock_log2, std::move(csv_file2));

    // ロガーを有効化して実行
    hako::logger::HakoLogger::enable();

    logger.run();
    logger.flush();

    // CsvLogFileを読み込んで確認 (1つ目のデータ)
    hako::logger::CsvLogFile csv_reader1(TEMP_FILE1);
    std::vector<hako::logger::LogDataType> read_data1;
    ASSERT_TRUE(csv_reader1.read(read_data1));

    // デバッグ用: 読み取ったデータを出力
    std::cout << "Read Data (string): " << std::get<std::string>(read_data1[0]) << std::endl;

    // データを型変換して検証
    ASSERT_EQ(std::stoull(std::get<std::string>(read_data1[0])), std::get<uint64_t>(sample_data1[0]));
    ASSERT_FLOAT_EQ(std::stof(std::get<std::string>(read_data1[1])), std::get<float>(sample_data1[1]));
    ASSERT_EQ(std::get<std::string>(read_data1[2]), std::get<std::string>(sample_data1[2]));

    // CsvLogFileを読み込んで確認 (2つ目のデータ)
    hako::logger::CsvLogFile csv_reader2(TEMP_FILE2);
    std::vector<hako::logger::LogDataType> read_data2;
    ASSERT_TRUE(csv_reader2.read(read_data2));

    // デバッグ用: 読み取ったデータを出力
    std::cout << "Read Data (string): " << std::get<std::string>(read_data2[0]) << std::endl;

    // データを型変換して検証
    ASSERT_EQ(std::stoull(std::get<std::string>(read_data2[0])), std::get<uint64_t>(sample_data2[0]));
    ASSERT_FLOAT_EQ(std::stof(std::get<std::string>(read_data2[1])), std::get<float>(sample_data2[1]));
    ASSERT_EQ(std::get<std::string>(read_data2[2]), std::get<std::string>(sample_data2[2]));
}


// テストケース: リセットとクローズの確認
TEST(HakoLoggerTest, ResetAndClose) {
    hako::logger::HakoLogger logger;

    // モックデータを準備
    MockLog mock_log(sample_data1);

    // CsvLogFileを生成してエントリに追加
    auto csv_file = std::make_unique<hako::logger::CsvLogFile>(TEMP_FILE, header);
    logger.add_entry(mock_log, std::move(csv_file));

    // ログを書き込み
    hako::logger::HakoLogger::enable();
    logger.run();

    // リセット
    logger.reset();

    // リセット後にデータが消えていることを確認
    hako::logger::CsvLogFile csv_reader(TEMP_FILE);
    std::vector<hako::logger::LogDataType> read_data;
    EXPECT_FALSE(csv_reader.read(read_data)); // データがないのでfalse

    // クローズ
    logger.close();

    // エントリが空であることを確認
    EXPECT_EQ(logger.get_entries().size(), 0);
}

// テスト後にファイルを削除するヘルパークラス
class HakoLoggerTestCleanup : public ::testing::Test {
protected:
    void TearDown() override {
        if (std::filesystem::exists(TEMP_FILE)) {
            std::filesystem::remove(TEMP_FILE);
        }
        if (std::filesystem::exists(TEMP_FILE1)) {
            std::filesystem::remove(TEMP_FILE1);
        }
        if (std::filesystem::exists(TEMP_FILE2)) {
            std::filesystem::remove(TEMP_FILE2);
        }
    }
};

// テスト終了後のファイル削除を確認
TEST_F(HakoLoggerTestCleanup, CleanupTestFile) {
    // テスト用の一時ファイルを作成
    std::ofstream ofs(TEMP_FILE);
    ofs << "Temporary data";
    ofs.close();

    // テスト終了後にTearDownが実行され、ファイルが削除される
    TearDown();

    // ファイルが存在しないことを確認
    EXPECT_FALSE(std::filesystem::exists(TEMP_FILE));
}
