if NOTEST == nil then
	package.path = package.path .. ";../../libraries/other/lua-json/?.lua;../../libraries/other/lua-luaunit/?.lua;"
end

local LUNIT = require ("luaunit")
local JSON = require ("json")

------------------------------------------------------------------------------
RV_OPCODE_LEN = 7
RVC_OPCODE_LEN = 2
CSRS =
{
	-- Standard User R/W
	[0x001] = "fflags",
	[0x002] = "frm",
	[0x003] = "fcsr",
	[0x000] = "ustatus",
	[0x004] = "uie",
	[0x005] = "utvec",
	[0x008] = "vstart",
	[0x009] = "vxsat",
	[0x00A] = "vxrm",
	[0x00F] = "vcsr",
	[0x040] = "uscratch",
	[0x041] = "uepc",
	[0x042] = "ucause",
	[0x043] = "utval",
	[0x044] = "uip",

	-- Standard User RO
	[0xC00] = "cycle",
	[0xC01] = "time",
	[0xC02] = "instret",
	[0xC03] = "hpmcounter3",
	[0xC04] = "hpmcounter4",
	[0xC05] = "hpmcounter5",
	[0xC06] = "hpmcounter6",
	[0xC07] = "hpmcounter7",
	[0xC08] = "hpmcounter8",
	[0xC09] = "hpmcounter9",
	[0xC0A] = "hpmcounter10",
	[0xC0B] = "hpmcounter11",
	[0xC0C] = "hpmcounter12",
	[0xC0D] = "hpmcounter13",
	[0xC0E] = "hpmcounter14",
	[0xC0F] = "hpmcounter15",
	[0xC10] = "hpmcounter16",
	[0xC11] = "hpmcounter17",
	[0xC12] = "hpmcounter18",
	[0xC13] = "hpmcounter19",
	[0xC14] = "hpmcounter20",
	[0xC15] = "hpmcounter21",
	[0xC16] = "hpmcounter22",
	[0xC17] = "hpmcounter23",
	[0xC18] = "hpmcounter24",
	[0xC19] = "hpmcounter25",
	[0xC1A] = "hpmcounter26",
	[0xC1B] = "hpmcounter27",
	[0xC1C] = "hpmcounter28",
	[0xC1D] = "hpmcounter29",
	[0xC1E] = "hpmcounter30",
	[0xC1F] = "hpmcounter31",
	[0xC20] = "vl",
	[0xC21] = "vtype",
	[0xC22] = "vlenb",

	-- Standard Supervisor R/W
	[0x100] = "sstatus",
	[0x102] = "sedeleg",
	[0x103] = "sideleg",
	[0x104] = "sie",
	[0x105] = "stvec",
	[0x106] = "scounteren",
	[0x140] = "sscratch",
	[0x141] = "sepc",
	[0x142] = "scause",
	[0x143] = "stval",
	[0x144] = "sip",
	[0x180] = "satp",

	-- Standard Hypervisor R/w
	[0x200] = "vsstatus",
	[0x204] = "vsie",
	[0x205] = "vstvec",
	[0x240] = "vsscratch",
	[0x241] = "vsepc",
	[0x242] = "vscause",
	[0x243] = "vstval",
	[0x244] = "vsip",
	[0x280] = "vsatp",
	[0x600] = "hstatus",
	[0x602] = "hedeleg",
	[0x603] = "hideleg",
	[0x604] = "hie",
	[0x605] = "htimedelta",
	[0x606] = "hcounteren",
	[0x607] = "hgeie",
	[0x643] = "htval",
	[0x644] = "hip",
	[0x645] = "hvip",
	[0x64A] = "htinst",
	[0x680] = "hgatp",
	[0xE12] = "hgeip",

	-- Tentative CSR assignment for CLIC
	[0x007] = "utvt",
	[0x045] = "unxti",
	[0x046] = "uintstatus",
	[0x048] = "uscratchcsw",
	[0x049] = "uscratchcswl",
	[0x107] = "stvt",
	[0x145] = "snxti",
	[0x146] = "sintstatus",
	[0x148] = "sscratchcsw",
	[0x149] = "sscratchcswl",
	[0x307] = "mtvt",
	[0x345] = "mnxti",
	[0x346] = "mintstatus",
	[0x348] = "mscratchcsw",
	[0x349] = "mscratchcswl",

	-- Standard Machine R/W
	[0x300] = "mstatus",
	[0x301] = "misa",
	[0x302] = "medeleg",
	[0x303] = "mideleg",
	[0x304] = "mie",
	[0x305] = "mtvec",
	[0x306] = "mcounteren",
	[0x320] = "mcountinhibit",
	[0x340] = "mscratch",
	[0x341] = "mepc",
	[0x342] = "mcause",
	[0x343] = "mtval",
	[0x344] = "mip",
	[0x34a] = "mtinst",
	[0x34b] = "mtval2",
	[0x3a0] = "pmpcfg0",
	[0x3a1] = "pmpcfg1",
	[0x3a2] = "pmpcfg2",
	[0x3a3] = "pmpcfg3",
	[0x3b0] = "pmpaddr0",
	[0x3b1] = "pmpaddr1",
	[0x3b2] = "pmpaddr2",
	[0x3b3] = "pmpaddr3",
	[0x3b4] = "pmpaddr4",
	[0x3b5] = "pmpaddr5",
	[0x3b6] = "pmpaddr6",
	[0x3b7] = "pmpaddr7",
	[0x3b8] = "pmpaddr8",
	[0x3b9] = "pmpaddr9",
	[0x3ba] = "pmpaddr10",
	[0x3bb] = "pmpaddr11",
	[0x3bc] = "pmpaddr12",
	[0x3bd] = "pmpaddr13",
	[0x3be] = "pmpaddr14",
	[0x3bf] = "pmpaddr15",
	[0x7a0] = "tselect",
	[0x7a1] = "tdata1",
	[0x7a2] = "tdata2",
	[0x7a3] = "tdata3",
	[0x7a4] = "tinfo",
	[0x7a5] = "tcontrol",
	[0x7a8] = "mcontext",
	[0x7aa] = "scontext",
	[0x7b0] = "dcsr",
	[0x7b1] = "dpc",
	[0x7b2] = "dscratch0",
	[0x7b3] = "dscratch1",
	[0xB00] = "mcycle",
	[0xB02] = "minstret",
	[0xB03] = "mhpmcounter3",
	[0xB04] = "mhpmcounter4",
	[0xB05] = "mhpmcounter5",
	[0xB06] = "mhpmcounter6",
	[0xB07] = "mhpmcounter7",
	[0xB08] = "mhpmcounter8",
	[0xB09] = "mhpmcounter9",
	[0xB0A] = "mhpmcounter10",
	[0xB0B] = "mhpmcounter11",
	[0xB0C] = "mhpmcounter12",
	[0xB0D] = "mhpmcounter13",
	[0xB0E] = "mhpmcounter14",
	[0xB0F] = "mhpmcounter15",
	[0xB10] = "mhpmcounter16",
	[0xB11] = "mhpmcounter17",
	[0xB12] = "mhpmcounter18",
	[0xB13] = "mhpmcounter19",
	[0xB14] = "mhpmcounter20",
	[0xB15] = "mhpmcounter21",
	[0xB16] = "mhpmcounter22",
	[0xB17] = "mhpmcounter23",
	[0xB18] = "mhpmcounter24",
	[0xB19] = "mhpmcounter25",
	[0xB1A] = "mhpmcounter26",
	[0xB1B] = "mhpmcounter27",
	[0xB1C] = "mhpmcounter28",
	[0xB1D] = "mhpmcounter29",
	[0xB1E] = "mhpmcounter30",
	[0xB1F] = "mhpmcounter31",
	[0x323] = "mhpmevent3",
	[0x324] = "mhpmevent4",
	[0x325] = "mhpmevent5",
	[0x326] = "mhpmevent6",
	[0x327] = "mhpmevent7",
	[0x328] = "mhpmevent8",
	[0x329] = "mhpmevent9",
	[0x32A] = "mhpmevent10",
	[0x32B] = "mhpmevent11",
	[0x32C] = "mhpmevent12",
	[0x32D] = "mhpmevent13",
	[0x32E] = "mhpmevent14",
	[0x32F] = "mhpmevent15",
	[0x330] = "mhpmevent16",
	[0x331] = "mhpmevent17",
	[0x332] = "mhpmevent18",
	[0x333] = "mhpmevent19",
	[0x334] = "mhpmevent20",
	[0x335] = "mhpmevent21",
	[0x336] = "mhpmevent22",
	[0x337] = "mhpmevent23",
	[0x338] = "mhpmevent24",
	[0x339] = "mhpmevent25",
	[0x33A] = "mhpmevent26",
	[0x33B] = "mhpmevent27",
	[0x33C] = "mhpmevent28",
	[0x33D] = "mhpmevent29",
	[0x33E] = "mhpmevent30",
	[0x33F] = "mhpmevent31",

	-- Standard Machine RO
	[0xF11] = "mvendorid",
	[0xF11] = "mvendorid",
	[0xF12] = "marchid",
	[0xF13] = "mimpid",
	[0xF14] = "mhartid",
	[0xF15] = "mentropy", -- crypto ext
	[0x7A9] = "mnoise",

	-- Standard Hypervisor R/w
	[0x615] = "htimedeltah",

	-- Standard User RO
	[0xC80] = "cycleh",
	[0xC81] = "timeh",
	[0xC82] = "instreth",
	[0xC83] = "hpmcounter3h",
	[0xC84] = "hpmcounter4h",
	[0xC85] = "hpmcounter5h",
	[0xC86] = "hpmcounter6h",
	[0xC87] = "hpmcounter7h",
	[0xC88] = "hpmcounter8h",
	[0xC89] = "hpmcounter9h",
	[0xC8A] = "hpmcounter10h",
	[0xC8B] = "hpmcounter11h",
	[0xC8C] = "hpmcounter12h",
	[0xC8D] = "hpmcounter13h",
	[0xC8E] = "hpmcounter14h",
	[0xC8F] = "hpmcounter15h",
	[0xC90] = "hpmcounter16h",
	[0xC91] = "hpmcounter17h",
	[0xC92] = "hpmcounter18h",
	[0xC93] = "hpmcounter19h",
	[0xC94] = "hpmcounter20h",
	[0xC95] = "hpmcounter21h",
	[0xC96] = "hpmcounter22h",
	[0xC97] = "hpmcounter23h",
	[0xC98] = "hpmcounter24h",
	[0xC99] = "hpmcounter25h",
	[0xC9A] = "hpmcounter26h",
	[0xC9B] = "hpmcounter27h",
	[0xC9C] = "hpmcounter28h",
	[0xC9D] = "hpmcounter29h",
	[0xC9E] = "hpmcounter30h",
	[0xC9F] = "hpmcounter31h",

	-- Standard Machine RW
	[0x310] = "mstatush",
	[0xB80] = "mcycleh",
	[0xB82] = "minstreth",
	[0xB83] = "mhpmcounter3h",
	[0xB84] = "mhpmcounter4h",
	[0xB85] = "mhpmcounter5h",
	[0xB86] = "mhpmcounter6h",
	[0xB87] = "mhpmcounter7h",
	[0xB88] = "mhpmcounter8h",
	[0xB89] = "mhpmcounter9h",
	[0xB8A] = "mhpmcounter10h",
	[0xB8B] = "mhpmcounter11h",
	[0xB8C] = "mhpmcounter12h",
	[0xB8D] = "mhpmcounter13h",
	[0xB8E] = "mhpmcounter14h",
	[0xB8F] = "mhpmcounter15h",
	[0xB90] = "mhpmcounter16h",
	[0xB91] = "mhpmcounter17h",
	[0xB92] = "mhpmcounter18h",
	[0xB93] = "mhpmcounter19h",
	[0xB94] = "mhpmcounter20h",
	[0xB95] = "mhpmcounter21h",
	[0xB96] = "mhpmcounter22h",
	[0xB97] = "mhpmcounter23h",
	[0xB98] = "mhpmcounter24h",
	[0xB99] = "mhpmcounter25h",
	[0xB9A] = "mhpmcounter26h",
	[0xB9B] = "mhpmcounter27h",
	[0xB9C] = "mhpmcounter28h",
	[0xB9D] = "mhpmcounter29h",
	[0xB9E] = "mhpmcounter30h",
	[0xB9F] = "mhpmcounter31h",
}
FENCE_OPS = { [0x1] = "w", [0x2] = "r", [0x4] = "o", [0x8] = "i" , [0xF] = "" }
------------------------------------------------------------------------------
BitSegment = {}
function BitSegment:new(lo, hi, imm_top, imm_bot, len)
	local instance = {}
	instance.lo = lo
	instance.hi = hi
	instance.imm_top = imm_top
	instance.imm_bot = imm_bot
	instance.len = len
	return instance
