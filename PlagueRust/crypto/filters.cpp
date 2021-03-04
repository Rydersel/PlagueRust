// filters.cpp - originally written and placed in the public domain by Wei Dai

#include "pch.h"
#include "config.h"

#if CRYPTOPP_MSC_VERSION
# pragma warning(disable: 4100 4189 4355)
#endif

#if CRYPTOPP_GCC_DIAGNOSTIC_AVAILABLE
# pragma GCC diagnostic ignored "-Wunused-value"
#endif

#ifndef CRYPTOPP_IMPORTS

#include "filters.h"
#include "mqueue.h"
#include "fltrimpl.h"
#include "argnames.h"
#include "smartptr.h"
#include "stdcpp.h"
#include "misc.h"

NAMESPACE_BEGIN(CryptoPP)

Filter::Filter(BufferedTransformation *attachment)
	: m_attachment(attachment), m_inputPosition(0), m_continueAt(0)
{
}

BufferedTransformation * Filter::NewDefaultAttachment() const
{
	return new MessageQueue;
}

BufferedTransformation * Filter::AttachedTransformation()
{
	if (m_attachment.get() == NULLPTR)
		m_attachment.reset(NewDefaultAttachment());
	return m_attachment.get();
}

const BufferedTransformation *Filter::AttachedTransformation() const
{
	if (m_attachment.get() == NULLPTR)
		const_cast<Filter *>(this)->m_attachment.reset(NewDefaultAttachment());
	return m_attachment.get();
}

void Filter::Detach(BufferedTransformation *newOut)
{
	m_attachment.reset(newOut);
}

void Filter::Insert(Filter *filter)
{
	filter->m_attachment.reset(m_attachment.release());
	m_attachment.reset(filter);
}

size_t Filter::CopyRangeTo2(BufferedTransformation &target, lword &begin, lword end, const std::string &channel, bool blocking) const
{
	return AttachedTransformation()->CopyRangeTo2(target, begin, end, channel, blocking);
}

size_t Filter::TransferTo2(BufferedTransformation &target, lword &transferBytes, const std::string &channel, bool blocking)
{
	return AttachedTransformation()->TransferTo2(target, transferBytes, channel, blocking);
}

void Filter::Initialize(const NameValuePairs &parameters, int propagation)
{
	m_inputPosition = m_continueAt = 0;
	IsolatedInitialize(parameters);
	PropagateInitialize(parameters, propagation);
}

bool Filter::Flush(bool hardFlush, int propagation, bool blocking)
{
	switch (m_continueAt)
	{
	case 0:
		if (IsolatedFlush(hardFlush, blocking))
			return true;
		// fall through
	case 1:
		if (OutputFlush(1, hardFlush, propagation, blocking))
			return true;
		// fall through
	default: ;;
	}
	return false;
}

bool Filter::MessageSeriesEnd(int propagation, bool blocking)
{
	switch (m_continueAt)
	{
	case 0:
		if (IsolatedMessageSeriesEnd(blocking))
			return true;
		// fall through
	case 1:
		if (ShouldPropagateMessageSeriesEnd() && OutputMessageSeriesEnd(1, propagation, blocking))
			return true;
		// fall through
	default: ;;
	}
	return false;
}

void Filter::PropagateInitialize(const NameValuePairs &parameters, int propagation)
{
	if (propagation)
		AttachedTransformation()->Initialize(parameters, propagation-1);
}

size_t Filter::OutputModifiable(int outputSite, byte *inString, size_t length, int messageEnd, bool blocking, const std::string &channel)
{
	if (messageEnd)
		messageEnd--;
	size_t result = AttachedTransformation()->ChannelPutModifiable2(channel, inString, length, messageEnd, blocking);
	m_continueAt = result ? outputSite : 0;
	return result;
}

size_t Filter::Output(int outputSite, const byte *inString, size_t length, int messageEnd, bool blocking, const std::string &channel)
{
	if (messageEnd)
		messageEnd--;
	size_t result = AttachedTransformation()->ChannelPut2(channel, inString, length, messageEnd, blocking);
	m_continueAt = result ? outputSite : 0;
	return result;
}

bool Filter::OutputFlush(int outputSite, bool hardFlush, int propagation, bool blocking, const std::string &channel)
{
	if (propagation && AttachedTransformation()->ChannelFlush(channel, hardFlush, propagation-1, blocking))
	{
		m_continueAt = outputSite;
		return true;
	}
	m_continueAt = 0;
	return false;
}

bool Filter::OutputMessageSeriesEnd(int outputSite, int propagation, bool blocking, const std::string &channel)
{
	if (propagation && AttachedTransformation()->ChannelMessageSeriesEnd(channel, propagation-1, blocking))
	{
		m_continueAt = outputSite;
		return true;
	}
	m_continueAt = 0;
	return false;
}

// *************************************************************

void MeterFilter::ResetMeter()
{
	m_currentMessageBytes = m_totalBytes = m_currentSeriesMessages = m_totalMessages = m_totalMessageSeries = 0;
	m_rangesToSkip.clear();
}

void MeterFilter::AddRangeToSkip(unsigned int message, lword position, lword size, bool sortNow)
{
	MessageRange r = {message, position, size};
	m_rangesToSkip.push_back(r);
	if (sortNow)
		std::sort(m_rangesToSkip.begin(), m_rangesToSkip.end());
}

size_t MeterFilter::PutMaybeModifiable(byte *begin, size_t length, int messageEnd, bool blocking, bool modifiable)
{
	if (!m_transparent)
		return 0;

	size_t t;
	FILTER_BEGIN;

	m_begin = begin;
	m_length = length;

	while (m_length > 0 || messageEnd)
	{
		if (m_length > 0  && !m_rangesToSkip.empty() && m_rangesToSkip.front().message == m_totalMessages && m_currentMessageBytes + m_length > m_rangesToSkip.front().position)
		{
			FILTER_OUTPUT_MAYBE_MODIFIABLE(1, m_begin, t = (size_t)SaturatingSubtract(m_rangesToSkip.front().position, m_currentMessageBytes), false, modifiable);

			CRYPTOPP_ASSERT(t < m_length);
			m_begin = PtrAdd(m_begin, t);
			m_length -= t;
			m_currentMessageBytes += t;
			m_totalBytes += t;

			if (m_currentMessageBytes + m_length < m_rangesToSkip.front().position + m_rangesToSkip.front().size)
				t = m_length;
			else
			{
				t = (size_t)SaturatingSubtract(m_rangesToSkip.front().position + m_rangesToSkip.front().size, m_currentMessageBytes);
				CRYPTOPP_ASSERT(t <= m_length);
				m_rangesToSkip.pop_front();
			}

			m_begin = PtrAdd(m_begin, t);
			m_length -= t;
			m_currentMessageBytes += t;
			m_totalBytes += t;
		}
		else
		{
			FILTER_OUTPUT_MAYBE_MODIFIABLE(2, m_begin, m_length, messageEnd, modifiable);

			m_currentMessageBytes += m_length;
			m_totalBytes += m_length;
			m_length = 0;

			if (messageEnd)
			{
				m_currentMessageBytes = 0;
				m_currentSeriesMessages++;
				m_totalMessages++;
				messageEnd = false;
			}
		}
	}

	FILTER_END_NO_MESSAGE_END;
}

