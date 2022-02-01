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

#ifndef BRIDGE_DOCUMENT_HPP_
#define BRIDGE_DOCUMENT_HPP_

#include <vector>
#include <memory>

#include "./field.hpp"

namespace bridge::schema {

    typedef std::unique_ptr<bridge_field> field_ptr;
    typedef std::vector<field_ptr>::iterator field_iterator;

    /**
     * @brief Bridge's document is the object that can be indexed and searched for.
     * @details Documents are really fundamentally a collection of unordered tuple (field, value).
     *         In this list, one field may appear more than once.
     * 
     */

    class document {
    public:

        /**
         * @brief Construct a new document object
         * 
         */
        explicit document();

        /**
         * @brief Construct a new document object
         * 
         * @param fields Field-Value tuples of any kind.
         */
        document(std::vector<field_ptr>&& fields);

        /**
         * @brief Destroy the document and all its fields
         * 
         */
        virtual ~document();

        /**
         * @brief Equality operator
         * 
         * @param other Other document
         * @return true If the documents are equal
         * @return false Otherwise
         */
        bool operator==(const document& other) const;

        /**
         * @brief Inequality operator
         * 
         * @param other Other document
         * @return true If the documents are not equal
         * @return false Otherwise
         */
        bool operator!=(const document& other) const;

        /**
         * @brief Returns the number of fields in the document
         * 
         * @return size_t The number of fields
         */
        size_t len() const;

        /**
         * @brief Adds a field to the document
         * 
         * @param field_id Field ID
         * @param value String value
         */
        void add_text(id_t field_id, const std::string& value);

        /**
         * @brief Adds a field to the document
         * 
         * @param field_id Field ID
         * @param value uint32_t value
         */
        void add_u32(id_t field_id, uint32_t value);

        /**
         * @brief Add a field to the document
         * 
         * @tparam V Type of the value.
         * @param field Field  of type V.
         */
        template <FieldValue V>
        void add(field<V> field);

        /**
         * @brief Get the fields iterator.
         * 
         * @return field_iterator Iterator over document's fields.
         */
        field_iterator get_fields();

        /**
         * @brief Get the sorted fields iterator.
         * 
         * @return field_iterator Iterator over document's fields.
         */
        field_iterator get_sorted_fields();

        /**
         * @brief Get all fields given a field_id.
         * 
         * @param field_id Field id.
         * @return field_iterator Iterator over document's fields.
         */
        field_iterator get_all_by_id(id_t field_id);

        /**
         * @brief Get the first field given a field_id.
         * 
         * @param field_id Field id.
         * @return field_iterator 
         */
        field_ptr get_first_by_id(id_t field_id);



    private:

        /**
         * @brief A document is represented as a vector of field unique pointers.
         * @todo Possibly it has a better way to represent this  polymorphic structure
         * rather than create an empty base class lol.
         */
        std::vector<field_ptr> _fields;

    };

}

#endif