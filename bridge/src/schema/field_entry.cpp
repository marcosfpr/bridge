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

#include "../../include/schema/field_entry.hpp"

#include <utility>

namespace bridge::schema {

    /**
     * @brief Explicit Constructor.
     * @param type Object of the field's type.
     */
    template <FieldType T> field_type<T>::field_type(T type) : _type(std::move(type)) {}

    /**
     * @brief Default Constructor.
     */

    template <FieldType T> field_type<T>::field_type() = default;

    /**
     * @brief Destructor.
     */
    template <FieldType T> field_type<T>::~field_type() noexcept = default;

    /**
     * @brief Copy Constructor.
     * @param other Object to be copied.
     */
    template <FieldType T> field_type<T>::field_type(const field_type &) = default;

    /**
     * @brief Move Constructor.
     * @param other Object to be moved.
     */
    template <FieldType T> field_type<T>::field_type(field_type &&) noexcept = default;

    /**
     * @brief Copy Assignment Operator.
     * @param other Object to be copied.
     * @return Reference to the object.
     */
    template <FieldType T> field_type<T> &field_type<T>::operator=(const field_type &) = default;

    /**
     * @brief Move Assignment Operator.
     * @param other Object to be moved.
     * @return Reference to the object.
     */
    template <FieldType T> field_type<T> &field_type<T>::operator=(field_type &&) noexcept = default;

    /**
     * @brief Returns the field type.
     * @return Field type object.
     */
    template <FieldType T> [[nodiscard]] T field_type<T>::get() const { return _type; }

    /**
     * @brief Equality operator.
     * @param other Other object to be compared.
     * @return True if the objects are equal, false otherwise.
     */
    template <FieldType T> [[nodiscard]] bool field_type<T>::operator==(const field_type &other) const {
        return _type == other._type;
    }

    /**
     * @brief Inequality operator.
     * @param other Other object to be compared.
     * @return True if the objects are not equal, false otherwise.
     */
    template <FieldType T> [[nodiscard]] bool field_type<T>::operator!=(const field_type &other) const {
        return _type != other._type;
    }

    /**
     * @brief Converts a field type to a JSON.
     * @return A JSON object.
     */
    template <FieldType T> [[nodiscard]] serialization::json_t field_type<T>::to_json() const {
        return {{"field", T::get_name()}, {"options", _type.to_json()}};
    }

    /**
     * @brief Converts a JSON to a field type.
     * @param json JSON object.
     * @return Field type object.
     */
    template <FieldType T> [[maybe_unused]] field_type<T> field_type<T>::from_json(const serialization::json_t &json) {
        return field_type(T::from_json(json["options"]));
    }

    /**
     * @brief Explicit Constructor.
     * @param name Name of the field.
     * @param type Object of the field's type.
     */
    template <FieldType T>
    field_entry<T>::field_entry(std::string name, field_type<T> type)
        : _name(std::move(name)), _type(std::move(type)) {}

    /**
     * @brief  Copy constructor.
     */
    template <FieldType T> field_entry<T>::field_entry(const field_entry &) = default;

    /**
     * Destructor
     */
    template <FieldType T> field_entry<T>::~field_entry() = default;

    /**
     * @brief Get the name of the field.
     * @return String containing the name of the field.
     */
    template <FieldType T> [[nodiscard]] [[maybe_unused]] std::string field_entry<T>::name() const { return _name; }

    /**
     * @brief Get the field type.
     * @return Field type object.
     */
    template <FieldType T> [[nodiscard]] [[maybe_unused]] field_type<T> field_entry<T>::type() const { return _type; }

    /**
     * @brief Static function that creates a text field.
     * @param name Field  name.
     * @param options Text field options.
     * @return A field_entry object.
     */
    template <>
    [[maybe_unused]] field_entry<text_field> field_entry<text_field>::create(std::string name, text_field options) {
        return field_entry(std::move(name), field_type(text_field(std::move(options))));
    }

    /**
     * @brief Static function that creates a numeric field.
     * @param name Field  name.
     * @param options Numeric field options.
     * @return A field_entry object.
     */
    template <>
    [[maybe_unused]] field_entry<numeric_field> field_entry<numeric_field>::create(std::string name, numeric_field options) {
        return field_entry(std::move(name), field_type(numeric_field(std::move(options))));
    }

    /**
     * @brief Converts a field entry to a JSON object.
     * @return JSON object.
     */
    template <FieldType T> [[nodiscard]] serialization::json_t field_entry<T>::to_json() const {
        return {{"name", _name}, {"type", _type.to_json()}};
    }

    /**
     * @brief Converts a JSON object to a field entry.
     * @param json JSON object.
     * @return A field entry.
     */
    template <FieldType T>
    [[maybe_unused]] field_entry<T> field_entry<T>::from_json(const serialization::json_t &json) {
        return field_entry(json["name"], field_type<T>::from_json(json["type"]));
    }

    /**
     * @brief Equality operator.
     * @param other Other field entry to compare with.
     * @return True if the two field entries are equal, false otherwise.
     */
    template <FieldType T> [[nodiscard]] bool field_entry<T>::operator==(const field_entry &other) const {
        return _name == other._name && _type == other._type;
    }

    /**
     * @brief Inequality operator.
     * @param other Other field entry to compare with.
     * @return True if the two field entries are not equal, false otherwise.
     */
    template <FieldType T> [[nodiscard]] bool field_entry<T>::operator!=(const field_entry &other) const {
        return _name != other._name || _type != other._type;
    }

    // --------------------------------------------------------------------------------------------- //
    // ------------------------------- Explicit template instantiations  --------------------------- //

    template class field_type<text_field>;
    template class field_type<numeric_field>;

    template class field_entry<text_field>;
    template class field_entry<numeric_field>;

} // namespace bridge::schema