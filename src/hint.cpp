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

#include "hint.hpp"
#include "util/random.hpp"
#include "util/crypto.hpp"
#include "data.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Hint>));
BOOST_CONCEPT_ASSERT((WireEncodable<Hint>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Hint>));
BOOST_CONCEPT_ASSERT((WireDecodable<Hint>));
static_assert(std::is_base_of<tlv::Error, Hint::Error>::value,
              "Hint::Error must inherit from tlv::Error");

Hint::Hint()
{
}

Hint::Hint(const Name& name, const uint32_t& size)
  : m_name(name)
  , m_size(size)
{
}

Hint::Hint(const Name& name, const uint32_t& size, const int& scope)
  : m_name(name)
  , m_size(size)
  , m_scope(scope)
{
}

Hint::Hint(const Block& wire)
{
  wireDecode(wire);
}

bool
Hint::matchesName(const Name& name) const
{
  if (name.size() < m_name.size())
    return false;

  if (!m_name.isPrefixOf(name))
    return false;

  return true;
}

template<encoding::Tag TAG>
size_t
Hint::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // HINT ::= HINT-TYPE TLV-LENGTH
  //                Name
  //                Size 
  //                Scope 
  //                Selectors?
  //
  // (reverse encoding)

  // Selectors
  if (hasSelectors())
      totalLength += getSelectors().wireEncode(encoder);


  // Scope
  if (getScope() >= 0 && getScope() != DEFAULT_HINT_SCOPE)
      totalLength += prependNonNegativeIntegerBlock(encoder,
                                                    tlv::Scope,
                                                    getScope());
  // Size
  totalLength += prependNonNegativeIntegerBlock(encoder,
                                                tlv::Size,
                                                getSize());

  // Name
  totalLength += getName().wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Hint);
  return totalLength;
}

template size_t
Hint::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Hint::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
Hint::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  // to ensure that Nonce block points to the right memory location
  const_cast<Hint*>(this)->wireDecode(buffer.block());

  return m_wire;
}

void
Hint::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // HINT ::= HINT-TYPE TLV-LENGTH
  //                Name
  //                Size
  //                Scope
  //                Selectors?

  if (m_wire.type() != tlv::Hint)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV number when decoding Hint"));

  // Name
  m_name.wireDecode(m_wire.get(tlv::Name));

  // Size
  Block::element_const_iterator val = m_wire.find(tlv::Size);
  m_size =  uint32_t(readNonNegativeInteger(*val));

  // Scope
  val = m_wire.find(tlv::Scope);
  if (val != m_wire.elements_end())
    {
      m_scope = uint32_t(readNonNegativeInteger(*val));
    }
  else
    {
      m_scope = DEFAULT_HINT_SCOPE;
    }
  
  // Selectors
  val = m_wire.find(tlv::ReplicationSelectors);
  if (val != m_wire.elements_end()) 
      m_selectors.wireDecode(*val);
  else
    m_selectors = ReplicationSelectors();

}

std::ostream&
operator<<(std::ostream& os, const Hint& hint)
{
  os << hint.getName() << "/" << hint.getSize();

  char delim = '?';

  if (hint.getScope() >= 0 && hint.getScope() != DEFAULT_HINT_SCOPE) {
    os << delim << "ndn.HintScope=" << hint.getScope(); 
    delim = '&';
  }

  if (hint.getNodeID() >= 0) {
    os << delim << "ndn.NodeID=" << hint.getNodeID();
    delim = '&';
  }

  if (hint.getInterested()) {
    os << delim << "ndn.Interested=" << hint.getInterested();
    delim = '&';
  }

  if (!hint.getAvailability() >= 0) {
    os << delim << "ndn.Availability=" << hint.getAvailability();
    delim = '&';
  }

  return os;
}

} // namespace ndn
