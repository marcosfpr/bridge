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

#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include <utility>
#include <filesystem>

#include <boost/asio/buffered_write_stream.hpp>

#include "bridge/directory/error.hpp"
#include "bridge/directory/read_only_source.hpp"

namespace bridge::directory {

    using Path = std::filesystem::path;
    using Writer = boost::asio::buffered_write_stream<std::ostream>;

    /**
     * @brief Write-once many read (WORM) abstraction for where bridge's index should be stored.
     *
     * @details There is actually two implementations of Directory:
     * 1. The MMapDirectory, which uses mmap() to map the index into memory.
     * 2. The RAMDirectory, which is a test functionality that stores the index in RAM.
     */
     class Directory {
     public:
         /**
          * @brief Virtual destructor for Directory.
          */
       virtual ~Directory() = default;

       /**
        * @brief Operator << for Debbuging purposes.
        */
       friend std::ostream& operator<<(std::ostream& os, const Directory& dir) {
           throw std::runtime_error("No implementation for operator<< for Directory was found.");
       }


       /**
        * @brief Opens a virtual file for read.
        * @details Once  a file is opened, its data may not be modified.
        * Specifically, subsequent write or flush should have  no effect in the object.
        * @return read_only_source Read only source.
        */
       [[nodiscard]] virtual std::optional<std::unique_ptr<read_only_source>> open_read() const = 0;

       /**
        * @brief Removes a file
        * @details Removing a file will not affect eventual existing read_only_source pointing to it.
        */
       virtual void remove() = 0;

       /**
        * @brief Opens a virtual file for write.
        * @details Once a file is opened, its data may not be modified.
        * Specifically, subsequent write or flush should have  no effect in the object.
        * @return Writer.
        */
       [[nodiscard]] virtual std::unique_ptr<Writer> open_write() = 0;

       /**
        * @brief Atomically replace the content  of a file by data.
        * This calls ensure that reads can never 'observe' a partially written file.
        * The file may or may not previously exist.
        */
       virtual void replace_content(const Path& path, unsigned char* data, size_t length) = 0;


     };

} // namespace bridge::directory

#endif