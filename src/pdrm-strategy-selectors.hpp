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

#ifndef PDRM_STRATEGY_SELECTORS_HPP
#define PDRM_STRATEGY_SELECTORS_HPP

#include "common.hpp"
#include "exclude.hpp"

namespace ndn {

/**
 * @brief Abstraction implementing Strategy selectors
 */
class PDRMStrategySelectors
{
public:
  class PDRMError : public tlv::Error
  {
  public:
    explicit
    PDRMError(const std::string& what)
      : tlv::Error(what)
    {
    }
  };

  PDRMStrategySelectors();

  /**
   * @brief Create from wire encoding
   */
  explicit
  PDRMStrategySelectors(const Block& wire);

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

  int32_t
  getScope() const
  {
    return m_scope;
  }

  PDRMStrategySelectors&
  setScope(int32_t scope); 

  int32_t
  getNodeId() const
  {
    return m_nodeId;
  }

  PDRMStrategySelectors&
  setNodeId(int32_t nodeId); 

  int32_t
  getHomeNetwork() const
  {
    return m_homeNetwork;
  }

  PDRMStrategySelectors&
  setHomeNetwork(int32_t homeNetowk);

  int32_t
  getPreferredLocation() const
  {
    return m_preferredLocation;
  }

  PDRMStrategySelectors&
  setPreferredLocation(int32_t preferredLocation); 

  double
  getTimeSpentAtPreferredLocation() const
  {
    return m_timeSpentAtPreferredLocation;
  }

  PDRMStrategySelectors&
  setTimeSpentAtPreferredLocation(double timeSpentAtPreferredLocation);

  int32_t
  getCurrentPosition() const
  {
    return m_currentPosition;
  }

  PDRMStrategySelectors&
  setCurrentPosition(int32_t currentPosition); 

  double
  getAvailability() const
  {
    return m_availability;
  }

  PDRMStrategySelectors&
  setAvailability(double availability);

  bool
  getInterest() const
  {
    return m_interest;
  }

  PDRMStrategySelectors&
  setInterest(bool interest);

public: // EqualityComparable concept
  bool
  operator==(const PDRMStrategySelectors& other) const;

  bool
  operator!=(const PDRMStrategySelectors& other) const
  {
    return !this->operator==(other);
  }

private:
  int32_t m_scope;
  int32_t m_nodeId;
  int32_t m_homeNetwork;
  int32_t m_preferredLocation;
  double m_timeSpentAtPreferredLocation;
  int32_t m_currentPosition;
  double m_availability;
  bool m_interest;

  mutable Block m_wire;
};

} // namespace ndn

#endif // PDRM_STRATEGY_SELECTORS_HPP
