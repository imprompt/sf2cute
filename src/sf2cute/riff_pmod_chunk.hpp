/// @file
/// SoundFont "pmod" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_PMOD_CHUNK_HPP_
#define SF2CUTE_RIFF_PMOD_CHUNK_HPP_

#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include <sf2cute/types.hpp>
#include <sf2cute/modulator.hpp>

#include "riff.hpp"

namespace sf2cute {

class SFPreset;

/// The SFRIFFPmodChunk class represents a SoundFont 2 "pmod" chunk.
class SFRIFFPmodChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of pmod chunk, in terms of bytes.
  static constexpr size_type kItemSize = 10;

  /// Constructs a new empty SFRIFFPmodChunk.
  SFRIFFPmodChunk();

  /// Constructs a new SFRIFFPmodChunk using the specified presets.
  /// @param presets The presets of the chunk.
  SFRIFFPmodChunk(
      const std::vector<std::shared_ptr<SFPreset>> & presets);

  /// Constructs a new copy of specified SFRIFFPmodChunk.
  /// @param origin a SFRIFFPmodChunk object.
  SFRIFFPmodChunk(const SFRIFFPmodChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFPmodChunk, replacing its current contents.
  /// @param origin a SFRIFFPmodChunk object.
  SFRIFFPmodChunk & operator=(const SFRIFFPmodChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFPmodChunk.
  /// @param origin a SFRIFFPmodChunk object.
  SFRIFFPmodChunk(SFRIFFPmodChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFPmodChunk, replacing its current contents.
  /// @param origin a SFRIFFPmodChunk object.
  SFRIFFPmodChunk & operator=(SFRIFFPmodChunk && origin) = default;

  /// Destructs the SFRIFFPmodChunk.
  virtual ~SFRIFFPmodChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual const std::string & name() const override;

  /// @copydoc RIFFChunkInterface::set_name()
  virtual void set_name(std::string name) override;

  /// Returns the presets of this chunk.
  /// @return the presets of this chunk.
  const std::vector<std::shared_ptr<SFPreset>> &
      presets() const;

  /// Sets the presets of this chunk.
  /// @param presets the presets of this chunk.
  void set_presets(
      const std::vector<std::shared_ptr<SFPreset>> & presets);

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const override;

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error if the chunk size exceeds the maximum.
  virtual void Write(std::ostream & out) const override;

private:
  /// Returns the number of preset modulator items.
  /// @return the number of preset modulator items, including the terminator item.
  uint16_t NumItems() const;

  /// Writes an item of pmod chunk.
  /// @param out the output stream.
  /// @param source_op the source of data for the modulator.
  /// @param destination_op the destination of the modulator.
  /// @param amount the degree to which the source modulates the destination.
  /// @param amount_source_op the modulation source to be applied to the modulation amount.
  /// @param transform_op the transform type to be applied to the modulation source.
  /// @return the output stream.
  static std::ostream & WriteItem(std::ostream & out,
      SFModulator source_op,
      SFGenerator destination_op,
      int16_t amount,
      SFModulator amount_source_op,
      SFTransform transform_op);

  /// The name of the chunk.
  std::string name_;

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The presets of the chunk.
  const std::vector<std::shared_ptr<SFPreset>> * presets_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_PMOD_CHUNK_HPP_