#include <gtest/gtest.h>
#include <libjson/json.hpp>
#include <filesystem>

std::string read_file(const std::string &fileName){
    std::fstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "cannot open file";
        std::abort();
    }

    std::string buffer = "";

    while (file) {
        std::string line;

        std::getline(file, line);

        line += "\n";

        buffer += line;
    }

    return buffer;
}

TEST (test, json_test) {
    std::string tests_directory = "json files/";

    for(const auto &file : std::filesystem::directory_iterator(tests_directory)) {
        std::string file_name = file.path().string().substr(file.path().string().find_last_of("/") + 1);
        std::string text = read_file(file.path().string());

        if (file_name.find("pass") != std::string::npos) {
            EXPECT_NO_THROW({
                auto json = js::read(text);
            });
        }
        else {
            EXPECT_ANY_THROW({
                auto json = js::read(text);
            });
        }
    }
}
