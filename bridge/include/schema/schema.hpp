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

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "./document.hpp"
#include "./field.hpp"
#include "./field_entry.hpp"
#include "./named_field_document.hpp"
#include "./options.hpp"
#include "./term.hpp"

namespace bridge::schema {

    typedef const std::vector<field_entry_v> &field_entry_view;

    class Schema {
      public:
        /**
         * @brief Construct a new Schema object
         *
         * @param field_schemas
         * @param field_names
         */
        explicit Schema(std::vector<field_entry_v> &&field_schemas, std::map<std::string, id_t> &&field_names);

        /**
         * @brief Destroy the Schema object
         *
         */
        virtual ~Schema();

        /**
         * @brief Get a field entry given a field id.
         *
         */
        [[nodiscard]] field_entry_v get_field_entry(id_t field_id) const;

        /**
         * @brief Get a field entry given a field id.
         *
         */
        [[nodiscard]] std::string get_field_name(id_t field_id) const;

        /**
         * @brief Get all field entries
         */
        [[nodiscard]] field_entry_view fields() const;

        /**
         * @brief Returns the field associated with a given name.
         */
        [[nodiscard]] id_t get_field_id(const std::string &field_name) const;

        /**
         * @brief Returns the named field document associated with a given document
         */
        [[nodiscard]] named_field_document to_named_doc(document &doc) const;

        /**
         * @brief Returns the document associated with the named field  document
         */
        [[nodiscard]] document from_named_doc(const named_field_document &nfd) const;

        /**
         * @brief Converts a Document type to a JSON.
         * @return A JSON object.
         */
        [[nodiscard]] serialization::json_t doc_to_json(document &doc) const;

        /**
         * @brief Converts a JSON to a Document type.
         * @param json JSON object.
         * @return Schema object.
         */
        [[nodiscard]] document doc_from_json(const serialization::json_t &json) const;

        /**
         * @brief Converts a Schema entry to a JSON object.
         * @return JSON object.
         */
        [[nodiscard]] serialization::json_t to_json() const;

        /**
         * @brief Converts a JSON object to a Schema entry.
         * @param json JSON object.
         * @return A Schema entry.
         */
        static Schema from_json(const serialization::json_t &json);

        /**
         * @brief Outputs the schema to a stream.
         *
         */
        friend std::ostream &operator<<(std::ostream &os, const Schema &schema);

      private:
        std::vector<field_entry_v> field_entries_;
        std::map<std::string, id_t> field_names_;
    };

    /**
     * @brief Currently, bridge has a very strict schema.
     * Like Tantivy library, you need to specify in advance, whether a field is indexed or not, stored or not, etc.
     *
     * This is done by creating a schema object, and setting up the fields one by one.
     * Currently is not possible to remove fields.
     */
    class SchemaBuilder {
      public:
        /**
         * @brief Construct a new Schema Builder object
         *
         */
        explicit SchemaBuilder();

        /**
         * @brief Destroy the Schema Builder object
         *
         */
        virtual ~SchemaBuilder();

        /**
         * @brief Add a new field to the schema
         *
         * @param name The name of the field
         * @param text_options The options of the text field
         * @return SchemaBuilder& A reference to the builder
         */
        SchemaBuilder &add_text_field(std::string &&name, text_field_option text_options);

        /**
         * @brief Add a new field to the schema
         *
         * @param name The name of the field
         * @param numeric_options The options of the numeric field
         * @return SchemaBuilder& A reference to the builder
         */
        SchemaBuilder &add_numeric_field(std::string &&name, numeric_field_option numeric_options);

        /**
         * @brief Add a new field to the schema
         *
         * @param field_entry The field entry
         */
        SchemaBuilder &add_field(std::string &&name, field_entry_v field_entry);

        /**
         * @brief Build the schema
         *
         * @return std::shared_ptr<Schema> A shared pointer to the schema
         */
        std::shared_ptr<Schema> build();

      private:
        std::vector<field_entry_v> field_entries_;
        std::map<std::string, id_t> field_names_;
    };

} // namespace bridge::schema

#endif // SCHEMA_HPP_