#include "bridge/bridge.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include <gtest/gtest.h>


TEST(TestDirectory, TestRamDirectory) {

    using namespace bridge::directory;

    // create temporary directory
    std::filesystem::path temp_file = std::filesystem::temp_directory_path() / "test_ram_directory";

    RAMDirectory ram_dir;

    std::vector<float> data = {1.0f, 2.0f, 3.0f};
    char byte[1] = {0x01};

    {
        // write file
        auto write_file = ram_dir.open_write(temp_file);

        write_file->write(byte, 1);

        for (auto& d : data) {
            bridge::serialization::marshall(*write_file, d);
        }

        write_file->flush();

    }
    // read file
    auto read_file = ram_dir.open_read(temp_file);

    auto data_read = read_file->deref();
    auto length = read_file->size();

    ASSERT_EQ(length, data.size() * sizeof(float) + 1); // todo: aqui não funciona

    std::stringstream ss(std::string(reinterpret_cast<const char *>(data_read), length));

    char first_byte;
    ss >> first_byte;
    ASSERT_EQ(first_byte, (char)0x20ac);

    for (auto& d : data) {
        auto read_d = bridge::serialization::unmarshall<float>(ss); //NOLINT UnconstrainedVariableType
        ASSERT_EQ(d, read_d);
    }

}