end

SubOpcode = {}
function SubOpcode:new(lo, hi, bits)
	local instance = {}
	instance.lo = lo
	instance.hi = hi
	instance.bits = bits
	return instance
end

Instruction = {}
function Instruction:new(name, syntax)
	local instance = {}
	instance.name = name
	instance.syntax = syntax
	instance.segments = {}
	instance.operands = {}
	instance.sub_opcodes = {}
	instance.opcode = nil
	return instance
end
------------------------------------------------------------------------------

function bin(value)
	-- Converts a number to a 32-bit binary string based on its endianess
	setmetatable(value, {__index = {endianess = "big"}})
	local number, endianess =
		value[1] or value.number,
		value[2] or value.endianess

	local str_binval = ""

	local hex2bits =  {
	["0"] ="0000",["1"] ="0001",["2"] ="0010",["3"] ="0011",
	["4"] ="0100",["5"] ="0101",["6"] ="0110",["7"] ="0111",
	["8"] ="1000",["9"] ="1001",["a"] ="1010",["b"] ="1011",
	["c"] ="1100",["d"] ="1101",["e"] ="1110",["f"] ="1111"}

	local str_hexval = string.format("%08x", number)
	if endianess == "big" then
		for byte = 1, 8 do
			str_binval = str_binval .. hex2bits[string.sub(str_hexval, byte, byte)]
		end
	elseif endianess == "little" then
		for byte = 8, 1, -2 do
			str_binval = str_binval ..
			hex2bits[string.sub(str_hexval, byte-1, byte-1)] ..
			hex2bits[string.sub(str_hexval, byte, byte)]
		end
	end
	return str_binval
