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

    bridge::byte_t byte[5] = {0x00, 0x01, 0x02, 0x03, 0x04};

    {
        // write file
        auto write_file = ram_dir.open_write(temp_file);

        write_file->write(byte, 5);

//        for (auto& value : float_vec) {
//            bridge::serialization::marshall(*write_file, value); // why does this not work?
//        }

        write_file->flush();

    }
    // read file
    auto read_file = ram_dir.open_read(temp_file);

    const bridge::byte_t* data_read = read_file->deref();
    auto length = read_file->size();

    ASSERT_EQ(length, 5);

    for (int i = 0; i < length; i++) {
        ASSERT_EQ((char)data_read[i], byte[i]);
    }
}
