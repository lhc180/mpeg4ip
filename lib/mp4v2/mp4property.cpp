/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 */

#include "mp4common.h"

bool MP4Property::FindProperty(char* name, 
	MP4Property** ppProperty, u_int32_t* pIndex) 
{
	if (name == NULL) {
		return false;
	}

	if (!strcasecmp(m_name, name)) {
		ASSERT(m_pParentAtom);
		VERBOSE_FIND(m_pParentAtom->GetFile()->GetVerbosity(),
			printf("FindProperty: matched %s\n", name));

		*ppProperty = this;
		return true;
	}
	return false;
}

void MP4Integer8Property::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = %u (0x%02x)\n", 
		m_name, m_values[index], m_values[index]);
}

void MP4Integer16Property::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = %u (0x%04x)\n", 
		m_name, m_values[index], m_values[index]);
}

void MP4Integer24Property::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = %u (0x%06x)\n", 
		m_name, m_values[index], m_values[index]);
}

void MP4Integer32Property::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = %u (0x%08x)\n", 
		m_name, m_values[index], m_values[index]);
}

void MP4Integer64Property::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = %llu (0x%016x)\n", 
		m_name, m_values[index], m_values[index]);
}

// MP4BitfieldProperty

void MP4BitfieldProperty::Read(MP4File* pFile, u_int32_t index)
{
	if (m_implicit) {
		return;
	}
	m_values[index] = pFile->ReadBits(m_numBits);
}

void MP4BitfieldProperty::Write(MP4File* pFile, u_int32_t index)
{
	if (m_implicit) {
		return;
	}
	pFile->WriteBits(m_values[index], m_numBits);
}

void MP4BitfieldProperty::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);

	u_int8_t hexWidth = m_numBits / 4;
	if (hexWidth == 0 || (m_numBits % 4)) {
		hexWidth++;
	}
	fprintf(pFile, "%s = %llu (0x%0*llx) <%u bits>\n", 
		m_name, m_values[index], (int)hexWidth, m_values[index], m_numBits);
}

// MP4Float32Property

void MP4Float32Property::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = %f (0x%08x)\n", 
		m_name, m_values[index], m_values[index]);
}

// MP4StringProperty

void MP4StringProperty::Read(MP4File* pFile, u_int32_t index)
{
	if (m_implicit) {
		return;
	}
	if (m_useCountedFormat) {
		m_values[index] = pFile->ReadCountedString(
			(m_useUnicode ? 2 : 1), m_useExpandedCount);
	} else {
		m_values[index] = pFile->ReadString();
	}
}

void MP4StringProperty::Write(MP4File* pFile, u_int32_t index)
{
	if (m_implicit) {
		return;
	}
	if (m_useCountedFormat) {
		pFile->WriteCountedString(m_values[index],
			(m_useUnicode ? 2 : 1), m_useExpandedCount);
	} else {
		pFile->WriteString(m_values[index]);
	}
}

void MP4StringProperty::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	if (m_useUnicode) {
		fprintf(pFile, "%s = %ls\n", m_name, m_values[index]);
	} else {
		fprintf(pFile, "%s = %s\n", m_name, m_values[index]);
	}
}

// MP4BytesProperty

void MP4BytesProperty::Read(MP4File* pFile, u_int32_t index)
{
	if (m_implicit) {
		return;
	}
	MP4Free(m_values[index]);
	WARNING(m_valueSizes[index] == 0);
	m_values[index] = (u_int8_t*)MP4Malloc(m_valueSizes[index]);
	pFile->ReadBytes(m_values[index], m_valueSizes[index]);
}

void MP4BytesProperty::Write(MP4File* pFile, u_int32_t index)
{
	if (m_implicit) {
		return;
	}
	WARNING(m_values[index] == NULL);
	WARNING(m_valueSizes[index] == 0);
	pFile->WriteBytes(m_values[index], m_valueSizes[index]);
}

void MP4BytesProperty::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(m_pParentAtom);
	Indent(pFile, m_pParentAtom->GetDepth() + 1);
	fprintf(pFile, "%s = <%u bytes> ", m_name, m_valueSizes[index]);
	for (u_int32_t i = 0; i < m_valueSizes[index]; i++) {
		if ((i % 16) == 0 && m_valueSizes[index] > 16) {
			fprintf(pFile, "\n");
			Indent(pFile, m_pParentAtom->GetDepth() + 1);
		}
		fprintf(pFile, "%02x ", m_values[index][i]);
	}
	fprintf(pFile, "\n");
}

