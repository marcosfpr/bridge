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

    /** @brief Indexing options for text field.
    *  @details This class is used to specify the options associated with a
    *  text field. It has the following properties:
    * - Default constructible
    * - Copyable
    * - Equality comparable
    * - Moveable
    * - Hashable
    * - Partial ordering
    */
    class text_indexing_option {
      public:
        /**
         * @brief Enum for the text indexing options.
         */
        enum Value : uint8_t {
            Unindexed = 0,
            Untokenized = 1,
            TokenizedNoFreq = 2,
            TokenizedWithFreq = 3,
            TokenizedWithFreqAndPosition = 4,
        };

        /**
         * @brief Default constructor of text_indexing_option class. It creates an Unindexed option.
         */
        text_indexing_option() : _index_options(Unindexed) {}

        /**
         * @brief Constructor of text_indexing_option class. It creates an option with the given value.
         *
         * @param option Value of the option.
         */
        constexpr text_indexing_option(Value option) // NOLINT(google-explicit-constructor)
            : _index_options(option) {}

        /**
         * @brief Copy constructor of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be copied.
         */
        text_indexing_option(const text_indexing_option &other) = default;

        /**
         * @brief Copy assignment operator of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be copied.
         * @return A new text_indexing_option with the same value as the other.
         */
        text_indexing_option &operator=(const text_indexing_option &other) = default;

        /**
         * @brief Move constructor of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be moved.
         */
        text_indexing_option(text_indexing_option &&other) noexcept : _index_options(other._index_options) {}

        /**
         * @brief Move assignment operator of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be moved.
         * @return A new text_indexing_option using the moved values from the other.
         */
        text_indexing_option &operator=(text_indexing_option &&other) noexcept {
            _index_options = other._index_options;
            return *this;
        }

        /**
         * @brief Operator that allows enum values to be used as text_indexing_option.
         *
         * @return Current Value.
         */
        constexpr explicit operator Value() const { return _index_options; }

        /**
         * @brief Preventing usage of text_indexing_class in if statements.
         *
         * @return
         */
        explicit operator bool() = delete;

        /**
         * @brief Check if the option has term frequencies enabled.
         *
         * @return True if term frequencies are enabled, false otherwise.
         */
        [[nodiscard]] constexpr bool is_termfreq_enabled() const {
            return _index_options == TokenizedWithFreq || _index_options == TokenizedWithFreqAndPosition;
        }

        /**
         * @brief Check if the option has tokenization enabled.
         *
         * @return True if tokenization is enabled, false otherwise.
         */
        [[nodiscard]] constexpr bool is_tokenized() const {
            return _index_options == TokenizedNoFreq || _index_options == TokenizedWithFreq ||
                   _index_options == TokenizedWithFreqAndPosition;
        }

        /**
         * @brief Check if the option has positions enabled.
         *
         * @return True if positions are enabled, false otherwise.
         */
        [[nodiscard]] constexpr bool is_position_enabled() const {
            return _index_options == TokenizedWithFreqAndPosition;
        }

        /**
         * @brief Check if the option has index enabled.
         *
         * @return True if index is enabled, false otherwise.
         */
        [[nodiscard]] constexpr bool is_indexed() const {
            return _index_options != Unindexed;
        }

        /**
         * @brief The | operator for text_indexing_option is responsible to combine two options.
         *
         * @param other Other text_indexing_option to be combined.
         * @return A new text_indexing_option with the combined values.
         */
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

        /**
         * @brief Compare two text_indexing_option objects.
         *
         * @param other Other text_indexing_option to be compared.
         * @return True if the two options are equal, false otherwise.
         */
        bool operator==(const text_indexing_option &other) const { return _index_options == other._index_options; }

        /**
         * @brief Compare two text_indexing_option objects.
         *
         * @param other Other text_indexing_option to be compared.
         * @return True if the two options are not equal, false otherwise.
         */
        bool operator!=(const text_indexing_option &other) const { return _index_options != other._index_options; }

        /**
         * @brief Three-way operator enables  strong ordering comparisons between text_indexing_option objects.
         *
         * @param other Other text_indexing_option to be compared.
         * @return Strong ordering value (Allow to use <, >, <=, >=).
         */
        std::strong_ordering operator<=>(const text_indexing_option &other) const {
            return _index_options <=> other._index_options;
        }

        /**
         * @brief Get the string representation of the text_indexing_option.
         *
         * @return The string representation of the text_indexing_option.
         */
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

        /**
         * @brief Constructs a text_indexing_option from a string.
         *
         * @param str String representation of the text_indexing_option.
         * @return A new text_indexing_option.
         */
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

        /**
         * @brief Get the hash value of the text_indexing_option.
         *
         * @return The hash value of the text_indexing_option.
         */
        [[nodiscard]] size_t hash() const {
            return std::hash<int>()(_index_options);
        }

        friend class boost::serialization::access; //! < Allow serialization.

        /**
         * @brief Serialize the text_indexing_option.
         *
         * @tparam Archive Input/output archive.
         * @param ar Archive object.
         * @param version Current version of object.
         */
        template <class Archive>
        [[maybe_unused]] void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &_index_options;
        }

      private:
        Value _index_options;
    };

    /**
     * @brief Options associated with a text field.
     *
     * @details This class is used to specify the options associated with a
     *  text field. It has the following properties:
     * - Default constructible
     * - Copyable
     * - Equality comparable
     * - Moveable
     * - Partial ordering
     */
    struct text_field {

        /**
         * @brief Default constructor.
         */
        text_field() : indexing_options(text_indexing_option::Unindexed), stored(false) {}

        /**
         * @brief Constructor.
         *
         * @param index_option_value Indexing option value.
         * @param stored True if the field is stored, false otherwise.
         */
        text_field(text_indexing_option::Value index_option_value, bool stored)
            : indexing_options(index_option_value), stored(stored) {}

        /**
         * @brief Constructor.
         *
         * @param indexing_options Indexing option.
         * @param stored True if the field is stored, false otherwise.
         */
        text_field(text_indexing_option indexing_options, bool stored)
            : indexing_options(std::move(indexing_options)), stored(stored) {}

        /**
         * @brief Copy constructor.
         *
         * @param other Other text_field to be copied.
         */
        text_field(const text_field &other) = default;

        /**
         * @brief Copy assignment operator.
         *
         * @param other Other text_field to be copied.
         * @return A new text_field.
         */
        text_field &operator=(const text_field &other) = default;

        /**
         * @brief  Move constructor.
         *
         * @param other Other text_field to be moved.
         */
        text_field(text_field &&other) noexcept
            : indexing_options(std::move(other.indexing_options)), stored(other.stored) {}

        /**
         * @briief Move assignment operator.
         *
         * @param other Other text_field to be moved.
         * @return A new text_field.
         */
        text_field &operator=(text_field &&other) noexcept {
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
        bool operator==(const text_field &other) const {
            return static_cast<const text_indexing_option>(indexing_options) == other.indexing_options &&
                   stored == other.stored;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other Other text_field to be compared.
         * @return True if the two text_field are not equal, false otherwise.
         */
        bool operator!=(const text_field &other) const { return !(*this == other); }

        /**
         * @brief Three-way operator.
         *
         * @param other Other text_field to be compared.
         * @return Strong ordering of the two text_field (Allow to use <, <=, >, >=).
         */
        std::strong_ordering operator<=>(const text_field &other) const {
            return indexing_options <=> other.indexing_options;
        }

        /**
         * @brief Get the indexing_options.
         *
         * @return The indexing_options.
         */
        [[nodiscard]] text_indexing_option get_indexing_options() const { return indexing_options; }

        /**
         * @brief Get the stored.
         *
         * @return The stored.
         */
        [[nodiscard]]  constexpr bool is_stored() const { return stored; }

        /**
         * @brief Set the indexing options.
         *
         * @param opt The new indexing options.
         */
        [[maybe_unused]] void set_indexing_options(text_indexing_option opt) { this->indexing_options = opt; }

        /**
         * @brief Set the stored.
         *
         * @param is_stored The new stored.
         */
        [[maybe_unused]] void set_stored(bool is_stored) { this->stored = is_stored; }

        /**
         * @brief The | operator allows to combine two text_field.
         *
         * @param other Other text_field to be combined.
         * @return A new text_field from the combination of the two text_field.
         */
        text_field operator|(const text_field &other) const {
            return {indexing_options | other.indexing_options, stored || other.stored};
        }

        friend class boost::serialization::access; //! < Allow boost::serialization to access private members.

        /**
         * @brief Serialize a text_field.
         * @tparam Archive Input/output archive type.
         * @param ar Archive object.
         * @param version Current object version.
         */
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &indexing_options;
            ar &stored;
        }

        /**
         * @brief Convert a text_field to a JSON object.
         * @return A JSON object.
         */
        [[nodiscard]] serialization::json_t to_json() const {
            serialization::json_t text_field_json = {
                    {"indexing", indexing_options.get_str()},
                    {"stored", is_stored()}
            };
            return text_field_json;
        }

        /**
         * @brief Convert a JSON object to a text_field.
         * @param json A JSON object.
         * @return A text_field.
         */
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

        /**
         * @brief Returns a string representation of the text_field.
         * @return A string representation of the text_field.
         */
        [[nodiscard]] [[maybe_unused]] static std::string get_name() {
            return "text";
        }

      private:
        text_indexing_option indexing_options;
        bool stored;
    };

    /**
     * @brief Numeric field details.
     * @details This class is used to specify the options associated with a
     *   numeric field. It has the following properties:
     * - Default constructible
     * - Copyable
     * - Equality comparable
     * - Moveable
     * - Partial ordering
     */
    struct numeric_field {

        /**
         * @brief Default constructor.
         */
        numeric_field() : indexed(false), fast(false), stored(false) {}

        /**
         * @brief Constructor.
         * @param indexed True if the field is indexed.
         * @param fast True if the field is fast.
         * @param stored True if the field is stored.
         */
        numeric_field(bool indexed, bool fast, bool stored) : indexed(indexed), fast(fast), stored(stored) {}

        /**
         * @brief Copy constructor.
         * @param other Other numeric_field to be copied.
         */
        numeric_field(const numeric_field &other) = default;

        /**
         * @brief Copy assignment operator.
         * @param other Other numeric_field to be copied.
         */
        numeric_field &operator=(const numeric_field &other) = default;

        /**
         * @brief Move constructor.
         * @param other Other numeric_field to be moved.
         */
         numeric_field(numeric_field &&other) = default;

        /**
         * @brief Move assignment operator.
         * @param other Other numeric_field to be moved.
         */
         numeric_field &operator=(numeric_field &&other) = default;

        /**
         * @brief Equality operator.
         * @param other Other numeric_field to be compared.
         * @return True if the two numeric_field are equal.
         */
        bool operator==(const numeric_field &other) const {
            return indexed == other.indexed && fast == other.fast && stored == other.stored;
        }

        /**
         * @brief Inequality operator.
         * @param other Other numeric_field to be compared.
         * @return True if the two numeric_field are not equal.
         */
        bool operator!=(const numeric_field &other) const { return !(*this == other); }

        /**
         * @brief Check if the numeric field is indexed.
         * @return True if the numeric field is indexed.
         */
        [[maybe_unused]] [[nodiscard]] constexpr bool is_indexed() const { return indexed; }

        /**
         * @brief Check if the numeric field is fast.
         * @return True if the numeric field is fast.
         */
        [[maybe_unused]] [[nodiscard]] constexpr bool is_fast() const { return fast; }

        /**
         * @brief Check if the numeric field is stored.
         * @return True if the numeric field is stored.
         */
        [[maybe_unused]] [[nodiscard]] constexpr bool is_stored() const { return stored; }

        /**
         * @brief Set the indexed flag.
         * @param is_indexed True if the field is indexed.
         */
        [[maybe_unused]] void set_indexed(bool is_indexed) { this->indexed = is_indexed; }

        /**
         * @brief Set the fast flag.
         * @param is_fast True if the field is fast.
         */
        [[maybe_unused]] void set_fast(bool is_fast) { this->fast = is_fast; }

        /**
         * @brief Set the stored flag.
         * @param is_stored True if the field is stored.
         */
        [[maybe_unused]] void set_stored(bool is_stored) { this->stored = is_stored; }

        friend class boost::serialization::access; //! < Allow serialization.

        /**
         * @brief Serialize the numeric_field.
         * @tparam Archive Input/Output archive.
         * @param ar Archive object.
         * @param version Current version of the serialized data.
         */
        template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &indexed;
            ar &fast;
            ar &stored;
        }

        /**
         * @brief Convert the numeric_field to a JSON
         * @return JSON representation of the numeric_field.
         */
        [[nodiscard]] serialization::json_t to_json() const {
            serialization::json_t numeric_field_json = {
                    {"indexed", is_indexed()},
                    {"fast", is_fast()},
                    {"stored", is_stored()}
            };
            return numeric_field_json;
        }

        /**
         * @brief Convert the numeric_field from a JSON
         * @param numeric_field_json JSON representation of the numeric_field.
         */
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

        /**
         * @brief Get the numeric_field as a string.
         * @return String representation of the numeric_field.
         */
        [[nodiscard]] [[maybe_unused]] static std::string get_name() {
            return "numeric";
        }

      private:
        bool indexed, fast, stored;
    };

    /// @brief STRING text_field is untokenized and indexed
    static const text_field STRING = // NOLINT(cert-err58-cpp)
        text_field(text_indexing_option::Untokenized, false);

    /// @brief TEXT text_field is tokenized and indexed
    static const text_field TEXT = // NOLINT(cert-err58-cpp)
        text_field(text_indexing_option::TokenizedWithFreqAndPosition, false);

    /// @brief STORED text_field is only stored. Thus, it is useful just for query results.
    /// @details Reading the stored fields of a document is relatively slow.
    static const text_field STORED = // NOLINT(cert-err58-cpp)
        text_field(text_indexing_option::Unindexed, true);

    /// @brief FAST numeric_field is fast.
    static const numeric_field FAST = numeric_field(false, true, false);

    /// @brief INDEXED numeric_field is not fast.
    static const numeric_field NUMERIC = numeric_field(false, false, false);

} // namespace bridge::schema

#endif
