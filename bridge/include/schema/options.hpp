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

#include <stdint.h>

#include "../common/serialization.hpp"
#include "../error.hpp"

namespace bridge {

    namespace field {

        //! \brief Options associated with a text field.
        struct text_option {

            //! \brief Default constructor.
            text_option()
                : indexing_options(text_indexing_option::Unindexed),
                  stored(false) {}

            //! \brief Constructor.
            text_option(text_indexing_option indexing_options, bool stored)
                : indexing_options(indexing_options), stored(stored) {}

            //! \brief Copy Constructor.
            text_option(const text_option &other)
                : indexing_options(other.indexing_options),
                  stored(other.stored) {}

            //! \brief Assignment operator.
            text_option &operator=(const text_option &other) {
                indexing_options = other.indexing_options;
                stored = other.stored;
                return *this;
            }

            //! \brief Equality operator.
            bool operator==(const text_option &other) const {
                return indexing_options == other.indexing_options &&
                       stored == other.stored;
            }

            //! \brief Inequality operator.
            bool operator!=(const text_option &other) const {
                return !(*this == other);
            }

            //! \brief Get index options
            text_indexing_option get_indexing_options() const {
                return indexing_options;
            }

            //! \brief Get stored flag
            bool is_stored() const { return stored; }

            //! \brief Set index options
            void set_indexing_options(text_indexing_option indexing_options) {
                this->indexing_options = indexing_options;
            }

            //! \brief Set stored flag
            void set_stored(bool stored) { this->stored = stored; }

            //! \brief Operator |
            text_option operator|(const text_option &other) const {
                return text_option(indexing_options | other.indexing_options,
                                   stored || other.stored);
            }

          private:
            friend class boost::serialization::access;
            template <class Archive>
            void serialize(Archive &ar, const unsigned int version) {
                ar & indexing_options;
                ar & stored;
            }

            text_indexing_option indexing_options;
            bool stored;
        };

        //! \brief Indexing options for text field.
        class text_indexing_option {
          public:
            enum Value : uint8_t {
                Unindexed,
                Untokenized,
                TokenizedNoFreq,
                TokenizedWithFreq,
                TokenizedWithFreqAndPosition
            };

            //! \brief Default constructor.
            text_indexing_option() : _index_options(Unindexed) {}

            //! \brief Constructor.
            constexpr text_indexing_option(Value option)
                : _index_options(option) {}

            //! \brief Copy constructor.
            text_indexing_option(const text_indexing_option &other)
                : _index_options(other._index_options) {}

            //! \brief Assignment operator.
            text_indexing_option &operator=(const text_indexing_option &other) {
                _index_options = other._index_options;
                return *this;
            }

            // Alow switch and comparisons
            constexpr operator Value() const { return _index_options; }

            // prevent usage if(fruit)
            explicit operator bool() = delete;

            constexpr bool is_termfreq_enabled() const {
                return _index_options == TokenizedWithFreq ||
                       _index_options == TokenizedWithFreqAndPosition;
            }

            constexpr bool is_tokenized() const {
                return _index_options == TokenizedNoFreq ||
                       _index_options == TokenizedWithFreq ||
                       _index_options == TokenizedWithFreqAndPosition;
            }

            constexpr bool is_position_enabled() const {
                return _index_options == TokenizedWithFreqAndPosition;
            }

            // operator |
            constexpr text_indexing_option
            operator|(const text_indexing_option &other) const {
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

          private:
            friend class boost::serialization::access;
            template <class Archive>
            void serialize(Archive &ar, const unsigned int version) {
                ar & _index_options;
            }
            Value _index_options;
        };

        // STRING text_option will be untokenized and indexed
        static text_option STRING =
            text_option(text_indexing_option::Untokenized, false);

        // TEXT text_option will be tokenized and indexed
        static text_option TEXT = text_option(
            text_indexing_option::TokenizedWithFreqAndPosition, false);

        // A stored fields of a document can be retrieved given its DocId.
        // Stored field are stored together and compressed.
        // Reading the stored fields of a document is relatively slow.
        static text_option STORED =
            text_option(text_indexing_option::Unindexed, true);

        //! \brief Numeric options
        struct numeric_option {

            //! \brief Default constructor.
            numeric_option() : indexed(false), fast(false), stored(false) {}

            //! \brief Constructor.
            numeric_option(bool indexed, bool fast, bool stored)
                : indexed(indexed), fast(fast), stored(stored) {}

            //! \brief Copy Constructor.
            numeric_option(const numeric_option &other)
                : indexed(other.indexed), fast(other.fast),
                  stored(other.stored) {}

            //! \brief Assignment operator.
            numeric_option &operator=(const numeric_option &other) {
                indexed = other.indexed;
                fast = other.fast;
                stored = other.stored;
                return *this;
            }

            //! \brief Equality operator.
            bool operator==(const numeric_option &other) const {
                return indexed == other.indexed && fast == other.fast &&
                       stored == other.stored;
            }

            //! \brief Inequality operator.
            bool operator!=(const numeric_option &other) const {
                return !(*this == other);
            }

            //! \brief Get indexed flag
            bool is_indexed() const { return indexed; }

            //! \brief Get fast flag
            bool is_fast() const { return fast; }

            //! \brief Get stored flag
            bool is_stored() const { return stored; }

            //! \brief Set indexed flag
            void set_indexed(bool indexed) { this->indexed = indexed; }

            //! \brief Set fast flag
            void set_fast(bool fast) { this->fast = fast; }

            //! \brief Set stored flag
            void set_stored(bool stored) { this->stored = stored; }

          private:
            friend class boost::serialization::access;
            template <class Archive>
            void serialize(Archive &ar, const unsigned int version) {
                ar & indexed;
                ar & fast;
                ar & stored;
            }

            bool indexed, fast, stored;
        };

        //! \brief FAST field will be tokenized and indexed
        static numeric_option FAST = numeric_option(false, true, false);

    } // namespace field

}; // namespace bridge

#endif
