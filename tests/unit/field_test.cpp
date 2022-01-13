//
// Created by marcosfpr on 13/01/2022.
//
#include <bridge.hpp>

#include <iostream>
#include <string>

#include <gtest/gtest.h>

TEST(FieldTest, FieldType) {

    // Field type: stores information about fields.
    bridge::schema::field_type ft1 = bridge::schema::field_type(bridge::schema::TEXT); // constructor

    bridge::schema::field_type<bridge::schema::text_field> ft2; // default constructor

    EXPECT_EQ(ft1.is_text(), ft2.is_text());
    EXPECT_EQ(ft1.is_text(), true);
    EXPECT_EQ(ft1.is_numeric(), false);

    // Copy constructor
    {
        auto ft3 = ft1;
        auto ft4 = bridge::schema::field_type(ft1);
        EXPECT_EQ(ft3.get(), ft4.get());
    }

    // Move assignment
    bridge::schema::field_type ft3 = std::move(ft1);
    EXPECT_NE(ft3.get(), ft2.get());

    EXPECT_EQ(ft3.get().get_indexing_options().is_indexed(), true);
    EXPECT_EQ(ft2.get().get_indexing_options().is_indexed(), false);

    EXPECT_EQ(bridge::schema::TEXT, ft3.get());

    // Numeric field
    bridge::schema::field_type<bridge::schema::numeric_field> ft5;
    EXPECT_EQ(ft5.is_numeric(), true);
    EXPECT_EQ(ft5.is_text(), false);
    EXPECT_EQ(ft5.get().is_fast(), false);

    auto ft6 = bridge::schema::field_type(bridge::schema::FAST);

    EXPECT_EQ(ft6.get().is_fast(), true);
}

TEST(FieldTest, FieldEntry) {

    using namespace bridge::schema;
    {
        // Constructor
       field_entry fe1 = field_entry("title",  field_type(STRING));

       field_entry fe2 = fe1;
       field_entry fe3 = field_entry(fe2);

       EXPECT_EQ(fe1.name(), fe2.name());
       EXPECT_EQ(fe3.type(), fe2.type());

       auto fe4 = field_entry<text_field>::create("title", TEXT);

       EXPECT_EQ(fe1.name(), fe4.name());
       EXPECT_NE(fe1.type(), fe4.type());

       auto fe5 = field_entry<text_field>::create("title", STORED);

       EXPECT_EQ(fe1.is_indexed(), true);
       EXPECT_EQ(fe5.is_indexed(), false);

    }

   // Numeric fields
    {
        field_entry fe1 = field_entry("revenue", field_type(NUMERIC));
        field_entry fe3 = field_entry<numeric_field>::create(fe1.name(), FAST);

        EXPECT_EQ(fe1.is_indexed(), false);
        EXPECT_EQ(fe1.is_numeric_fast(), false);

        EXPECT_EQ(fe3.is_indexed(), false);
        EXPECT_EQ(fe3.is_numeric_fast(), true);

    }
}

TEST(FieldTest,  FieldValue) {

    using namespace bridge::schema;
    {
        // Constructor
        field_value fv1 = field_value<uint32_t>::create(23);
        field_value fv2 = field_value<std::string>::create("foo");

        {
            field_value fv3 = fv1;
            EXPECT_EQ(fv3.value(), fv1.value());
        }

        {
            field_value fv3 = fv2;
            field_value fv4 = std::move(fv3);
            EXPECT_EQ(fv4.value(), fv2.value());
        }

    }
}

TEST(FieldTest, Field) {
    using namespace bridge::schema;

    field f1 = field<unsigned int>(0, 23);
    field f2 = field<std::string>(1, "foo");
    EXPECT_NE(f1, f2);
    {
        // copy
        field f3 = f1;
        EXPECT_EQ(f3, f1);
    }

    {
        // move
        field f3 = f1;
        field f4 = std::move(f3);
        EXPECT_EQ(f4, f1);
    }

    ASSERT_TRUE(f1 < f2);
    ASSERT_TRUE(f2 > f1);
    ASSERT_TRUE(f1 <= f2);
    ASSERT_TRUE(f2 >= f1);

    EXPECT_EQ(*f1.get_value(), 23);
    EXPECT_EQ(*f2.get_value(), "foo");


    EXPECT_EQ(f1.hash(), field<unsigned int>(0, 1203).hash());

}