end

function test_bin()
    LUNIT.assertEquals(bin{0x0000ffff}, "00000000000000001111111111111111")
	LUNIT.assertEquals(bin{0x0000ffff, endianess = "little"}, "11111111111111110000000000000000")
	LUNIT.assertEquals(bin{0x0000ffff, endianess = "big"}, "00000000000000001111111111111111")
	LUNIT.assertEquals(bin{0x0000ffff, endianes = "Little"}, "00000000000000001111111111111111")
	LUNIT.assertEquals(bin{0xa0b0c0d0, endianess = "little"}, bin{0xd0c0b0a0, endianess = "big"})
	LUNIT.assertEquals(bin{0xa0b0c0d0, endianess = "big"}, bin{0xd0c0b0a0, endianess = "little"})
end

------------------------------------------------------------------------------


function parse_json_sources(sources)

	local rv_instructions = {}

	local rv_sources = setmetatable({}, {
		__add =
		function(rv_sources, other)
			for i = 1, #other do
				rv_sources[#rv_sources+1] = other[i]
			end
			return rv_sources
		end
	})

	for _, source in pairs(sources) do
		local file = io.open(source,"rb")
		if not file then return nil end
		local content = file:read("all")
		rv_sources = rv_sources + JSON.decode(content)
	end

	for _, instruction in pairs(rv_sources) do

		local rv_instruction = Instruction:new(instruction["name"], instruction["syntax"])
		rv_instruction.opcode = instruction["encoding"][#instruction["encoding"]]["pattern"]

		local bit = 1 -- lua uses 1-base indexing
		for idx, enc_part in pairs(instruction["encoding"]) do

			-- last part is always the opcode, skip it
			if idx == #instruction["encoding"] then
				break
			end

			-- compose instruction segments
			if enc_part["pattern"] ~= nil then
				table.insert(rv_instruction.segments, enc_part["pattern"])
			else
				table.insert(rv_instruction.segments, string.rep("?", enc_part["bits"]))
			end

			-- gather all operands
			if enc_part["type"] == "operand" then
				if rv_instruction.operands[enc_part["name"]] == nil then
					rv_instruction.operands[enc_part["name"]] = {BitSegment:new(bit, bit + enc_part["bits"] - 1, enc_part['bits_top'], enc_part['bits_bottom'], enc_part["bits"])}
				else
					table.insert(rv_instruction.operands[enc_part["name"]], BitSegment:new(bit, bit + enc_part["bits"] - 1, enc_part['bits_top'], enc_part['bits_bottom'], enc_part["bits"]))
				end

			-- gather all sub opcodes
			else
				table.insert(rv_instruction.sub_opcodes, SubOpcode:new(bit, bit + enc_part["bits"] - 1, enc_part["pattern"]))
			end

			bit = bit + enc_part["bits"]
		end

		table.insert(rv_instructions, rv_instruction)

	end

	return rv_instructions
end

function decode(instructions, value, endianess)

	local function _trim(str)
		-- trims left and right whitespace from string
		local a = string.match(str, '^%s*()')
		local b = string.match(str, '()%s*$', a)
		return string.sub(str,a,b-1)
	end

	local function _find_opcode_match(value)
		-- Checks the foreign value against all sources'
		-- opcodes and returns those that match
		local matches = {}
		for _, instruction in pairs(instructions) do
			if instruction.opcode == value then
				matches[#matches+1] = instruction
			end
		end
		return matches
	end

	local function _is_total_match(sub_opcodes, value)
		-- Checks all func bits of a match and compares with the foreign value
		if #sub_opcodes == 0 then
			return true
		end

		for _, sub_opcode in pairs(sub_opcodes) do
			if string.sub(value, sub_opcode.lo, sub_opcode.hi) ~= sub_opcode.bits then
				return false
			end
		end

		return true

	end

	local function _imm_reorder(immediate, parts)
		-- based on the top bits of each immediate part, breaks down and reconstructs
		-- the correct immediate. Keeps track of the lowest immediate bit index. In the case
		-- that this is != 0 then it appends the required ammount of zeroes. e.g., 'jal' imm stops at [:1]
		local retval = ""
		local immediate_segments = {}
		local immediate_top_bits = {}
		local lowest_bit = math.maxinteger
		local bit = 1
		for _, segment in pairs(parts) do
			local bits = string.sub(immediate, bit, bit + segment.len-1)
			bit = bit + segment.len

			if segment.imm_bot <= lowest_bit then lowest_bit = segment.imm_bot end

			immediate_segments[segment.imm_top] = bits
			table.insert(immediate_top_bits, segment.imm_top)
		end

		-- ascending sort, hence, each part must be prepended
		table.sort(immediate_top_bits)

		for _, top_bit in pairs(immediate_top_bits) do

			retval = immediate_segments[top_bit] .. retval
		end

		return  retval .. string.rep("0", lowest_bit)
	end

	local function _compose_instruction(instruction, value)

		-- cleanup syntax
		local operands = _trim(string.gsub(instruction.syntax, instruction.name, "", 1))
		-- remove 'rm' from 'F'/'D' extension instructions
		operands = string.gsub(operands, "rm,", "", 1)
		-- remove '(fm, rs1, rd)' from FENCEs
		operands, is_fence = string.gsub(operands, ",?%(fm[^)]+%)", "", 1)

		for operand, attributes in pairs(instruction.operands) do
			-- Immediates & Shift amounts
			if (string.find(operand, "imm1") ~= nil) or
			   (string.find(operand, "imm2") ~= nil) or
			   (string.find(operand, "imm") ~= nil) or
			   (string.find(operand, "shamt") ~= nil) then

				local reconstructed_immediate = ""
				local needs_reordering = #instruction.operands[operand] > 1

				for _, bit_segment in pairs(instruction.operands[operand]) do
					reconstructed_immediate =
					reconstructed_immediate
					 .. string.sub(value, bit_segment.lo, bit_segment.hi)
				end

				-- e.g, jal/branch instructions have immediate parts scattered
				if needs_reordering then
					--print("Immediate of instruction " .. instruction.name .. " needs reordering")
					reconstructed_immediate = _imm_reorder(reconstructed_immediate, instruction.operands[operand])
				end

				operands = string.gsub(
					operands,
					operand,
					"0x" .. string.format("%x",tonumber(reconstructed_immediate,2)))

			-- Registers
			elseif (string.find(operand, "rs1") ~= nil)
			 or (string.find(operand, "rs2") ~= nil)
			 or (string.find(operand, "rs3") ~= nil)
			 or (string.find(operand, "L") ~= nil)
			 or (string.find(operand, "rd") ~= nil) then

				local reg_segment = instruction.operands[operand][1]
				local reconstructed_value = tonumber(string.sub(value, reg_segment.lo, reg_segment.hi),2)

				for _operand in string.gmatch(operands, "[^,]+") do
					if string.find(_operand, operand) then
						if string.find(_operand, "f rd") ~= nil or string.find(_operand, "f rs") ~= nil then -- Floating point register
							operands = string.gsub(operands, _operand, "f" .. reconstructed_value)
						elseif string.find(operands, "+") ~= nil then -- 'C' register
							operands = string.gsub(operands, "8[^,]+", "x" .. (reconstructed_value + 8))
						else
							operands = string.gsub(operands, operand, "x" .. reconstructed_value)
						end
					end
				end
			-- CSRs
			elseif operand == "csr" then
				local csr_segment = instruction.operands[operand][1]
				local reconstructed_value = tonumber(
					string.sub(
						value,
						csr_segment.lo,
						csr_segment.hi),
					2)
				if CSRS[reconstructed_value] ~= nil then
					operands = string.gsub(operands, operand, CSRS[reconstructed_value])
				else -- Unknown CSR, use as hex
					operands = string.gsub(operands, operand, "0x" .. string.format("%x", reconstructed_value))
				end
			-- FENCEs    		     [ignore fm operand]
			elseif is_fence == 1 and operand ~= "fm" then

				local succ_pred_segment = instruction.operands[operand][1]
				local reconstructed_value = tonumber(
					string.sub(
						value,
						succ_pred_segment.lo,
						succ_pred_segment.hi),
					2)
				if FENCE_OPS[reconstructed_value] ~= nil then
					operands = string.gsub(operands, operand, FENCE_OPS[reconstructed_value])
				else
					error("INVALID FENCE OPERAND")
				end
			end

		end

		while true do
			operands = _trim(operands)
			if string.match(operands,"(),$") then
				operands = string.sub(operands, 1, #operands - 1)
			else
				break
			end
		end

		local instruction_name = string.gsub(instruction.name, "_",".")
		if operands == "" then
			return instruction_name
		else
			return instruction_name .. " " .. operands
		end
	end

	local retval = ""
	local binary_foreign_value = bin{value, endianess = string.lower(endianess)}
	-- Regular 32bit RV instruction
	local foreign_opcode = string.sub(binary_foreign_value,
		#binary_foreign_value - RV_OPCODE_LEN + 1,
		#binary_foreign_value)
	-- Potential double 'C' instructions
	local foreign_c_opcodes = {
		string.sub(binary_foreign_value,
			#binary_foreign_value - RVC_OPCODE_LEN + 1,
			#binary_foreign_value)}
	local foreign_c_instructions = {
		string.sub(binary_foreign_value, 17, 32)}

	-- there is a second 'C' instruction
	if (value & 0xffff0000) ~= 0 then
		table.insert(foreign_c_opcodes,
			string.sub(
				binary_foreign_value,
				#binary_foreign_value//2 - RVC_OPCODE_LEN + 1,
				#binary_foreign_value//2
		))
		table.insert(foreign_c_instructions,
			string.sub(binary_foreign_value, 1, 16))
	end

	-- Finding Regular matches
	local matches = _find_opcode_match(foreign_opcode)

	if #matches ~= 0 then
		for _, instruction in pairs(matches) do

			if _is_total_match(instruction.sub_opcodes, binary_foreign_value) then
				retval = _compose_instruction(instruction, binary_foreign_value)
			end

		end
	else
		for idx, foreign_c_opcode in pairs(foreign_c_opcodes) do
			matches = _find_opcode_match(foreign_c_opcode)
			for _, instruction in pairs(matches) do
				if _is_total_match(instruction.sub_opcodes, foreign_c_instructions[idx]) then
					if retval ~= "" then retval = retval .. "\n" end
					retval = retval .. _compose_instruction(instruction, foreign_c_instructions[idx])
				end
			end
		end
	end

	if retval == "" then
		retval = string.format("%08x", value) .. " /* unknown instruction */"
	end

	return retval

end

function test_decode()

	local sources = {
		"../code_generation/generate_vcm/json-dbt-rise/rv32i.json",
		"../code_generation/generate_vcm/json-dbt-rise/rvzicsr.json",
		"../code_generation/generate_vcm/json-dbt-rise/rv32f.json",
		"../code_generation/generate_vcm/json-dbt-rise/rv32c.json"
	}

	local rv_isa = parse_json_sources(sources)

	LUNIT.assertEquals(decode(rv_isa, 0x00ff2337, "big"), "lui x6, 0xff2")
	LUNIT.assertEquals(decode(rv_isa, 0x0210000f, "big"), "fence r, w")
	LUNIT.assertEquals(decode(rv_isa, 0x30539373, "big"), "csrrw x6, mtvec, x7")
	LUNIT.assertEquals(decode(rv_isa, 0xd00170d3, "big"), "fcvt.s.w f1, x2")
	LUNIT.assertEquals(decode(rv_isa, 0x0ff0000f, "big"), "fence")
	LUNIT.assertEquals(decode(rv_isa, 0x00100073, "big"), "ebreak")
	LUNIT.assertEquals(decode(rv_isa, 0x00c0006f, "big"), "jal x0, 0xc")
	LUNIT.assertEquals(decode(rv_isa, 0x0242aa23, "big"), "sw x4, 0x34(x5)")
	LUNIT.assertEquals(decode(rv_isa, 0x84958495, "big"), "c.srai x9, 0x5\nc.srai x9, 0x5")
	LUNIT.assertEquals(decode(rv_isa,     0x8495, "big"), "c.srai x9, 0x5")
	LUNIT.assertEquals(decode(rv_isa, 0xfe0e0ce3, "big"), "bne x28, x0, 0x1ff8")

end
------------------------------------------------------------------------------
if NOTEST == nil then

	local sources = {
		"../code_generation/generate_vcm/json-dbt-rise/rv32i.json",
		"../code_generation/generate_vcm/json-dbt-rise/rvzicsr.json",
		"../code_generation/generate_vcm/json-dbt-rise/rv32f.json"
	}
	local rv_isa = parse_json_sources(sources)

	os.exit(LUNIT.LuaUnit.run())
end
