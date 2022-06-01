#include "bridge/bridge.hpp"
#include <string>
#include <filesystem>
#include <gtest/gtest.h>

using namespace bridge::directory;

std::shared_ptr<bridge::schema::Schema> write_lorem_ipsum_store(bridge::store::WriterPtr<ArrayDevice> writer) {
    using namespace bridge::schema;

    SchemaBuilder schema_builder;

    auto field_body = schema_builder.add_text_field("body", text_field_option(text_indexing_option::Untokenized, true));
    auto field_title = schema_builder.add_text_field("title", text_field_option(text_indexing_option::Untokenized, true));

    auto schema = schema_builder.build();

    auto lorem = "Doc Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut "
                 "labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris"
                 " nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit"
                 " esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt"
                 " in culpa qui officia deserunt mollit anim id est laborum.";

    {
        bridge::store::store_writer<ArrayDevice> store_writer(std::move(writer), true);
        for (int i = 0; i < 100; i++) {
            std::vector<field_v> fields;
            {
                auto field = text_field(field_body, lorem);
                fields.emplace_back(field);
            }
            {
                auto title_text = std::string("Doc ") + std::to_string(i);
                auto field = text_field(field_title, title_text);
                fields.emplace_back(field);
            }
            store_writer.store(fields);
        }
        store_writer.close();
    }
    return schema;
}

TEST(StoreTest, TestWriteRead) {

    using namespace bridge::store;

    // temporary path
    std::filesystem::path tmp_path =  std::filesystem::temp_directory_path() / "bridge_store_test";

    // create a RAMDirectory
    auto ram_directory = RAMDirectory();

    auto store_file = ram_directory.open_write(tmp_path);

    auto schema = write_lorem_ipsum_store(std::move(store_file));

    auto field_title = schema->get_field_id("title");

    auto store_source = ram_directory.source(tmp_path);
    std::cout << "Source with " << store_source->size() << " bytes" << std::endl;

    store_reader reader(std::move(store_source));

    for (int i = 0; i < 100; i+=5) {
        auto doc = reader.get(i);
        auto it = doc.get_first_by_id(field_title);
        field_v title_field = *it.first;
        if (std::holds_alternative<text_field>(title_field)) {
            text_field title_text_field = std::get<text_field>(title_field);
            string_value title_value = title_text_field.get_value();
            EXPECT_EQ(*title_value, std::string("Doc ") + std::to_string(i));
        }
        else  {
            ASSERT_TRUE(false);
        }
    }

}

TEST(SerializeTest,  WithDirectory) {
    using namespace bridge::store;

    // temporary path
    std::filesystem::path tmp_path =  std::filesystem::temp_directory_path() / "tmp_test";

    // delete the file if it exists
    if (std::filesystem::exists(tmp_path)) {
        std::filesystem::remove(tmp_path);
    }

    // create a RAMDirectory
    auto ram = RAMDirectory();

    // std::vector<bridge::byte_t> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::string data = "Hello, World!";
    {
        auto writer = ram.open_write(tmp_path);
        auto length =  bridge::serialization::marshall(*writer, data);
        ASSERT_TRUE(length > 0);
        writer->flush();
    }
    {
        auto reader = ram.open_read(tmp_path);
        auto deserialized = bridge::serialization::unmarshall<std::string>(*reader);
        ASSERT_EQ(deserialized.size(), data.size());
        reader->close();
    }

}

TEST(SerializeTest, WithoutDirectory) {
    using namespace bridge::serialization;
    // create temporary binary file

    std::vector<bridge::byte_t> buf;
    bridge::directory::ArrayDevice device(buf);
    bridge::directory::ArrayWriter writer(device);

    std::string foo("Hello, World!");
    size_t size = 0;
    {
        size = marshall(writer, foo);
        ASSERT_TRUE(size > 0);
    }
    writer.close();

    ASSERT_EQ(size, buf.size());
    bridge::directory::ArraySource source(buf.data(),  buf.size());
    bridge::directory::ArrayReader reader(source);

    {
        auto deserialized_value = unmarshall<std::string>(reader);
        ASSERT_EQ(deserialized_value, foo);
    }
}