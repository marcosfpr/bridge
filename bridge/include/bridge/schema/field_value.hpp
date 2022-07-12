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

/// \brief [C++] FieldValue class

#ifndef BRIDGE_FIELD_VALUE_HPP
#define BRIDGE_FIELD_VALUE_HPP

#include <concepts>
#include <string>
#include <variant>

#include "bridge/common/serialization.hpp"

namespace bridge::schema {

    /// @brief  Concept of a field value: a field value is a string or numeric value and must be serializable.
    template <typename T>
    concept FieldValue = (std::is_same_v<T, std::string> || std::unsigned_integral<T>) && serialization::Serializable<T>;
    
    /**
     * @brief Value represents the value of a any field. It is generic over all of the possible field types.
     *
     */
    template <FieldValue V>
    class field_value {
      public:
        /**
         * @brief Default constructor.
         */
        field_value() = default;

        /**
         * @brief Destructor.
         */
         virtual ~field_value() = default;

        /**
         * @brief Constructor
         * @param value Value of the field.
         */
        explicit field_value(V value) : _value(std::move(value)) {}

        /**
         * @brief Copy constructor.
         */
        field_value(const field_value &) = default;

        /**
         * @brief Move constructor.
         */
        field_value(field_value &&) noexcept = default;

        /**
         * @brief Copy assignment operator.
         */
        field_value &operator=(const field_value &) = default;

        /**
         * @brief Move assignment operator.
         */
        field_value &operator=(field_value &&) noexcept = default;

        /**
         * @brief Get the value.
         * @return The value.
         */
        [[nodiscard]] [[maybe_unused]] V value() const { return _value; }

        /**
         * @brief Get the value through * overload.
         * @param value The value.
         */
        [[nodiscard]] [[maybe_unused]] V operator*() const { return _value; }

        /**
         * @brief Static function that creates a field of type T based  on an  integer value.
         * @tparam T Type of an integral field.
         * @param value Value of  type T.
         * @return A new field of type T.
         */
        template <typename T>
        requires std::is_integral_v<T>
        [[maybe_unused]] static field_value create(T value) { return field_value(value); }

        /**
         * Static  function  that  creates  a  field  of  type  T  based  on  string value.
         * @param value String value.
         * @return A new field of type T.
         */
        [[maybe_unused]] static field_value create(std::string value) { return field_value(std::move(value)); }

        /// \brief Serialization of field value it's trivial
        /**
         * @brief Serialization of field value.
         * @tparam Archive Archive type.
         * @param ar Archive object.
         * @param version Current version of the object.
         */
        template <class Archive> void serialize(Archive &ar) {
            ar(_value);
        }

      private:
        V _value;
    };

    /// @brief Field values defined over the default types.
    using string_value = field_value<std::string>;
    using uint32_value = field_value<uint32_t>;
    using field_value_v = std::variant<string_value, uint32_value>; //!< Type of field value.

} // namespace bridge::schema

#endif // BRIDGE_FIELD_VALUE_HPP