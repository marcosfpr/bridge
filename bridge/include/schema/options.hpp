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

#ifndef BRIDGE_FIELD_OPTIONS_HPP_
#define BRIDGE_FIELD_OPTIONS_HPP_

#include <concepts>
#include <cstdint>
#include <iostream>
#include <optional>
#include <utility>

#include "../common/serialization.hpp"
#include "../error.hpp"

namespace bridge::schema {

    //! \brief Indexing options for text field.
    //  \details This class is used to specify the options associated with a
    //  text field. It has the following properties:
    // - Default constructible
    // - Copyable
    // - Equality comparable
    // - Moveable
    // - Hashable
    // - Partial ordering
    class text_indexing_option {
      public:
        enum Value : uint8_t {
            Unindexed = 0,
            Untokenized = 1,
            TokenizedNoFreq = 2,
            TokenizedWithFreq = 3,
            TokenizedWithFreqAndPosition = 4,
        };

        //! \brief Default constructor.
        text_indexing_option() : _index_options(Unindexed) {}

        //! \brief Constructor.
        constexpr text_indexing_option(Value option) // NOLINT(google-explicit-constructor)
            : _index_options(option) {}

        //! \brief Copy constructor.
        text_indexing_option(const text_indexing_option &other) = default;

        //! \brief Assignment operator.
        text_indexing_option &operator=(const text_indexing_option &other) = default;

        //! \brief Move constructor.
        text_indexing_option(text_indexing_option &&other) noexcept : _index_options(other._index_options) {}

        //! \brief Move assignment operator.
        text_indexing_option &operator=(text_indexing_option &&other) noexcept {
            _index_options = other._index_options;
            return *this;
        }

        // Alow switch and comparisons
        constexpr explicit operator Value() const { return _index_options; }

        // prevent usage if(fruit)
        explicit operator bool() = delete;

        [[nodiscard]] constexpr bool is_termfreq_enabled() const {
            return _index_options == TokenizedWithFreq || _index_options == TokenizedWithFreqAndPosition;
        }

        [[nodiscard]] constexpr bool is_tokenized() const {
            return _index_options == TokenizedNoFreq || _index_options == TokenizedWithFreq ||
                   _index_options == TokenizedWithFreqAndPosition;
        }

        [[nodiscard]] constexpr bool is_position_enabled() const {
            return _index_options == TokenizedWithFreqAndPosition;
        }

        [[nodiscard]] constexpr bool is_indexed() const {
            return _index_options != Unindexed;
        }

        // operator |
        text_indexing_option operator|(const text_indexing_option &other) const {
            // if this == unidexed, return other
            if (_index_options == Unindexed) {
                return other;
            } else if (other._index_options == Unindexed) {
                return *this;
            } else if (_index_options == other._index_options) {
                return *this;
            } else {
                throw bridge_error("Cannot combine indexing options");
            }
        }

        //!  \brief Equality operator.
        bool operator==(const text_indexing_option &other) const { return _index_options == other._index_options; }

        //!  \brief Inequality operator.
        bool operator!=(const text_indexing_option &other) const { return _index_options != other._index_options; }

        // Three-way comparison
        std::strong_ordering operator<=>(const text_indexing_option &other) const {
            return _index_options <=> other._index_options;
        }

        //! \brief Get string representation of indexing options.
        [[nodiscard]] std::string get_str() const {
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

        [[nodiscard]] static text_indexing_option from_str(const std::string & str) {
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

        // Allow  Hashing
        friend std::hash<text_indexing_option>;

        friend class boost::serialization::access;
        template <class Archive>
        [[maybe_unused]] void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &_index_options;
        }

      private:
        Value _index_options;
    };

    //! \brief Options associated with a text field.
    //  \details This class is used to specify the options associated with a
    //  text field. It has the following properties:
    // - Default constructible
    // - Copyable
    // - Equality comparable
    // - Moveable
    // - Partial ordering
    struct text_field {

        //! \brief Default constructor.
        text_field() : indexing_options(text_indexing_option::Unindexed), stored(false) {}

        //! \brief Constructor.
        text_field(text_indexing_option::Value index_option_value, bool stored)
            : indexing_options(index_option_value), stored(stored) {}

        //! \brief Constructor
        text_field(text_indexing_option indexing_options, bool stored)
            : indexing_options(std::move(indexing_options)), stored(stored) {}

        //! \brief Copy Constructor.
        text_field(const text_field &other) = default;

        //! \brief Assignment operator.
        text_field &operator=(const text_field &other) = default;

        //! \brief Move Constructor.
        text_field(text_field &&other) noexcept
            : indexing_options(std::move(other.indexing_options)), stored(other.stored) {}

        //! \brief Move Assignment operator.
        text_field &operator=(text_field &&other) noexcept {
            indexing_options = other.indexing_options;
            stored = other.stored;
            return *this;
        }

