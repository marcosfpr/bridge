#include "bridge/bridge.hpp"
#include <string>
#include <filesystem>
#include <gtest/gtest.h>


template <class Device>
std::shared_ptr<bridge::schema::Schema> write_lorem_ipsum_store(bridge::directory::WriterPtr<Device> writer) {
    using namespace bridge::schema;

    SchemaBuilder schema_builder;

    auto field_body = schema_builder.add_text_field("body", text_field_option(text_indexing_option::Untokenized, true));
    auto field_title = schema_builder.add_text_field("title", text_field_option(text_indexing_option::Untokenized, true));

    auto schema = schema_builder.build();

    auto lorem = "Doc Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

    {
        auto store_writer = bridge::store::store_writer<Device>(std::move(writer), true);
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
    using namespace bridge::directory;

    // temporary path
    std::filesystem::path tmp_path =  std::filesystem::temp_directory_path() / "bridge_store_test";

    // create a RAMDirectory
    auto ram_directory = RAMDirectory();

    auto store_file = ram_directory.open_write(tmp_path);

    auto schema = write_lorem_ipsum_store<ArrayDevice>(std::move(store_file));

    auto field_title = schema->get_field_id("title");

    auto store_source = ram_directory.open_read(tmp_path);

    store_reader reader(std::move(store_source));

    for (int i = 0; i < 100; i+=5) {
        auto doc = reader.get(i);
        auto it = doc.get_first_by_id(field_title);
        auto title_field = *it.first;
        std::visit([&i](auto &&field) { ASSERT_EQ(field.get_value(), "Doc " + std::to_string(i)); }, title_field);
    }

}

// todo: benchmarks (timeit)