#include <bridge.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

TEST(NamedFieldDocumentTest, JSONSerialization) {
    using bridge::schema::field_value;
    using bridge::schema::string_value;
    using bridge::schema::uint32_value;

    bridge::schema::field_map fmap = {{"body", {field_value<std::string>::create("Hello, world!"), field_value<uint32_t>::create(32)} },
                                     {"title", {field_value<std::string>::create("That is weird!")}}};

    bridge::schema::named_field_document nfd(std::move(fmap));

    // create temporary file
    std::string tmp_file = "tmp_file.json";
    std::ofstream ofs(tmp_file);
    {
        // serialize to json
        auto s = bridge::serialization::marshall_json(ofs, nfd);
        ASSERT_EQ(s, sizeof(nfd));
    }
    ofs.close();

    // open for reading
    std::ifstream ifs;
    ifs.open(tmp_file);
    {
        // deserialize from json
        bridge::serialization::JSONSerializable auto from_json_entry =
            bridge::serialization::unmarshall_json<bridge::schema::named_field_document>(ifs);
        
        // compare two maps
        auto original_iterator = nfd.fmap.begin();
        auto deserialized_iterator = from_json_entry.fmap.begin();

        while(original_iterator != nfd.fmap.end()) {

            auto& [key, value] =  *original_iterator;
            auto& [key2, value2] =  *deserialized_iterator;

            EXPECT_EQ(key, key2);

            std::for_each(value.begin(), value.end(), [&](auto& v) {
                // check if v is in value2
                auto it = std::find_if(value2.begin(), value2.end(), [&](auto& v2) {
                    if (std::holds_alternative<string_value>(v) && std::holds_alternative<string_value>(v2)) {
                        return *std::get<string_value>(v) == *std::get<string_value>(v2);
                    }
                    else if (std::holds_alternative<uint32_value>(v) && std::holds_alternative<uint32_value>(v2)) {
                        return *std::get<uint32_value>(v) == *std::get<uint32_value>(v2);
                    }
                    else {
                        return false;
                    }
                });
                ASSERT_NE(it, value2.end());
            });

            original_iterator++;
            deserialized_iterator++;
        }
        
    }
    // close and remove temporary json file
    ifs.close();

    // ifs.open(tmp_file);

    // // print all file  buffer
    // std::stringstream ss;
    // ss << ifs.rdbuf();
    // std::cout << ss.str() << std::endl;

    // ifs.close();
    
    std::remove(tmp_file.c_str());
}