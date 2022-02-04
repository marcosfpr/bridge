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

#include <string>
#include <variant>

#include "./field_value.hpp"

namespace bridge::schema {

    /// @brief A identity type for  fields is a unsigned integer.
    using id_t = unsigned char;

    /**
     *  @brief A Field holds together an ID and its FieldValue.
     *  @details It has the following properties:
     * 1. Moveable
     * 2. Copyable
     * 3. Partial ordering
     * 4. Equality
     * 5. Hashable
     */
    template <FieldValue V>
    class field {
      public:

        /**
         * @brief Default constructor.
         */
        explicit field() = default;

        /**
         * Destructor.
         */
         virtual ~field() = default;

        /**
         * @brief Constructor.
         * @param id The ID of the field.
         * @param value The value of the field.
         */
        explicit field(id_t id, V value) {
            this->id = id;
            this->value = field_value<V>::create(std::move(value));
        }

        /**
         * @brief Copy constructor.
         */
        field(const field &) = default;

        /**
         * @brief Move constructor.
         */
        field(field && other)  noexcept {
            this->id = other.id;
            this->value = std::move(other.value);
        }

        /**
         * @brief Copy assignment operator.
         */
        field &operator=(const field &) = default;

        /**
         * @brief Move assignment operator.
         */
        field &operator=(field &&)  noexcept = default;

        /**
         * @brief Equality operator.
         * @param other The other field.
         * @return True if the fields are equal, false otherwise.
         */
        template<FieldValue  U>
        bool operator==(const field<U> &other) const { return this->get_id() == other.get_id(); }

        /**
         * @brief Inequality operator.
         * @param other The other field.
         * @return True if the fields are not equal, false otherwise.
         */
        template<FieldValue  U>
        bool operator!=(const field<U> &other) const { return this->get_id() != other.get_id(); }

        /**
         * @brief Three-way comparison operator.
         * @tparam U The other field type.
         * @param other The other field object.
         * @return Strong ordering between the fields. Allow to use the following operators: <, <=, >, >=.
         */
        template<FieldValue  U>
        std::strong_ordering operator<=>(const field<U> &other) const { return this->get_id() <=> other.get_id(); }

        /**
         * @brief Get the ID of the field.
         * @return Field ID.
         */
        [[nodiscard]] [[maybe_unused]] id_t get_id() const { return id; }

        /**
         * @brief Get the value of the field.
         * @return Field value.
         */
        [[nodiscard]] [[maybe_unused]] field_value<V> get_value() const {
            return value;
        }

        template<FieldValue U>
        [[nodiscard]] [[maybe_unused]] static field<U> from_value(id_t field_id, field_value<U> value) {
            return field<U>(field_id, *value);
        }

         /**
         * @brief Serialize a field.
         * @tparam Archive Archive type.
         * @param ar Archive object.
         * @param version Current version of the field.
         */
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar & id;
            ar & value;
        }
        friend boost::serialization::access; //! Allow to access the private members of field.            

        /**
         * @brief Hash the field.
         * @return Hash value.
         */
        [[nodiscard]] [[maybe_unused]] std::size_t hash() const { return std::hash<id_t>{}(id); }

      private:
        field_value<V> value;
        id_t id{};
    };


    // string field
    typedef field<std::string> text_field;
    typedef field<uint32_t> uint32_field;

    /**
     * @brief In the Bridge project, documents are represented as a collection of fields.
     * @details For this first version, we will only support a two variants of field types: string and integer.
     * @todo It must change in the future lol.
     */
    typedef std::variant<text_field, uint32_field> field_v;

} // namespace bridge::schema

#endif // BRIDGE_FIELD_HPP_