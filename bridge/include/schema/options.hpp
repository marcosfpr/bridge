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

#include <compare>
#include <string>

#include "../common/serialization.hpp"

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
        text_indexing_option();

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
        text_indexing_option(const text_indexing_option &other);

        /**
         * @brief Copy assignment operator of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be copied.
         * @return A new text_indexing_option with the same value as the other.
         */
        text_indexing_option &operator=(const text_indexing_option &other);

        /**
         * @brief Move constructor of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be moved.
         */
        text_indexing_option(text_indexing_option &&other) noexcept;

        /**
         * @brief Move assignment operator of text_indexing_option class.
         *
         * @param other Other text_indexing_option to be moved.
         * @return A new text_indexing_option using the moved values from the other.
         */
        text_indexing_option &operator=(text_indexing_option &&other) noexcept;

        /**
         * @brief Operator that allows enum values to be used as text_indexing_option.
         *
         * @return Current Value.
         */
        constexpr explicit operator Value() const {
            return _index_options;
        }

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
        [[nodiscard]] constexpr bool is_indexed() const { return _index_options != Unindexed; }

        /**
         * @brief The | operator for text_indexing_option is responsible to combine two options.
         *
         * @param other Other text_indexing_option to be combined.
         * @return A new text_indexing_option with the combined values.
         */
        text_indexing_option operator|(const text_indexing_option &other) const;

        /**
         * @brief Compare two text_indexing_option objects.
         *
         * @param other Other text_indexing_option to be compared.
         * @return True if the two options are equal, false otherwise.
         */
        bool operator==(const text_indexing_option &other) const;

        /**
         * @brief Compare two text_indexing_option objects.
         *
         * @param other Other text_indexing_option to be compared.
         * @return True if the two options are not equal, false otherwise.
         */
        bool operator!=(const text_indexing_option &other) const;

        /**
         * @brief Three-way operator enables  strong ordering comparisons between text_indexing_option objects.
         *
         * @param other Other text_indexing_option to be compared.
         * @return Strong ordering value (Allow to use <, >, <=, >=).
         */
        std::strong_ordering operator<=>(const text_indexing_option &other) const;

        /**
         * @brief Get the string representation of the text_indexing_option.
         *
         * @return The string representation of the text_indexing_option.
         */
        [[nodiscard]] std::string get_str() const;

        /**
         * @brief Constructs a text_indexing_option from a string.
         *
         * @param str String representation of the text_indexing_option.
         * @return A new text_indexing_option.
         */
        [[nodiscard]] static text_indexing_option from_str(const std::string &str);

        /**
         * @brief Get the hash value of the text_indexing_option.
         *
         * @return The hash value of the text_indexing_option.
         */
        [[nodiscard]] [[maybe_unused]] size_t hash() const;

        /**
         * @brief Serialize the text_indexing_option object.
         *
         * @tparam Archive Input/output archive.
         * @param ar Archive object.
         * @param version Current version of object.
         */
        template <class Archive>
        [[maybe_unused]] void serialize(Archive &ar, [[maybe_unused]] const unsigned int version) {
            ar &_index_options;
        }

        friend boost::serialization::access; //! Allow to access the private members of text_indexing_option.

      private:
        /**
         * @brief The Value enumeration describes all indexing options available yet.
         */
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
    struct text_field_option {

        /**
         * @brief Default constructor.
         */
        text_field_option();

        /**
         * @brief Destructor
         */
        virtual ~text_field_option();

        /**
         * @brief Constructor.
         *
         * @param index_option_value Indexing option value.
         * @param stored True if the field is stored, false otherwise.
         */
        text_field_option(text_indexing_option::Value index_option_value, bool stored);

        /**
         * @brief Constructor.
         *
         * @param indexing_options Indexing option.
         * @param stored True if the field is stored, false otherwise.
         */
        text_field_option(text_indexing_option indexing_options, bool stored);

        /**
         * @brief Copy constructor.
         *
         * @param other Other text_field to be copied.
         */
        text_field_option(const text_field_option &other);

        /**
         * @brief Copy assignment operator.
         *
         * @param other Other text_field to be copied.
         * @return A new text_field.
         */
        text_field_option &operator=(const text_field_option &other);

        /**
         * @brief  Move constructor.
         *
         * @param other Other text_field to be moved.
         */
        text_field_option(text_field_option &&other) noexcept;

        /**
         * @briief Move assignment operator.
         *
         * @param other Other text_field to be moved.
         * @return A new text_field.
         */
        text_field_option &operator=(text_field_option &&other) noexcept;

        /**
         * @brief Equality operator.
         *
         * @param other Other text_field to be compared.
         * @return True if the two text_field are equal, false otherwise.
         */
        bool operator==(const text_field_option &other) const;

        /**
         * @brief Inequality operator.
         *
         * @param other Other text_field to be compared.
         * @return True if the two text_field are not equal, false otherwise.
         */
        bool operator!=(const text_field_option &other) const;

        /**
         * @brief Three-way operator.
         *
         * @param other Other text_field to be compared.
         * @return Strong ordering of the two text_field (Allow to use <, <=, >, >=).
         */
        std::strong_ordering operator<=>(const text_field_option &other) const;

        /**
         * @brief Get the indexing_options.
         *
         * @return The indexing_options.
         */
        [[nodiscard]] text_indexing_option get_indexing_options() const {  return indexing_options; }

        /**
         * @brief Get the stored.
         *
         * @return The stored.
         */
        [[nodiscard]] constexpr bool is_stored() const { return stored; }

        /**
         * @brief Set the indexing options.
         *
         * @param opt The new indexing options.
         */
        [[maybe_unused]] void set_indexing_options(text_indexing_option opt);

        /**
         * @brief Set the stored.
         *
         * @param is_stored The new stored.
         */
        [[maybe_unused]] void set_stored(bool is_stored);

        /**
         * @brief The | operator allows to combine two text_field.
         *
         * @param other Other text_field to be combined.
         * @return A new text_field from the combination of the two text_field.
         */
        text_field_option operator|(const text_field_option &other) const;

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

        friend boost::serialization::access; //! Allow to access the private members of text_field.

        /**
         * @brief Convert a text_field to a JSON object.
         * @return A JSON object.
         */
        [[nodiscard]] serialization::json_t to_json() const;

        /**
         * @brief Convert a JSON object to a text_field.
         * @param json A JSON object.
         * @return A text_field.
         */
        [[maybe_unused]] static text_field_option from_json(const serialization::json_t &json);

        /**
         * @brief Returns a string representation of the text_field.
         * @return A string representation of the text_field.
         */
        [[nodiscard]] [[maybe_unused]] static std::string get_name();

      private:
        /**
         * Holds convenience methods and holds information about the indexing options.
         */
        text_indexing_option indexing_options;
        bool stored; //! < True if the text_field is stored, false otherwise.
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
    struct numeric_field_option {

        /**
         * @brief Default constructor.
         */
        numeric_field_option();

        /**
         * @brief Destructor
         */
        virtual ~numeric_field_option();

        /**
         * @brief Constructor.
         * @param indexed True if the field is indexed.
         * @param fast True if the field is fast.
         * @param stored True if the field is stored.
         */
        numeric_field_option(bool indexed, bool fast, bool stored);

        /**
         * @brief Copy constructor.
         * @param other Other numeric_field to be copied.
         */
        numeric_field_option(const numeric_field_option &other);

        /**
         * @brief Copy assignment operator.
         * @param other Other numeric_field to be copied.
         */
        numeric_field_option &operator=(const numeric_field_option &other);

        /**
         * @brief Move constructor.
         * @param other Other numeric_field to be moved.
         */
        numeric_field_option(numeric_field_option &&other) noexcept;

        /**
         * @brief Move assignment operator.
         * @param other Other numeric_field to be moved.
         */
        numeric_field_option &operator=(numeric_field_option &&other) noexcept;

        /**
         * @brief Equality operator.
         * @param other Other numeric_field to be compared.
         * @return True if the two numeric_field are equal.
         */
        bool operator==(const numeric_field_option &other) const;

        /**
         * @brief Inequality operator.
         * @param other Other numeric_field to be compared.
         * @return True if the two numeric_field are not equal.
         */
        bool operator!=(const numeric_field_option &other) const;

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
        [[maybe_unused]] void set_indexed(bool is_indexed);

        /**
         * @brief Set the fast flag.
         * @param is_fast True if the field is fast.
         */
        [[maybe_unused]] void set_fast(bool is_fast);

        /**
         * @brief Set the stored flag.
         * @param is_stored True if the field is stored.
         */
        [[maybe_unused]] void set_stored(bool is_stored);

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
        [[nodiscard]] serialization::json_t to_json() const;

        /**
         * @brief Convert the numeric_field from a JSON
         * @param numeric_field_json JSON representation of the numeric_field.
         */
        [[maybe_unused]] static numeric_field_option from_json(const serialization::json_t &json);

        /**
         * @brief Get the numeric_field as a string.
         * @return String representation of the numeric_field.
         */
        [[nodiscard]] [[maybe_unused]] static std::string get_name();

      private:
        bool indexed, fast, stored;
    };

    /// @brief STRING text_field is untokenized and indexed
    static const text_field_option STRING = // NOLINT(cert-err58-cpp)
        text_field_option(text_indexing_option::Untokenized, false);

    /// @brief TEXT text_field is tokenized and indexed
    static const text_field_option TEXT = // NOLINT(cert-err58-cpp)
        text_field_option(text_indexing_option::TokenizedWithFreqAndPosition, false);

    /// @brief STORED text_field is only stored. Thus, it is useful just for query results.
    /// @details Reading the stored fields of a document is relatively slow.
    static const text_field_option STORED = // NOLINT(cert-err58-cpp)
        text_field_option(text_indexing_option::Unindexed, true);

    /// @brief FAST numeric_field is fast.
    static const numeric_field_option FAST = numeric_field_option(false, true, false);

    /// @brief INDEXED numeric_field is not fast.
    static const numeric_field_option NUMERIC = numeric_field_option(false, false, false);

} // namespace bridge::schema


#endif
