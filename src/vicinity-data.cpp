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

#include "vicinity-data.hpp"
#include "util/random.hpp"
#include "util/crypto.hpp"
#include "data.hpp"

namespace ndn {

BOOST_CONCEPT_ASSERT((boost::EqualityComparable<VicinityData>));
BOOST_CONCEPT_ASSERT((WireEncodable<VicinityData>));
BOOST_CONCEPT_ASSERT((WireEncodableWithEncodingBuffer<VicinityData>));
BOOST_CONCEPT_ASSERT((WireDecodable<VicinityData>));
static_assert(std::is_base_of<tlv::Error, VicinityData::Error>::value,
              "VicinityData::Error must inherit from tlv::Error");

VicinityData::VicinityData()
{
}

VicinityData::VicinityData(const Name& name)
  : m_name(name)
  , m_nodeID(-1)
{
}

VicinityData::VicinityData(const Name& name, int nodeID)
  : m_name(name)
  , m_nodeID(nodeID)
{
}

VicinityData::VicinityData(const Block& wire)
{
  wireDecode(wire);
}

bool
VicinityData::matchesName(const Name& name) const
{
  if (name.size() < m_name.size())
    return false;

  if (!m_name.isPrefixOf(name))
    return false;

  return true;
}

bool
VicinityData::matchesData(const VicinityData& data) const
{
//   size_t interestNameLength = m_name.size();
//   const Name& dataName = data.getName();
//   size_t fullNameLength = dataName.size() + 1;

//   // check MinSuffixComponents
//   bool hasMinSuffixComponents = getMinSuffixComponents() >= 0;
//   size_t minSuffixComponents = hasMinSuffixComponents ?
//                                static_cast<size_t>(getMinSuffixComponents()) : 0;
//   if (!(interestNameLength + minSuffixComponents <= fullNameLength))
//     return false;

//   // check MaxSuffixComponents
//   bool hasMaxSuffixComponents = getMaxSuffixComponents() >= 0;
//   if (hasMaxSuffixComponents &&
//       !(interestNameLength + getMaxSuffixComponents() >= fullNameLength))
//     return false;

//   // check prefix
//   if (interestNameLength == fullNameLength) {
//     if (m_name.get(-1).isImplicitSha256Digest()) {
//       if (m_name != data.getFullName())
//         return false;
//     }
//     else {
//       // Vicinity Name is same length as Data full Name, but last component isn't digest
//       // so there's no possibility of matching
//       return false;
//     }
//   }
//   else {
//     // Vicinity Name is a strict prefix of Data full Name
//     if (!m_name.isPrefixOf(dataName))
//       return false;
//   }

//   // check Exclude
//   // Exclude won't be violated if Vicinity Name is same as Data full Name
//   if (!getExclude().empty() && fullNameLength > interestNameLength) {
//     if (interestNameLength == fullNameLength - 1) {
//       // component to exclude is the digest
//       if (getExclude().isExcluded(data.getFullName().get(interestNameLength)))
//         return false;
//       // There's opportunity to inspect the Exclude filter and determine whether
//       // the digest would make a difference.
//       // eg. "<NameComponent>AA</NameComponent><Any/>" doesn't exclude any digest -
//       //     fullName not needed;
//       //     "<Any/><NameComponent>AA</NameComponent>" and
//       //     "<Any/><ImplicitSha256DigestComponent>ffffffffffffffffffffffffffffffff
//       //      </ImplicitSha256DigestComponent>"
//       //     excludes all digests - fullName not needed;
//       //     "<Any/><ImplicitSha256DigestComponent>80000000000000000000000000000000
//       //      </ImplicitSha256DigestComponent>"
//       //     excludes some digests - fullName required
//       // But Interests that contain the exact Data Name before digest and also
//       // contain Exclude filter is too rare to optimize for, so we request
//       // fullName no mater what's in the Exclude filter.
//     }
//     else {
//       // component to exclude is not the digest
//       if (getExclude().isExcluded(dataName.get(interestNameLength)))
//         return false;
//     }
//   }

//   // check PublisherPublicKeyLocator
//   const KeyLocator& publisherPublicKeyLocator = this->getPublisherPublicKeyLocator();
//   if (!publisherPublicKeyLocator.empty()) {
//     const Signature& signature = data.getSignature();
//     const Block& signatureInfo = signature.getInfo();
//     Block::element_const_iterator it = signatureInfo.find(tlv::KeyLocator);
//     if (it == signatureInfo.elements_end()) {
//       return false;
//     }
//     if (publisherPublicKeyLocator.wireEncode() != *it) {
//       return false;
//     }
//   }

  return true;
}

template<encoding::Tag TAG>
size_t
VicinityData::wireEncode(EncodingImpl<TAG>& encoder) const
{
  size_t totalLength = 0;

  // VicinityData ::= VICINITY-DATA-TYPE TLV-LENGTH
  //                Name
  //                NodeID

  // (reverse encoding)

  // Scope
  if (getNodeID() >= 0)
    {
      totalLength += prependNonNegativeIntegerBlock(encoder,
                                                    tlv::NodeID,
                                                    getNodeID());
    }

  // Name
  totalLength += getName().wireEncode(encoder);

  totalLength += encoder.prependVarNumber(totalLength);
  totalLength += encoder.prependVarNumber(tlv::VicinityData);
  return totalLength;
}

template size_t
VicinityData::wireEncode<encoding::EncoderTag>(EncodingImpl<encoding::EncoderTag>& encoder) const;

template size_t
VicinityData::wireEncode<encoding::EstimatorTag>(EncodingImpl<encoding::EstimatorTag>& encoder) const;

const Block&
VicinityData::wireEncode() const
{
  if (m_wire.hasWire())
    return m_wire;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  // to ensure that Nonce block points to the right memory location
  const_cast<VicinityData*>(this)->wireDecode(buffer.block());

  return m_wire;
}

void
VicinityData::wireDecode(const Block& wire)
{
  m_wire = wire;
  m_wire.parse();

  // VicinityData ::= VICINITY-DATA-TYPE TLV-LENGTH
  //                Name
  //                NodeID

  if (m_wire.type() != tlv::VicinityData)
    BOOST_THROW_EXCEPTION(Error("Unexpected TLV number when decoding VicinityData"));

  // Name
  m_name.wireDecode(m_wire.get(tlv::Name));

  // Scope
  Block::element_const_iterator val = m_wire.find(tlv::NodeID);
  if (val != m_wire.elements_end())
    {
      m_nodeID = int(readNonNegativeInteger(*val));
    }
  else
    {
      m_nodeID = DEFAULT_VICINITY_DATA_NODEID;
    }

}

std::ostream&
operator<<(std::ostream& os, const VicinityData& vicinityData)
{
  os << vicinityData.getName();

  char delim = '?';

  if (vicinityData.getNodeID() >= 0) {
    os << delim << "ndn.VicinityDataNodeID=" << vicinityData.getNodeID();
    delim = '&';
  }

  return os;
}

} // namespace ndn
