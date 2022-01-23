#include <bridge.hpp>

#include <filesystem>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

TEST(SchemaOptionsTest, TextFieldOptions) {

    auto string_field = bridge::schema::STRING; // assignment operator

    auto another_string_field =
        bridge::schema::text_field(bridge::schema::text_indexing_option::Untokenized, false); // copy constructor

    ASSERT_EQ(string_field, another_string_field);
    ASSERT_FALSE(string_field.is_stored());

    auto string_index_options = string_field.get_indexing_options();

    ASSERT_EQ(string_index_options, bridge::schema::text_indexing_option::Untokenized);
    ASSERT_FALSE(string_index_options.is_termfreq_enabled());
    ASSERT_FALSE(string_index_options.is_position_enabled());
    ASSERT_FALSE(string_index_options.is_tokenized());

    auto text_field = bridge::schema::TEXT; // assignment operator
    ASSERT_FALSE(text_field.is_stored());

    auto text_index_options = text_field.get_indexing_options();
    ASSERT_TRUE(text_index_options.is_tokenized());
    ASSERT_TRUE(text_index_options.is_termfreq_enabled());
    ASSERT_TRUE(text_index_options.is_position_enabled());

    auto stored_field = bridge::schema::STORED; // assignment operator
    ASSERT_TRUE(stored_field.is_stored());

    auto stored_index_options = stored_field.get_indexing_options();
    ASSERT_FALSE(stored_index_options.is_tokenized());
    ASSERT_FALSE(stored_index_options.is_termfreq_enabled());
    ASSERT_FALSE(stored_index_options.is_position_enabled());

    ASSERT_NE(string_field, stored_field);
    ASSERT_NE(text_field, stored_field);
    ASSERT_NE(string_field, text_field);

    // operator |
    auto text_and_stored_field = bridge::schema::TEXT | bridge::schema::STORED;
    ASSERT_TRUE(text_and_stored_field.is_stored());

    auto text_and_stored_options = text_and_stored_field.get_indexing_options();
    ASSERT_TRUE(text_and_stored_options.is_tokenized());
    ASSERT_TRUE(text_and_stored_options.is_termfreq_enabled());
    ASSERT_TRUE(text_and_stored_options.is_position_enabled());
}

TEST(SchemaOptionsTest, SerializeTest) {
    auto string_field = bridge::schema::STRING; // assignment operator
    auto text_field = bridge::schema::TEXT;     // assignment operator
    auto stored_field = bridge::schema::STORED; // assignment operator

    // create temporary binary file
    std::string tmp_file_name = "./tmp_file";
    std::fstream tmp_file(tmp_file_name, std::ios::out | std::ios::binary);

    //  serialize to file
    {
        bridge::serialization::output_archive out(tmp_file);
        out << string_field;
        out << text_field;
        out << stored_field;
    }

    tmp_file.close();
    tmp_file.open(tmp_file_name, std::ios::in | std::ios::binary);

    // deserialize from file
    {
        bridge::serialization::input_archive in(tmp_file);

        bridge::schema::text_field deserialized_option;
        in >> deserialized_option;
        ASSERT_EQ(string_field, deserialized_option);

        in >> deserialized_option;
        ASSERT_EQ(text_field, deserialized_option);

        in >> deserialized_option;
        ASSERT_EQ(stored_field, deserialized_option);
    }

}

TEST(SchemaOptionsTest, MarshallTest) {
    // safe serialization
    auto string_field = bridge::schema::STRING; // assignment operator
    auto text_field = bridge::schema::TEXT;     // assignment operator
    auto stored_field = bridge::schema::STORED; // assignment operator

    // create temporary binary file
    std::string tmp_file_name = "./tmp_file";
    std::fstream tmp_file(tmp_file_name, std::ios::out | std::ios::binary);

    //  serialize to file
    unsigned long total_bytes_write = 0L;
    {
        using namespace bridge::serialization;

        total_bytes_write += marshall(tmp_file, string_field);
        total_bytes_write += marshall(tmp_file, text_field);
        total_bytes_write += marshall(tmp_file, stored_field);
    }

    ASSERT_EQ(total_bytes_write, sizeof(string_field) + sizeof(text_field) + sizeof(stored_field));

    tmp_file.close();
    tmp_file.open(tmp_file_name, std::ios::in | std::ios::binary);

    // deserialize from file
    {
        using namespace bridge::serialization;

        bridge::serialization::Serializable auto deserialized_option =
            unmarshall<bridge::schema::text_field>(tmp_file);

        ASSERT_EQ(string_field, deserialized_option);

        deserialized_option = unmarshall<bridge::schema::text_field>(tmp_file);
        ASSERT_EQ(text_field, deserialized_option);

        deserialized_option = unmarshall<bridge::schema::text_field>(tmp_file);
        ASSERT_EQ(stored_field, deserialized_option);

        ASSERT_ANY_THROW(unmarshall<bridge::schema::text_field>(tmp_file));
    }

    tmp_file.close();

    // remove temporary file
    std::remove(tmp_file_name.c_str());
}

