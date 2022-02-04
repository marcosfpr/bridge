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

#ifndef NAMED_FIELD_DOCUMENT_HPP_
#define NAMED_FIELD_DOCUMENT_HPP_

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "common/serialization.hpp"
#include "schema/field_value.hpp"

#include "absl/container/btree_map.h"

namespace bridge::schema {

    using field_map = absl::btree_map<std::string, std::vector<field_value_v>>;

    struct named_field_document {
        field_map fields_by_name;

        /**
         * @brief Construct a new named field document object
         *
         */
        named_field_document() = default;

        /**
         * @brief Construct a new named field document object
         *
         * @param fields_by_name
         */
        explicit named_field_document(field_map &&fields_by_name) : fields_by_name(std::move(fields_by_name)) {}

        /**
         * @brief Destroy the named field document object
         *
         */
        ~named_field_document() = default;

        /**
         * @brief Converts a named_field_document type to a JSON.
         * @return A JSON object.
         */
        [[nodiscard]] serialization::json_t to_json() const {
            serialization::json_t json;

            for (const auto &[key, value] : fields_by_name) {
                serialization::json_t values_json;
                for (const auto &v : value) {
                    if (std::holds_alternative<string_value>(v)) {
                        string_value sv = std::get<string_value>(v);
                        values_json.push_back(*sv);
                    } else if (std::holds_alternative<uint32_value>(v)) {
                        uint32_value nv = std::get<uint32_value>(v);
                        values_json.push_back(*nv);
                    }
                }
                json[key] = values_json;
            }

            return json;
        }

        /**
         * @brief Converts a JSON to a named_field_document type.
         * @param json JSON object.
         * @return named_field_document object.
         */
        [[maybe_unused]] static named_field_document from_json(const serialization::json_t &json) {

            named_field_document nfd;
            for (auto &[key, values_json] : json.items()) {
                std::vector<field_value_v> values;
                for (auto &value : values_json) {

                    if (value.is_string()) {
                        values.emplace_back(string_value(value.get<std::string>()));
                    } else if (value.is_number()) {
                        values.emplace_back(uint32_value(value.get<uint32_t>()));
                    }
                }
                nfd.fields_by_name[key] = values;
            }

            return nfd;
        }
    };

} // namespace bridge::schema

#endif