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

#include "announcement.hpp"
#include "util/random.hpp"
#include "util/crypto.hpp"
#include "data.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<Announcement>));
BOOST_CONCEPT_ASSERT((WireEncodable<Announcement>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<Announcement>));
BOOST_CONCEPT_ASSERT((WireDecodable<Announcement>));
static_assert(std::is_base_of<tlv::Error, Announcement::Error>::value,
              "Announcement::Error must inherit from tlv::Error");

Announcement::Announcement()
{
}

Announcement::Announcement(const Name& name)
  : m_name(name)
  , m_timeout(time::milliseconds::min())
{
}

Announcement::Announcement(const Name& name, const time::milliseconds& timeout)
  : m_name(name)
  , m_timeout(timeout)
{
}

Announcement::Announcement(const Block& wire)
{
  wireDecode(wire);
}

uint32_t
Announcement::getNonce() const
{
  if (!m_nonce.hasWire())
    const_cast<Announcement*>(this)->setNonce(random::generateWord32());

  if (m_nonce.value_size() == sizeof(uint32_t))
    return *reinterpret_cast<const uint32_t*>(m_nonce.value());
  else {
    // for compatibility reasons.  Should be removed eventually
    return readNonNegativeInteger(m_nonce);
  }
}

Announcement&
Announcement::setNonce(uint32_t nonce)
{
  if (m_wire.hasWire() && m_nonce.value_size() == sizeof(uint32_t)) {
    std::memcpy(const_cast<uint8_t*>(m_nonce.value()), &nonce, sizeof(nonce));
  }
  else {
    m_nonce = makeBinaryBlock(tlv::Nonce,
                              reinterpret_cast<const uint8_t*>(&nonce),
                              sizeof(nonce));
    m_wire.reset();
  }
  return *this;
}

void
Announcement::refreshNonce()
{
  if (!hasNonce())
    return;

  uint32_t oldNonce = getNonce();
  uint32_t newNonce = oldNonce;
  while (newNonce == oldNonce)
    newNonce = random::generateWord32();

  setNonce(newNonce);
}

bool
Announcement::matchesName(const Name& name) const
{
  if (name.size() < m_name.size())
    return false;

  if (!m_name.isPrefixOf(name))
    return false;

  return true;
}

template<encoding::Tag TAG>
size_t
Announcement::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // Announcement ::= ANNOUNCEMENT-TYPE TLV-LENGTH
  //                Name
  //                Nonce
  //                Timeout

  // (reverse encoding)

  // Timeout
  if (getTimeout() >= time::milliseconds::zero() &&
      getTimeout() != DEFAULT_ANNOUNCEMENT_TIMEOUT)
    {
      totalLength += prependNonNegativeIntegerBlock(encoder,
                                                    tlv::Timeout,
                                                    getTimeout().count());
    }

  // Nonce
  getNonce(); // to ensure that Nonce is properly set
  totalLength += encoder.prependBlock(m_nonce);

  // Name
  totalLength += getName().wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::Announcement);
  return totalLength;
}

template size_t
Announcement::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
Announcement::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
Announcement::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  // to ensure that Nonce block points to the right memory location
  const_cast<Announcement*>(this)->wireDecode(buffer.block());

  return m_wire;
}

void
Announcement::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // Announcement ::= ANNOUNCEMENT-TYPE TLV-LENGTH
  //                Name
  //                Nonce
  //                Timeout

  if (m_wire.type() != tlv::Announcement)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV number when decoding Announcement"));

  // Name
  m_name.wireDecode(m_wire.get(tlv::Name));

  // Nonce
  m_nonce = m_wire.get(tlv::Nonce);

  // Timeout
  Block::element_const_iterator val = m_wire.find(tlv::Timeout);
  if (val != m_wire.elements_end())
    {
      m_timeout = time::milliseconds(readNonNegativeInteger(*val));
    }
  else
    {
      m_timeout = DEFAULT_ANNOUNCEMENT_TIMEOUT;
    }

}

std::ostream&
operator<<(std::ostream& os, const Announcement& announcement)
{
  os << announcement.getName();

  char delim = '?';

  if (announcement.getTimeout() >= time::milliseconds::zero()
      && announcement.getTimeout() != DEFAULT_ANNOUNCEMENT_TIMEOUT) {
    os << delim << "ndn.AnnouncementTimeout=" << announcement.getTimeout().count();
    delim = '&';
  }

  if (announcement.hasNonce()) {
    os << delim << "ndn.Nonce=" << announcement.getNonce();
    delim = '&';
  }

  return os;
}

} // namespace ndn
