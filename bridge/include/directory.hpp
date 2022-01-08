#ifndef DIRECTORY_HPP_
#define DIRECTORY_HPP_

#include <stdint.h>
#include <string>

#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"

namespace fs = boost::filesystem;

namespace bridge
{

  enum segment_component
  {
    POSTING,
    POSITIONS
  };

  struct segment_directory_metadata
  {
    uint32_t segment_id;
    fs::path index_path;
  };

  class segment_directory
  {
  public:
    char *path_suffix(segment_component component) const
    {
      switch (component)
      {
      case POSTING:
        return ".pstgs";
      case POSITIONS:
        return ".pos";
      }
      throw std::invalid_argument("Invalid segment component type.");
    }

    fs::path get_file(segment_component component)
    {
      std::string segment_id_str = std::to_string(this->segment_info.segment_id);
      std::string filename = segment_id_str + "." + this->path_suffix(component);
      return fs::path(this->segment_info.index_path / filename);
    }

    std::ifstream open(segment_component component)
    {
      return fs::ifstream(this->get_file(component));
    }

  private:
    segment_directory_metadata segment_info;
  };

  struct index_directory
  {
  };

}; // namespace bridge

#endif // DIRECTORY_HPP_