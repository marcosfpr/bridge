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

//! \brief Defines a custom iterator over a string based on regular expressions.

#ifndef REGEX_ANALYZER_HPP_
#define REGEX_ANALYZER_HPP_

#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <utility>

namespace bridge {

    namespace analyzer {

        /**
         * @brief Tokenizer concept.
         *  This is a concept that defines a tokenizer.
         *  A tokenizer is an iterator over strings.
         */
        template <typename T>
        concept Tokenizer =
            std::input_iterator<T>;  // TODO: constrain to string iterators

        /**
         * @brief Generic tokenizer class.
         *
         * @tparam T Tokenizer type
         */
        template <Tokenizer T> class regex_tokenizer {
          public:
            using iterator = T;
            using value_type = typename T::value_type;
            using reference = typename T::reference;
            using pointer = typename T::pointer;
            using difference_type = typename T::difference_type;
            using iterator_category = typename T::iterator_category;

            regex_tokenizer(std::string text, std::regex pattern, int type)
                : text_(std::move(text)), pattern_(std::move(pattern)), type_(type) {}

            iterator begin() {
                // getting args template as values
                return T(text_.begin(), text_.end(), pattern_, type_);
            }

            iterator end() { return T(); }

          private:
            std::string text_;
            std::regex pattern_;
            int type_;
        };

        // Defines a custom regex iterator over strings.
        using regex_token_iterator =
            std::regex_token_iterator<std::string::const_iterator>;

        /**
         * @brief Alphanumeric regex tokenizer
         *
         */
        static std::regex alphanumeric  = std::regex("[a-zA-Z0-9]+"); // NOLINT(cert-err58-cpp)

        class alphanumeric_tokenizer
            : public regex_tokenizer<regex_token_iterator> {
            public:
                explicit alphanumeric_tokenizer(std::string text)
                    : regex_tokenizer(std::move(text), alphanumeric, 0) {}
        };

    }; // namespace analyzer

}; // namespace bridge

#endif // REGEX_ANALYZER_HPP_