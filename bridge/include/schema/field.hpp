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

#ifndef BRIDGE_FIELD_HPP_
#define BRIDGE_FIELD_HPP_

#include "./options.hpp"
#include <concepts>
#include <cstdint>
#include <utility>
#include <variant>

namespace bridge::schema {

    /**
     * Currently, a field type can be one of the following:
     * 1. text field
     * 2. numeric field
     * todo: possibly variant it's not the best way to do this.
     */
    using field_type_variant = std::variant<text_field, numeric_field>;

    // Concept of a field type: a field type is a string  type or numeric type
    template <typename T>
    concept FieldType = std::is_same_v<T, text_field> || std::is_same_v<T, numeric_field>;

    using field_value_variant = std::variant<std::string, uint32_t>;

    // Concept of a field value: a field value is a string or numeric value
    template <typename T>
    concept FieldValue = std::is_same_v<T, std::string> || std::unsigned_integral<T>;

    /**
     * Holds general information about the field type.
     */
    class field_type {

      public:
        /// \brief Constructor
        explicit field_type(field_type_variant type) : _type(std::move(type)) {}

        /// \brief Default Constructor
        explicit field_type() = default;

        /// \brief Copy Constructor
        field_type(const field_type &) = default;

        /// \brief Move Constructor
        field_type(field_type &&) = default;

        /// \brief Copy Assignment
        field_type &operator=(const field_type &) = default;

        /// \brief Move Assignment
        field_type &operator=(field_type &&) = default;

        /// \brief Returns the field type
        [[nodiscard]] field_type_variant type() const { return _type; }

        [[nodiscard]] bool is_text() const { return std::holds_alternative<text_field>(_type); }

        [[nodiscard]] bool is_numeric() const { return std::holds_alternative<numeric_field>(_type); }

        // load from json

      private:
        field_type_variant _type;
    };

    /**
     * Holds information about the field. Contains the field name, and the field type.
     */
    class field_entry {
      public:
        /// \brief Constructor
        explicit field_entry(std::string name, field_type type) : _name(std::move(name)), _type(std::move(type)) {}

        /// \brief Copy Constructor
        field_entry(const field_entry &) = default;

        /// \brief Get the field name
        [[nodiscard]] [[maybe_unused]] std::string name() const { return _name; }

        /// \brief Get the field type
        [[nodiscard]] [[maybe_unused]] field_type type() const { return _type; }

        /// \brief Static function that creates a text field
        [[maybe_unused]] static field_entry new_text_field(std::string name, text_field options) {
            return field_entry(std::move(name), field_type(text_field(std::move(options))));
        }

        /// \brief Static function that creates a numeric field
        [[maybe_unused]] static field_entry new_numeric_field(std::string name, numeric_field options) {
            return field_entry(std::move(name), field_type(numeric_field(options)));
        }

        /// \brief Return true if the field is indexed
        [[nodiscard]] [[maybe_unused]] bool is_indexed() const {
            if (_type.is_text()) {
                return std::get<text_field>(_type.type()).get_indexing_options().is_indexed();
            }
            return false;
        }

        /// \brief Return true if the field is numeric fast
        [[nodiscard]] [[maybe_unused]] bool is_numeric_fast() const {
            if (_type.is_numeric()) {
                return std::get<numeric_field>(_type.type()).is_fast();
            }
            return false;
        }

        // Avoid move semantics
        field_entry(field_entry &&) = delete;
        field_entry &operator=(field_entry &&) = delete;

      private:
        std::string _name;
        field_type _type;
    };

    class field_value {
      public:
        /// \brief Default Constructor
        field_value() = default;

        /// \brief Constructor
        explicit field_value(field_value_variant value) : _value(std::move(value)) {}

        /// \brief Copy Constructor
        field_value(const field_value &) = default;

        /// \brief Move Constructor
        field_value(field_value &&) = default;

        /// \brief Copy Assignment
        field_value &operator=(const field_value &) = default;

        /// \brief Move Assignment
        field_value &operator=(field_value &&) = default;

        /// \brief Returns the field value
        [[nodiscard]] [[maybe_unused]] field_value_variant value() const { return _value; }

        /// \brief implicit cast int to field_value
        [[maybe_unused]] static field_value from_int(uint32_t value) { return field_value(field_value_variant(value)); }

        /// \brief implicit cast string to field_value
        [[maybe_unused]] static field_value from_string(std::string value) {
            return field_value(field_value_variant(std::move(value)));
        }

      private:
        field_value_variant _value;
    };

    /**
     * A field is defined by a single ID and has the following properties:
     * 1. Moveable
     * 2. Copyable
     * 3. Partial ordering
     * 4. Equality
     * 5. Hashable
     */
    class field {
      public:
        /// \brief Default constructor
        explicit field() = default;

        explicit field(uint8_t id, uint32_t value) {
            this->id = id;
            this->value = field_value::from_int(value);
        }

        explicit field(uint8_t id, std::string value) {
            this->id = id;
            this->value = field_value::from_string(std::move(value));
        }

        /// \brief Copy Constructor
        field(const field &) = default;

        /// \brief Move Constructor
        field(field &&) = default;

        /// \brief Copy Assignment
        field &operator=(const field &) = default;

        /// \brief Move Assignment
        field &operator=(field &&) = default;

        /// \brief Equality
        bool operator==(const field &other) const { return id == other.id; }

        /// \brief Inequality
        bool operator!=(const field &other) const { return !(*this == other); }

        /// \brief Three-way comparison
        std::strong_ordering operator<=>(const field &other) const { return id <=> other.id; }

        /// \brief Hash
        friend std::hash<field>;

        /// \brief Get id
        [[nodiscard]] [[maybe_unused]] uint8_t get_id() const { return id; }

        /// \brief Get value
        [[nodiscard]] [[maybe_unused]] field_value get_value() const { return value; }

      private:
        field_value value;
        uint8_t id{};
    };

} // namespace bridge::schema

#endif // BRIDGE_FIELD_HPP_