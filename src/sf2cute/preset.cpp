/// @file
/// SoundFont 2 Preset class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/preset.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <functional>
#include <string>
#include <vector>

#include <sf2cute/preset_zone.hpp>
#include <sf2cute/file.hpp>

namespace sf2cute {

/// Constructs a new empty preset.
SFPreset::SFPreset() :
    preset_number_(0),
    bank_(0),
    library_(0),
    genre_(0),
    morphology_(0),
    parent_file_(nullptr) {
}

/// Constructs a new empty SFPreset using the specified name.
SFPreset::SFPreset(std::string name) :
    name_(std::move(name)),
    preset_number_(0),
    bank_(0),
    library_(0),
    genre_(0),
    morphology_(0),
    parent_file_(nullptr) {
}

/// Constructs a new SFPreset using the specified name and preset numbers.
SFPreset::SFPreset(std::string name, uint16_t preset_number, uint16_t bank) :
    name_(std::move(name)),
    preset_number_(std::move(preset_number)),
    bank_(std::move(bank)),
    library_(0),
    genre_(0),
    morphology_(0),
    parent_file_(nullptr) {
}

/// Constructs a new SFPreset using the specified name, preset numbers and zones.
SFPreset::SFPreset(std::string name,
    uint16_t preset_number,
    uint16_t bank,
    std::vector<SFPresetZone> zones) :
    name_(std::move(name)),
    preset_number_(std::move(preset_number)),
    bank_(std::move(bank)),
    library_(0),
    genre_(0),
    morphology_(0),
    zones_(),
    global_zone_(nullptr),
    parent_file_(nullptr) {
  // Set preset zones.
  zones_.reserve(zones.size());
  for (auto && zone : zones) {
    zones_.push_back(std::make_unique<SFPresetZone>(std::move(zone)));
  }

  // Repair references.
  SetBackwardReferences();
}

/// Constructs a new SFPreset using the specified name, preset numbers, zones and global zone.
SFPreset::SFPreset(std::string name,
    uint16_t preset_number,
    uint16_t bank,
    std::vector<SFPresetZone> zones,
    SFPresetZone global_zone) :
    name_(std::move(name)),
    preset_number_(std::move(preset_number)),
    bank_(std::move(bank)),
    library_(0),
    genre_(0),
    morphology_(0),
    zones_(),
    global_zone_(std::make_unique<SFPresetZone>(std::move(global_zone))),
    parent_file_(nullptr) {
  // Set preset zones.
  zones_.reserve(zones.size());
  for (auto && zone : zones) {
    zones_.push_back(std::make_unique<SFPresetZone>(std::move(zone)));
  }

  // Repair references.
  SetBackwardReferences();
}

/// Constructs a new copy of specified SFPreset.
SFPreset::SFPreset(const SFPreset & origin) :
    name_(origin.name_),
    preset_number_(origin.preset_number_),
    bank_(origin.bank_),
    library_(origin.library_),
    genre_(origin.genre_),
    morphology_(origin.morphology_),
    zones_(),
    global_zone_(nullptr),
    parent_file_(nullptr) {
  // Copy global zone.
  if (origin.has_global_zone()) {
    global_zone_ = std::make_unique<SFPresetZone>(origin.global_zone());
  }

  // Copy preset zones.
  zones_.reserve(origin.zones().size());
  for (const auto & zone : origin.zones()) {
    zones_.push_back(std::make_unique<SFPresetZone>(*zone));
  }

  // Repair references.
  SetBackwardReferences();
}

/// Copy-assigns a new value to the SFPreset, replacing its current contents.
SFPreset & SFPreset::operator=(const SFPreset & origin) {
  // Copy global zone.
  global_zone_ = nullptr;
  if (origin.has_global_zone()) {
    global_zone_ = std::make_unique<SFPresetZone>(origin.global_zone());
  }

  // Copy preset zones.
  zones_.clear();
  zones_.reserve(origin.zones().size());
  for (const auto & zone : origin.zones()) {
    zones_.push_back(std::make_unique<SFPresetZone>(*zone));
  }

  // Copy other fields.
  name_ = origin.name_;
  preset_number_ = origin.preset_number_;
  bank_ = origin.bank_;
  library_ = origin.library_;
  genre_ = origin.genre_;
  morphology_ = origin.morphology_;
  parent_file_ = nullptr;

  // Repair references.
  SetBackwardReferences();
  return *this;
}

/// Acquires the contents of specified SFPreset.
SFPreset::SFPreset(SFPreset && origin) noexcept :
    name_(std::move(origin.name_)),
    preset_number_(std::move(origin.preset_number_)),
    bank_(std::move(origin.bank_)),
    library_(std::move(origin.library_)),
    genre_(std::move(origin.genre_)),
    morphology_(std::move(origin.morphology_)),
    zones_(std::move(origin.zones_)),
    global_zone_(std::move(origin.global_zone_)),
    parent_file_(nullptr) {
  SetBackwardReferences();
}

/// Move-assigns a new value to the SFPreset, replacing its current contents.
SFPreset & SFPreset::operator=(SFPreset && origin) noexcept {
  name_ = std::move(origin.name_);
  preset_number_ = std::move(origin.preset_number_);
  bank_ = std::move(origin.bank_);
  library_ = std::move(origin.library_);
  genre_ = std::move(origin.genre_);
  morphology_ = std::move(origin.morphology_);
  zones_ = std::move(origin.zones_);
  global_zone_ = std::move(origin.global_zone_);
  parent_file_ = nullptr;
  SetBackwardReferences();
  return *this;
}

/// Returns the name of this preset.
const std::string & SFPreset::name() const noexcept {
  return name_;
}

/// Sets the name of this preset.
void SFPreset::set_name(std::string name) {
  name_ = std::move(name);
}

/// Returns the preset number.
uint16_t SFPreset::preset_number() const noexcept {
  return preset_number_;
}

/// Sets the preset number.
void SFPreset::set_preset_number(uint16_t preset_number) {
  preset_number_ = std::move(preset_number);
}

/// Returns the bank number.
uint16_t SFPreset::bank() const noexcept {
  return bank_;
}

/// Sets the bank number.
void SFPreset::set_bank(uint16_t bank) {
  bank_ = std::move(bank);
}

/// Returns the library.
uint32_t SFPreset::library() const noexcept {
  return library_;
}

/// Sets the library.
void SFPreset::set_library(uint32_t library) {
  library_ = std::move(library);
}

/// Returns the genre.
uint32_t SFPreset::genre() const noexcept {
  return genre_;
}

/// Sets the genre.
void SFPreset::set_genre(uint32_t genre) {
  genre_ = std::move(genre);
}

/// Returns the morphology.
uint32_t SFPreset::morphology() const noexcept {
  return morphology_;
}

/// Sets the morphology.
void SFPreset::set_morphology(uint32_t morphology) {
  morphology_ = std::move(morphology);
}

/// Returns the list of preset zones.
const std::vector<std::unique_ptr<SFPresetZone>> & SFPreset::zones() const noexcept {
  return zones_;
}

/// Adds a preset zone to the preset.
void SFPreset::AddZone(SFPresetZone zone) {
  // Check the parent preset of the zone.
  if (zone.has_parent_preset()) {
    if (&zone.parent_preset() == this) {
      // If the zone is already be owned by this preset, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. A zone cannot be shared by two parents.
      throw std::invalid_argument("Preset zone has already been owned by another preset.");
    }
  }

  // Set this preset to the parent instrument of the zone.
  zone.set_parent_preset(*this);

  // If the zone has an orphan instrument, add it to the parent file.
  if (has_parent_file()) {
    if (zone.has_instrument()) {
      parent_file().AddInstrument(zone.instrument());
    }
  }

  // Add the zone to the list.
  zones_.push_back(std::make_unique<SFPresetZone>(std::move(zone)));
}

/// Removes a preset zone from the preset.
void SFPreset::RemoveZone(
    std::vector<std::unique_ptr<SFPresetZone>>::const_iterator position) {
  zones_.erase(std::move(position));
}

/// Removes preset zones from the preset.
void SFPreset::RemoveZone(
    std::vector<std::unique_ptr<SFPresetZone>>::const_iterator first,
    std::vector<std::unique_ptr<SFPresetZone>>::const_iterator last) {
  zones_.erase(first, last);
}

/// Removes preset zones from the preset.
void SFPreset::RemoveZoneIf(
    std::function<bool(const std::unique_ptr<SFPresetZone> &)> predicate) {
  zones_.erase(std::remove_if(zones_.begin(), zones_.end(),
    [&predicate](const std::unique_ptr<SFPresetZone> & zone) -> bool {
    if (predicate(zone)) {
      return true;
    }
    else {
      return false;
    }
  }), zones_.end());
}

/// Removes all of the preset zones.
void SFPreset::ClearZones() noexcept {
  zones_.clear();
}

/// Returns true if the preset has a global zone.
bool SFPreset::has_global_zone() const noexcept {
  return static_cast<bool>(global_zone_);
}

/// Returns the global zone.
SFPresetZone & SFPreset::global_zone() const noexcept {
  return *global_zone_;
}

/// Sets the global zone.
void SFPreset::set_global_zone(SFPresetZone global_zone) {
  // Check the parent preset of the zone.
  if (global_zone.has_parent_preset()) {
    if (&global_zone.parent_preset() == this) {
      // If the zone is already be owned by this preset, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. A zone cannot be shared by two parents.
      throw std::invalid_argument("Preset zone has already been owned by another preset.");
    }
  }

  // Set this preset to the parent preset of the zone.
  global_zone.set_parent_preset(*this);

  // Set the global zone to this preset.
  global_zone_ = std::make_unique<SFPresetZone>(std::move(global_zone));
}

/// Resets the global zone.
void SFPreset::reset_global_zone() noexcept {
  global_zone_ = nullptr;
}

/// Returns true if the preset has a parent file.
bool SFPreset::has_parent_file() const noexcept {
  return parent_file_ != nullptr;
}

/// Returns the parent file.
SoundFont & SFPreset::parent_file() const noexcept {
  return *parent_file_;
}

/// Sets the parent file.
void SFPreset::set_parent_file(SoundFont & parent_file) noexcept {
  parent_file_ = &parent_file;
}

/// Resets the parent file.
void SFPreset::reset_parent_file() noexcept {
  parent_file_ = nullptr;
}

/// Sets backward references of every children elements.
void SFPreset::SetBackwardReferences() noexcept {
  // Update the preset zones.
  for (const auto & zone : zones_) {
    zone->set_parent_preset(*this);
  }

  // Update the global preset zone.
  if (has_global_zone()) {
    global_zone_->set_parent_preset(*this);
  }
}

} // namespace sf2cute
