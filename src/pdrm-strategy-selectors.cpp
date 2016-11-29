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

#include "pdrm-strategy-selectors.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/block-helpers.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<PDRMStrategySelectors>));
BOOST_CONCEPT_ASSERT((WireEncodable<PDRMStrategySelectors>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<PDRMStrategySelectors>));
BOOST_CONCEPT_ASSERT((WireDecodable<PDRMStrategySelectors>));
static_assert(std::is_base_of<tlv::Error, PDRMStrategySelectors::PDRMError>::value,
              "PDRMStrategySelectors::PDRMError must inherit from tlv::Error");

PDRMStrategySelectors::PDRMStrategySelectors()
  : m_scope(-1)
  , m_nodeId(-1)
  , m_homeNetwork(-1)
  , m_preferredLocation(-1)
  , m_timeSpentAtPreferredLocation(-1)
  , m_currentPosition(-1)
  , m_availability(-1)
  , m_interest(false)
{
}

PDRMStrategySelectors::PDRMStrategySelectors(const Block& wire)
{
  wireDecode(wire);
}

bool
PDRMStrategySelectors::empty() const
{
  return m_nodeId == -1 && m_homeNetwork == -1 && m_preferredLocation == -1 && m_timeSpentAtPreferredLocation == -1
         && m_currentPosition == -1 && m_availability == -1 && !m_interest;
}

template<encoding::Tag TAG>
size_t
PDRMStrategySelectors::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;
  // PDRMStrategySelectors ::= PDRM-STRATEGY-SELECTORS-TYPE TLV-LENGTH
  //                              Scope?
  //                              NodeId?
  //                              HomeNetwork?
  //                              PreferredLocation?
  //                              TimeSpentAtPreferredLocation?
  //                              CurrentPosition?
  //                              Availability?
  //                              Interest?
  // (reverse encoding)

  if (getInterest()) {
    totalLength += prependEmptyBlock(encoder, tlv::PDRMInterest);
  }

  if (getAvailability() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMAvailability, getAvailability());
  }

  if (getCurrentPosition() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMCurrentPosition, getCurrentPosition());
  }

  if (getTimeSpentAtPreferredLocation() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMTimeSpentAtPreferredLocation, getTimeSpentAtPreferredLocation());
  }

  if (getPreferredLocation() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMPreferredLocation, getPreferredLocation());
  }

  if (getHomeNetwork() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMHomeNetwork, getHomeNetwork());
  }

  if (getNodeId() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMNodeId, getNodeId());
  }

  if (getScope() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::PDRMScope, getScope());
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::PDRMStrategySelectors);
  return totalLength;
}

template size_t
PDRMStrategySelectors::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
PDRMStrategySelectors::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
PDRMStrategySelectors::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();
  return m_wire;
}

void
PDRMStrategySelectors::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::PDRMStrategySelectors)
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV type when decoding PDRMStrategySelectors"));

  *this = PDRMStrategySelectors();

  m_wire = wire;
  m_wire.parse();

  Block::element_const_iterator val = m_wire.find(tlv::PDRMScope);
  if (val != m_wire.elements_end()) {
    m_scope = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMNodeId);
  if (val != m_wire.elements_end()) {
    m_nodeId = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMHomeNetwork);
  if (val != m_wire.elements_end()) {
    m_homeNetwork = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMPreferredLocation);
  if (val != m_wire.elements_end()) {
    m_preferredLocation = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMTimeSpentAtPreferredLocation);
  if (val != m_wire.elements_end()) {
    m_timeSpentAtPreferredLocation = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMCurrentPosition);
  if (val != m_wire.elements_end()) {
    m_currentPosition = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMAvailability);
  if (val != m_wire.elements_end()) {
    m_availability = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::PDRMInterest);
  if (val != m_wire.elements_end()) {
    m_interest = true;
  }

}

PDRMStrategySelectors&
PDRMStrategySelectors::setScope(int32_t scope)
{
  m_scope = scope;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setNodeId(int32_t nodeId)
{
  m_nodeId = nodeId;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setHomeNetwork(int32_t homeNetwork)
{
  m_homeNetwork = homeNetwork;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setPreferredLocation(int32_t preferredLocation)
{
  m_preferredLocation = preferredLocation;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setTimeSpentAtPreferredLocation(double timeSpentAtPreferredLocation)
{
  m_timeSpentAtPreferredLocation = timeSpentAtPreferredLocation;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setCurrentPosition(int32_t currentPosition)
{
  m_currentPosition = currentPosition;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setAvailability(double availability)
{
  m_availability = availability;
  m_wire.reset();
  return *this;
}

PDRMStrategySelectors&
PDRMStrategySelectors::setInterest(bool interest)
{
  m_interest = interest;
  m_wire.reset();
  return *this;
}

bool
PDRMStrategySelectors::operator==(const PDRMStrategySelectors& other) const
{
  return wireEncode() == other.wireEncode();
}

} // namespace ndn
