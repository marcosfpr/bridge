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

#include "../common/serialization.hpp"

namespace bridge::schema {

    // Concept of a field value: a field value is a string or numeric value
    // // todo: define better with good constraints.
    template <typename T>
    concept FieldValue = std::is_same_v<T, std::string> || std::unsigned_integral<T> && serialization::Serializable<T>;

    using id_t = unsigned short int;

    /**
     * \brief Value represents the value of a any field. It is generic over all of the possible field types.
     *
     */
    template <FieldValue V> class field_value {
      public:
        /// \brief Default Constructor
        field_value() = default;

        /// \brief Constructor
        explicit field_value(V value) : _value(std::move(value)) {}

        /// \brief Copy Constructor
        field_value(const field_value &) = default;

        /// \brief Move Constructor
        field_value(field_value &&) noexcept = default;

        /// \brief Copy Assignment
        field_value &operator=(const field_value &) = default;

        /// \brief Move Assignment
        field_value &operator=(field_value &&) noexcept = default;

        /// \brief Returns the field value
        [[nodiscard]] [[maybe_unused]] V value() const { return _value; }

        /// \brief Override * operator
        [[nodiscard]] [[maybe_unused]] V operator*() const { return _value; }

        /// \brief implicit cast int to field_value
        template <typename T>
        requires std::is_integral_v<T>
        [[maybe_unused]] static field_value create(T value) { return field_value(value); }

        /// \brief implicit cast string to field_value
        [[maybe_unused]] static field_value create(std::string value) { return field_value(std::move(value)); }

        /// \brief Serialization of field value it's trivial
        friend class boost::serialization::access;
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &_value;
        }

      private:
        V _value;
    };

} // namespace bridge::schema

#endif // BRIDGE_FIELD_VALUE_HPP