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

#ifndef READ_ONLY_SOURCE_HPP_
#define READ_ONLY_SOURCE_HPP_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <mio/shared_mmap.hpp>

namespace bridge::directory {

    /**
     * @brief Read object that represents files in bridge
     * @details These objects are only in charge to deliver the data
     * in the form of a constant read-only byte array.
     * Whatever happens to the directory file, the data hold by this
     * object should never be changed.
     */
    class read_only_source {
      public:
        /**
         * @brief Return a copy of the read-only source
         */
        [[nodiscard]] virtual std::unique_ptr<read_only_source> clone() const = 0;

        /**
         * @brief Return a sequence of bytes given the source
         */
        [[nodiscard]] virtual const unsigned char *deref() const = 0;

        /**
         * @brief Return the size of the source
         */
        [[nodiscard]] virtual size_t size() const = 0;

        /**
         * @brief Creates an empty source.
         */
        [[nodiscard]] static std::shared_ptr<read_only_source> empty() {
            throw io_error("Empty source is not implemented");
        }

        /**
         * @brief Creates a read_only_source that is just a view over a slice of the data.
         */
        [[nodiscard]] virtual std::unique_ptr<read_only_source> slice(size_t from_offset, size_t to_offset) const = 0;
    };

    class mmap_source : public read_only_source {
      public:
        /**
         * @brief Constructs a mmap_source from a file
         * @warning The file must exist and be readable
         */
        explicit mmap_source(std::string path) : path_(std::move(path)) {
            std::error_code error;
            mmap_ = std::make_unique<mio::shared_ummap_source>(std::move(mio::make_mmap_source(path_, error)));
            if (error) {
                throw io_error(error.message());
            }
        }

        /**
         * @brief Constructs a mmap_source from a file
         * @warning The file must exist and be readable
         */
        explicit mmap_source(std::string path, size_t offset, size_t size) : path_(std::move(path)) {
            std::error_code error;
            mmap_ = std::make_unique<mio::shared_ummap_source>(
                std::move(mio::make_mmap_source(path_, offset, size, error)));
            if (error) {
                throw io_error(error.message());
            }
        }

        /**
         * @brief Constructs a mmap_source from a path and a mmap for the path.
         * @warning Use clone() instead.
         */
        explicit mmap_source(std::string path, const mio::shared_ummap_source &mmap)
            : path_(std::move(path)), mmap_(std::make_unique<mio::shared_ummap_source>(mmap)) {}

        /**
         * @brief Return a copy of the read-only source
         */
        [[nodiscard]] std::unique_ptr<read_only_source> clone() const override {
            return std::make_unique<mmap_source>(path_, *mmap_);
        }

        /**
         * @brief Return a sequence of bytes given the source
         */
        [[nodiscard]] const unsigned char *deref() const override { return mmap_->data(); }

        /**
         * @brief Return the size of the source
         */
        [[nodiscard]] size_t size() const override { return mmap_->size(); }

        /**
         * @brief Creates an empty source.
         */
        [[nodiscard]] static std::shared_ptr<read_only_source> empty() {
            throw io_error("Empty source is not implemented for mmap_source");
        }

        /**
         * @brief Creates a read_only_source that is just a view over a slice of the data.
         */
        [[nodiscard]] std::unique_ptr<read_only_source> slice(size_t from_offset, size_t to_offset) const override {
            return std::make_unique<mmap_source>(path_, from_offset, to_offset);
        }

      private:
        std::unique_ptr<mio::shared_ummap_source> mmap_;
        std::string path_;
    };

    class in_memory_source : public read_only_source {
      public:
        /**
         * @brief Constructs a in_memory_source from a file
         * @warning The file must exist and be readable
         */
        explicit in_memory_source(std::vector<unsigned char> data) : data_(std::move(data)) {}

        /**
         * @brief Constructs an in_memory_source from a raw pointer and a size.
         * @param data raw pointer to the data
         * @param size size of the data
         */
        explicit in_memory_source(const char *data, size_t size) {
            data_.resize(size);
            std::copy(data, data + size, data_.begin());
        }

        /**
         * @brief Return a copy of the read-only source
         */
        [[nodiscard]] std::unique_ptr<read_only_source> clone() const override {
            return std::make_unique<in_memory_source>(data_);
        }

        /**
         * @brief Return a sequence of bytes given the source
         */
        [[nodiscard]] const unsigned char *deref() const override { return data_.data(); }

        /**
         * @brief Return the size of the source
         */
        [[nodiscard]] size_t size() const override { return data_.size(); }

        /**
         * @brief Creates an empty source.
         */
        [[nodiscard]] static std::shared_ptr<read_only_source> empty() {
            return std::make_shared<in_memory_source>(std::vector<unsigned char>());
        }

        /**
         * @brief Creates a read_only_source that is just a view over a slice of the data.
         */
        [[nodiscard]] std::unique_ptr<read_only_source> slice(size_t from_offset, size_t to_offset) const override {
            return std::make_unique<in_memory_source>(std::vector<unsigned char>(
                data_.begin() + from_offset,               // NOLINT(cppcoreguidelines-narrowing-conversions)
                data_.begin() + from_offset + to_offset)); // NOLINT(cppcoreguidelines-narrowing-conversions)
        }

      private:
        std::vector<unsigned char> data_;
    };

} // namespace bridge::directory

#endif