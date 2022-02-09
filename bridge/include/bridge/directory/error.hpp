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

#ifndef BRIDGE_DIRECTORY_ERROR_HPP_
#define BRIDGE_DIRECTORY_ERROR_HPP_

namespace bridge::directory {
    /**
     * @brief IO error thrown when a directory operation fails
     */
    struct io_error : public std::runtime_error {
        explicit io_error(const std::string &what)
            : std::runtime_error("Exception bridge::io_error thrown. Reason: " + what) {}
    };

    /**
     * @brief Thrown when a file already exists
     */
    struct file_already_exists : public io_error {
        explicit file_already_exists() : io_error("File already exists") {}
    };

    /**
     * @brief Thrown when a file does not exist
     */
    struct file_not_found : public io_error {
        explicit file_not_found() : io_error("File not found") {}
    };

    enum open_directory_error_type {
        directory_not_found,
        not_a_directory,
    };

    struct open_directory_error : public io_error {
        explicit open_directory_error(open_directory_error_type type) : io_error(get_what(type)) {}

        static std::string get_what(open_directory_error_type type) {
            switch (type) {
                case directory_not_found:
                    return "Directory not found";
                case not_a_directory:
                    return "Not a directory";
                default:
                    return "Unknown error";
            }
        }
    };

    /**
     * @brief Thrown when some unknown error occurs
     */
    struct file_error : public std::runtime_error {
        explicit file_error(const std::string &what)
            : std::runtime_error("Exception bridge::file_error thrown. Reason: " + what) {}
    };

} // namespace bridge::directory

#endif