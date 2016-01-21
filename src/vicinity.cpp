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
 */

#include "vicinity.hpp"
#include "util/random.hpp"
#include "util/crypto.hpp"
#include "data.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Vicinity>));
BOOST_CONCEPT_ASSERT((WireEncodable<Vicinity>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Vicinity>));
BOOST_CONCEPT_ASSERT((WireDecodable<Vicinity>));
static_assert(std::is_base_of<tlv::Error, Vicinity::Error>::value,
              "Vicinity::Error must inherit from tlv::Error");

Vicinity::Vicinity()
{
}

Vicinity::Vicinity(const Name& name)
  : m_name(name)
  , m_scope(0)
{
}

Vicinity::Vicinity(const Name& name, uint32_t scope)
  : m_name(name)
  , m_scope(scope)
{
}

Vicinity::Vicinity(const Block& wire)
{
  wireDecode(wire);
}

bool
Vicinity::matchesName(const Name& name) const
{
  if (name.size() < m_name.size())
    return false;

  if (!m_name.isPrefixOf(name))
    return false;

  return true;
}

template<encoding::Tag TAG>
size_t
Vicinity::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Vicinity ::= VICINITY-TYPE TLV-LENGTH
  //                Name
  //                Scope
  //                Selectors?

  // (reverse encoding)

  // AnnouncementTimeout
  if (getScope() >= 0 &&
      getScope() != DEFAULT_VICINITY_SCOPE)
    {
      totalLength += prependNonNegativeIntegerBlock(encoder,
                                                    tlv::Scope,
                                                    getScope());
    }

  // Name
  totalLength += getName().wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Vicinity);

  return totalLength;
}

template size_t
Vicinity::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Vicinity::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
Vicinity::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  // to ensure that Nonce block points to the right memory location
  const_cast<Vicinity*>(this)->wireDecode(buffer.block());

  return m_wire;
}

void
Vicinity::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // Interest ::= INTEREST-TYPE TLV-LENGTH
  //                Name
  //                Scope
  //                Selectors?

  if (m_wire.type() != tlv::Vicinity)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV number when decoding Interest"));

  // Name
  m_name.wireDecode(m_wire.get(tlv::Name));

  // Scope
  Block::element_const_iterator val = m_wire.find(tlv::Scope);
  if (val != m_wire.elements_end())
    {
      m_scope = uint32_t(readNonNegativeInteger(*val));
    }
  else
    {
      m_scope = DEFAULT_VICINITY_SCOPE;
    }

}

std::ostream&
operator<<(std::ostream& os, const Vicinity& vicinity)
{
  os << vicinity.getName();

  char delim = '?';

  if (vicinity.getScope() >= 0
      && vicinity.getScope() != DEFAULT_VICINITY_SCOPE) {
    os << delim << "ndn.VicinityScope=" << vicinity.getScope();
    delim = '&';
  }

  return os;
}

} // namespace ndn
