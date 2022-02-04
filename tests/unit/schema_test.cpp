#include <bridge.hpp>

#include <filesystem>
#include <iostream>
#include <string>

#include <gtest/gtest.h>

TEST(SchemaTest, SchemaSerialization) {
    using bridge::schema::SchemaBuilder;
    using bridge::schema::Schema;

    SchemaBuilder builder;

    bridge::schema::numeric_field_option count_option;
    count_option.set_stored(true);
    count_option.set_fast(true);

    builder.add_numeric_field("count", count_option);
    builder.add_text_field("title", bridge::schema::TEXT);
    builder.add_text_field("author", bridge::schema::TEXT);

    std::shared_ptr<Schema> schema = builder.build();

    // store stream buffer
     std::stringstream ss;
     ss << schema->to_json().dump(4);

     // raw string
    std::string expected  = "{\n"
                            "    \"fields\": [\n"
                            "        {\n"
                            "            \"name\": \"count\",\n"
                            "            \"type\": {\n"
                            "                \"field\": \"numeric\",\n"
                            "                \"options\": {\n"
                            "                    \"indexed\": false,\n"
                            "                    \"fast\": true,\n"
                            "                    \"stored\": true\n"
                            "                }\n"
                            "            }\n"
                            "        },\n"
                            "        {\n"
                            "            \"name\": \"title\",\n"
                            "            \"type\": {\n"
                            "                \"field\": \"text\",\n"
                            "                \"options\": {\n"
                            "                    \"indexing\": \"tokenized_with_freq_and_position\",\n"
                            "                    \"stored\": false\n"
                            "                }\n"
                            "            }\n"
                            "        },\n"
                            "        {\n"
                            "            \"name\": \"author\",\n"
                            "            \"type\": {\n"
                            "                \"field\": \"text\",\n"
                            "                \"options\": {\n"
                            "                    \"indexing\": \"tokenized_with_freq_and_position\",\n"
                            "                    \"stored\": false\n"
                            "                }\n"
                            "            }\n"
                            "        }\n"
                            "    ]\n"
                            "}";

    ASSERT_EQ(expected, ss.str());

    Schema schema_from_json = Schema::from_json(nlohmann::json::parse(expected));
    ASSERT_EQ(schema->to_json(), schema_from_json.to_json());

}

TEST(SchemaTest, DocumentJSON) {

    using bridge::schema::SchemaBuilder;
    using bridge::schema::Schema;
    using bridge::schema::document;

    SchemaBuilder builder;

    bridge::schema::numeric_field_option count_option;
    count_option.set_stored(true);
    count_option.set_fast(true);

    builder.add_numeric_field("count", count_option); //0
    builder.add_text_field("title", bridge::schema::TEXT); //1
    builder.add_text_field("author", bridge::schema::TEXT); //2

    std::shared_ptr<Schema> schema = builder.build();

    // agnostic of the order of the fields
    std::string raw_doc = "{\n"
                          "    \"author\": [\n"
                          "        \"World\"\n"
                          "    ],\n"
                          "    \"count\": [\n"
                          "        42\n"
                          "    ],\n"
                          "    \"title\": [\n"
                          "        \"Hello\"\n"
                          "    ]\n"
                          "}";

    document doc = schema->doc_from_json(nlohmann::json::parse(raw_doc));
    document doc2 = schema->doc_from_json(schema->doc_to_json(doc));

    ASSERT_TRUE(doc.len() == doc2.len());
    ASSERT_TRUE(doc.len() == 3);

    ASSERT_EQ(doc, doc2);

}