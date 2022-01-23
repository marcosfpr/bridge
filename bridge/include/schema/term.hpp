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
#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include "./field.hpp"

namespace bridge::schema {

        // type aliases and bringing to scope.
        typedef std::string::iterator term_iterator;
        typedef unsigned const char* raw_bytes;

        class term {

            /**
         * @brief Default constructor.
             */
            explicit term();

            /**
         * @brief Constructor.
         * @param data The data to be stored in the term.
             */
           [[maybe_unused]] explicit term(unsigned const char* data);

            /**
         * Move  constructor.
         * @param other The term to be moved.
             */
            term(term &&other) noexcept;

            /**
         * @brief  Move assignment operator.
         * @param other The term to be moved.
         * @return The term.
             */
            term &operator=(term &&other) noexcept;

            /**
         * @brief Equality operator.
         * @param other The term to be compared.
         * @return True if the terms are equal, false otherwise.
             */
            bool operator==(const term &other) const;

            /**
         * @brief Inequality operator.
         * @brief other The term to be compared.
         * @return True if the terms are not equal, false otherwise.
             */
            bool operator!=(const term &other) const;

            /**
         * @brief Three-way comparison operator.
         * @param other The term to be compared.
         * @return Strong ordering of the terms.  Allow to use operators <, >, <=, >=.
             */
            std::strong_ordering operator<=>(const term &other) const;

            /**
         * @brief Overload output operator.
             */
            friend std::ostream &operator<<(std::ostream &os, const term &t);

            /**
         * @brief Hash function.
         * @return The hash value of the term.
             */
            size_t hash() const;

            /**
         * @brief Get the field id.
         * @return The field id.
             */
            id_t get_field_id() const;

            /**
         * @brief  Get the term data from an uint32_t.
         * @return A new term with the data.
             */
            static term from_uint32(id_t field_id, uint32_t data);

            /**
         * @brief  Get the term data from an string
         * @return A new term with the data.
             */
            static term from_string(id_t field_id, const std::string &data);

            /**
         * @brief Get the term from a raw array of bytes.
         * @return A new term with the data.
             */
            static term from_bytes(id_t field_id, unsigned char *data, size_t size);

            /**
         * @brief Get the bytes iterator of the term.
         * @return The bytes iterator.
             */
            term_iterator begin() const;

            /**
         * @brief Get the end of term bytes iterator.
         * @return The end bytes iterator
             */
            term_iterator end() const;

            // Avoid copy semantics
            term(const term &) = delete;
            term &operator=(const term &) = delete;

          private:
            const std::string _data;
        };
    } // namespace bridge::schema

#endif // BRIDGE_TERM_HPP
