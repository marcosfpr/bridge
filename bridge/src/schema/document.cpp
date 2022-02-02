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

#include "../../include/schema/document.hpp"

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
    bool document::operator==(const document &other) const { return fields_ == other.fields_; }

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
     * @brief Add a field to the document
     *
     * @tparam V Type of the value.
     * @param field Field  of type V.
     */
    template <FieldValue V> void document::add(field<V> field) { fields_.emplace_back(field); }

    /**
     * @brief Get the fields iterator.
     *
     * @return field_iterator Iterator over document's fields.
     */
    field_iterator document::get_fields() { return fields_.begin(); }

    /**
     * @brief Get the sorted fields iterator.
     * @warning If you'll call  for the second time without changing the document, it
     * will probably be better to use the get_fields() method.
     *
     * @return field_iterator Iterator over document's fields.
     */
    field_iterator document::get_sorted_fields() {
        // todo: can be optimized if is already sorted
        std::sort(fields_.begin(), fields_.end(), [](const field_v &a, const field_v &b) {
             return document::unwrap_field_id(a) < document::unwrap_field_id(b);
        });
        return fields_.begin();
    }

    /**
     * @brief Get all fields given a field_id.
     *
     * @param field_id Field id.
     * @return field_iterator Iterator over document's fields.
     */
    field_iterator document::get_all_by_id(id_t field_id) {
        return std::find_if(fields_.begin(), fields_.end(), [field_id](const field_v &f) {
            return document::unwrap_field_id(f) == field_id;
        });
    }

} // namespace bridge::schema