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

#include <iostream>
#include <optional>
#include <utility>

#include "../../include/error.hpp"
#include "../../include/schema/options.hpp"

namespace bridge::schema {

    /**
     * @brief Default constructor of text_indexing_option class. It creates an Unindexed option.
     */
    text_indexing_option::text_indexing_option() : _index_options(Unindexed) {}

    /**
     * @brief Copy constructor of text_indexing_option class.
     *
     * @param other Other text_indexing_option to be copied.
     */
    text_indexing_option::text_indexing_option(const text_indexing_option &other) = default;

    /**
     * @brief Copy assignment operator of text_indexing_option class.
     *
     * @param other Other text_indexing_option to be copied.
     * @return A new text_indexing_option with the same value as the other.
     */
    text_indexing_option &text_indexing_option::operator=(const text_indexing_option &other) = default;

    /**
     * @brief Move constructor of text_indexing_option class.
     *
     * @param other Other text_indexing_option to be moved.
     */
    text_indexing_option::text_indexing_option(text_indexing_option &&other) noexcept
        : _index_options(other._index_options) {}

    /**
     * @brief Move assignment operator of text_indexing_option class.
     *
     * @param other Other text_indexing_option to be moved.
     * @return A new text_indexing_option using the moved values from the other.
     */
    text_indexing_option &text_indexing_option::operator=(text_indexing_option &&other) noexcept {
        _index_options = other._index_options;
        return *this;
    }

    /**
     * @brief The | operator for text_indexing_option is responsible to combine two options.
     *
     * @param other Other text_indexing_option to be combined.
     * @return A new text_indexing_option with the combined values.
     */
    text_indexing_option text_indexing_option::operator|(const text_indexing_option &other) const {
        // if this == unindexed, return other
        if (_index_options == Unindexed) {
            return other;
        } else if (other._index_options == Unindexed || _index_options == other._index_options) {
            return *this;
        } else {
            throw bridge_error("Cannot combine indexing options");
        }
    }

    /**
     * @brief Compare two text_indexing_option objects.
     *
     * @param other Other text_indexing_option to be compared.
     * @return True if the two options are equal, false otherwise.
     */
    bool text_indexing_option::operator==(const text_indexing_option &other) const {
        return _index_options == other._index_options;
    }

    /**
     * @brief Compare two text_indexing_option objects.
     *
     * @param other Other text_indexing_option to be compared.
     * @return True if the two options are not equal, false otherwise.
     */
    bool text_indexing_option::operator!=(const text_indexing_option &other) const {
        return _index_options != other._index_options;
    }

    /**
     * @brief Three-way operator enables  strong ordering comparisons between text_indexing_option objects.
     *
     * @param other Other text_indexing_option to be compared.
     * @return Strong ordering value (Allow to use <, >, <=, >=).
     */
    std::strong_ordering text_indexing_option::operator<=>(const text_indexing_option &other) const {
        return _index_options <=> other._index_options;
    }

    /**
     * @brief Get the string representation of the text_indexing_option.
     *
     * @return The string representation of the text_indexing_option.
     */
    [[nodiscard]] std::string text_indexing_option::get_str() const {
        switch (_index_options) {
        case Unindexed:
            return "unindexed";
        case Untokenized:
            return "untokenized";
        case TokenizedNoFreq:
            return "tokenized_no_freq";
        case TokenizedWithFreq:
            return "tokenized_with_freq";
        case TokenizedWithFreqAndPosition:
            return "tokenized_with_freq_and_position";
        default:
            throw bridge_error("Unknown indexing option");
        }
    }

    /**
     * @brief Constructs a text_indexing_option from a string.
     *
     * @param str String representation of the text_indexing_option.
     * @return A new text_indexing_option.
     */
    [[nodiscard]] text_indexing_option text_indexing_option::from_str(const std::string &str) {
        if (str == "unindexed") {
            return Unindexed;
        } else if (str == "untokenized") {
            return Untokenized;
        } else if (str == "tokenized_no_freq") {
            return TokenizedNoFreq;
        } else if (str == "tokenized_with_freq") {
            return TokenizedWithFreq;
        } else if (str == "tokenized_with_freq_and_position") {
            return TokenizedWithFreqAndPosition;
        } else {
            throw bridge_error("Unknown indexing option");
        }
    }