size_t MeterFilter::Put2(const byte *begin, size_t length, int messageEnd, bool blocking)
{
	return PutMaybeModifiable(const_cast<byte *>(begin), length, messageEnd, blocking, false);
}

size_t MeterFilter::PutModifiable2(byte *begin, size_t length, int messageEnd, bool blocking)
{
	return PutMaybeModifiable(begin, length, messageEnd, blocking, true);
}

bool MeterFilter::IsolatedMessageSeriesEnd(bool blocking)
{
	CRYPTOPP_UNUSED(blocking);
	m_currentMessageBytes = 0;
	m_currentSeriesMessages = 0;
	m_totalMessageSeries++;
	return false;
}

// *************************************************************

void FilterWithBufferedInput::BlockQueue::ResetQueue(size_t blockSize, size_t maxBlocks)
{
	m_buffer.New(blockSize * maxBlocks);
	m_blockSize = blockSize;
	m_maxBlocks = maxBlocks;
	m_size = 0;
	m_begin = m_buffer;
}

byte *FilterWithBufferedInput::BlockQueue::GetBlock()
{
	if (m_size >= m_blockSize)
	{
		byte *ptr = m_begin;
		if ((m_begin = PtrAdd(m_begin, m_blockSize)) == m_buffer.end())
			m_begin = m_buffer;
		m_size -= m_blockSize;
		return ptr;
	}
	else
		return NULLPTR;
}

byte *FilterWithBufferedInput::BlockQueue::GetContigousBlocks(size_t &numberOfBytes)
{
	numberOfBytes = STDMIN(numberOfBytes, STDMIN<size_t>(PtrDiff(m_buffer.end(), m_begin), m_size));
	byte *ptr = m_begin;
	m_begin = PtrAdd(m_begin, numberOfBytes);
	m_size -= numberOfBytes;
	if (m_size == 0 || m_begin == m_buffer.end())
		m_begin = m_buffer;
	return ptr;
}

size_t FilterWithBufferedInput::BlockQueue::GetAll(byte *outString)
{
	// Avoid passing NULL pointer to memcpy
	if (!outString) return 0;

	size_t size = m_size;
	size_t numberOfBytes = m_maxBlocks*m_blockSize;
	const byte *ptr = GetContigousBlocks(numberOfBytes);
	memcpy(outString, ptr, numberOfBytes);
	memcpy(PtrAdd(outString, numberOfBytes), m_begin, m_size);
	m_size = 0;
	return size;
}

void FilterWithBufferedInput::BlockQueue::Put(const byte *inString, size_t length)
{
	// Avoid passing NULL pointer to memcpy
	if (!inString || !length) return;

	CRYPTOPP_ASSERT(m_size + length <= m_buffer.size());
	byte *end = (m_size < static_cast<size_t>(PtrDiff(m_buffer.end(), m_begin)) ?
		PtrAdd(m_begin, m_size) : PtrAdd(m_begin, m_size - m_buffer.size()));
	size_t len = STDMIN(length, size_t(m_buffer.end()-end));
	memcpy(end, inString, len);
	if (len < length)
		memcpy(m_buffer, PtrAdd(inString, len), length-len);
	m_size += length;
}

FilterWithBufferedInput::FilterWithBufferedInput(BufferedTransformation *attachment)
	: Filter(attachment), m_firstSize(SIZE_MAX), m_blockSize(0), m_lastSize(SIZE_MAX), m_firstInputDone(false)
{
}

FilterWithBufferedInput::FilterWithBufferedInput(size_t firstSize, size_t blockSize, size_t lastSize, BufferedTransformation *attachment)
	: Filter(attachment), m_firstSize(firstSize), m_blockSize(blockSize), m_lastSize(lastSize), m_firstInputDone(false)
{
	if (m_firstSize == SIZE_MAX || m_blockSize < 1 || m_lastSize == SIZE_MAX)
		throw InvalidArgument("FilterWithBufferedInput: invalid buffer size");

	m_queue.ResetQueue(1, m_firstSize);
}

void FilterWithBufferedInput::IsolatedInitialize(const NameValuePairs &parameters)
{
	InitializeDerivedAndReturnNewSizes(parameters, m_firstSize, m_blockSize, m_lastSize);
	if (m_firstSize == SIZE_MAX || m_blockSize < 1 || m_lastSize == SIZE_MAX)
		throw InvalidArgument("FilterWithBufferedInput: invalid buffer size");
	m_queue.ResetQueue(1, m_firstSize);
	m_firstInputDone = false;
}

bool FilterWithBufferedInput::IsolatedFlush(bool hardFlush, bool blocking)
{
	if (!blocking)
		throw BlockingInputOnly("FilterWithBufferedInput");

	if (hardFlush)
		ForceNextPut();
	FlushDerived();

	return false;
}

size_t FilterWithBufferedInput::PutMaybeModifiable(byte *inString, size_t length, int messageEnd, bool blocking, bool modifiable)
{
	if (!blocking)
		throw BlockingInputOnly("FilterWithBufferedInput");

	if (length != 0)
	{
		size_t newLength = m_queue.CurrentSize() + length;

		if (!m_firstInputDone && newLength >= m_firstSize)
		{
			size_t len = m_firstSize - m_queue.CurrentSize();
			m_queue.Put(inString, len);
			FirstPut(m_queue.GetContigousBlocks(m_firstSize));
			CRYPTOPP_ASSERT(m_queue.CurrentSize() == 0);
			m_queue.ResetQueue(m_blockSize, (2*m_blockSize+m_lastSize-2)/m_blockSize);

			inString = PtrAdd(inString, len);
			newLength -= m_firstSize;
			m_firstInputDone = true;
		}

		if (m_firstInputDone)
		{
			if (m_blockSize == 1)
			{
				while (newLength > m_lastSize && m_queue.CurrentSize() > 0)
				{
					size_t len = newLength - m_lastSize;
					byte *ptr = m_queue.GetContigousBlocks(len);
					NextPutModifiable(ptr, len);
					newLength -= len;
				}

				if (newLength > m_lastSize)
				{
					size_t len = newLength - m_lastSize;
					NextPutMaybeModifiable(inString, len, modifiable);
					inString = PtrAdd(inString, len);
					newLength -= len;
				}
			}
			else
			{
				while (newLength >= m_blockSize + m_lastSize && m_queue.CurrentSize() >= m_blockSize)
				{
					NextPutModifiable(m_queue.GetBlock(), m_blockSize);
					newLength -= m_blockSize;
				}

				if (newLength >= m_blockSize + m_lastSize && m_queue.CurrentSize() > 0)
				{
					CRYPTOPP_ASSERT(m_queue.CurrentSize() < m_blockSize);
					size_t len = m_blockSize - m_queue.CurrentSize();
					m_queue.Put(inString, len);
					inString = PtrAdd(inString, len);
					NextPutModifiable(m_queue.GetBlock(), m_blockSize);
					newLength -= m_blockSize;
				}

				if (newLength >= m_blockSize + m_lastSize)
				{
					size_t len = RoundDownToMultipleOf(newLength - m_lastSize, m_blockSize);
					NextPutMaybeModifiable(inString, len, modifiable);
					inString = PtrAdd(inString, len);
					newLength -= len;
				}
			}
		}

		m_queue.Put(inString, newLength - m_queue.CurrentSize());
	}

	if (messageEnd)
	{
		if (!m_firstInputDone && m_firstSize==0)
			FirstPut(NULLPTR);

		SecByteBlock temp(m_queue.CurrentSize());
		m_queue.GetAll(temp);
		LastPut(temp, temp.size());

		m_firstInputDone = false;
		m_queue.ResetQueue(1, m_firstSize);

		// Cast to void to suppress Coverity finding
		(void)Output(1, NULLPTR, 0, messageEnd, blocking);
	}
	return 0;
}