        //! \brief Equality operator.
        bool operator==(const text_field &other) const {
            return static_cast<const text_indexing_option>(indexing_options) == other.indexing_options &&
                   stored == other.stored;
        }

        //! \brief Inequality operator.
        bool operator!=(const text_field &other) const { return !(*this == other); }

        //! \brief Three  way comparison operator.
        std::strong_ordering operator<=>(const text_field &other) const {
            return indexing_options <=> other.indexing_options;
        }

        //! \brief Get index options
        [[nodiscard]] text_indexing_option get_indexing_options() const { return indexing_options; }

        //! \brief Get stored flag
        [[nodiscard]]  constexpr bool is_stored() const { return stored; }

        //! \brief Set index options
        [[maybe_unused]] void set_indexing_options(text_indexing_option opt) { this->indexing_options = opt; }

        //! \brief Set stored flag
        [[maybe_unused]] void set_stored(bool is_stored) { this->stored = is_stored; }

        //! \brief Operator |
        text_field operator|(const text_field &other) const {
            return {indexing_options | other.indexing_options, stored || other.stored};
        }

        friend class boost::serialization::access;
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &indexing_options;
            ar &stored;
        }

        //! \brief JSON serialization
        [[nodiscard]] serialization::json_t to_json() const {
            serialization::json_t text_field_json = {
                    {"indexing", indexing_options.get_str()},
                    {"stored", is_stored()}
            };
            return text_field_json;
        }

        //! \brief JSON deserialization
        [[maybe_unused]] static text_field from_json(const serialization::json_t &json) {
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

      private:
        text_indexing_option indexing_options;
        bool stored;
    };

    //! \brief Numeric options
    //  \details This class is used to specify the options associated with a
    //  numeric field. It has the following properties:
    // - Default constructible
    // - Copyable
    // - Equality comparable
    // - Moveable
    // - Partial ordering
    struct numeric_field {

        //! \brief Default constructor.
        numeric_field() : indexed(false), fast(false), stored(false) {}

        //! \brief Constructor.
        numeric_field(bool indexed, bool fast, bool stored) : indexed(indexed), fast(fast), stored(stored) {}

        //! \brief Copy Constructor.
        numeric_field(const numeric_field &other) = default;

        //! \brief Assignment operator.
        numeric_field &operator=(const numeric_field &other) = default;

        //! \brief Equality operator.
        bool operator==(const numeric_field &other) const {
            return indexed == other.indexed && fast == other.fast && stored == other.stored;
        }

        //! \brief Inequality operator.
        bool operator!=(const numeric_field &other) const { return !(*this == other); }

        //! \brief Get indexed flag
        [[maybe_unused]] [[nodiscard]] constexpr bool is_indexed() const { return indexed; }

        //! \brief Get fast flag
        [[maybe_unused]] [[nodiscard]] constexpr bool is_fast() const { return fast; }

        //! \brief Get stored flag
        [[maybe_unused]] [[nodiscard]] constexpr bool is_stored() const { return stored; }

        //! \brief Set indexed flag
        [[maybe_unused]] void set_indexed(bool is_indexed) { this->indexed = is_indexed; }

        //! \brief Set fast flag
        [[maybe_unused]] void set_fast(bool is_fast) { this->fast = is_fast; }

        //! \brief Set stored flag
        [[maybe_unused]] void set_stored(bool is_stored) { this->stored = is_stored; }

        friend class boost::serialization::access;
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &indexed;
            ar &fast;
            ar &stored;
        }

        //! \brief JSON serialization
        [[nodiscard]] serialization::json_t to_json() const {
            serialization::json_t numeric_field_json = {
                    {"indexed", is_indexed()},
                    {"fast", is_fast()},
                    {"stored", is_stored()}
            };
            return numeric_field_json;
        }

        //! \brief JSON deserialization
        [[maybe_unused]] static numeric_field from_json(const serialization::json_t &json) {
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

      private:
        bool indexed, fast, stored;
    };

    // STRING text_field will be untokenized and indexed
    static const text_field STRING = // NOLINT(cert-err58-cpp)
        text_field(text_indexing_option::Untokenized, false);

    // TEXT text_field will be tokenized and indexed
    static const text_field TEXT = // NOLINT(cert-err58-cpp)
        text_field(text_indexing_option::TokenizedWithFreqAndPosition, false);

    // A stored fields of a document can be retrieved given its DocId.
    // Stored field are stored together and compressed.
    // Reading the stored fields of a document is relatively slow.
    static const text_field STORED = // NOLINT(cert-err58-cpp)
        text_field(text_indexing_option::Unindexed, true);

    //! \brief FAST field will be fast.
    static const numeric_field FAST = numeric_field(false, true, false);

    //! \brief NUMERIC field will be the generic numeric option.
    static const numeric_field NUMERIC = numeric_field(false, false, false);

} // namespace bridge::schema

#endif
