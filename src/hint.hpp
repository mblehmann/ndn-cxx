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
 * @author Paulo R Lanzarin <prlanzarin@inf.ufrgs.br>
 */

#ifndef NDN_HINT_HPP
#define NDN_HINT_HPP

#include "common.hpp"

#include "name.hpp"
#include "replication-selectors.hpp"
#include "util/time.hpp"
#include "management/nfd-local-control-header.hpp"
#include "tag-host.hpp"

namespace ndn {

/** @var const unspecified_duration_type DEFAULT_HINT_SCOPE
 *  @brief default value for scope 
 */
const int DEFAULT_HINT_SCOPE = 2;


/** @brief represents an Hint packet
 */
class Hint : public TagHost, public enable_shared_from_this<Hint>
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

  /** @brief Create a new Hint with an empty name (`ndn:/`)
   *  @warning In certain contexts that use Hint::shared_from_this(), Hint must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  Hint();

  /** @brief Create a new Hint with the given name
   *  @param name The name for the hint.
   *  @note This constructor allows implicit conversion from Name.
   *  @warning In certain contexts that use Hint::shared_from_this(), Hint must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  Hint(const Name& name);

  /** @brief Create a new Hint with the given name and scope value
   *  @param name The name for the hint.
   *  @param hintScope Scope for the hint.
   *  @warning In certain contexts that use Hint::shared_from_this(), Hint must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  Hint(const Name& name, const int& scope);

   /** @brief Create from wire encoding
   *  @warning In certain contexts that use Hint::shared_from_this(), Hint must be created
   *           using `make_shared`. Otherwise, .shared_from_this() will throw an exception.
   */
  explicit
  Hint(const Block& wire);

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
   * If there are hint selectors, this method will append "?" and add the selectors as
   * a query string.  For example, "/test/name?ndn.ChildSelector=1"
   */
  std::string
  toUri() const;

public: // matching
  /** @brief Check if Hint, including selectors, matches the given @p name
   *  @param name The name to be matched. If this is a Data name, it shall contain the
   *              implicit digest component
   */
  bool
  matchesName(const Name& name) const;

public: // Name and guiders
  const Name&
  getName() const
  {
    return m_name;
  }

  Hint&
  setName(const Name& name)
  {
    m_name = name;
    m_wire.reset();
    return *this;
  }

  const int&
  getScope() const
  {
    return m_scope;
  }

  Hint&
  setScope(const int& scope)
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

  Hint&
  setIncomingFaceId(uint64_t incomingFaceId)
  {
    getLocalControlHeader().setIncomingFaceId(incomingFaceId);
    // ! do not reset Hint's wire !
    return *this;
  }

  uint64_t
  getNextHopFaceId() const
  {
    return getLocalControlHeader().getNextHopFaceId();
  }

  Hint&
  setNextHopFaceId(uint64_t nextHopFaceId)
  {
    getLocalControlHeader().setNextHopFaceId(nextHopFaceId);
    // ! do not reset Hint's wire !
    return *this;
  }

public: // Selectors (ReplicationSelectors)
  /**
   * @return true if Hint has any selector present
   */
  bool
  hasSelectors() const
  {
    return !m_selectors.empty();
  }

  const ReplicationSelectors&
  getSelectors() const
  {
    return m_selectors;
  }

  Hint&
  setSelectors(const ReplicationSelectors& selectors)
  {
    m_selectors = selectors;
    m_wire.reset();
    return *this;
  }

  int
  getNodeID() const
  {
    return m_selectors.getNodeID();
  }

  Hint&
  setNodeID(int nodeID)
  {
    m_selectors.setNodeID(nodeID);
    m_wire.reset();
    return *this;
  }
  
  bool
  getInterested() const
  {
    return m_selectors.getInterested();
  }

  Hint&
  setInterested(bool interested)
  {
    m_selectors.setInterested(interested);
    m_wire.reset();
    return *this;
  }

  int
  getAvailability() const
  {
    return m_selectors.getAvailability();
  }

  Hint&
  setAvailability(int availability)
  {
    m_selectors.setAvailability(availability);
    m_wire.reset();
    return *this;
  }
  
public: // EqualityComparable concept
  bool
  operator==(const Hint& other) const
  {
    return wireEncode() == other.wireEncode();
  }

  bool
  operator!=(const Hint& other) const
  {
    return !(*this == other);
  }

private:
  Name m_name;
  int m_scope;
  ReplicationSelectors m_selectors;

  mutable Block m_wire;

  nfd::LocalControlHeader m_localControlHeader;
  friend class nfd::LocalControlHeader;
};

std::ostream&
operator<<(std::ostream& os, const Hint& hint);

inline std::string
Hint::toUri() const
{
  std::ostringstream os;
  os << *this;
  return os.str();
}

} // namespace ndn

#endif // NDN_HINT_HPP
