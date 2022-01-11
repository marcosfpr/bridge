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
        auto operator<=>(const text_indexing_option &other) const { return _index_options <=> other._index_options; }

//        //! \brief Safe serialize the text indexing option.
//        [[maybe_unused]] std::optional<uint64_t> marshall(bridge::serialization::output_archive &out,
//                                                          const unsigned int version = 0) {
//            try {
//                out << _index_options;
//                // return the size of this.
//                return sizeof(*this);
//            } catch (bridge::serialization::invalid_archive & /*e*/) {
//                return std::nullopt;
//            }
//        }
//
//        //! \brief Safe deserialize the text indexing option.
//        [[maybe_unused]] static std::optional<text_indexing_option> unmarshall(bridge::serialization::input_archive &in,
//                                                                               const unsigned int version = 0) {
//            Value option;
//            try {
//                in >> option;
//                // return the size of this.
//                return text_indexing_option(option);
//            } catch (bridge::serialization::invalid_archive & /*e*/) {
//                return std::nullopt;
//            }
//        }

        // Allow  Hashing
        friend std::hash<text_indexing_option>;

        friend class boost::serialization::access;
        template <class Archive> [[maybe_unused]] void serialize(Archive &ar, const unsigned int version) {
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
    struct text_option {

        //! \brief Default constructor.
        text_option() : indexing_options(text_indexing_option::Unindexed), stored(false) {}

        //! \brief Constructor.
        text_option(text_indexing_option::Value index_option_value, bool stored)
            : indexing_options(index_option_value), stored(stored) {}

        //! \brief Constructor
        text_option(text_indexing_option indexing_options, bool stored)
            : indexing_options(std::move(indexing_options)), stored(stored) {}

        //! \brief Copy Constructor.
        text_option(const text_option &other) = default;

        //! \brief Assignment operator.
        text_option &operator=(const text_option &other) = default;

        //! \brief Move Constructor.
        text_option(text_option &&other) noexcept
            : indexing_options(std::move(other.indexing_options)), stored(other.stored) {}

        //! \brief Move Assignment operator.
        text_option &operator=(text_option &&other) noexcept {
            indexing_options = other.indexing_options;
            stored = other.stored;
            return *this;
        }

        //! \brief Equality operator.
        bool operator==(const text_option &other) const {
            return static_cast<const text_indexing_option>(indexing_options) == other.indexing_options &&
                   stored == other.stored;
        }

        //! \brief Inequality operator.
        bool operator!=(const text_option &other) const { return !(*this == other); }

        //! \brief Three  way comparison operator.
        auto operator<=>(const text_option &other) const { return indexing_options <=> other.indexing_options; }

        //! \brief Get index options
        [[nodiscard]] text_indexing_option get_indexing_options() const { return indexing_options; }

        //! \brief Get stored flag
        [[nodiscard]] bool is_stored() const { return stored; }

        //! \brief Set index options
        void set_indexing_options(text_indexing_option opt) { this->indexing_options = opt; }

        //! \brief Set stored flag
        void set_stored(bool is_stored) { this->stored = is_stored; }

        //! \brief Operator |
        text_option operator|(const text_option &other) const {
            return {indexing_options | other.indexing_options, stored || other.stored};
        }

//        //! \brief Safe serialize the text indexing option.
//        [[maybe_unused]] std::optional<uint64_t> marshall(bridge::serialization::output_archive &out,
//                                                          const unsigned int version = 0) {
//            try {
//                out << indexing_options << stored;
//                // return the size of this.
//                return sizeof(*this);
//            } catch (bridge::serialization::invalid_archive & /*e*/) {
//                return std::nullopt;
//            }
//        }
//
//        //! \brief Safe deserialize the text indexing option.
//        [[maybe_unused]] static std::optional<text_option> unmarshall(bridge::serialization::input_archive &in,
//                                                                               const unsigned int version = 0) {
//            text_indexing_option indexing_options;
//            bool stored;
//            try {
//                in >> indexing_options >> stored;
//                // return the size of this.
//                return text_option(std::move(indexing_options), stored);
//            } catch (bridge::serialization::invalid_archive & /*e*/) {
//                return std::nullopt;
//            }
//        }

        friend class boost::serialization::access;
        template <class Archive> void serialize(Archive &ar, const unsigned int version) { // NOLINT UnusedParameter
            ar &indexing_options;
            ar &stored;
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
    struct numeric_option {

        //! \brief Default constructor.
        numeric_option() : indexed(false), fast(false), stored(false) {}

        //! \brief Constructor.
        numeric_option(bool indexed, bool fast, bool stored) : indexed(indexed), fast(fast), stored(stored) {}

        //! \brief Copy Constructor.
        numeric_option(const numeric_option &other) = default;

        //! \brief Assignment operator.
        numeric_option &operator=(const numeric_option &other) = default;

        //! \brief Equality operator.
        bool operator==(const numeric_option &other) const {
            return indexed == other.indexed && fast == other.fast && stored == other.stored;
        }

        //! \brief Inequality operator.
        bool operator!=(const numeric_option &other) const { return !(*this == other); }

        //! \brief Get indexed flag
        [[maybe_unused]] [[nodiscard]] bool is_indexed() const { return indexed; }

        //! \brief Get fast flag
        [[maybe_unused]] [[nodiscard]] bool is_fast() const { return fast; }

        //! \brief Get stored flag
        [[maybe_unused]] [[nodiscard]] bool is_stored() const { return stored; }

        //! \brief Set indexed flag
        [[maybe_unused]] void set_indexed(bool is_indexed) { this->indexed = is_indexed; }

        //! \brief Set fast flag
        [[maybe_unused]] void set_fast(bool is_fast) { this->fast = is_fast; }

        //! \brief Set stored flag
        [[maybe_unused]] void set_stored(bool is_stored) { this->stored = is_stored; }


        friend class boost::serialization::access;
        template <class Archive> void serialize(Archive &ar, const unsigned int version) {
            ar &indexed;
            ar &fast;
            ar &stored;
        }
      private:
        bool indexed, fast, stored;
    };

    // STRING text_option will be untokenized and indexed
    static const text_option STRING = // NOLINT(cert-err58-cpp)
        text_option(text_indexing_option::Untokenized, false);

    // TEXT text_option will be tokenized and indexed
    static const text_option TEXT = // NOLINT(cert-err58-cpp)
        text_option(text_indexing_option::TokenizedWithFreqAndPosition, false);

    // A stored fields of a document can be retrieved given its DocId.
    // Stored field are stored together and compressed.
    // Reading the stored fields of a document is relatively slow.
    static const text_option STORED = // NOLINT(cert-err58-cpp)
        text_option(text_indexing_option::Unindexed, true);

    //! \brief FAST field will be tokenized and indexed
    static const numeric_option FAST = numeric_option(false, true, false);

} // namespace bridge::schema

#endif