void FilterWithBufferedInput::ForceNextPut()
{
	if (!m_firstInputDone)
		return;

	if (m_blockSize > 1)
	{
		while (m_queue.CurrentSize() >= m_blockSize)
			NextPutModifiable(m_queue.GetBlock(), m_blockSize);
	}
	else
	{
		size_t len;
		while ((len = m_queue.CurrentSize()) > 0)
			NextPutModifiable(m_queue.GetContigousBlocks(len), len);
	}
}

void FilterWithBufferedInput::NextPutMultiple(const byte *inString, size_t length)
{
	CRYPTOPP_ASSERT(m_blockSize > 1);	// m_blockSize = 1 should always override this function
	while (length > 0)
	{
		CRYPTOPP_ASSERT(length >= m_blockSize);
		NextPutSingle(inString);
		inString = PtrAdd(inString, m_blockSize);
		length -= m_blockSize;
	}
}

// *************************************************************

void Redirector::Initialize(const NameValuePairs &parameters, int propagation)
{
	m_target = parameters.GetValueWithDefault("RedirectionTargetPointer", (BufferedTransformation*)NULLPTR);
	m_behavior = parameters.GetIntValueWithDefault("RedirectionBehavior", PASS_EVERYTHING);

	if (m_target && GetPassSignals())
		m_target->Initialize(parameters, propagation);
}

// *************************************************************

ProxyFilter::ProxyFilter(BufferedTransformation *filter, size_t firstSize, size_t lastSize, BufferedTransformation *attachment)
	: FilterWithBufferedInput(firstSize, 1, lastSize, attachment), m_filter(filter)
{
	if (m_filter.get())
		m_filter->Attach(new OutputProxy(*this, false));
}

bool ProxyFilter::IsolatedFlush(bool hardFlush, bool blocking)
{
	return m_filter.get() ? m_filter->Flush(hardFlush, -1, blocking) : false;
}

void ProxyFilter::SetFilter(Filter *filter)
{
	m_filter.reset(filter);
	if (filter)
	{
		OutputProxy *proxy;
		member_ptr<OutputProxy> temp(proxy = new OutputProxy(*this, false));
		m_filter->TransferAllTo(*proxy);
		m_filter->Attach(temp.release());
	}
}

void ProxyFilter::NextPutMultiple(const byte *s, size_t len)
{
	if (m_filter.get())
		m_filter->Put(s, len);
}

void ProxyFilter::NextPutModifiable(byte *s, size_t len)
{
	if (m_filter.get())
		m_filter->PutModifiable(s, len);
}

// *************************************************************

void RandomNumberSink::IsolatedInitialize(const NameValuePairs &parameters)
{
	parameters.GetRequiredParameter("RandomNumberSink", "RandomNumberGeneratorPointer", m_rng);
}

size_t RandomNumberSink::Put2(const byte *begin, size_t length, int messageEnd, bool blocking)
{
	CRYPTOPP_UNUSED(messageEnd); CRYPTOPP_UNUSED(blocking);
	m_rng->IncorporateEntropy(begin, length);
	return 0;
}

size_t ArraySink::Put2(const byte *begin, size_t length, int messageEnd, bool blocking)
{
	CRYPTOPP_UNUSED(messageEnd); CRYPTOPP_UNUSED(blocking);

	// Avoid passing NULL pointer to memcpy. Using memmove due to
	//  Valgrind finding on overlapping buffers.
	size_t copied = 0;
	if (m_buf && begin)
	{
		copied = STDMIN(length, SaturatingSubtract(m_size, m_total));
		memmove(PtrAdd(m_buf, m_total), begin, copied);
	}
	m_total += copied;
	return length - copied;
}

byte * ArraySink::CreatePutSpace(size_t &size)
{
	size = SaturatingSubtract(m_size, m_total);
	return PtrAdd(m_buf, m_total);
}

void ArraySink::IsolatedInitialize(const NameValuePairs &parameters)
{
	ByteArrayParameter array;
	if (!parameters.GetValue(Name::OutputBuffer(), array))
		throw InvalidArgument("ArraySink: missing OutputBuffer argument");
	m_buf = array.begin();
	m_size = array.size();
}

size_t ArrayXorSink::Put2(const byte *begin, size_t length, int messageEnd, bool blocking)
{
	CRYPTOPP_UNUSED(messageEnd); CRYPTOPP_UNUSED(blocking);

	// Avoid passing NULL pointer to xorbuf
	size_t copied = 0;
	if (m_buf && begin)
	{
		copied = STDMIN(length, SaturatingSubtract(m_size, m_total));
		xorbuf(PtrAdd(m_buf, m_total), begin, copied);
	}
	m_total += copied;
	return length - copied;
}

// *************************************************************

StreamTransformationFilter::StreamTransformationFilter(StreamTransformation &c, BufferedTransformation *attachment, BlockPaddingScheme padding)
	: FilterWithBufferedInput(attachment), m_cipher(c), m_padding(DEFAULT_PADDING)
{
	CRYPTOPP_ASSERT(c.MinLastBlockSize() == 0 || c.MinLastBlockSize() > c.MandatoryBlockSize());

	const bool authenticatedFilter = dynamic_cast<AuthenticatedSymmetricCipher *>(&c) != NULLPTR;
	if (authenticatedFilter)
		throw InvalidArgument("StreamTransformationFilter: please use AuthenticatedEncryptionFilter and AuthenticatedDecryptionFilter for AuthenticatedSymmetricCipher");

	// InitializeDerivedAndReturnNewSizes may override some of these
	m_mandatoryBlockSize = m_cipher.MandatoryBlockSize();
	m_optimalBufferSize = m_cipher.OptimalBlockSize();
	m_isSpecial = m_cipher.IsLastBlockSpecial() && m_mandatoryBlockSize > 1;
	m_reservedBufferSize = STDMAX(2*m_mandatoryBlockSize, m_optimalBufferSize);

	IsolatedInitialize(MakeParameters(Name::BlockPaddingScheme(), padding));
}

