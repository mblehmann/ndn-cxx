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

#include "replication-selectors.hpp"
#include "encoding/encoding-buffer.hpp"
#include "encoding/block-helpers.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ReplicationSelectors>));
BOOST_CONCEPT_ASSERT((WireEncodable<ReplicationSelectors>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<ReplicationSelectors>));
BOOST_CONCEPT_ASSERT((WireDecodable<ReplicationSelectors>));
static_assert(std::is_base_of<tlv::Error, ReplicationSelectors::Error>::value,
              "ReplicationSelectors::Error must inherit from tlv::Error");

ReplicationSelectors::ReplicationSelectors()
  : m_nodeID(-1)
  , m_interested(false)
  , m_availability(-1)
{
}

ReplicationSelectors::ReplicationSelectors(const Block& wire)
{
  wireDecode(wire);
}

bool
ReplicationSelectors::empty() const
{
  return m_nodeID == -1 && m_interested && m_availability == -1;
}

template<encoding::Tag TAG>
size_t
ReplicationSelectors::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;
  // ReplicationSelectors ::= REPLICATION-SELECTORS-TYPE TLV-LENGTH
  //                              NodeID?
  //                              Interested?
  //                              Availability?
  // (reverse encoding)

  if (getAvailability() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::Availability, getAvailability());
  }

  if (getInterested()) {
    totalLength += prependEmptyBlock(encoder, tlv::Interested);
  }

  if (getNodeID() >= 0) {
    totalLength += prependNonNegativeIntegerBlock(encoder, tlv::NodeID, getNodeID());
  }

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::ReplicationSelectors);
  return totalLength;
}

template size_t
ReplicationSelectors::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
ReplicationSelectors::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
ReplicationSelectors::wireEncode() const
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
ReplicationSelectors::wireDecode(const Block& wire)
{
  if (wire.type() != tlv::ReplicationSelectors)
    BOOST_THROW_EXCEPTION(tlv::Error("Unexpected TLV type when decoding ReplicationSelectors"));

  *this = ReplicationSelectors();

  m_wire = wire;
  m_wire.parse();

  Block::element_const_iterator val = m_wire.find(tlv::NodeID);
  if (val != m_wire.elements_end()) {
    m_nodeID = readNonNegativeInteger(*val);
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

ReplicationSelectors&
ReplicationSelectors::setNodeID(int nodeID)
{
  m_nodeID = nodeID;
  m_wire.reset();
  return *this;
}

ReplicationSelectors&
ReplicationSelectors::setInterested(bool interested)
{
  m_interested = interested;
  m_wire.reset();
  return *this;
}

ReplicationSelectors&
ReplicationSelectors::setAvailability(int availability)
{
  m_availability = availability;
  m_wire.reset();
  return *this;
}

bool
ReplicationSelectors::operator==(const ReplicationSelectors& other) const
{
  return wireEncode() == other.wireEncode();
}

} // namespace ndn