    /**
     * @brief Get the hash value of the text_indexing_option.
     *
     * @return The hash value of the text_indexing_option.
     */
    [[nodiscard]] [[maybe_unused]] size_t text_indexing_option::hash() const {
        return std::hash<int>()(_index_options);
    }

    /**
     * @brief Default constructor.
     */
    text_field_option::text_field_option() : indexing_options(text_indexing_option::Unindexed), stored(false) {}

    /**
     * @brief Destructor
     */
    text_field_option::~text_field_option() = default;

    /**
     * @brief Constructor.
     *
     * @param index_option_value Indexing option value.
     * @param stored True if the field is stored, false otherwise.
     */
    text_field_option::text_field_option(text_indexing_option::Value index_option_value, bool stored)
        : indexing_options(index_option_value), stored(stored) {}

    /**
     * @brief Constructor.
     *
     * @param indexing_options Indexing option.
     * @param stored True if the field is stored, false otherwise.
     */
    text_field_option::text_field_option(text_indexing_option indexing_options, bool stored)
        : indexing_options(std::move(indexing_options)), stored(stored) {}

    /**
     * @brief Copy constructor.
     *
     * @param other Other text_field to be copied.
     */
    text_field_option::text_field_option(const text_field_option &other) = default;

    /**
     * @brief Copy assignment operator.
     *
     * @param other Other text_field to be copied.
     * @return A new text_field.
     */
    text_field_option &text_field_option::operator=(const text_field_option &other) = default;

    /**
     * @brief  Move constructor.
     *
     * @param other Other text_field to be moved.
     */
    text_field_option::text_field_option(text_field_option &&other) noexcept
        : indexing_options(std::move(other.indexing_options)), stored(other.stored) {}

    /**
     * @briief Move assignment operator.
     *
     * @param other Other text_field to be moved.
     * @return A new text_field.
     */
    text_field_option &text_field_option::operator=(text_field_option &&other) noexcept {
        indexing_options = other.indexing_options;
        stored = other.stored;
        return *this;
    }

    /**
     * @brief Equality operator.
     *
     * @param other Other text_field to be compared.
     * @return True if the two text_field are equal, false otherwise.
     */
    bool text_field_option::operator==(const text_field_option &other) const {
        return static_cast<const text_indexing_option>(indexing_options) == other.indexing_options &&
               stored == other.stored;
    }

    /**
     * @brief Inequality operator.
     *
     * @param other Other text_field to be compared.
     * @return True if the two text_field are not equal, false otherwise.
     */
    bool text_field_option::operator!=(const text_field_option &other) const { return !(*this == other); }

    /**
     * @brief Three-way operator.
     *
     * @param other Other text_field to be compared.
     * @return Strong ordering of the two text_field (Allow to use <, <=, >, >=).
     */
    std::strong_ordering text_field_option::operator<=>(const text_field_option &other) const {
        return indexing_options <=> other.indexing_options;
    }

    /**
     * @brief Set the indexing options.
     *
     * @param opt The new indexing options.
     */
    [[maybe_unused]] void text_field_option::set_indexing_options(text_indexing_option opt) {
        this->indexing_options = std::move(opt);
    }

    /**
     * @brief Set the stored.
     *
     * @param is_stored The new stored.
     */
    [[maybe_unused]] void text_field_option::set_stored(bool is_stored) { this->stored = is_stored; }

    /**
     * @brief The | operator allows to combine two text_field.
     *
     * @param other Other text_field to be combined.
     * @return A new text_field from the combination of the two text_field.
     */
    text_field_option text_field_option::operator|(const text_field_option &other) const {
        return {indexing_options | other.indexing_options, stored || other.stored};
    }

    /**
     * @brief Convert a text_field to a JSON object.
     * @return A JSON object.
     */
    [[nodiscard]] serialization::json_t text_field_option::to_json() const {
        serialization::json_t text_field_json = {{"indexing", indexing_options.get_str()}, {"stored", is_stored()}};
        return text_field_json;
    }

