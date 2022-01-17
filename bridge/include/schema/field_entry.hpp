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

/// \brief [C++] FieldEntry class

#include "./options.hpp"
#include <concepts>
#include <utility>

#ifndef BRIDGE_FIELD_ENTRY_HPP
#define BRIDGE_FIELD_ENTRY_HPP_

namespace bridge::schema {

    /**
     * Currently, a field type can be one of the following:
     * 1. text field
     * 2. numeric field
     * todo: I don't know if this is the best way to do this, but I'm not sure.
     */

    // Concept of a field type: a field type is a string  type or numeric type
    // todo: define better with good constraints.

    // Concept that defines a function called get_name() that returns a string which is the name of the field
    template <typename T> concept HasName = requires(T t) {
        { t.get_name() } -> std::same_as<std::string>;
    };

    template <typename T>
    concept FieldType = (std::is_same_v<T, text_field> || std::is_same_v<T, numeric_field>) && HasName<T>;


    /**
     * Holds general information about the field type.
     */
    template <FieldType T>
    class field_type {

      public:
        /// \brief Constructor
        explicit field_type(T type) : _type(std::move(type)) {}

        /// \brief Default Constructor
        field_type() = default;

        /// \brief Destructor
        ~field_type() noexcept = default;

        /// \brief Copy Constructor
        field_type(const field_type &) = default;

        /// \brief Move Constructor
        field_type(field_type &&)  noexcept = default;

        /// \brief Copy Assignment
        field_type &operator=(const field_type &) = default;

        /// \brief Move Assignment
        field_type &operator=(field_type &&)  noexcept = default;

        /// \brief Returns the field type
        [[nodiscard]] T get() const { return _type; }

        [[nodiscard]] constexpr bool is_text() const {
            // check if generic T is of type: text_field
            return std::is_same_v<T, text_field>;
        }

        [[nodiscard]] constexpr bool is_numeric() const {
            // check if generic T is of type: numeric_field
            return std::is_same_v<T, numeric_field>;
        }

        /// \brief Equality operator
        [[nodiscard]] bool operator==(const field_type &other) const {
            return _type == other._type;
        }

        /// \brief Inequality operator
        [[nodiscard]] bool operator!=(const field_type &other) const {
            return _type != other._type;
        }

        //! \brief JSON serialization
        [[nodiscard]] serialization::json_t to_json() const {
            return {
                {"field", T::get_name()},
                {"options", _type.to_json()}
            };
        }

        //! \brief JSON deserialization
        [[maybe_unused]] static field_type from_json(const serialization::json_t &json) {
            return field_type(T::from_json(json["options"]));
        }

      private:
        T _type;
    };

    /**
     *  \brief A FieldEntry represents a field and its configuration. Schema are a collection of FieldEntry
     *  \details It consists  of:
     *  - a field  name;
     *  - a  field type, itself wrapping up options describing  how the  field  should be indexed.
     */
    template <FieldType T>
    class field_entry {
      public:

        /// \brief Constructor
        field_entry(std::string name, field_type<T> type) : _name(std::move(name)), _type(std::move(type)) {}

        /// \brief Copy Constructor
        field_entry(const field_entry &) = default;

        /// \brief Destructor
        ~field_entry() = default;

        /// \brief Get the field name
        [[nodiscard]] [[maybe_unused]] std::string name() const { return _name; }

        /// \brief Get the field type
        [[nodiscard]] [[maybe_unused]] field_type<T> type() const { return _type; }

        /// \brief Static function that creates a text field
        [[maybe_unused]] static field_entry create(std::string name, text_field options) {
            return field_entry(std::move(name), field_type(text_field(std::move(options))));
        }

        /// \brief Static function that creates a numeric field
        [[maybe_unused]] static field_entry create(std::string name, numeric_field options) {
            return field_entry(std::move(name), field_type(numeric_field(options)));
        }

        /// \brief Return true if the field is indexed
        [[nodiscard]] [[maybe_unused]] constexpr bool is_indexed() const {
            // check at compile time if T is text_field
            if constexpr (std::is_same_v<T, text_field>) { // todo: try use _type.is_text()
                // unsafe cast _type.get() to text_field
                return static_cast<text_field>(_type.get()).get_indexing_options().is_indexed();
            }
            return false;
        }

        /// \brief Return true if the field is numeric fast
        [[nodiscard]] [[maybe_unused]] constexpr bool is_numeric_fast() const {
            if constexpr  (std::is_same_v<T, numeric_field>){ // todo: try use _type.is_numeric()
                // unsafe cast _type.get() to numeric_field
                return static_cast<numeric_field>(_type.get()).is_fast();
            }
            return false;
        }

        //! \brief JSON Serialization
        [[nodiscard]] serialization::json_t to_json() const {
            return {
                {"name", _name},
                {"type", _type.to_json()}
            };
        }

        //! \brief JSON Deserialization
        [[maybe_unused]] static field_entry from_json(const serialization::json_t &json) {
            return field_entry(json["name"], field_type<T>::from_json(json["type"]));
        }

        //! \brief Equality operator
        [[nodiscard]] bool operator==(const field_entry &other) const {
            return _name == other._name && _type == other._type;
        }

        //! \brief Inequality operator
        [[nodiscard]] bool operator!=(const field_entry &other) const {
            return _name != other._name || _type != other._type;
        }

        // Avoid move semantics
        field_entry(field_entry &&) = delete;
        field_entry &operator=(field_entry &&) = delete;

      private:
        const std::string _name;
        const field_type<T> _type;
    };

} // namespace bridge::schema

#endif