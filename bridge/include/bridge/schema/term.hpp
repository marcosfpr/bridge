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

#ifndef BRIDGE_TERM_HPP
#define BRIDGE_TERM_HPP

#include <compare>
#include <iostream>
#include <memory>
#include <vector>

#include "bridge/global.hpp"
#include "bridge/schema/field.hpp"

namespace bridge::schema {

    // type aliases and bringing to scope.
    typedef std::vector<bridge::byte_t>::iterator term_iterator;
    typedef const unsigned char *raw_bytes;

    /**
     * @brief Term class represents a stream of bytes.
     * @warning Take care of using it like a string container. It is not!
     */
    class term {
      public:
        /**
         * @brief Default constructor.
         */
        explicit term();

        /**
         * @brief Constructor.
         * @param data The data to be stored in the term.
         * @param size The size of the data.
         */
        explicit term(bridge::byte_t *data, size_t size); //! < From raw bytes

        /**
         * @brief Copy construct a new term object
         *
         * @param other Other term
         */
        term(const term &other);

        /**
         * @brief Copy assignment operator.
         *
         * @param other Term to be copied.
         * @return term& a new term.
         */
        term &operator=(const term &other);

        /**
         * Move  constructor.
         * @param other The term to be moved.
         */
        term(term &&other) noexcept; //! < Moveable

        /**
         * @brief  Move assignment operator.
         * @param other The term to be moved.
         * @return The term.
         */
        term &operator=(term &&other) noexcept; //! < Move semantics
        /**
         * @brief Equality operator.
         * @param other The term to be compared.
         * @return True if the terms are equal, false otherwise.
         */
        bool operator==(const term &other) const; //! < Equality operator

        /**
         * @brief Inequality operator.
         * @brief other The term to be compared.
         * @return True if the terms are not equal, false otherwise.
         */
        bool operator!=(const term &other) const; //! < Inequality operator

        /**
         * @brief Three-way comparison operator.
         * @param other The term to be compared.
         * @return Strong ordering of the terms.  Allow to use operators <, >, <=, >=.
         */
        std::strong_ordering operator<=>(
            const term &other) const; //! < Three-way comparison operator allowing to use operators <, >, <=, >=.

        /**
         * @brief Overload output operator.
         */
        friend std::ostream &operator<<(std::ostream &os, const term &t);

        /**
         * @brief Hash function.
         * @return The hash value of the term.
         */
        [[nodiscard]] size_t hash() const; //! < Hash function.

        /**
         * @brief Get the field id.
         * @return The field id.
         */
        [[nodiscard]] id_t get_field_id() const;

        /**
         * @brief Get the term data from an uint16_t
         * @return A new term with the data.
         */
        static term from_uint8(id_t field_id, uint8_t data) {
            //  serialize id_t and uint8_t as a std::vector
            // first position is the field id
            // the rest is the uint32_t data (4 bytes)
            std::vector<bridge::byte_t> bytes;
            bytes.reserve(sizeof(id_t) + sizeof(uint8_t));

            bytes.push_back(static_cast<bridge::byte_t>(field_id));
            bytes.push_back(static_cast<bridge::byte_t>(data));

            return term(bytes.data(), bytes.size());
        }

        /**
         * @brief Get the term data from an uint16_t
         * @return A new term with the data.
         */
        static term from_uint16(id_t field_id, uint16_t data) {
            //  serialize id_t and uint16_t as a std::vector
            // first position is the field id
            // the rest is the uint32_t data (4 bytes)
            std::vector<bridge::byte_t> bytes;
            bytes.reserve(sizeof(id_t) + sizeof(uint16_t));
            bytes.push_back(static_cast<bridge::byte_t>(field_id));
            bytes.push_back(static_cast<bridge::byte_t>(data >> 8));
            bytes.push_back(static_cast<bridge::byte_t>(data));
            return term(bytes.data(), bytes.size());
        }

        /**
         * @brief  Get the term data from an uint32_t.
         * @return A new term with the data.
         */
        static term from_uint32(id_t field_id, uint32_t data) {
            //  serialize id_t and uint32_t as a std::vector
            // first position is the field id
            // the rest is the uint32_t data (4 bytes)

            std::vector<bridge::byte_t> bytes;

            bytes.reserve(sizeof(id_t) + sizeof(uint32_t));          // reserve space for the field id and the data
            bytes.push_back(static_cast<bridge::byte_t>(field_id));   // push the field id
            bytes.push_back(static_cast<bridge::byte_t>(data >> 24)); // push the first byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 16)); // push the second byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 8));  // push the third byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data));       // push the fourth byte of the data

            return term(bytes.data(), bytes.size());
        }

        /**
         * @brief  Get the term data from an uint64_t.
         * @return A new term with the data.
         */
        static term from_uint64(id_t field_id, u_int64_t data) {
            std::vector<bridge::byte_t> bytes;

            bytes.reserve(sizeof(id_t) + sizeof(uint64_t));          // reserve space for the field id and the data
            bytes.push_back(static_cast<bridge::byte_t>(field_id));   // push the field id
            bytes.push_back(static_cast<bridge::byte_t>(data >> 56)); // push the first byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 48)); // push the second byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 40)); // push the third byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 32)); // push the fourth byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 24)); // push the fifth byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 16)); // push the sixth byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data >> 8));  // push the seventh byte of the data
            bytes.push_back(static_cast<bridge::byte_t>(data));       // push the eighth byte of the data

            return term(bytes.data(), bytes.size());
        }

        /**
         * @brief  Get the term data from an string
         * @return A new term with the data.
         */
        static term from_string(id_t field_id, const std::string &data) {
            // serialize id_t and std::string as a std::vector
            // first position is the field id
            // the rest is the std::string data

            std::vector<bridge::byte_t> bytes;

            bytes.reserve(sizeof(id_t) + data.size()); // reserve space for the field id and the data

            bytes.push_back(static_cast<bridge::byte_t>(field_id)); // push the field id

            for (char c : data) {
                bytes.push_back(c); // push the data
            }

            return term(bytes.data(), bytes.size());
        }

        /**
         * @brief Get the term from a raw array of bytes.
         * @return A new term with the data.
         */
        static term from_bytes(id_t field_id, bridge::byte_t *data, size_t size) {
            // serialize id_t and byte raw array as a std::vector
            // first position is the field id
            // the rest is the byte raw array data

            std::vector<bridge::byte_t> bytes;

            bytes.reserve(sizeof(id_t) + size); // reserve space for the field id and the data

            bytes.push_back(static_cast<bridge::byte_t>(field_id)); // push the field id

            for (size_t i = 0; i < size; i++) {
                bytes.push_back(data[i]); // push the data
            }

            return term(bytes.data(), bytes.size());
        }

        /**
         * @brief Get the bytes iterator of the term.
         * @return The bytes iterator.
         */
        [[nodiscard]] auto begin() const;

        /**
         * @brief Get the end of term bytes iterator.
         * @return The end bytes iterator
         */
        [[nodiscard]] auto end() const;

        /**
         * @brief Get the raw bytes of the term.
         * @warning Unsafe.
         * @return const char* Raw bytes of the term.
         */
        [[nodiscard]] const bridge::byte_t *as_ref() const; //! As bytes ref.

        /**
         * @brief Get the number of bytes of the term.
         * @return The number of bytes of the term.
         */
        [[nodiscard]] size_t size() const;

      private:
        std::vector<bridge::byte_t> data_;
    };
} // namespace bridge::schema

#endif // BRIDGE_TERM_HPP