StreamTransformationFilter::StreamTransformationFilter(StreamTransformation &c, BufferedTransformation *attachment, BlockPaddingScheme padding, bool authenticated)
	: FilterWithBufferedInput(attachment), m_cipher(c), m_padding(DEFAULT_PADDING)
{
	const bool authenticatedFilter = dynamic_cast<AuthenticatedSymmetricCipher *>(&c) != NULLPTR;
	if (!authenticatedFilter)
	{
		CRYPTOPP_ASSERT(c.MinLastBlockSize() == 0 || c.MinLastBlockSize() > c.MandatoryBlockSize());
	}

	if (authenticatedFilter && !authenticated)
		throw InvalidArgument("StreamTransformationFilter: please use AuthenticatedEncryptionFilter and AuthenticatedDecryptionFilter for AuthenticatedSymmetricCipher");

	// InitializeDerivedAndReturnNewSizes may override some of these
	m_mandatoryBlockSize = m_cipher.MandatoryBlockSize();
	m_optimalBufferSize = m_cipher.OptimalBlockSize();
	m_isSpecial = m_cipher.IsLastBlockSpecial() && m_mandatoryBlockSize > 1;
	m_reservedBufferSize = STDMAX(2*m_mandatoryBlockSize, m_optimalBufferSize);

	IsolatedInitialize(MakeParameters(Name::BlockPaddingScheme(), padding));
}

size_t StreamTransformationFilter::LastBlockSize(StreamTransformation &c, BlockPaddingScheme padding)
{
	if (c.MinLastBlockSize() > 0)
		return c.MinLastBlockSize();
	else if (c.MandatoryBlockSize() > 1 && !c.IsForwardTransformation() && padding != NO_PADDING && padding != ZEROS_PADDING)
		return c.MandatoryBlockSize();

	return 0;
}

void StreamTransformationFilter::InitializeDerivedAndReturnNewSizes(const NameValuePairs &parameters, size_t &firstSize, size_t &blockSize, size_t &lastSize)
{
	BlockPaddingScheme padding = parameters.GetValueWithDefault(Name::BlockPaddingScheme(), DEFAULT_PADDING);
	bool isBlockCipher = (m_mandatoryBlockSize > 1 && m_cipher.MinLastBlockSize() == 0);

	if (padding == DEFAULT_PADDING)
		m_padding = isBlockCipher ? PKCS_PADDING : NO_PADDING;
	else
		m_padding = padding;

	if (!isBlockCipher)
	{
		if (m_padding == PKCS_PADDING)
			throw InvalidArgument("StreamTransformationFilter: PKCS_PADDING cannot be used with " + m_cipher.AlgorithmName());
		else if (m_padding == W3C_PADDING)
			throw InvalidArgument("StreamTransformationFilter: W3C_PADDING cannot be used with " + m_cipher.AlgorithmName());
		else if (m_padding == ONE_AND_ZEROS_PADDING)
			throw InvalidArgument("StreamTransformationFilter: ONE_AND_ZEROS_PADDING cannot be used with " + m_cipher.AlgorithmName());
	}

	firstSize = 0;
	blockSize = m_mandatoryBlockSize;
	lastSize = LastBlockSize(m_cipher, m_padding);
}

void StreamTransformationFilter::FirstPut(const byte* inString)
{
	CRYPTOPP_UNUSED(inString);
	m_optimalBufferSize = STDMAX<unsigned int>(m_optimalBufferSize, RoundDownToMultipleOf(4096U, m_optimalBufferSize));
}

void StreamTransformationFilter::NextPutMultiple(const byte *inString, size_t length)
{
	if (!length)
		{return;}

	const size_t s = m_cipher.MandatoryBlockSize();
	do
	{
		size_t len = m_optimalBufferSize;
		byte *space = HelpCreatePutSpace(*AttachedTransformation(), DEFAULT_CHANNEL, s, length, len);
		if (len < length)
		{
			if (len == m_optimalBufferSize)
				len -= m_cipher.GetOptimalBlockSizeUsed();
			len = RoundDownToMultipleOf(len, s);
		}
		else
			len = length;
		m_cipher.ProcessString(space, inString, len);
		AttachedTransformation()->PutModifiable(space, len);
		inString = PtrAdd(inString, len);
		length -= len;
	}
	while (length > 0);
}

void StreamTransformationFilter::NextPutModifiable(byte *inString, size_t length)
{
	m_cipher.ProcessString(inString, length);
	AttachedTransformation()->PutModifiable(inString, length);
}