    /**
     * @brief Convert a JSON object to a text_field.
     * @param json A JSON object.
     * @return A text_field.
     */
    [[maybe_unused]] text_field_option text_field_option::from_json(const serialization::json_t &json) {
        if (json.find("indexing") == json.end()) {
            throw bridge_error("Missing indexing option");
        }
        if (json.find("stored") == json.end()) {
            throw bridge_error("Missing stored flag");
        }
        text_indexing_option indexing_option = text_indexing_option::from_str(json.at("indexing"));
        bool stored = json.at("stored").get<bool>();
        return {indexing_option, stored};
    }

    /**
     * @brief Returns a string representation of the text_field.
     * @return A string representation of the text_field.
     */
    [[nodiscard]] [[maybe_unused]] std::string text_field_option::get_name() { return "text"; }

    /**
     * @brief Default constructor.
     */
    numeric_field_option::numeric_field_option() : indexed(false), fast(false), stored(false) {}

    /**
     * @brief Destructor
     */
    numeric_field_option::~numeric_field_option() = default;

    /**
     * @brief Constructor.
     * @param indexed True if the field is indexed.
     * @param fast True if the field is fast.
     * @param stored True if the field is stored.
     */
    numeric_field_option::numeric_field_option(bool indexed, bool fast, bool stored) noexcept
        : indexed(indexed), fast(fast), stored(stored) {}

    /**
     * @brief Copy constructor.
     * @param other Other numeric_field to be copied.
     */
    numeric_field_option::numeric_field_option(const numeric_field_option &other) = default;

    /**
     * @brief Copy assignment operator.
     * @param other Other numeric_field to be copied.
     */
    numeric_field_option &numeric_field_option::operator=(const numeric_field_option &other) = default;

    /**
     * @brief Move constructor.
     * @param other Other numeric_field to be moved.
     */
    numeric_field_option::numeric_field_option(numeric_field_option &&other) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other Other numeric_field to be moved.
     */
    numeric_field_option &numeric_field_option::operator=(numeric_field_option &&other) noexcept = default;

    /**
     * @brief Equality operator.
     * @param other Other numeric_field to be compared.
     * @return True if the two numeric_field are equal.
     */
    bool numeric_field_option::operator==(const numeric_field_option &other) const {
        return indexed == other.indexed && fast == other.fast && stored == other.stored;
    }

    /**
     * @brief Inequality operator.
     * @param other Other numeric_field to be compared.
     * @return True if the two numeric_field are not equal.
     */
    bool numeric_field_option::operator!=(const numeric_field_option &other) const { return !(*this == other); }

    /**
     * @brief Set the indexed flag.
     * @param is_indexed True if the field is indexed.
     */
    [[maybe_unused]] void numeric_field_option::set_indexed(bool is_indexed) { this->indexed = is_indexed; }

    /**
     * @brief Set the fast flag.
     * @param is_fast True if the field is fast.
     */
    [[maybe_unused]] void numeric_field_option::set_fast(bool is_fast) { this->fast = is_fast; }

    /**
     * @brief Set the stored flag.
     * @param is_stored True if the field is stored.
     */
    [[maybe_unused]] void numeric_field_option::set_stored(bool is_stored) { this->stored = is_stored; }

    /**
     * @brief Convert the numeric_field to a JSON
     * @return JSON representation of the numeric_field.
     */
    [[nodiscard]] serialization::json_t numeric_field_option::to_json() const {
        serialization::json_t numeric_field_json = {
            {"indexed", is_indexed()}, {"fast", is_fast()}, {"stored", is_stored()}};
        return numeric_field_json;
    }

    /**
     * @brief Convert the numeric_field from a JSON
     * @param numeric_field_json JSON representation of the numeric_field.
     */
    [[maybe_unused]] numeric_field_option numeric_field_option::from_json(const serialization::json_t &json) {
        if (json.find("indexed") == json.end()) {
            throw bridge_error("Missing indexed flag");
        }
        if (json.find("fast") == json.end()) {
            throw bridge_error("Missing fast flag");
        }
        if (json.find("stored") == json.end()) {
            throw bridge_error("Missing stored flag");
        }
        bool indexed = json.at("indexed").get<bool>();
        bool fast = json.at("fast").get<bool>();
        bool stored = json.at("stored").get<bool>();
        return {indexed, fast, stored};
    }

    /**
     * @brief Get the numeric_field as a string.
     * @return String representation of the numeric_field.
     */
    [[nodiscard]] [[maybe_unused]] std::string numeric_field_option::get_name() { return "numeric"; }

} // namespace bridge::schema