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
// Defines a custom iterator over a vector of `field`s.

#ifndef ANALYZER_HPP_
#define ANALYZER_HPP_

#include <iostream>
#include <iterator>
#include <regex>
#include <string>

namespace bridge {

    namespace analyzer {

        using regex_token_iterator =
            std::regex_token_iterator<std::string::const_iterator>;

        struct simple_tokenizer {
          public:
            simple_tokenizer(std::string text) : text(text) {
                // alphanumeric regex
                this->pattern = std::regex("[a-zA-Z0-9]+");
                this->type = 0; // exact match
            }

            simple_tokenizer(std::string text, std::regex, int type)
                : text(text), pattern(pattern), type(type) {}

            regex_token_iterator begin() {
                return regex_token_iterator(text.begin(), text.end(), pattern,
                                            0);
            }
            regex_token_iterator end() { return regex_token_iterator(); }

          private:
            std::string text;
            std::regex pattern;
            int type;
        };

        /**
         * @brief Returns a token iterator over the given text.
         *
         * @param text Raw text data.
         * @return token_iterator Iterator over tokens of text.
         */
        simple_tokenizer tokenize(std::string text) {
            return simple_tokenizer(std::move(text));
        }
    }; // namespace analyzer

}; // namespace bridge

#endif // ANALYZER_HPP_