void StreamTransformationFilter::LastPut(const byte *inString, size_t length)
{
	// This block is new to StreamTransformationFilter. It is somewhat of a hack and was
	//  added for OCB mode; see GitHub Issue 515. The rub with OCB is, its a block cipher
	//  and the last block size can be 0. However, "last block = 0" is not the 0 predicated
	//  in the original code. In the orginal code 0 means "nothing special" so
	//  DEFAULT_PADDING is applied. OCB's 0 literally means a final block size can be 0 or
	//  non-0; and no padding is needed in either case because OCB has its own scheme (see
	//  handling of P_* and A_*).
	// Stream ciphers have policy objects to convey how to operate the cipher. The Crypto++
	//  framework operates well when MinLastBlockSize() is 1. However, it did not appear to
	//  cover the OCB case either because we can't stream OCB. It needs full block sizes. In
	//  response we hacked a IsLastBlockSpecial(). When true StreamTransformationFilter
	//  defers to the mode for processing of the last block.
	// The behavior supplied when IsLastBlockSpecial() will likely have to evolve to capture
	//  more complex cases from different authenc modes. I suspect it will have to change
	//  from a simple bool to something that conveys more information, like "last block
	//  no padding" or "custom padding applied by cipher".
	// In some respect we have already hit the need for more information. For example, OCB
	//  calculates the checksum on the cipher text at the same time, so we don't need the
	//  disjoint behavior of calling "EncryptBlock" followed by a separate "AuthenticateBlock".
	//  Additional information may allow us to avoid the two separate calls.
	if (m_isSpecial)
	{
		const size_t leftOver = length % m_mandatoryBlockSize;
		byte* space = HelpCreatePutSpace(*AttachedTransformation(), DEFAULT_CHANNEL, m_reservedBufferSize);
		length -= leftOver;

		if (length)
		{
			// Process full blocks
			m_cipher.ProcessData(space, inString, length);
			AttachedTransformation()->Put(space, length);
			inString = PtrAdd(inString, length);
		}

		if (leftOver)
		{
			// Process final partial block
			length = m_cipher.ProcessLastBlock(space, m_reservedBufferSize, inString, leftOver);
			AttachedTransformation()->Put(space, length);
		}
		else
		{
			// Process final empty block
			length = m_cipher.ProcessLastBlock(space, m_reservedBufferSize, NULLPTR, 0);
			AttachedTransformation()->Put(space, length);
		}

		return;
	}

	switch (m_padding)
	{
	case NO_PADDING:
	case ZEROS_PADDING:
		if (length > 0)
		{
			const size_t minLastBlockSize = m_cipher.MinLastBlockSize();
			const bool isForwardTransformation = m_cipher.IsForwardTransformation();

			if (isForwardTransformation && m_padding == ZEROS_PADDING && (minLastBlockSize == 0 || length < minLastBlockSize))
			{
				// do padding
				size_t blockSize = STDMAX(minLastBlockSize, (size_t)m_mandatoryBlockSize);
				byte* space = HelpCreatePutSpace(*AttachedTransformation(), DEFAULT_CHANNEL, blockSize);
				if (inString) {memcpy(space, inString, length);}
				memset(PtrAdd(space, length), 0, blockSize - length);
				size_t used = m_cipher.ProcessLastBlock(space, blockSize, space, blockSize);
				AttachedTransformation()->Put(space, used);
			}
			else
			{
				if (minLastBlockSize == 0)
				{
					if (isForwardTransformation)
						throw InvalidDataFormat("StreamTransformationFilter: plaintext length is not a multiple of block size and NO_PADDING is specified");
					else
						throw InvalidCiphertext("StreamTransformationFilter: ciphertext length is not a multiple of block size");
				}

				byte* space = HelpCreatePutSpace(*AttachedTransformation(), DEFAULT_CHANNEL, length, m_optimalBufferSize);
				size_t used = m_cipher.ProcessLastBlock(space, length, inString, length);
				AttachedTransformation()->Put(space, used);
			}
		}
		break;

	case PKCS_PADDING:
	case W3C_PADDING:
	case ONE_AND_ZEROS_PADDING:
		unsigned int s;
		byte* space;
		s = m_mandatoryBlockSize;
		CRYPTOPP_ASSERT(s > 1);
		space = HelpCreatePutSpace(*AttachedTransformation(), DEFAULT_CHANNEL, s, m_optimalBufferSize);
		if (m_cipher.IsForwardTransformation())
		{
			CRYPTOPP_ASSERT(length < s);
			if (inString) {memcpy(space, inString, length);}
			if (m_padding == PKCS_PADDING)
			{
				CRYPTOPP_ASSERT(s < 256);
				byte pad = static_cast<byte>(s-length);
				memset(PtrAdd(space, length), pad, s-length);
			}
			else if (m_padding == W3C_PADDING)
			{
				CRYPTOPP_ASSERT(s < 256);
				memset(PtrAdd(space, length), 0, s-length-1);
				space[s-1] = static_cast<byte>(s-length);
			}
			else
			{
				space[length] = 0x80;
				memset(PtrAdd(space, length+1), 0, s-length-1);
			}
			m_cipher.ProcessData(space, space, s);
			AttachedTransformation()->Put(space, s);
		}
		else
		{
			if (length != s)
				throw InvalidCiphertext("StreamTransformationFilter: ciphertext length is not a multiple of block size");
			m_cipher.ProcessData(space, inString, s);
			if (m_padding == PKCS_PADDING)
			{
				byte pad = space[s-1];
                                if (pad < 1 || pad > s || FindIfNot(PtrAdd(space, s-pad), PtrAdd(space, s), pad) != PtrAdd(space, s))
                                    throw InvalidCiphertext("StreamTransformationFilter: invalid PKCS #7 block padding found");
				length = s-pad;
			}
			else if (m_padding == W3C_PADDING)
			{
				byte pad = space[s - 1];
				if (pad < 1 || pad > s)
					throw InvalidCiphertext("StreamTransformationFilter: invalid W3C block padding found");
				length = s - pad;
			}
			else
			{
				while (length > 1 && space[length-1] == 0)
					--length;
				if (space[--length] != 0x80)
					throw InvalidCiphertext("StreamTransformationFilter: invalid ones-and-zeros padding found");
			}
			AttachedTransformation()->Put(space, length);
		}
		break;

	default:
		CRYPTOPP_ASSERT(false);
	}
}

// *************************************************************

HashFilter::HashFilter(HashTransformation &hm, BufferedTransformation *attachment, bool putMessage, int truncatedDigestSize, const std::string &messagePutChannel, const std::string &hashPutChannel)
	: m_hashModule(hm), m_putMessage(putMessage), m_digestSize(0), m_space(NULLPTR)
	, m_messagePutChannel(messagePutChannel), m_hashPutChannel(hashPutChannel)
{
	m_digestSize = truncatedDigestSize < 0 ? m_hashModule.DigestSize() : truncatedDigestSize;
	Detach(attachment);
}

void HashFilter::IsolatedInitialize(const NameValuePairs &parameters)
{
	m_putMessage = parameters.GetValueWithDefault(Name::PutMessage(), false);
	int s = parameters.GetIntValueWithDefault(Name::TruncatedDigestSize(), -1);
	m_digestSize = s < 0 ? m_hashModule.DigestSize() : s;
}

size_t HashFilter::Put2(const byte *inString, size_t length, int messageEnd, bool blocking)
{
	FILTER_BEGIN;
	if (m_putMessage)
		FILTER_OUTPUT3(1, 0, inString, length, 0, m_messagePutChannel);
	if (inString && length)
		m_hashModule.Update(inString, length);
	if (messageEnd)
	{
		{
			size_t size;
			m_space = HelpCreatePutSpace(*AttachedTransformation(), m_hashPutChannel, m_digestSize, m_digestSize, size = m_digestSize);
			m_hashModule.TruncatedFinal(m_space, m_digestSize);
		}
		FILTER_OUTPUT3(2, 0, m_space, m_digestSize, messageEnd, m_hashPutChannel);
	}
	FILTER_END_NO_MESSAGE_END;
}

// *************************************************************

HashVerificationFilter::HashVerificationFilter(HashTransformation &hm, BufferedTransformation *attachment, word32 flags, int truncatedDigestSize)
	: FilterWithBufferedInput(attachment)
	, m_hashModule(hm), m_flags(0), m_digestSize(0), m_verified(false)
{
	IsolatedInitialize(MakeParameters(Name::HashVerificationFilterFlags(), flags)(Name::TruncatedDigestSize(), truncatedDigestSize));
}

void HashVerificationFilter::InitializeDerivedAndReturnNewSizes(const NameValuePairs &parameters, size_t &firstSize, size_t &blockSize, size_t &lastSize)
{
	m_flags = parameters.GetValueWithDefault(Name::HashVerificationFilterFlags(), (word32)DEFAULT_FLAGS);
	int s = parameters.GetIntValueWithDefault(Name::TruncatedDigestSize(), -1);
	m_digestSize = s < 0 ? m_hashModule.DigestSize() : s;
	m_verified = false;
	firstSize = m_flags & HASH_AT_BEGIN ? m_digestSize : 0;
	blockSize = 1;
	lastSize = m_flags & HASH_AT_BEGIN ? 0 : m_digestSize;
}

void HashVerificationFilter::FirstPut(const byte *inString)
{
	if (m_flags & HASH_AT_BEGIN)
	{
		m_expectedHash.New(m_digestSize);
		if (inString) {memcpy(m_expectedHash, inString, m_expectedHash.size());}
		if (m_flags & PUT_HASH)
			AttachedTransformation()->Put(inString, m_expectedHash.size());
	}
}

