#include "Tpg/Sbst/SbstMemory.hpp"

#include <cmath>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Sbst
{

SbstMemoryBlock::SbstMemoryBlock(uint64_t offset, uint64_t size):
	blockOffset(offset),
	blockSize(size),
	writeOffset(offset),
	bitmap(size, false),
	memory(size, 0u)
{
}

SbstMemoryBlock::~SbstMemoryBlock(void) = default;

uint64_t SbstMemoryBlock::GetOffset(void) const
{
	return blockOffset;
}

uint64_t SbstMemoryBlock::GetSize(void) const
{
	return blockSize;
}

std::vector<bool>& SbstMemoryBlock::GetBitmap(void)
{
	return bitmap;
}

std::vector<bool> const& SbstMemoryBlock::GetBitmap(void) const
{
	return bitmap;
}

std::vector<uint8_t>& SbstMemoryBlock::GetMemory(void)
{
	return memory;
}

std::vector<uint8_t> const& SbstMemoryBlock::GetMemory(void) const
{
	return memory;
}

uint64_t SbstMemoryBlock::GetNextWriteLocation(void)
{
	return writeOffset;
}

void SbstMemoryBlock::WriteUInt8(uint64_t address, uint8_t value)
{
	ASSERT(address >= blockOffset) << "Address is smaller than minimum address";
	ASSERT(address < blockOffset + bitmap.size()) << "Address is larger than maximum address";

	writeOffset = std::max(writeOffset, address + 1u);
	bitmap[address - blockOffset] = true;
	memory[address - blockOffset] = value;
}

void SbstMemoryBlock::WriteUInt16(uint64_t address, uint16_t value)
{
	WriteUInt8(address + 0u, (uint8_t) ((value & 0x00FFu) >> 0u));
	WriteUInt8(address + 1u, (uint8_t) ((value & 0xFF00u) >> 8u));
}

void SbstMemoryBlock::WriteUInt32(uint64_t address, uint32_t value)
{
	WriteUInt8(address + 0u, (uint8_t) ((value & 0x000000FFu) >> 0u));
	WriteUInt8(address + 1u, (uint8_t) ((value & 0x0000FF00u) >> 8u));
	WriteUInt8(address + 2u, (uint8_t) ((value & 0x00FF0000u) >> 16u));
	WriteUInt8(address + 3u, (uint8_t) ((value & 0xFF000000u) >> 24u));
}

void SbstMemoryBlock::WriteUInt64(uint64_t address, uint64_t value)
{
	WriteUInt8(address + 0u, (uint8_t) ((value & 0x00000000000000FFul) >> 0u));
	WriteUInt8(address + 1u, (uint8_t) ((value & 0x000000000000FF00ul) >> 8u));
	WriteUInt8(address + 2u, (uint8_t) ((value & 0x0000000000FF0000ul) >> 16u));
	WriteUInt8(address + 3u, (uint8_t) ((value & 0x00000000FF000000ul) >> 24u));
	WriteUInt8(address + 4u, (uint8_t) ((value & 0x000000FF00000000ul) >> 32u));
	WriteUInt8(address + 5u, (uint8_t) ((value & 0x0000FF0000000000ul) >> 40u));
	WriteUInt8(address + 6u, (uint8_t) ((value & 0x00FF000000000000ul) >> 48u));
	WriteUInt8(address + 7u, (uint8_t) ((value & 0xFF00000000000000ul) >> 56u));
}

void SbstMemoryBlock::WriteUInt32Masked(uint64_t address, uint32_t value, uint32_t mask)
{
	if (mask & 0x000000FFu) WriteUInt8(address + 0u, (uint8_t) ((value & 0x000000FFu) >> 0u));
	if (mask & 0x0000FF00u) WriteUInt8(address + 1u, (uint8_t) ((value & 0x0000FF00u) >> 8u));
	if (mask & 0x00FF0000u) WriteUInt8(address + 2u, (uint8_t) ((value & 0x00FF0000u) >> 16u));
	if (mask & 0xFF000000u) WriteUInt8(address + 3u, (uint8_t) ((value & 0xFF000000u) >> 24u));
}

void SbstMemoryBlock::WriteUInt64Masked(uint64_t address, uint64_t value, uint64_t mask)
{
	if (mask & 0x00000000000000FFul) WriteUInt8(address + 0u, (uint8_t) ((value & 0x00000000000000FFul) >> 0u));
	if (mask & 0x000000000000FF00ul) WriteUInt8(address + 1u, (uint8_t) ((value & 0x000000000000FF00ul) >> 8u));
	if (mask & 0x0000000000FF0000ul) WriteUInt8(address + 2u, (uint8_t) ((value & 0x0000000000FF0000ul) >> 16u));
	if (mask & 0x00000000FF000000ul) WriteUInt8(address + 3u, (uint8_t) ((value & 0x00000000FF000000ul) >> 24u));
	if (mask & 0x000000FF00000000ul) WriteUInt8(address + 4u, (uint8_t) ((value & 0x000000FF00000000ul) >> 32u));
	if (mask & 0x0000FF0000000000ul) WriteUInt8(address + 5u, (uint8_t) ((value & 0x0000FF0000000000ul) >> 40u));
	if (mask & 0x00FF000000000000ul) WriteUInt8(address + 6u, (uint8_t) ((value & 0x00FF000000000000ul) >> 48u));
	if (mask & 0xFF00000000000000ul) WriteUInt8(address + 7u, (uint8_t) ((value & 0xFF00000000000000ul) >> 56u));
}

uint8_t SbstMemoryBlock::ReadUInt8(uint64_t address)
{
	return memory[address - blockOffset];
}

uint16_t SbstMemoryBlock::ReadUInt16(uint64_t address)
{
	return ((uint16_t) memory[address - blockOffset + 0u]) << 0u
		| ((uint16_t) memory[address - blockOffset + 1u]) << 8u;
}

uint32_t SbstMemoryBlock::ReadUInt32(uint64_t address)
{
	return ((uint32_t) memory[address - blockOffset + 0u]) << 0u
		| ((uint32_t) memory[address - blockOffset + 1u]) << 8u
		| ((uint32_t) memory[address - blockOffset + 2u]) << 16u
		| ((uint32_t) memory[address - blockOffset + 3u]) << 24u;
}

uint64_t SbstMemoryBlock::ReadUInt64(uint64_t address)
{
	return ((uint64_t) memory[address - blockOffset + 0u]) << 0u
		| ((uint64_t) memory[address - blockOffset + 1u]) << 8u
		| ((uint64_t) memory[address - blockOffset + 2u]) << 16u
		| ((uint64_t) memory[address - blockOffset + 3u]) << 24u
		| ((uint64_t) memory[address - blockOffset + 4u]) << 32u
		| ((uint64_t) memory[address - blockOffset + 5u]) << 40u
		| ((uint64_t) memory[address - blockOffset + 6u]) << 48u
		| ((uint64_t) memory[address - blockOffset + 7u]) << 56u;
}

SbstMemory::SbstMemory(void) = default;
SbstMemory::~SbstMemory(void) = default;

size_t SbstMemory::AddBlock(uint64_t offset, uint64_t size)
{
	std::lock_guard guard { lock };
	blockTypes.emplace_back(BlockType::Closed);
	blocks.emplace_back(std::make_shared<SbstMemoryBlock>(offset, size));
	return blocks.size() - 1u;
}

void SbstMemory::ClearBlocks(void)
{
	std::lock_guard guard { lock };
	blockTypes.clear();
	blocks.clear();
}

size_t SbstMemory::GetNumberOfBlocks(void)
{
	std::lock_guard guard { lock };
	return blocks.size();
}

size_t SbstMemory::GetNumberOfBlocks(void) const
{
	std::lock_guard guard { lock };
	return blocks.size();
}

size_t SbstMemory::OpenBlock(void)
{
	std::lock_guard guard { lock };
	for (size_t blockId {0u}; blockId < blocks.size(); blockId++)
	{
		if (blockTypes[blockId] == BlockType::Closed)
		{
			blockTypes[blockId] = BlockType::Opened;
			return blockId;
		}
	}

	return INVALID_BLOCK;
}

void SbstMemory::CloseBlock(size_t blockId, bool finalize)
{
	std::lock_guard guard { lock };
	blockTypes[blockId] = finalize ? BlockType::Finalized : BlockType::Closed;
}

std::shared_ptr<SbstMemoryBlock> SbstMemory::GetBlock(size_t blockId)
{
	std::lock_guard guard { lock };
	return blocks[blockId];
}

std::shared_ptr<const SbstMemoryBlock> SbstMemory::GetBlock(size_t blockId) const
{
	std::lock_guard guard { lock };
	return blocks[blockId];
}

};
};
};
