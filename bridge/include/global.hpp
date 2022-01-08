#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include<stdint.h>

namespace bridge {

  /// A `u32` identifying a document within a segment.
  /// Documents have their `DocId` assigned incrementally,
  /// as they are added in the segment.
  ///
  /// At most, a segment can contain 2^31 documents.
  using DocId = uint32_t;

}; // namespace bridge

#endif // GLOBAL_HPP_