void HashVerificationFilter::NextPutMultiple(const byte *inString, size_t length)
{
	m_hashModule.Update(inString, length);
	if (m_flags & PUT_MESSAGE)
		AttachedTransformation()->Put(inString, length);
}

void HashVerificationFilter::LastPut(const byte *inString, size_t length)
{
	if (m_flags & HASH_AT_BEGIN)
	{
		CRYPTOPP_ASSERT(length == 0);
		m_verified = m_hashModule.TruncatedVerify(m_expectedHash, m_digestSize);
	}
	else
	{
		m_verified = (length==m_digestSize && m_hashModule.TruncatedVerify(inString, length));
		if (m_flags & PUT_HASH)
			AttachedTransformation()->Put(inString, length);
	}

	if (m_flags & PUT_RESULT)
		AttachedTransformation()->Put(m_verified);

	if ((m_flags & THROW_EXCEPTION) && !m_verified)
		throw HashVerificationFailed();
}

// *************************************************************

AuthenticatedEncryptionFilter::AuthenticatedEncryptionFilter(AuthenticatedSymmetricCipher &c, BufferedTransformation *attachment,
								bool putAAD, int truncatedDigestSize, const std::string &macChannel, BlockPaddingScheme padding)
	: StreamTransformationFilter(c, attachment, padding, true)
	, m_hf(c, new OutputProxy(*this, false), putAAD, truncatedDigestSize, AAD_CHANNEL, macChannel)
{
	CRYPTOPP_ASSERT(c.IsForwardTransformation());
}

void AuthenticatedEncryptionFilter::IsolatedInitialize(const NameValuePairs &parameters)
{
	m_hf.IsolatedInitialize(parameters);
	StreamTransformationFilter::IsolatedInitialize(parameters);
}

byte * AuthenticatedEncryptionFilter::ChannelCreatePutSpace(const std::string &channel, size_t &size)
{
	if (channel.empty())
		return StreamTransformationFilter::CreatePutSpace(size);

	if (channel == AAD_CHANNEL)
		return m_hf.CreatePutSpace(size);

	throw InvalidChannelName("AuthenticatedEncryptionFilter", channel);
}

size_t AuthenticatedEncryptionFilter::ChannelPut2(const std::string &channel, const byte *begin, size_t length, int messageEnd, bool blocking)
{
	if (channel.empty())
		return StreamTransformationFilter::Put2(begin, length, messageEnd, blocking);

	if (channel == AAD_CHANNEL)
		return m_hf.Put2(begin, length, 0, blocking);

	throw InvalidChannelName("AuthenticatedEncryptionFilter", channel);
}

void AuthenticatedEncryptionFilter::LastPut(const byte *inString, size_t length)
{
	StreamTransformationFilter::LastPut(inString, length);
	m_hf.MessageEnd();
}

// *************************************************************

AuthenticatedDecryptionFilter::AuthenticatedDecryptionFilter(AuthenticatedSymmetricCipher &c, BufferedTransformation *attachment, word32 flags, int truncatedDigestSize, BlockPaddingScheme padding)
	: FilterWithBufferedInput(attachment)
	, m_hashVerifier(c, new OutputProxy(*this, false))
	, m_streamFilter(c, new OutputProxy(*this, false), padding, true)
{
	CRYPTOPP_ASSERT(!c.IsForwardTransformation() || c.IsSelfInverting());
	IsolatedInitialize(MakeParameters(Name::BlockPaddingScheme(), padding)(Name::AuthenticatedDecryptionFilterFlags(), flags)(Name::TruncatedDigestSize(), truncatedDigestSize));
}

void AuthenticatedDecryptionFilter::InitializeDerivedAndReturnNewSizes(const NameValuePairs &parameters, size_t &firstSize, size_t &blockSize, size_t &lastSize)
{
	word32 flags = parameters.GetValueWithDefault(Name::AuthenticatedDecryptionFilterFlags(), (word32)DEFAULT_FLAGS);

	m_hashVerifier.Initialize(CombinedNameValuePairs(parameters, MakeParameters(Name::HashVerificationFilterFlags(), flags)));
	m_streamFilter.Initialize(parameters);

	firstSize = m_hashVerifier.m_firstSize;
	blockSize = 1;
	lastSize = m_hashVerifier.m_lastSize;
}

byte * AuthenticatedDecryptionFilter::ChannelCreatePutSpace(const std::string &channel, size_t &size)
{
	if (channel.empty())
		return m_streamFilter.CreatePutSpace(size);

	if (channel == AAD_CHANNEL)
		return m_hashVerifier.CreatePutSpace(size);

	throw InvalidChannelName("AuthenticatedDecryptionFilter", channel);
}

size_t AuthenticatedDecryptionFilter::ChannelPut2(const std::string &channel, const byte *begin, size_t length, int messageEnd, bool blocking)
{
	if (channel.empty())
	{
		if (m_lastSize > 0)
			m_hashVerifier.ForceNextPut();
		return FilterWithBufferedInput::Put2(begin, length, messageEnd, blocking);
	}

	if (channel == AAD_CHANNEL)
		return m_hashVerifier.Put2(begin, length, 0, blocking);

	throw InvalidChannelName("AuthenticatedDecryptionFilter", channel);
}

void AuthenticatedDecryptionFilter::FirstPut(const byte *inString)
{
	m_hashVerifier.Put(inString, m_firstSize);
}

void AuthenticatedDecryptionFilter::NextPutMultiple(const byte *inString, size_t length)
{
	m_streamFilter.Put(inString, length);
}

void AuthenticatedDecryptionFilter::LastPut(const byte *inString, size_t length)
{
	m_streamFilter.MessageEnd();
	m_hashVerifier.PutMessageEnd(inString, length);
}

// *************************************************************

void SignerFilter::IsolatedInitialize(const NameValuePairs &parameters)
{
	m_putMessage = parameters.GetValueWithDefault(Name::PutMessage(), false);
	m_messageAccumulator.reset(m_signer.NewSignatureAccumulator(m_rng));
}

size_t SignerFilter::Put2(const byte *inString, size_t length, int messageEnd, bool blocking)
{
	FILTER_BEGIN;
	m_messageAccumulator->Update(inString, length);
	if (m_putMessage)
		FILTER_OUTPUT(1, inString, length, 0);
	if (messageEnd)
	{
		m_buf.New(m_signer.SignatureLength());
		m_signer.Sign(m_rng, m_messageAccumulator.release(), m_buf);
		FILTER_OUTPUT(2, m_buf, m_buf.size(), messageEnd);
		m_messageAccumulator.reset(m_signer.NewSignatureAccumulator(m_rng));
	}
	FILTER_END_NO_MESSAGE_END;
}

SignatureVerificationFilter::SignatureVerificationFilter(const PK_Verifier &verifier, BufferedTransformation *attachment, word32 flags)
	: FilterWithBufferedInput(attachment)
	, m_verifier(verifier), m_flags(0), m_verified(0)
{
	IsolatedInitialize(MakeParameters(Name::SignatureVerificationFilterFlags(), flags));
}

