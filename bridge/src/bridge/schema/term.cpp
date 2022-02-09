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

#include <boost/container_hash/hash.hpp>

#include "bridge/common/serialization.hpp"
#include "bridge/schema/options.hpp"
#include "bridge/schema/term.hpp"

namespace bridge::schema {

    /**
     * @brief Default constructor.
     */
    term::term() {
        // Creates a new term with empty body and a field_id (1st byte) equal to -1.
        // This is the default value for a term.
        // The term is not valid.

        this->data_.reserve(1);
        id_t invalid_id = 0;
        this->data_.push_back(static_cast<bridge::byte_t>(invalid_id));
    }

    /**
     * @brief Constructor.
     * @param data The data to be stored in the term.
     */
    [[maybe_unused]] term::term(bridge::byte_t *data, size_t size) {
        this->data_ = std::vector<bridge::byte_t>(data, data + size);
    }

    /**
     * @brief Copy construct a new term object
     *
     * @param other Other term
     */
    term::term(const term &other) { this->data_ = other.data_; }

    /**
     * @brief Copy assignment operator.
     *
     * @param other Term to be copied.
     * @return term& a new term.
     */
    term &term::operator=(const term &other) = default;

    /**
     * Move  constructor.
     * @param other The term to be moved.
     */
    term::term(term &&other) noexcept { this->data_ = std::move(other.data_); }

    /**
     * @brief  Move assignment operator.
     * @param other The term to be moved.
     * @return The term.
     */
    term &term::operator=(term &&other) noexcept {
        this->data_ = std::move(other.data_);
        return *this;
    }

    /**
     * @brief Equality operator.
     * @param other The term to be compared.
     * @return True if the terms are equal, false otherwise.
     */
    bool term::operator==(const term &other) const { return data_ == other.data_; }

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
    std::strong_ordering term::operator<=>(const term &other) const {
        // compare just the first byte that represents the field_id.
        return data_[0] <=> other.data_[0];
    }

    /**
     * @brief Overload output operator.
     */
    std::ostream &operator<<(std::ostream &os, const term &t) {
        os << "term(";
        for (const auto &b : t.data_) {
            os << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
        }
        os << ")" << std::endl;
        return os;
    }

    /**
     * @brief Hash function.
     * @return The hash value of the term.
     */
    size_t term::hash() const { return boost::hash_value(data_); }

    /**
     * @brief Get the field id.
     * @return The field id.
     */
    [[maybe_unused]] id_t term::get_field_id() const {
        // first byte of std::string _data is the field id
        return static_cast<id_t>(data_[0]);
    }

    /**
     * @brief Get the bytes iterator of the term.
     * @return The bytes iterator.
     */
    auto term::begin() const { return data_.begin(); }

    /**
     * @brief Get the end of term bytes iterator.
     * @return The end bytes iterator
     */
    auto term::end() const { return data_.end(); }

    /**
     * @brief Get the raw bytes of the term.
     *
     * @return const char* Raw bytes of the term.
     */
    const bridge::byte_t *term::as_ref() const { return data_.data(); }

    /**
         * @brief Get the number of bytes of the term.
         * @return The number of bytes of the term.
     */
    size_t term::size() const { return data_.size(); }

} // namespace bridge::schema