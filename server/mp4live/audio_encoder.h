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
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 */

#ifndef __AUDIO_ENCODER_H__
#define __AUDIO_ENCODER_H__

#include "media_codec.h"

class CAudioEncoder : public CMediaCodec {
public:
	CAudioEncoder() { };

	virtual bool EncodeSamples(
		u_int16_t* pBuffer, u_int32_t bufferLength) = NULL;

	virtual bool GetEncodedFrame(
		u_int8_t** ppBuffer, u_int32_t* pBufferLength) = NULL;
};

#endif /* __AUDIO_ENCODER_H__ */
