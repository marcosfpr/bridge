#include "bridge/bridge.hpp"
#include <string>
#include <gtest/gtest.h>

TEST(TermTest, TestSemantics) {

    // Constructor
    using bridge::schema::term;

    term t1; // explicit default constructor

    // Let's suppose a simple textual term.  Thus, we need an id and a string
    std::string text = "Tax is a good thing";
    bridge::schema::id_t  id = 1;

    // Building from string
    term t2 = term::from_string(id, text);

    term t3 = t2; // copy assignment

    ASSERT_EQ(t3, t2);
    ASSERT_NE(t3, t1);

    // Building from uint32_t
    term t4 = term::from_uint32(id + 1, text.size()); // stores the size as i32

    ASSERT_TRUE(t4 > t2); // ?
    ASSERT_TRUE(t2 > t1); // ?

    size_t hash_t2 = t2.hash();
    size_t hash_t3 = t3.hash();
    size_t hash_t4 = t4.hash();

    ASSERT_EQ(hash_t2, hash_t3);    
    ASSERT_NE(hash_t2, hash_t4);

}

 TEST(TermTest, SchemaBuilderTerm) {
     using bridge::schema::term;

     bridge::schema::SchemaBuilder schema_builder;

     auto title_field = schema_builder.add_text_field("title", bridge::schema::STRING);
     auto count_field = schema_builder.add_text_field("count", bridge::schema::STRING);

     {
         term term = term::from_string(title_field, "Hello"); // first byte - id, rest - text
         ASSERT_EQ(term.get_field_id(), title_field);

         auto raw_bytes = term.as_ref();

         ASSERT_EQ(term.size(), 1 + 5);
         ASSERT_EQ(raw_bytes[0], static_cast<id_t>(0)); // CHECK

         ASSERT_EQ(raw_bytes[1], 'H');
         ASSERT_EQ(raw_bytes[2], 'e');
         ASSERT_EQ(raw_bytes[3], 'l');
         ASSERT_EQ(raw_bytes[4], 'l');
         ASSERT_EQ(raw_bytes[5], 'o');

     }

     {
         term term = term::from_uint32(count_field, 932); // first byte - id, rest - count
         ASSERT_EQ(term.get_field_id(), count_field);

         auto raw_bytes = term.as_ref();

         ASSERT_EQ(term.size(), 1 + 4);
         ASSERT_EQ(raw_bytes[0], static_cast<id_t>(1)); // CHECK

         ASSERT_EQ(raw_bytes[1], static_cast<bridge::byte_t>(0x00));
         ASSERT_EQ(raw_bytes[2], static_cast<bridge::byte_t>(0x00));
         ASSERT_EQ(raw_bytes[3], static_cast<bridge::byte_t>(0x03));
         ASSERT_EQ(raw_bytes[4], static_cast<bridge::byte_t>(0xA4));
     }

 }