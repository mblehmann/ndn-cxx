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

#ifndef NDN_REPLICATION_SELECTORS_HPP
#define NDN_REPLICATION_SELECTORS_HPP

#include "common.hpp"
#include "exclude.hpp"

namespace ndn {

/**
 * @brief Abstraction implementing Hint and Vicinity selectors
 */
class ReplicationSelectors
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

  ReplicationSelectors();

  /**
   * @brief Create from wire encoding
   */
  explicit
  ReplicationSelectors(const Block& wire);

  bool
  empty() const;

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
   * @brief Decode the input from wire format
   */
  void
  wireDecode(const Block& wire);

public: // getters & setters for replication selectors

  int
  getNodeID() const
  {
    return m_nodeID;
  }

  ReplicationSelectors&
  setNodeID(int nodeID); 

  bool
  getInterested() const
  {
    return m_interested;
  }

  ReplicationSelectors&
  setInterested(bool interested); 

  int
  getAvailability() const
  {
    return m_availability;
  }

  ReplicationSelectors&
  setAvailability(int availability);

public: // EqualityComparable concept
  bool
  operator==(const ReplicationSelectors& other) const;

  bool
  operator!=(const ReplicationSelectors& other) const
  {
    return !this->operator==(other);
  }

private:
  int m_nodeID;
  bool m_interested;
  int m_availability;

  mutable Block m_wire;
};

} // namespace ndn

#endif // NDN_REPLICATION_SELECTORS_HPP
