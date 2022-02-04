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

#include <map>
#include <utility>

#include "bridge/schema/document.hpp"

namespace bridge::schema {

    /**
     * @brief Construct a new document object
     *
     */
    document::document() = default;

    /**
     * @brief Construct a new document object
     *
     * @param fields Field-Value tuples of any kind.
     */
    document::document(std::vector<field_v> &&fields) : fields_(std::move(fields)) {}

    /**
     * @brief Destroy the document and all its fields
     *
     */
    document::~document() = default;

    /**
     * @brief Equality operator
     *
     * @param other Other document
     * @return true If the documents are equal
     * @return false Otherwise
     */
    bool document::operator==(const document &other) const {
        return std::equal(
            fields_.begin(), fields_.end(), other.fields_.begin(), other.fields_.end(),
            [](const field_v &a, const field_v &b) {
                if (a.index() != b.index()) {
                    return false;
                }
                return std::visit(
                    [](auto &&va, auto &&vb) {
                        return va.get_id() == vb.get_id(); // shallow comparison
                    },
                    a, b
                );
            }
        );
    }

    /**
     * @brief Inequality operator
     *
     * @param other Other document
     * @return true If the documents are not equal
     * @return false Otherwise
     */
    bool document::operator!=(const document &other) const { return !(*this == other); }

    /**
     * @brief Returns the number of fields in the document
     *
     * @return size_t The number of fields
     */
    size_t document::len() const { return fields_.size(); }

    /**
     * @brief Adds a field to the document
     *
     * @param field_id Field ID
     * @param value String value
     */
    void document::add_text(id_t field_id, const std::string &value) { fields_.emplace_back(field(field_id, value)); }

    /**
     * @brief Adds a field to the document
     *
     * @param field_id Field ID
     * @param value uint32_t value
     */
    void document::add_u32(id_t field_id, uint32_t value) { fields_.emplace_back(field(field_id, value)); }

    /**
     * @brief Get the fields iterator.
     *
     * @return field_view Iterator over document's fields.
     */
    field_view document::get_fields() const { return fields_; }

    /**
     * @brief Get the sorted fields iterator.
     * @warning If you'll call  for the second time without changing the document, it
     * will probably be better to use the get_fields() method.
     *
     * @return field_by_id Iterator over document's fields.
     */
    field_by_id document::get_sorted_fields() {
        sort_by_id();

        std::map<id_t, std::vector<field_v>> sorted_fields;
        for (const auto &field : fields_) {
            sorted_fields[document::unwrap_field_id(field)].emplace_back(field);
        }

        // convert map to vector of pairs
        std::vector<std::pair<id_t, std::vector<field_v>>> sorted_fields_v(sorted_fields.begin(), sorted_fields.end());
        return sorted_fields_v;
    }

    /**
     * @brief Get all fields given a field_id.
     *
     * @param field_id Field id.
     * @return field_iterator Iterator over document's fields.
     */
    field_iterator document::get_all_by_id(id_t field_id) const {
        std::vector<field_v> fields_with_id;
        std::copy_if(fields_.begin(), fields_.end(), fields_with_id.begin(),
                     [field_id](const field_v &f) { return document::unwrap_field_id(f) == field_id; });
        return {fields_with_id.begin(), fields_with_id.end()};
    }

    /**
     * @brief Get all fields given a field_id.
     *
     * @param field_id Field id.
     * @return field_iterator Pair field_iterator. The first part points to the first and last field with the given
     * field_id. The second part points to the end of the iterator and it's useful  to compare wheter the field_id
     * exists or not.
     */
    field_iterator document::get_first_by_id(id_t field_id) const {
        return {std::find_if(fields_.begin(), fields_.end(),
                             [field_id](const field_v &f) { return document::unwrap_field_id(f) == field_id; }),
                fields_.end()};
    }

    void document::sort_by_id() {
        if(is_sorted_) return;
        std::sort(std::begin(fields_), std::end(fields_), [](const field_v &a, const field_v &b) {
            return document::unwrap_field_id(a) < document::unwrap_field_id(b);
        });
        is_sorted_ = true;
    }

} // namespace bridge::schema