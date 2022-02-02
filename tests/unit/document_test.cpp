#include <bridge.hpp>
#include <string>
#include <gtest/gtest.h>

TEST(DocumentTest, DocSemantics) {

    using namespace bridge::schema;

    // Create a document
    document doc;

    // Add a field
    doc.add_text(1, "Hello");

    // Add another field
    doc.add_u32(2, 42);

    // Check the number of fields
    ASSERT_EQ(2, doc.len());

    // Check the fields
    auto it = doc.get_fields();

    // Check the first field;
    auto text_field = document::get_field_value<std::string>(*it);
    ASSERT_EQ(1, text_field.get_id());
    ASSERT_EQ("Hello", *text_field.get_value());

    ++it;

    // If i don't know what type of field i'm looking at, i need to discover first
    ASSERT_THROW(document::get_field_value<std::string>(*it), bridge::bridge_error);

    // Check the second field
    if (document::holds_type<uint32_t>(*it)) {
        auto u32_field = document::get_field_value<uint32_t>(*it);
        ASSERT_EQ(2, u32_field.get_id());
        ASSERT_EQ(42, *u32_field.get_value());
    }
    else { ASSERT_TRUE(false);}

}

TEST(DocumentTest, UnorderedDocument) {

    using namespace bridge::schema;

    // Create a document
    document doc;

    // Add fields
    doc.add_text(3, "Hello");

    doc.add_u32(1, 42);

    doc.add_text(10, "World");

    doc.add_u32(2, 101);


    // Check the number of fields
    ASSERT_EQ(4, doc.len());

    // Ordered iterator
    auto ordered_iterator = doc.get_sorted_fields();

    // Check the first field;
    ASSERT_EQ(1, document::get_field_value<uint32_t>(*ordered_iterator++).get_id());
    ASSERT_EQ(2, document::get_field_value<uint32_t>(*ordered_iterator++).get_id());
    ASSERT_EQ(3, document::get_field_value<std::string>(*ordered_iterator++).get_id());
    ASSERT_EQ(10, document::get_field_value<std::string>(*ordered_iterator++).get_id());



}