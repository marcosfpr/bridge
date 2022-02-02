// Copyright (c) 2021 Bridge Project (Marcos Pontes)

//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:

//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.

//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.

#ifndef SCHEMA_HPP_
#define SCHEMA_HPP_

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <map>

#include "./options.hpp"
#include "./field.hpp"
#include "./field_entry.hpp"
#include "./term.hpp"
#include "./document.hpp"
#include "./named_field_document.hpp"

// namespace bridge::schema {

    // /**
    //  * @brief In the Bridge project, schemas are represented as a collection of field entries.
    //  * @details For this first version, we will only support a two variants of field entries: string and integer.
    //  * @todo It must change in the future lol.
    //  */
    // typedef std::variant<field_entry<text_field>, field_entry<numeric_field>> bridge_field_entry;
    // typedef std::vector<bridge_field_entry>::iterator field_entry_iterator;

    // /**
    //  * @brief Currently, bridge has a very strict schema.
    //  * Like Tantivy library, you need to specify in advance, wheater a field is indexed or not, stored or not, etc.
    //  * 
    //  * This is done by creating a schema object, and setting up the fields one by one.
    //  * Currently is not possible to remove fields.
    //  */
    // class SchemaBuilder {
    //     public:

    //         /**
    //          * @brief Construct a new Schema Builder object
    //          * 
    //          */
    //         explicit SchemaBuilder();

    //         /**
    //          * @brief Destroy the Schema Builder object
    //          * 
    //          */ 
    //         virtual ~SchemaBuilder();

    //         /**
    //          * @brief Add a new field to the schema
    //          * 
    //          * @param name The name of the field
    //          * @param text_options The options of the text field
    //          * @return SchemaBuilder& A reference to the builder
    //          */
    //         SchemaBuilder& add_text_field(const std::string& name, const text_field text_options);

    //         /**
    //          * @brief Add a new field to the schema
    //          * 
    //          * @param name The name of the field
    //          * @param numeric_options The options of the numeric field
    //          * @return SchemaBuilder& A reference to the builder
    //          */
    //         SchemaBuilder& add_numeric_field(const std::string& name, const numeric_field numeric_options);

    //         /**
    //          * @brief Add a new field to the schema
    //          * 
    //          * @param name The name of the field
    //          * @param field_entry The field entry
    //          */
    //         SchemaBuilder& add_field(const std::string& name, const bridge_field_entry& field_entry);

    //         // /**
    //         //  * @brief Build the schema
    //         //  * 
    //         //  * @return std::shared_ptr<Schema> A shared pointer to the schema
    //         //  */
    //         // std::shared_ptr<Schema> build();


    //     private:
    //         std::vector<bridge_field_entry> field_schemas;
    //         std::map<std::string, id_t> field_names;
    // };

// }; // namespace bridge::schema

#endif // SCHEMA_HPP_