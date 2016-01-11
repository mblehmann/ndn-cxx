/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2013-2015 Regents of the University of California.
 *
 * This file is part of ndn-cxx library (NDN C++ library with eXperimental eXtensions).
 *
 * ndn-cxx library is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * ndn-cxx library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received copies of the GNU General Public License and GNU Lesser
 * General Public License along with ndn-cxx, e.g., in COPYING.md file.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * See AUTHORS.md for complete list of ndn-cxx authors and contributors.
 *
 * @author Matheus Lehmann <mblehmann@inf.ufrgs.br>
 */

#ifndef NDN_VICINITY_DATA_HPP
#define NDN_VICINITY_DATA_HPP

#include "common.hpp"

#include "name.hpp"
#include "management/nfd-local-control-header.hpp"
#include "tag-host.hpp"

namespace ndn {

class Data;

/** @var const unspecified_duration_type DEFAULT_VICINIYT_SCOPE;
 *  @brief default value for the packet scope
 */
const uint32_t DEFAULT_VICINIYT_SCOPE = 2;

/** @brief represents a Vicinity packet
 */
class VicinityData : public TagHost, public enable_shared_from_this<VicinityData>
{
public:
  class Error : public tlv::Error
  {
  public:
    explicit
    Error(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  /** @brief Create a new Vicinity with the given name
   *  @param name The name for the vicinity probe.
   *  @note This constructor allows implicit conversion from Name.
   *  @warning In certain contexts that use Vicinity::shared_from_this(), Vicinity must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  VicinityData(const Name& name);

  /** @brief Create a new Vicinity with the given name and vicinity scope
   *  @param name  The name for the vicinity probe.
   *  @param scope The vicinity scope.
   *  @warning In certain contexts that use Vicinity::shared_from_this(), Vicinity must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  VicinityData(const Name& name, uint32_t scope);

  /** @brief Create from wire encoding
   *  @warning In certain contexts that use Vicinity::shared_from_this(), Vicinity must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  explicit
  VicinityData(const Block& wire);

  /**
   * @brief Fast encoding or block size estimation
   */
  template<encoding::Tag TAG>
  size_t
  wireEncode(EncodingImpl<TAG>& encoder) const;

  /**
   * @brief Encode to a wire format
   */
  const Block&
  wireEncode() const;

  /**
   * @brief Decode from the wire format
   */
  void
  wireDecode(const Block& wire);

  /**
   * @brief Check if already has wire
   */
  bool
  hasWire() const
  {
    return m_wire.hasWire();
  }

  /**
   * @brief Encode the name according to the NDN URI Scheme
   *
   * If there are vicinity selectors, this method will append "?" and add the selectors as
   * a query string.  For example, "/test/name?ndn.ChildSelector=1"
   */
  std::string
  toUri() const;

public: // matching
  /** @brief Check if Vicinity, including selectors, matches the given @p name
   *  @param name The name to be matched. If this is a Data name, it shall contain the
   *              implicit digest component
   */
  bool
  matchesName(const Name& name) const;

  /**
   * @brief Check if Vicinity can be satisfied by @p data.
   *
   * This method considers Name
   *
   * @todo recognize implicit digest component
   */
  bool
  matchesData(const Data& data) const;

public: // Name and guiders
  const Name&
  getName() const
  {
    return m_name;
  }

  VicinityData&
  setName(const Name& name)
  {
    m_name = name;
    m_wire.reset();
    return *this;
  }

  uint32_t
  getScope()
  {
    return m_scope;
  }

  VicinityData&
  setScope(uint32_t scope)
  {
    m_scope = scope;
    m_wire.reset();
    return *this;
  }

public: // local control header
  nfd::LocalControlHeader&
  getLocalControlHeader()
  {
    return m_localControlHeader;
  }

  const nfd::LocalControlHeader&
  getLocalControlHeader() const
  {
    return m_localControlHeader;
  }

  uint64_t
  getIncomingFaceId() const
  {
    return getLocalControlHeader().getIncomingFaceId();
  }

  VicinityData&
  setIncomingFaceId(uint64_t incomingFaceId)
  {
    getLocalControlHeader().setIncomingFaceId(incomingFaceId);
    // ! do not reset Vicinity's wire !
    return *this;
  }

  uint64_t
  getNextHopFaceId() const
  {
    return getLocalControlHeader().getNextHopFaceId();
  }

  VicinityData&
  setNextHopFaceId(uint64_t nextHopFaceId)
  {
    getLocalControlHeader().setNextHopFaceId(nextHopFaceId);
    // ! do not reset Vicinity's wire !
    return *this;
  }

public: // EqualityComparable concept
  bool
  operator==(const VicinityData& other) const
  {
    return wireEncode() == other.wireEncode();
  }

  bool
  operator!=(const VicinityData& other) const
  {
    return !(*this == other);
  }

private:
  Name m_name;
  uint32_t m_scope;

  mutable Block m_wire;

  nfd::LocalControlHeader m_localControlHeader;
  friend class nfd::LocalControlHeader;
};

std::ostream&
operator<<(std::ostream& os, const Vicinity& vicinity);

inline std::string
Vicinity::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

} // namespace ndn

#endif // NDN_VICINITY_DATA_HPP