// MP4TableProperty

bool MP4TableProperty::FindProperty(char *name,
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	ASSERT(m_name);

	// check if first component of name matches ourselves
	if (!MP4NameFirstMatches(m_name, name)) {
		return false;
	}

	// check if the specific table entry exists
	u_int32_t index;
	if (!MP4NameFirstIndex(name, &index)) {
		return false;
	}
	if (index >= GetCount()) {
		return false;
	}
	*pIndex = index;

	VERBOSE_FIND(m_pParentAtom->GetFile()->GetVerbosity(),
		printf("FindProperty: matched %s\n", name));

	// get name of table property
	char *tablePropName = MP4NameAfterFirst(name);
	if (tablePropName == NULL) {
		return false;
	}

	// check if this table property exists
	return FindContainedProperty(tablePropName, ppProperty, pIndex);
}

bool MP4TableProperty::FindContainedProperty(char *name,
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	u_int32_t numProperties = m_pProperties.Size();

	for (u_int32_t i = 0; i < numProperties; i++) {
		if (m_pProperties[i]->FindProperty(name, ppProperty, pIndex)) {
			return true;
		}
	}
	return false;
}

void MP4TableProperty::Read(MP4File* pFile, u_int32_t index)
{
	ASSERT(index == 0);

	if (m_implicit) {
		return;
	}

	u_int32_t numProperties = m_pProperties.Size();

	if (numProperties == 0) {
		WARNING(numProperties == 0);
		return;
	}

	u_int32_t numEntries = GetCount();

	/* for each property set size */
	for (u_int32_t j = 0; j < numProperties; j++) {
		m_pProperties[j]->SetCount(numEntries);
	}

	for (u_int32_t i = 0; i < numEntries; i++) {
		ReadEntry(pFile, i);
	}
}

void MP4TableProperty::ReadEntry(MP4File* pFile, u_int32_t index)
{
	for (u_int32_t j = 0; j < m_pProperties.Size(); j++) {
		m_pProperties[j]->Read(pFile, index);
	}
}

void MP4TableProperty::Write(MP4File* pFile, u_int32_t index)
{
	ASSERT(index == 0);

	if (m_implicit) {
		return;
	}

	u_int32_t numProperties = m_pProperties.Size();

	if (numProperties == 0) {
		WARNING(numProperties == 0);
		return;
	}

#ifdef BUGGY
	u_int32_t numEntries = GetCount();

printf("atom %s table %s\n", m_pParentAtom->GetType(), m_name);
printf("numEntries %u, property count %u\n", m_pProperties[0]->GetCount());
	ASSERT(m_pProperties[0]->GetCount() == numEntries);

	for (u_int32_t i = 0; i < numEntries; i++) {
		WriteEntry(pFile, i);
	}
#endif
}

void MP4TableProperty::WriteEntry(MP4File* pFile, u_int32_t index)
{
	for (u_int32_t j = 0; j < m_pProperties.Size(); j++) {
		m_pProperties[j]->Write(pFile, index);
	}
}

void MP4TableProperty::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(index == 0);

	u_int32_t numProperties = m_pProperties.Size();

	if (numProperties == 0) {
		WARNING(numProperties == 0);
		return;
	}

	u_int32_t numEntries = GetCount();

	ASSERT(m_pProperties[0]->GetCount() == numEntries);

	for (u_int32_t i = 0; i < numEntries; i++) {
		for (u_int32_t j = 0; j < numProperties; j++) {
			m_pProperties[j]->Dump(pFile, i);
		}
	}
}

// MP4DescriptorProperty
  
void MP4DescriptorProperty::SetParentAtom(MP4Atom* pParentAtom) {
	m_pParentAtom = pParentAtom;
	for (u_int32_t i = 0; i < m_pDescriptors.Size(); i++) {
		m_pDescriptors[i]->SetParentAtom(pParentAtom);
	}
}