void SignatureVerificationFilter::InitializeDerivedAndReturnNewSizes(const NameValuePairs &parameters, size_t &firstSize, size_t &blockSize, size_t &lastSize)
{
	m_flags = parameters.GetValueWithDefault(Name::SignatureVerificationFilterFlags(), (word32)DEFAULT_FLAGS);
	m_messageAccumulator.reset(m_verifier.NewVerificationAccumulator());
	size_t size = m_verifier.SignatureLength();
	CRYPTOPP_ASSERT(size != 0);	// TODO: handle recoverable signature scheme
	m_verified = false;
	firstSize = m_flags & SIGNATURE_AT_BEGIN ? size : 0;
	blockSize = 1;
	lastSize = m_flags & SIGNATURE_AT_BEGIN ? 0 : size;
}

void SignatureVerificationFilter::FirstPut(const byte *inString)
{
	if (?!«Ä1‰œãT¡9Tºñ¸GÌ¿F•}∞§sâ¿$C¬°ìÒQ4´ì1ì‹!”¨§Û™∏@C…{av’ÆÁ4·≤‹1≥OIBGO‡M	‘œ	@ÿJu	Ã3›Ãê¡¡w–whD3XR(WP1–≥	ŸU¿œ1– L9œ1‰L9q§2w·r«Ñ≥ﬁú0«‡ëÿO ∫§ôˇª≥«L««Q{Oä≈	– …Ö8\&L¡	‡ÿ§( >°a/$-C¬ôCƒÓs ‰¡⁄∞ ˘ªT5[`Ã!*¡PU)29*1î*9T%ËS%œ1ËL9lRV¡2
TåC9í„ß«	 –«Ã«‡«1»≈
§(≈‰ «ƒ≈	ËOãOÜ0#Oaº Ã≈1¯¡2T ≈	≈å¡9¯È  O‰≈ ‡≥	¸«	¸w œNLLC¿g ≥≥≥«	‡≈œ ƒLLL«4≈… Dåã… M (ut9J «‡≈4≈	d¸un¬ z å…ÄåCƒŸ±≥≥úD¡≈ «œ8 ¸¡Hƒ«|$h dL«Ç≈˘$ O§/®∏≥$0§‡…  ≥wã9euÒA9auÚ– v80Cã…8  Ç•Å1 ã…<(C»o «	»u…0 2E«∫≈	‡ u1¸2O≥	‡ ≥:H¡‰§ñ˜∏ q‰≈1∞≈ 1¨≈1®≈1§ ≈1†u4∏9SDuÒÅrC…› 8«Ñ«¸ÅM≥‹¥Ä+@M	‡ß2«Z≥ 9Ï«J«Ç≥‹F¨Ä¡¡xÄ8Ä	«D«H «	¿≈ÿ« »gçOç≈…3Ä™¡pÇ+≈…Ä≈‘Oú«êM≈Ÿ ¡˘(¡1¨ ÕÈÉ:O	‘ <œÈÅJL«ÒAôœãT¡9œ7tLÄƒ	†≈ 4«	†«‰œà∞≥ÿ≈…,Ä(! Üœçº≈Çá§T  £≥•ÿÄ,wãàC»¿¿«J¿aëB<≥2F1˘AQ¿2 2OÇw¿2\¿3wΩ8E†M‡≈¡DÉ$«é«Ÿ¡OÜ≈î@ÜÄ(≈ú≈ò¿>	ê¡9úã4B+OÄ*Oÿ…(≈Ç°ˇAçt ûwÜ9B °w	@‹0O«	‹@¡ ? «
DgJ≈uŸÄC»g¡ÄgwÜC…|¿ ¡˜7ÄœåHÈ@≈œ‹ ;≈¸$uﬂÇ∏¬ø¡>	¸iÅ™¶∂á™í¿Ü™«xƒÑ•§å≠∏≥ô¿
…ÎÄØ§‡ƒﬂ¿u ÅßC…À¿QËQœ1ËL 9lœ1‰L9V¿ﬁå≈	ú≈	 ò≈	î≈ÜEß «	–«Ã« ‡«1»≈
« 8g ≈‰ «ƒ≈	ËOã’@U¡¡äO¿ÒBàÄÒ˘Çî≈¡Åç¿·CéAYëÙ\BÇ@z‘ ßA¡œ·#≥Y tLœÒajL9F¡0E)¸Lœ˜aA%LC¬ a*c%Ú:µg%˛`f%¿z≥|¢%§˛¨∏≥H≈	‰ &»≤¿Ö≈ƒ `≈ƒ<¢ 8¢ 4† ¡<T¡1¡ê¡	¯ÈX≥Y†¡,Ñ†« 	ÙO	å«¯ ’gé«_&Lù ¥œç≤«á ≥XÚ¡Ö§@«1‰ªîW0å≈Àa£’P«ÁNÂ§X@‰°«	¯–O	¿Ù‚`@&Lù¥„‡§®ì∏≥‰·Á*å‰NÎ¯‰≈Àa´Z¯≤≥@≥åu… ‡18"^√ãuÕº`£C»
í†u‚õ8E««—aØßO @uÇú`ÇX`4gç&¿X≥˘4·:†EB…««Ë` ≥§àDπ≥ &P¡	à §\ÂOL«ﬂÃÄgﬂÇ∫	‹«¡°Ég	ƒOÜœ à@≈9àã	ÑK ≈1ÄwÑ2v`T&MÄ≈ú√
&N§6uπ≥«œa&N≥`<l≥Yx 6ƒMÑ§L`ßWuÒÅ$9_&O(§0 GHLéDL& Ù°}L§¶F HL«9D«ï ≥≈9åuN9Kå•ì ««¯¿wÉ8Y«M≥‹Ë‡E…å8D`å•å¿‡≈ 1àœ≤h;8Äuœ≤EC»4†( œ≤A8Sœ≤l 8Vœ≤o9u@7PC»VH†ÿá@§lã≥≥•‡Làã	à`	L•N‡ÅµOLL•¶Ä  «äœ§iC»∆a¿C»d‡	¿ [-£V†¢á §^‚¥a E§B‹Å	å≥CC¬Î`…≥Cƒöc!¬C¡Ü`„e≥Ä9å¡HÙ«|#»6‡Å^o@9¢”Ù§ p˘∏≥&L&j@¡Ùã	∞‡L§‘nπ ê4x« Ù«∏wé 1`4<w∏ 3'*«H*≈	Ñå† Ü¡	ÑÄxuLC˚Ä	Ñw	å8m`°¬¿¿§[Ã£≥íw	¿•∞pÇt…å9F≈L9ÄÙX≠3`cüÄ$LKÃ§Œ¡‡ §ªˆ∏≥ Ê≥ë’«–rQF†¶3¿yÊ ‹·R«œ«¡h9–&h$LÀ–í%∏•PBAÒœ≤iC…—VœÑ˜‡LC» Ü–…Ö8]Å(‡ÄÄI≥ßN¿*ëÚ`wL§ *π≥$Ù' L§FêsJÑ… ≥9«Ñ§ô∑µ8@
1§jßB≤· [u‘¸–&9¿C&M≥¸‡ê ê Ê∞≥«Ñ& L§ã\∂≥•&áëj‚0ãÉC»¡0àu˜¸∞	C»N∞  œ≤d8Eœ≤jhC…∏Ò
œ·!™Å– u4lC»ëb´a5aÉeäÄ 3UÏó±
—˝@bSu¡	¨ÄJÈ§√‡ ≥≥œ1åd9y «9êw9†C¿n∆`8É:‡u<« ¥§∑ π≥w¥ 1*g9òßl«Ä9òw9®3 …å2g9ê«"ä w&M∞§Q&â∞°"$I∞&LP¡¨LRDMt?` ∫ã	ËpL ≈9‰≈9¯≈9Ä¸≈9‡¡	Ë  ≈9∞§[Ñ≥≥!a+≥ˇ!!≈9@Ñœ†\«∞p~ÈÈ`È3È§g ∞≥œàdœ7ÄPL≈	Ä8W%[Q«á9K§z é≥≥ßI§wè≥≥¿·.¡Ë§ r	∞≥ßQœ≤k8BTEC…º‚∑ á§H†0,`1ÄC»5ê&)w
	‡ +‡ ∫wä 8u< 8^≈l< µ+†HÃÄ≥u‘<<¡<»»¡“ Ò(T– ≈Buy@@,L80«Ûä 8>u?P8!«09ÄœäPp“ú« ¨≈«®≈π`òsÄ+C˚å@C˚9««¡#« MçÆ\Gö≥‹R1¿H∫C∞aPY9Ä«Ok P&∞$Ip_≥Y`ëZ9Ä"≥„«Jêü≥‹±œ¬*L8D«á§=`RS°2 ≈p2<≥‹#0K»N‡B
”5D—5T@«L§‡î”5ÄÖaU`œÙºB@)òC»ß°)"¡:Ñåå@öhë	ú3≤DÅ%	†w	ê 3«	®w	òHC¡Í@ß7‰Õ ò@LL§Ó∏µp9Äp9âV∂4≥«;È‡± 2h«Äë¨0	@ÑßY§VN3_P1]TêÕUTD1&Lí"1 	Ä«ƒ\LL- JXÛ-‡ˇ1≥‹F‡†Jœ1à–L§Å2Q1ñ– $ºR∞,õ¿ º0ï9¿å…∫C»åq¬«dV	«◊" ó8O(«lSe–@…ó¿8C≥Y¯îé¥p/
ÄX82¿e)¨Vß&G`§oT™≤¿4à¿?]TPE$&MOAÒOå«M¡=ï+≥9®≥`9¨≥‹å —8ÄW«à§À+Ak48C± 7∞ ∞&Lπ`c«ã§<O∞ÄHL&HÙW~%∞Ä9‡x$$,§õÇË†#«Ñ≈º—≤ ∞wÑ8K§@õ¶≥≥§d0è ÙH˚Lè«≥ «†«	D&HÄ≈D≈HpÖÙ˙ò≈Fé ÜÑ≈„ ¿«Ω«J≥‹∞†"Y@	›Ò≥±^F«@\«Ñ≥ﬁ¥†œB2p’{«dPZ%·"x}å8(«B`¿_§t Õ†‚	¿ç…∫9IH§Üˇ‡cà¡‹L¿∏Õ∂£ê:mqL¿s?V≥Ä9\≈d«ÿ@ÿ
H§î¬π ≥œ*dLéê@L«çd*S«``F«Ã&å	∞—tpI•ò9®≤†•vgça´ªúoçE“º †«g	Dª îWåªúoçÄéHL«≥« ∞
∞|X«Ω«J≥xÜoä®è«≥""`"|u	D≈`Ñt	…å8G ßN«é«|…@û9ª≈| d…å9[«t \«`…Ö8 K«ÕåOLLèÄåè«ù«  8T«ƒ|BLL§Û≥≥èà«Ãl è«ÄÃ9ß!8BÉ 60H«P!Çi
 	|≥<l≥Y¯uL o«@«M≥‹àMÄ+å 9S«
œÏl L &L§˜D ∂≥œ*LœÍLÏL   é ^ œ¥≥8A§zê¬ª≥ÄYN2 Å«D§kp`≥≥åÜ|®‹§ !ÄVœ2L 9Xœ2,L9B «
PO	@≈
hÄßNåéD·¡	¥ §u·¥≥«LÖ ÀÄ.œ†lÌ¨ ÿLâ≈	∞ «	D«ï ¡?T≈	®≥àYt5C…W 0¡1†È  ∫u ?$8
«	†O 	∏&±’géù ¥≈	†œåN ≈	∏¡	†≥YÃ C§sd∫ ≥«\œ†\«∞¡9¡≥9®« ÑÈ≥ﬁËNLLD•ã¿.uˇ‡@8ÄX«	ºO	¥¬ ≈	º≈	¥ßjSD∑D†@â« twä9K«á §E±≥≥≈	§ wä8;≈9¨uˇ‹Å~¬8g« ÃÑ@LL≈	¨§¸kÅ#«Ñ≥ﬁúÄ«†¡%Ä,† 
≈¸	§¬	@Ö√$«\Cÿ
çÄ-¬/≥9§È	 /ÈÈÅ/« t…ó8E«	¨≈œA«∞ Å§€»OLÖEÃ«1D« Ω≈2…≥8F «É§ ∏µ≥ ßF«J&≥≥‹",ÄT«⁄»¿*ÖÄ…≥CŸç≈¬ …Ö9Jm8m  wù8Ä
«Ç(≥‹TÅ

¨Rœ 4lL8T…≥9Xù`wL§ ˚ π≥ÔX,ïÄ|AÌ  Ω¡2DT≈q9r«B_BA$I¿&L·@ßR«JAÅN‡I–Z«l¿ C% :ZL :AybFtgF°≥ßfu2$ 8nu2 9Fu 289Iu2<8_°
≈2<≈28≈2 :|u2(|9ù‰‡≈	¿ÄwãC»ñ@†`C»ù «Ç§~h∑≥≥·é¿àÜ π≥«îwìC»Â·¿Õ1Ä L≥LL93«
†≈	ÑFs"Ä
ë∂Å
8/«Ñ§ \-π≥$d˜ L§iú∏≥« ºwª8« Ñ¡	úãH	ú`!Yt œ 1îN9E&E≥Y‰¿$n`H≥˙\ (u1D 8D≥Y@‡ßJ≥Y∏‡ !o\≥9Ä$] ≥?lêßñåÜD>ŒÖD)¢xœÍ·>L•#t…`∫8R≥: äÉ;ñ†π≥&L  \Ç ¡&L«Ωœ24 L8Kå•û1†¡
T`5°v«Ç`…åC»˚`@±‹*¿· ¥¿Põ†PnÏ‘≥≥†;Ä#0‘AÄÄ9 &M§Q≤BÄ˝œ2tL8z‡% ·µ≥≥≥:t«ê¥§–Ò‡.\‡g, 	U 	Çπ5	B@è@`*˛¥¬∏¡îß§8 ÄKÄà∞≥&LâÄ