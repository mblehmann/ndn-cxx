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

#ifndef NDN_STRATEGY_SELECTORS_HPP
#define NDN_STRATEGY_SELECTORS_HPP

#include "common.hpp"
#include "exclude.hpp"

namespace ndn {

/**
 * @brief Abstraction implementing Strategy selectors
 */
class StrategySelectors
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

  StrategySelectors();

  /**
   * @brief Create from wire encoding
   */
  explicit
  StrategySelectors(const Block& wire);

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
  getScope() const
  {
    return m_scope;
  }

  StrategySelectors&
  setScope(int scope); 

  int
  getNodeId() const
  {
    return m_nodeId;
  }

  StrategySelectors&
  setNodeId(int nodeId); 

  bool
  getInterested() const
  {
    return m_interested;
  }

  StrategySelectors&
  setInterested(bool interested); 

  int
  getAvailability() const
  {
    return m_availability;
  }

  StrategySelectors&
  setAvailability(int availability);

public: // EqualityComparable concept
  bool
  operator==(const StrategySelectors& other) const;

  bool
  operator!=(const StrategySelectors& other) const
  {
    return !this->operator==(other);
  }

private:
  int m_scope;
  int m_nodeId;
  bool m_interested;
  int m_availability;

  mutable Block m_wire;
};

} // namespace ndn

#endif // NDN_STRATEGY_SELECTORS_HPP