void MP4DescriptorProperty::Generate()
{
	// generate a default descriptor
	// if it is mandatory, single, and the tag is fixed
	if (m_mandatory && m_onlyOne && m_tagsEnd == 0) {
		MP4Descriptor* pDescriptor = CreateDescriptor(m_tagsStart);
		ASSERT(pDescriptor);
		m_pDescriptors.Add(pDescriptor);
		pDescriptor->SetParentAtom(m_pParentAtom);
		pDescriptor->Generate();
	}
}

bool MP4DescriptorProperty::FindProperty(char *name,
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	// we're unnamed, so just check contained properties
	if (m_name == NULL || !strcmp(m_name, "")) {
		return FindContainedProperty(name, ppProperty, pIndex);
	}

	// check if first component of name matches ourselves
	if (!MP4NameFirstMatches(m_name, name)) {
		return false;
	}

	// check if the specific descriptor entry exists
	u_int32_t descrIndex;
	bool haveDescrIndex = MP4NameFirstIndex(name, &descrIndex);

	if (haveDescrIndex && descrIndex >= GetCount()) {
		return false;
	}

	VERBOSE_FIND(m_pParentAtom->GetFile()->GetVerbosity(),
		printf("FindProperty: matched %s\n", name));

	// get name of descriptor property
	name = MP4NameAfterFirst(name);
	if (name == NULL) {
		return false;
	}

	/* check rest of name */
	if (haveDescrIndex) {
		return m_pDescriptors[descrIndex]->FindProperty(name, 
			ppProperty, pIndex); 
	} else {
		return FindContainedProperty(name, ppProperty, pIndex);
	}
}

bool MP4DescriptorProperty::FindContainedProperty(char *name,
	MP4Property** ppProperty, u_int32_t* pIndex)
{
	for (u_int32_t i = 0; i < m_pDescriptors.Size(); i++) {
		if (m_pDescriptors[i]->FindProperty(name, ppProperty, pIndex)) {
			return true;
		}
	}
	return false;
}

void MP4DescriptorProperty::Read(MP4File* pFile, u_int32_t index)
{
	ASSERT(index == 0);

	if (m_implicit) {
		return;
	}

	// m_tagsEnd == 0 signals endTag == m_tagsStart
	u_int8_t endTag = (m_tagsEnd ? m_tagsEnd : m_tagsStart);

	u_int64_t start = pFile->GetPosition();

	while (true) {
		if (m_sizeLimit && pFile->GetPosition() > start + m_sizeLimit) {
			break;
		}

		u_int8_t tag;
		try {
			pFile->PeekBytes(&tag, 1);
		}
		catch (MP4Error* e) {
			if (pFile->GetPosition() >= pFile->GetSize()) {
				// EOF
				break;
			}
			throw e;
		}

		// check if tag is in desired range
		if (tag < m_tagsStart || tag > endTag) {
			break;
		}

		MP4Descriptor* pDescriptor = CreateDescriptor(tag);
		ASSERT(pDescriptor);
		m_pDescriptors.Add(pDescriptor);
		pDescriptor->SetParentAtom(m_pParentAtom);
		pDescriptor->Read(pFile);
	}

	// warnings
	if (m_mandatory && m_pDescriptors.Size() == 0) {
		VERBOSE_READ(pFile->GetVerbosity(),
			printf("Warning: Mandatory descriptor %u missing\n",
				m_tagsStart));
	} else if (m_onlyOne && m_pDescriptors.Size() > 1) {
		VERBOSE_READ(pFile->GetVerbosity(),
			printf("Warning: Descriptor %u has more than one instance\n",
				m_tagsStart));
	}
}

void MP4DescriptorProperty::Write(MP4File* pFile, u_int32_t index)
{
	ASSERT(index == 0);

	if (m_implicit) {
		return;
	}

	for (u_int32_t i = 0; i < m_pDescriptors.Size(); i++) {
		m_pDescriptors[i]->Write(pFile);
	}
}

void MP4DescriptorProperty::Dump(FILE* pFile, u_int32_t index)
{
	ASSERT(index == 0);

	if (m_name) {
		ASSERT(m_pParentAtom);
		Indent(pFile, m_pParentAtom->GetDepth() + 1);
		fprintf(pFile, "%s\n", m_name);
	}

	for (u_int32_t i = 0; i < m_pDescriptors.Size(); i++) {
		m_pDescriptors[i]->Dump(pFile);
	}
}
