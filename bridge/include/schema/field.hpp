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

#include "./field_value.hpp"

namespace bridge::schema {

    /**
     *  \brief A Field holds together an ID and its FieldValue.
     *  \details It has the following properties:
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