TEST(SchemaOptionsTest, NumericOptions) {
    auto numeric_field = bridge::schema::FAST;

    auto another_numeric_field = bridge::schema::numeric_field(false, true, false);

    ASSERT_TRUE(numeric_field.is_fast());
    ASSERT_FALSE(numeric_field.is_indexed());
    ASSERT_FALSE(numeric_field.is_stored());

    ASSERT_EQ(numeric_field, another_numeric_field);

    numeric_field.set_fast(false);
    ASSERT_FALSE(numeric_field.is_fast());

    ASSERT_NE(numeric_field, another_numeric_field);
}

TEST(SchemaOptionsTest, NumericMarshall) {

    auto numeric_field = bridge::schema::FAST;

    // create temporary binary file
    std::string tmp_file_name = "./tmp_file";
    std::fstream tmp_file(tmp_file_name, std::ios::out | std::ios::binary);

    // serialize to file
    {
        using namespace bridge::serialization;
        ASSERT_EQ(sizeof(numeric_field), marshall(tmp_file, numeric_field));
    }

    tmp_file.close();
    tmp_file.open(tmp_file_name, std::ios::in | std::ios::binary);

    // deserialize to object

    {
        using namespace bridge::serialization;

        bridge::serialization::Serializable auto deserialized_numeric_field =
            unmarshall<bridge::schema::numeric_field>(tmp_file);
        ASSERT_EQ(numeric_field, deserialized_numeric_field);
    }

    tmp_file.close();
    // remove file
    std::remove(tmp_file_name.c_str());
}

TEST(SchemaOptionsTest, MarshallJSON) {
    auto numeric_field_ = bridge::schema::FAST;
    auto text_field_ = bridge::schema::TEXT;

    // create temporary json file
    std::string tmp_file_text = "./tmp_file.json";

    std::ofstream tmp_file_ofstream;
    tmp_file_ofstream.open(tmp_file_text);
    {
        // serialize to json
        auto s = bridge::serialization::marshall_json(tmp_file_ofstream, text_field_);
        ASSERT_EQ(s, sizeof(text_field_));
    }
    tmp_file_ofstream.close();

    // open for reading
    std::ifstream tmp_file_ifstream;
    tmp_file_ifstream.open(tmp_file_text);
    {
        // deserialize from json
        bridge::serialization::JSONSerializable auto from_json_field =
            bridge::serialization::unmarshall_json<bridge::schema::text_field>(tmp_file_ifstream);
        ASSERT_EQ(from_json_field, text_field_);
    }
    // close and remove temporary json file
    tmp_file_ifstream.close();
    std::remove(tmp_file_text.c_str());

    // create temporary json file
    std::string tmp_file_numeric = "./tmp_file.json";

    tmp_file_ofstream.open(tmp_file_numeric);
    {
        // serialize to json
        auto s = bridge::serialization::marshall_json(tmp_file_ofstream, numeric_field_);
        ASSERT_EQ(s, sizeof(numeric_field_));
    }
    tmp_file_ofstream.close();

    // open for reading
    tmp_file_ifstream.open(tmp_file_numeric);
    {
        // deserialize from json
        bridge::serialization::JSONSerializable auto from_json_field =
            bridge::serialization::unmarshall_json<bridge::schema::numeric_field>(tmp_file_ifstream);
        ASSERT_EQ(from_json_field, numeric_field_);
    }
    // close and remove temporary json file
    tmp_file_ifstream.close();
    std::remove(tmp_file_numeric.c_str());
}