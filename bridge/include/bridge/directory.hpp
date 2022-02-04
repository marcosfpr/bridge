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

#include <stdint.h>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"

namespace fs = boost::filesystem;

namespace bridge {

    typedef uint32_t segment_id_t;

    //! \brief An enumarion over segment components.
    enum segment_component {
        POSTING,  //!< The posting component.
        POSITIONS //!< The positions component.
    };

    //! \brief A struct that represents all segment-related metadata.
    struct segment_directory_metadata {
        segment_id_t segment_id; //!< The segment identification.
        fs::path index_path;     //!< The index path of that segment.
    };

    //! \brief A class that represents all segment-related operations.
    class segment_directory {
      public:
        //! \brief Default constructor.
        segment_directory() = default;

        //! \brief Constructor.
        segment_directory(segment_id_t segment_id, const fs::path index_path) {
            this->segment_info.segment_id = segment_id;
            this->segment_info.index_path = std::move(index_path);
        }

        //! \brief Destructor.
        ~segment_directory() = default;

        /**
         * @brief Get the path suffix string given a segment component.
         *
         * @param component Segment component enumeration.
         * @return char* The path suffix string.
         */
        const char *path_suffix(segment_component component) const {
            switch (component) {
            case POSTING:
                return ".pstgs";
            case POSITIONS:
                return ".pos";
            }
            throw std::invalid_argument("Invalid segment component type.");
        }

        /**
         * @brief Get the full segment file object.
         *
         * @param component Segment component enumeration.
         * @return fs::path Segment file object.
         */
        fs::path get_file(segment_component component) {
            std::string segment_id_str =
                std::to_string(this->segment_info.segment_id);
            std::string filename =
                segment_id_str + "." + this->path_suffix(component);
            return fs::path(this->segment_info.index_path / filename);
        }

        /**
         * @brief Get the full segment file stream object.
         *
         * @param component Segment component enumeration.
         * @return fs::ifstream Segment file object.
         */
        std::ifstream open(segment_component component) {
            return fs::ifstream(this->get_file(component));
        }

      private:
        segment_directory_metadata segment_info; //!< The segment information.

      private:
        // Avoid copy constructor and assignment operator.
        segment_directory(const segment_directory &) = delete;
        segment_directory &operator=(const segment_directory &) = delete;

      public:
        // Alowing move semantics

        //! \brief Move constructor.
        segment_directory(segment_directory &&other) noexcept {
            this->segment_info = std::move(other.segment_info);
        }

        //! \brief Move assignment operator.
        segment_directory &operator=(segment_directory &&other) noexcept {
            this->segment_info = std::move(other.segment_info);
            return *this;
        }
    };

    class index_directory {
      public:
        //! \brief Default constructor.
        index_directory() = default;

        //! \brief Constructor.
        index_directory(const fs::path index_path) {
            this->index_path = std::move(index_path);
        }

        //! \brief Destructor.
        ~index_directory() = default;

        /**
         * @brief Read a segment by id.
         *
         * @param segment_id Segment identification.
         * @return segment_directory Segment directory object.
         */
        segment_directory
        read_segment_directory(segment_id_t segment_id) const {
            return segment_directory(segment_id, this->index_path);
        }

      private:
        fs::path index_path; //!< The index path.

      private:
        // Avoid copy constructor and assignment operator.
        index_directory(const index_directory &) = delete;
        index_directory &operator=(const index_directory &) = delete;

      public:
        // Alowing move semantics

        //! \brief Move constructor.
        index_directory(index_directory &&other) noexcept {
            this->index_path = std::move(other.index_path);
        }

        //! \brief Move assignment operator.
        index_directory &operator=(index_directory &&other) noexcept {
            this->index_path = std::move(other.index_path);
            return *this;
        }
    };

}; // namespace bridge

#endif // DIRECTORY_HPP_