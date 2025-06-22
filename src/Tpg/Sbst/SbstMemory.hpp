#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <limits>
#include <vector>

namespace FreiTest
{
namespace Tpg
{
namespace Sbst
{

class SbstMemoryBlock
{
public:
	SbstMemoryBlock(uint64_t offset, uint64_t size);
	virtual ~SbstMemoryBlock(void);

	uint64_t GetOffset(void) const;
	uint64_t GetSize(void) const;
	std::vector<bool>& GetBitmap(void);
	std::vector<bool> const& GetBitmap(void) const;
	std::vector<uint8_t>& GetMemory(void);
	std::vector<uint8_t> const& GetMemory(void) const;

	uint64_t GetNextWriteLocation(void);
	void WriteUInt8(uint64_t address, uint8_t value);
	void WriteUInt16(uint64_t address, uint16_t value);
	void WriteUInt32(uint64_t address, uint32_t value);
	void WriteUInt64(uint64_t address, uint64_t value);
	void WriteUInt32Masked(uint64_t address, uint32_t value, uint32_t mask);
	void WriteUInt64Masked(uint64_t address, uint64_t value, uint64_t mask);
	uint8_t ReadUInt8(uint64_t address);
	uint16_t ReadUInt16(uint64_t address);
	uint32_t ReadUInt32(uint64_t address);
	uint64_t ReadUInt64(uint64_t address);

private:
	uint64_t blockOffset;
	uint64_t blockSize;
	uint64_t writeOffset;
	std::vector<bool> bitmap;
	std::vector<uint8_t> memory;

};

constexpr size_t INVALID_BLOCK = std::numeric_limits<size_t>::max();

class SbstMemory
{
public:
	SbstMemory(void);
	virtual ~SbstMemory(void);

	size_t AddBlock(uint64_t offset, uint64_t size);
	void ClearBlocks(void);
	size_t GetNumberOfBlocks(void);
	size_t GetNumberOfBlocks(void) const;

	size_t OpenBlock(void);
	void CloseBlock(size_t blockId, bool finalize);
	std::shared_ptr<SbstMemoryBlock> GetBlock(size_t blockId);
	std::shared_ptr<const SbstMemoryBlock> GetBlock(size_t blockId) const;

private:
	enum class BlockType { Closed, Opened, Finalized };

	mutable std::mutex lock;
	std::vector<BlockType> blockTypes;
	std::vector<std::shared_ptr<SbstMemoryBlock>> blocks;

};

};
};
};
