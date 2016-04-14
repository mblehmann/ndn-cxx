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

#include "strategy-selectors.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/block-helpers.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<StrategySelectors>));
BOOST_CONCEPT_ASSERT((WireEncodable<StrategySelectors>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<StrategySelectors>));
BOOST_CONCEPT_ASSERT((WireDecodable<StrategySelectors>));
static_assert(std::is_base_of<tlv::Error, StrategySelectors::Error>::value,
              "StrategySelectors::Error must inherit from tlv::Error");

StrategySelectors::StrategySelectors()
  : m_scope(-1)
  , m_nodeId(-1)
  , m_interested(false)
  , m_availability(-1)
{
}

StrategySelectors::StrategySelectors(const Block& wire)
{
  wireDecode(wire);
}

bool
StrategySelectors::empty() const
{
  return m_scope == -1 && m_nodeId == -1 && !m_interested && m_availability == -1;
}

template<encoding::Tag TAG>
size_t
StrategySelectors::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;
  // StrategySelectors ::= STRATEGY-SELECTORS-TYPE TLV-LENGTH
  //                              Scope?
  //                              NodeId?
  //                              Interested?
  //                              Availability?
  // (reverse encoding)

  if (getAvailability() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::Availability, getAvailability());
  }

  if (getInterested()) {
    totalLength += prependEmptyBlock(encoder, tlv::Interested);
  }

  if (getNodeId() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::NodeId, getNodeId());
  }

  if (getScope() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::Scope, getScope());
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::StrategySelectors);
  return totalLength;
}

template size_t
StrategySelectors::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
StrategySelectors::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
StrategySelectors::wireEncode() const
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
StrategySelectors::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::StrategySelectors)
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV type when decoding StrategySelectors"));

  *this = StrategySelectors();

  m_wire = wire;
  m_wire.parse();

  Block::element_const_iterator val = m_wire.find(tlv::Scope);
  if (val != m_wire.elements_end()) {
    m_scope = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::NodeId);
  if (val != m_wire.elements_end()) {
    m_nodeId = readNonNegativeInteger(*val);
  }

  val = m_wire.find(tlv::Interested);
  if (val != m_wire.elements_end()) {
    m_interested = true;
  }

  val = m_wire.find(tlv::Availability);
  if (val != m_wire.elements_end()) {
    m_availability = readNonNegativeInteger(*val);
  }

}

StrategySelectors&
StrategySelectors::setScope(int scope)
{
  m_scope = scope;
  m_wire.reset();
  return *this;
}

StrategySelectors&
StrategySelectors::setNodeId(int nodeId)
{
  m_nodeId = nodeId;
  m_wire.reset();
  return *this;
}

StrategySelectors&
StrategySelectors::setInterested(bool interested)
{
  m_interested = interested;
  m_wire.reset();
  return *this;
}

StrategySelectors&
StrategySelectors::setAvailability(int availability)
{
  m_availability = availability;
  m_wire.reset();
  return *this;
}

bool
StrategySelectors::operator==(const StrategySelectors& other) const
{
  return wireEncode() == other.wireEncode();
}

} // namespace ndn
