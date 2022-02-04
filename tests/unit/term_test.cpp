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

// todo
// TEST(TermTest, SchemaBuilderTerm) {

// }