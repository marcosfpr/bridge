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

#include <concepts>
#include <utility>
#include <variant>

#include "./options.hpp"

#ifndef BRIDGE_FIELD_ENTRY_HPP_
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

    /// @brief Concept that defines a function called get_name() that returns a string which is the name of the field
    template <typename T> concept HasName = requires(T t) {
        { t.get_name() } -> std::same_as<std::string>;
    };

    /// @brief Concept that defines a FieldType based on  the two possible types of fields: text and numeric.
    template <typename T>
    concept FieldType = (std::is_same_v<T, text_field_option> || std::is_same_v<T, numeric_field_option>) && HasName<T>;


    /**
     * @brief Holds general information about the field type.
     */
    template <FieldType T>
    class field_type {

      public:
        /**
         * @brief Explicit Constructor.
         * @param type Object of the field's type.
         */
        explicit field_type(T type);

        /**
         * @brief Default Constructor.
         */
        field_type();

        /**
         * @brief Destructor.
         */
        ~field_type() noexcept;

        /**
         * @brief Copy Constructor.
         * @param other Object to be copied.
         */
        field_type(const field_type &);

        /**
         * @brief Move Constructor.
         * @param other Object to be moved.
         */
        field_type(field_type &&)  noexcept;

        /**
         * @brief Copy Assignment Operator.
         * @param other Object to be copied.
         * @return Reference to the object.
         */
        field_type &operator=(const field_type &);

        /**
         * @brief Move Assignment Operator.
         * @param other Object to be moved.
         * @return Reference to the object.
         */
        field_type &operator=(field_type &&)  noexcept;

        /**
         * @brief Returns the field type.
         * @return Field type object.
         */
        [[nodiscard]] T get() const;

        /**
         * @brief Check if the field type is text.
         * @return True if the field type is text, false otherwise.
         */
        [[nodiscard]] constexpr bool is_text() const {
            // check if generic T is of type: text_field
            return std::is_same_v<T, text_field_option>;
        }

        /**
         * @brief Check if the field type is numeric.
         * @return True if the field type is numeric, false otherwise.
         */
        [[nodiscard]] constexpr bool is_numeric() const {
            // check if generic T is of type: numeric_field
            return std::is_same_v<T, numeric_field_option>;
        }

        /**
         * @brief Equality operator.
         * @param other Other object to be compared.
         * @return True if the objects are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const field_type &other) const;

        /**
         * @brief Inequality operator.
         * @param other Other object to be compared.
         * @return True if the objects are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const field_type &other) const;

        /**
         * @brief Converts a field type to a JSON.
         * @return A JSON object.
         */
        [[nodiscard]] serialization::json_t to_json() const;


        /**
         * @brief Converts a JSON to a field type.
         * @param json JSON object.
         * @return Field type object.
         */
        [[maybe_unused]] static field_type from_json(const serialization::json_t &json);

      private:
        T _type;
    };

    /**
     *  \brief A FieldEntry represents a field and its configuration. Schema are a collection of FieldEntry
     *  \details It consists  of:
     *  - a field  name;
     *  - a field type, itself wrapping up options describing  how the  field  should be indexed.
     */
    template <FieldType T>
    class field_entry {
      public:

        /**
         * @brief Explicit Constructor.
         * @param name Name of the field.
         * @param type Object of the field's type.
         */
        explicit field_entry(std::string name, field_type<T> type);

        /**
         * @brief  Copy constructor.
         */
        field_entry(const field_entry &);

        /**
         * Destructor
         */
        virtual ~field_entry();

        /**
         * @brief Get the name of the field.
         * @return String containing the name of the field.
         */
        [[nodiscard]] [[maybe_unused]] std::string name() const;

        /**
         * @brief Get the field type.
         * @return Field type object.
         */
        [[nodiscard]] [[maybe_unused]] field_type<T> type() const;

        /**
         * @brief Static function that creates a text field.
         * @param name Field  name.
         * @param options Text field options.
         * @return A field_entry object.
         */
        [[maybe_unused]] static field_entry create(std::string name, text_field_option options);

        /**
         * @brief Static function that creates a numeric field.
         * @param name Field  name.
         * @param options Numeric field options.
         * @return A field_entry object.
         */
        [[maybe_unused]] static field_entry create(std::string name, numeric_field_option options);

        /**
         * @brief Check if the field is indexed.
         * @return True if the field is indexed, false otherwise.
         */
        [[nodiscard]] [[maybe_unused]] constexpr bool is_indexed() const {
            // check at compile time if T is text_field
            if constexpr (std::is_same_v<T, text_field_option>) { // todo: try use _type.is_text()
                // unsafe cast _type.get() to text_field
                return static_cast<text_field_option>(_type.get()).get_indexing_options().is_indexed();
            }
            return false;
        }

        /**
         * @brief Check if the field is numeric fast.
         * @return True if the field is numeric fast, false otherwise.
         */
        [[nodiscard]] [[maybe_unused]] constexpr bool is_numeric_fast() const {
            if constexpr  (std::is_same_v<T, numeric_field_option>){ // todo: try use _type.is_numeric()
                // unsafe cast _type.get() to numeric_field
                return static_cast<numeric_field_option>(_type.get()).is_fast();
            }
            return false;
        }

        /**
         * @brief Converts a field entry to a JSON object.
         * @return JSON object.
         */
        [[nodiscard]] serialization::json_t to_json() const;

        /**
         * @brief Converts a JSON object to a field entry.
         * @param json JSON object.
         * @return A field entry.
         */
        [[maybe_unused]] static field_entry from_json(const serialization::json_t &json);

        /**
         * @brief Equality operator.
         * @param other Other field entry to compare with.
         * @return True if the two field entries are equal, false otherwise.
         */
        [[nodiscard]] bool operator==(const field_entry &other) const;

        /**
         * @brief Inequality operator.
         * @param other Other field entry to compare with.
         * @return True if the two field entries are not equal, false otherwise.
         */
        [[nodiscard]] bool operator!=(const field_entry &other) const;

        //! < Avoid move semantics
        field_entry(field_entry &&) = delete;
        field_entry &operator=(field_entry &&) = delete;

      private:
        const std::string _name;
        const field_type<T> _type;
    };

    /**
     * @brief A field entry is a variant of either a text field or a numeric field.
     */
    using field_entry_v = std::variant<field_entry<text_field_option>, field_entry<numeric_field_option>>;


} // namespace bridge::schema

#endif