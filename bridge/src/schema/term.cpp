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

#include <span>

#include <boost/container_hash/hash.hpp>

#include "../../include/common/serialization.hpp"
#include "../../include/schema/term.hpp"
#include "schema/options.hpp"

namespace bridge::schema {

    using mem::byte_t;
    using mem::bytes_buffer;
    using mem::bytes_container;
    using mem::bytes_iterator;

    /**
     * @brief Default constructor.
     */
    term::term() = default;

    /**
     * @brief Constructor.
     * @param data The data to be stored in the term.
     */
    term::term(bytes_container data)
        : _data(std::make_unique<bytes_buffer>(std::move(data))) {} // implicit conversion vector -> term_t

    /**
     * Move  constructor.
     * @param other The term to be moved.
     */
    term::term(term &&other) noexcept : _data(std::move(other._data)) {}

    /**
     * @brief  Move assignment operator.
     * @param other The term to be moved.
     * @return The term.
     */
    term &term::operator=(term &&other) noexcept {
        _data = std::move(other._data);
        return *this;
    }

    /**
     * @brief Equality operator.
     * @param other The term to be compared.
     * @return True if the terms are equal, false otherwise.
     */
    bool term::operator==(const term &other) const { return _data->bytes() == other._data->bytes(); }

    /**
     * @brief Inequality operator.
     * @brief other The term to be compared.
     * @return True if the terms are not equal, false otherwise.
     */
    bool term::operator!=(const term &other) const { return !(*this == other); }

    /**
     * @brief Three-way comparison operator.
     * @param other The term to be compared.
     * @return Strong ordering of the terms.  Allow to use operators <, >, <=, >=.
     */
    std::strong_ordering term::operator<=>(const term &other) const { return _data->bytes() <=> other._data->bytes(); }

    /**
     * @brief Overload output operator.
     */
    std::ostream &operator<<(std::ostream &os, const term &t) {

        // string representation of the term.data() iterator comma separated
        std::stringstream ss;
        for (auto it = t.begin(); it != t.end(); ++it) {
            if (it != t.begin()) {
                ss << ",";
            }
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*it);
        }

        os << "term(" << ss.str() << ")";
        return os;
    }

    /**
     * @brief Hash function.
     * @return The hash value of the term.
     */
    size_t term::hash() const {
        boost::hash<vector<byte>> bytes_hash;
        return bytes_hash(_data->bytes());
    }

    /**
     * @brief Get the field id.
     * @return The field id.
     */
    id_t term::get_field_id() const { return static_cast<id_t>(_data->bytes()[0]); }

    /**
     * @brief  Get the term data from an uint32_t.
     * @return A new term with the data.
     */
    term term::from_uint32(id_t field_id, uint32_t data) {
        bytes_container bytes(1 + 4);

        bytes_buffer buffer(bytes.data(), bytes.size());

        std::ostream os(&buffer);

        serialization::marshall(os, field_id);
        serialization::marshall(os, data);

        return term(std::move(bytes));
    }

    /**
     * @brief  Get the term data from an string
     * @return A new term with the data.
     */
    static term from_string(const std::string &data);

    /**
     * @brief Get the term from a raw array of bytes.
     * @return A new term with the data.
     */
    static term from_bytes(const byte *data, size_t size);

    /**
     * @brief Get the bytes iterator of the term.
     * @return The bytes slice.
     */
    bytes_iterator data() const;

} // namespace bridge::schema