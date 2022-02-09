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

#include <utility> // visit

#include "bridge/schema/schema.hpp"

namespace bridge::schema {

    /**
     * @brief Construct a new Schema Builder object
     *
     */
    SchemaBuilder::SchemaBuilder() = default;

    /**
     * @brief Destroy the Schema Builder object
     *
     */
    SchemaBuilder::~SchemaBuilder() = default;

    /**
     * @brief Add a new field to the schema
     *
     * @param name The name of the field
     * @param text_options The options of the text field
     * @return Id attributed to the field
     */
    id_t SchemaBuilder::add_text_field(std::string &&name, text_field_option text_options) {
        // create text field entry
        field_entry<text_field_option> text_field_entry =
            field_entry<text_field_option>::create(name, std::move(text_options));
        field_entry_v new_field(text_field_entry);
        return add_field(std::move(name), new_field);
    }

    /**
     * @brief Add a new field to the schema
     *
     * @param name The name of the field
     * @param numeric_options The options of the numeric field
     * @return Id attributed to the field
     */
    id_t SchemaBuilder::add_numeric_field(std::string &&name, numeric_field_option numeric_options) {
        // create numeric field entry
        field_entry<numeric_field_option> numeric_field_entry =
            field_entry<numeric_field_option>::create(name, std::move(numeric_options));
        field_entry_v new_field(numeric_field_entry);
        return add_field(std::move(name), new_field);
    }

    /**
     * @brief Add a new field to the schema
     *
     * @param name The name of the field
     * @param field_entry The field entry
     */
    id_t SchemaBuilder::add_field(std::string &&name, field_entry_v field_entry) {
        // add field entry to schema
        id_t field_id = field_entries_.size();
        field_entries_.push_back(std::move(field_entry));
        field_names_.insert({std::move(name), field_id});
        return field_id;
    }

    /**
     * @brief Build the schema
     *
     * @return std::shared_ptr<Schema> A shared pointer to the schema
     */
    std::shared_ptr<Schema> SchemaBuilder::build() {
        return std::make_shared<Schema>(std::move(field_entries_), std::move(field_names_));
    }

    /**
     * @brief Construct a new Schema object
     *
     * @param field_schemas
     * @param field_names
     */
    Schema::Schema(std::vector<field_entry_v> &&field_schemas, std::map<std::string, id_t> &&field_names)
        : field_entries_(std::move(field_schemas)), field_names_(std::move(field_names)) {}

    /**
     * @brief Destroy the Schema object
     *
     */
    Schema::~Schema() = default;

    /**
     * @brief Get a field entry given a field id.
     *
     */
    field_entry_v Schema::get_field_entry(id_t field_id) const { return field_entries_[field_id]; }

    /**
     * @brief Get a field entry given a field id.
     *
     */
    std::string Schema::get_field_name(id_t field_id) const {
        return std::visit([](auto &&field_entry) { return field_entry.name(); }, get_field_entry(field_id));
    }

    /**
     * @brief Get all field entries
     */
    field_entry_view Schema::fields() const { return field_entries_; }

    /**
     * @brief Returns the field associated with a given name.
     */
    id_t Schema::get_field_id(const std::string &field_name) const {
        auto it = field_names_.find(field_name);
        if (it == field_names_.end()) {
            throw bridge::bridge_error("Field name not found");
        }
        return it->second;
    }

    /**
     * @brief Returns the named field document associated with a given document
     */
    named_field_document Schema::to_named_doc(document &doc) const {
        named_field_document named_doc;
        auto sorted_fields = doc.get_sorted_fields();
        for (const auto &[field, values] : sorted_fields) {
            std::string field_name = get_field_name(field);
            std::vector<field_value_v> field_values;
            for (const auto &value : values) {
                std::visit([&field_values](auto &&value) { field_values.push_back(value.get_value()); }, value);
            }
            named_doc.fields_by_name.insert({field_name, field_values});
        }
        return named_doc;
    }

    /**
     * @brief Returns the document associated with the named field  document
     * @warning It does not insert the fields in the document following the field_id order.
     */
    document Schema::from_named_doc(const named_field_document &nfd) const {

        document doc;

        for (const auto &[field_name, values] : nfd.fields_by_name) {
            id_t field_id = get_field_id(field_name);
            for (const auto &value : values) {
                if (std::holds_alternative<string_value>(value)) {
                    doc.add<std::string>(field<std::string>::from_value(field_id, std::get<string_value>(value)));
                } else if (std::holds_alternative<uint32_value>(value)) {
                    doc.add<uint32_t>(field<uint32_t>::from_value(field_id, std::get<uint32_value>(value)));
                } else
                    throw bridge::bridge_error("Unsupported field type");
            }
        }

        doc.sort_by_id();

        return doc;
    }

    /**
     * @brief Converts a Schema entry to a JSON object.
     * @return JSON object.
     */
    [[nodiscard]] serialization::json_t Schema::to_json() const {

        serialization::json_t schema_json;

        try {
            schema_json["fields"] = serialization::json_t::array();
            for (const auto &field_entry : field_entries_) {
                serialization::json_t field_json;
                std::visit([&field_json](auto &&fe) { field_json = fe.to_json(); }, field_entry);
                schema_json["fields"].push_back(field_json);
            }
        } catch (const std::exception &e) {
            throw bridge::bridge_error(e.what());
        }

        return schema_json;
    }

    /**
     * @brief Converts a JSON object to a Schema entry.
     * @param json JSON object.
     * @return A Schema entry.
     */
    [[maybe_unused]] Schema Schema::from_json(const serialization::json_t &json) {
        SchemaBuilder builder;

        try {
            for (const auto &field_json : json["fields"].get<serialization::json_t::array_t>()) {
                auto get_entry = [](auto &&fj) -> field_entry_v {
                    try {
                        std::string field_type = fj["type"]["field"];
                        if (field_type == "text") {
                            field_entry<text_field_option> entry = field_entry<text_field_option>::from_json(fj);
                            return entry;
                        } else if (field_type == "numeric") {
                            field_entry<numeric_field_option> entry = field_entry<numeric_field_option>::from_json(fj);
                            return entry;
                        } else {
                            throw bridge::bridge_error("Unsupported field type");
                        }
                    }
                    catch (const std::exception &e) {
                        throw bridge::bridge_error(e.what());
                    }
                };
                field_entry_v entry(get_entry(field_json));
                std::string field_name = std::visit([](auto &&fe) { return fe.name(); }, entry);
                builder.add_field(std::move(field_name), entry);
            }
        } catch (const std::exception &e) {
            throw bridge::bridge_error(e.what());
        }

        return *builder.build();
    }

    /**
     * @brief Converts a document type to a JSON.
     * @return A JSON object.
     */
    [[nodiscard]] serialization::json_t Schema::doc_to_json(document &doc) const { return to_named_doc(doc).to_json(); }

    /**
     * @brief Converts a JSON to a document type.
     * @param json JSON object.
     * @return document object.
     */
    document Schema::doc_from_json(const serialization::json_t &json) const {
        return from_named_doc(named_field_document::from_json(json));
    }

    /**
     * @brief Outputs the schema to a stream.
     *
     */
    std::ostream &operator<<(std::ostream &os, const Schema &schema) {
        os << schema.to_json();
        return os;
    }

} // namespace bridge::schema