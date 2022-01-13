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
#include <compare>

namespace bridge::schema {

    /**
     * Currently, a field type can be one of the following:
     * 1. text field
     * 2. numeric field
     * todo: I don't know if this is the best way to do this, but I'm not sure.
     */

    // Concept of a field type: a field type is a string  type or numeric type
    // todo: define better with good constraints.
    template <typename T>
    concept FieldType = std::is_same_v<T, text_field> || std::is_same_v<T, numeric_field>;

    // Concept of a field value: a field value is a string or numeric value
    // // todo: define better with good constraints.
    template <typename T>
    concept FieldValue = std::is_same_v<T, std::string> || std::unsigned_integral<T>;

    using id_t = unsigned short int;

    /**
     * Holds general information about the field type.
     */
     template <FieldType T>
    class field_type {

      public:
        /// \brief Constructor
        explicit field_type(T type) : _type(std::move(type)) {}

        /// \brief Default Constructor
        explicit field_type() = default;

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

        // load from json

      private:
        T _type;
    };

    /**
     * Holds information about the field. Contains the field name, and the field type.
     */
     template <FieldType T>
    class field_entry {
      public:
        /// \brief Constructor
        explicit field_entry(std::string name, field_type<T> type) : _name(std::move(name)), _type(std::move(type)) {}

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

        // Avoid move semantics
        field_entry(field_entry &&) = delete;
        field_entry &operator=(field_entry &&) = delete;

      private:
        const std::string _name;
        const field_type<T> _type;
    };

    template <FieldValue V>
    class field_value {
      public:
        /// \brief Default Constructor
        field_value() = default;

        /// \brief Constructor
        explicit field_value(V value) : _value(std::move(value)) {}

        /// \brief Copy Constructor
        field_value(const field_value &) = default;

        /// \brief Move Constructor
        field_value(field_value &&)  noexcept = default;

        /// \brief Copy Assignment
        field_value &operator=(const field_value &) = default;

        /// \brief Move Assignment
        field_value &operator=(field_value &&)  noexcept = default;

        /// \brief Returns the field value
        [[nodiscard]] [[maybe_unused]] V value() const { return _value; }

        /// \brief Override * operator
        [[nodiscard]] [[maybe_unused]] V operator*() const { return _value; }

        /// \brief implicit cast int to field_value
        template<typename T> requires std::is_integral_v<T>
        [[maybe_unused]] static field_value create(T value) { return field_value(value); }

        /// \brief implicit cast string to field_value
        [[maybe_unused]] static field_value create(std::string value) {
            return field_value(std::move(value));
        }

      private:
        V _value;
    };

    /**
     * A field is defined by a single ID and has the following properties:
     * 1. Moveable
     * 2. Copyable
     * 3. Partial ordering
     * 4. Equality
     * 5. Hashable
     */
     template <FieldValue V>
    class field {
      public:
        /// \brief Default constructor
        explicit field() = default;

        explicit field(id_t id, V value) {
            this->id = id;
            this->value = field_value<V>::create(std::move(value));
        }

        /// \brief Copy Constructor
        field(const field &) = default;

        /// \brief Move Constructor
        field(field && other)  noexcept {
            this->id = other.id;
            this->value = std::move(other.value);
        }

        /// \brief Copy Assignment
        field &operator=(const field &) = default;

        /// \brief Move Assignment
        field &operator=(field &&)  noexcept = default;

        /// \brief Equality
        template <FieldValue U>
        bool operator==(const field<U> &other) const { return id == other.get_id(); }

        /// \brief Inequality
        template <FieldValue U>
        bool operator!=(const field<U> &other) const { return id != other.get_id(); }

        /// \brief Three-way comparison
        template <FieldValue U>
        std::strong_ordering operator<=>(const field<U> &other) const { return id <=> other.get_id(); }

        /// \brief Get id
        [[nodiscard]] [[maybe_unused]] id_t get_id() const { return id; }

        /// \brief Get value
        [[nodiscard]] [[maybe_unused]] field_value<V> get_value() const { return value; }

        /// \brief Hashing function
        [[nodiscard]] [[maybe_unused]] std::size_t hash() const { return std::hash<id_t>{}(id); }

      private:
        field_value<V> value;
        id_t id{};
    };

} // namespace bridge::schema

#endif // BRIDGE_FIELD_HPP_