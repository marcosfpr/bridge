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

#ifndef SCHEMA_HPP_
#define SCHEMA_HPP_

#include <memory>
#include <string>
#include <vector>

// #include "./field.hpp"
#include "./options.hpp"

namespace bridge {

    typedef struct field {
        std::string name;

        //! \brief Default constructor
        field() = default;

        //! \brief Copy constructor
        field(const field &other) = default;

        //! \brief Move constructor
        field(field &&other) = default;

        //! \brief Copy assignment operator
        field &operator=(const field &other) = default;

        //! \brief Move assignment operator
        field &operator=(field &&other) = default;

        //! \brief Comparison trait
        bool operator==(const field &other) const { return name == other.name; }

        //! \brief Less than trait
        bool operator<(const field &other) const { return name < other.name; }

        //! \brief Greater than trait
        bool operator>(const field &other) const { return name > other.name; }

        //! \brief Hashing trait
        size_t hash() const { return std::hash<std::string>()(name); }
    } field_t;

    struct field_value {
        field_t name;
        std::string value;

        //! \brief Default constructor
        field_value() = default;

        //! \brief Constructor
        field_value(field_t name, std::string value)
            : name(name), value(std::move(value)) {}

        //! \brief Copy constructor
        field_value(const field_value &other) = default;

        //! \brief Move constructor
        field_value(field_value &&other) = default;

        //! \brief Copy assignment operator
        field_value &operator=(const field_value &other) = default;

        //! \brief Move assignment operator
        field_value &operator=(field_value &&other) = default;

        //! \brief Comparison trait
        bool operator==(const field_value &other) const {
            return name == other.name && value == other.value;
        }

        //! \brief Less than trait
        bool operator<(const field_value &other) const {
            return name < other.name ||
                   (name == other.name && value < other.value);
        }

        //! \brief Greater than trait
        bool operator>(const field_value &other) const {
            return name > other.name ||
                   (name == other.name && value > other.value);
        }
    };

    struct term {
        std::shared_ptr<std::string> text_ptr;
        std::shared_ptr<field_t> field_ptr;

        //! \brief Copy constructor
        term(const term &other) = default;

        //! \brief Move constructor
        term(term &&other) = default;

        //! \brief Copy assignment operator
        term &operator=(const term &other) = default;

        //! \brief Move assignment operator
        term &operator=(term &&other) = default;

        //! \brief Comparison trait
        bool operator==(const term &other) const {
            return *text_ptr == *other.text_ptr &&
                   *field_ptr == *other.field_ptr;
        }

        //! \brief Less than trait
        bool operator<(const term &other) const {
            return *field_ptr < *other.field_ptr ||
                   (*field_ptr == *other.field_ptr &&
                    *text_ptr < *other.text_ptr);
        }

        //! \brief Greater than trait
        bool operator>(const term &other) const {
            return *field_ptr > *other.field_ptr ||
                   (*field_ptr == *other.field_ptr &&
                    *text_ptr > *other.text_ptr);
        }

        //! \brief Hashing trait
        size_t hash() const {
            return std::hash<std::string>()(*text_ptr) ^ field_ptr->hash();
        }
    };

    typedef std::vector<field_value> field_vec;

    class document {
      public:
        //! \brief Default constructor
        document() = default;

        /**
         * @brief Set a custom text value to a field and insert into the
         * document
         *
         * @param f field name
         * @param text content of field
         */
        void set(field_t f, std::string text) {
            this->add(field_value(std::move(f), std::move(text)));
        }

        /**
         * @brief Add a field-value pair into the document.
         *
         * @param fv Field-value pair
         */
        void add(const field_value fv) {
            this->fields.push_back(std::move(fv));
        }

        // Iterator access
        field_vec::iterator begin() { return this->fields.begin(); }

        field_vec::iterator end() { return this->fields.end(); }

      private:
        field_vec fields;

      private:
        // Avoid copy constructor and assignment operator
        document(const document &other) = delete;
        document &operator=(const document &other) = delete;

      private:
        // Alow move semantics

        //! \brief Move constructor
        document(document &&other) = default;

        //! \brief Move assignment operator
        document &operator=(document &&other) = default;
    };

}; // namespace bridge

#endif // SCHEMA_HPP_