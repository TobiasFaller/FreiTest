#!/bin/env python3

import argparse
import subprocess
import re
import json
import copy
from typing import *
from lark import Lark, Transformer, Token, v_args

GRAMMAR = r"""
	///////////
	// START //
	///////////
	instructions: "{" instruction+ "}"

	///////////
	// BODY  //
	///////////
	instruction: INSTR_NAME ("[[" _SINK_ATTRIBUTE "]]")* "{" encoding assembly? behavior? "}"
	encoding: "encoding:" ((binary | register) "::"? )+ ";"
	assembly: "assembly:" /[^;]+/ ";"
	behavior: "behavior:" expression

	binary: INT "'"? "b" BINARY
	register: OPERAND "[" INT ":" INT "]"
	expression: ( "(" expression? ")" )
		| ( "{" expression? "}" )
		| (expression expression+)
		| _SINK_EXPRESSION

	///////////////
	// TERMINALS //
	///////////////
	INT: /\d+/
	BINARY: /[01]+/
	INSTR_NAME: /[\w._]+/
	OPERAND: /[a-z]+\d?/
	_SINK_ATTRIBUTE: /[^\]]+/
	_SINK_EXPRESSION: /[^(){}]+/

	///////////////////
	// LARK-SPECIFIC //
	///////////////////
	%import common.WS
	%ignore WS
"""

""" Unit tests are based on the DBT RISE and include parts licensed under the BSD-3 license.
	See dsl2-minres-riscv/LICENSE.txt for more information.
"""
class IntermediateTransformer(Transformer):

	@v_args(inline=True)
	def behavior(self, *args):
		"""
		>>> data = '''behavior: {
		... if(PRIV<4)
		... raise(0, 2);
		... else {
		... PC=DPC;
		... PRIV &= 0x3;
		... }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = ''' behavior: {
		... unsigned<XLEN> xrd = CSR[csr];
		... if (zimm != 0) CSR[csr] = xrd & ~((unsigned<XLEN>)zimm);
		... if ((rd % RFS) != 0) X[rd % RFS] = xrd;
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		... unsigned<XLEN> xrs1 = X[rs1 % RFS];
		... if ((rd % RFS) != 0) {
		... unsigned<XLEN> xrd = CSR[csr];
		... CSR[csr] = xrs1;
		... X[rd % RFS] = xrd;
		... } else {
		... CSR[csr] = xrs1;
		... }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		... unsigned<XLEN> offs = X[rs1 % RFS];
		... unsigned<32> res1 = RES[offs];
		... if (res1 != 0)
		... MEM[offs] = X[rs2 % RFS];
		... if ((rd % RFS) != 0)
		... X[rd % RFS] = res1 ? 0 : 1;
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		... unsigned<XLEN> offs = X[rs1 % RFS] + (signed<12>)imm;
		... unsigned<64> res = (unsigned<64>)MEM[offs];
		... if (FLEN == 64)
		... F[rd] = res;
		... else { // NaN boxing
		... F[rd] = (-1<<64) + res;
		... }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: leave(3);'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: if ((rd % RFS) != 0) X[rd % RFS] = X[rs1 % RFS] ^ X[rs2 % RFS];'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		... if ((signed<XLEN>)X[rs1 % RFS] < (signed<XLEN>)X[rs2 % RFS]) {
		... if (imm % INSTR_ALIGNMENT) {
		... raise(0, 0);
		... } else {
		... PC = PC + (signed)imm;
		... }
		... }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		...    unsigned<XLEN> offs = X[rs1 + 8]+uimm;
		...    unsigned<32> res = (unsigned<32>)MEM[offs];
		...    if (FLEN == 32)
		...        F[rd + 8] = res;
		...    else {
		...        F[rd + 8] = ((signed<FLEN>)-1 << 32) | res;
		...    }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		...    unsigned<XLEN> offs = X[2] + uimm;
		...    signed<64> res = MEM[offs];
		...    if (rd % RFS)
		...        X[rd % RFS] = res;
		...    else
		...        raise(0,2);
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
        >>> data = '''behavior: {
        ...      if(rd % RFS) {
        ...         signed<32> res = MEM[X[2] + uimm];
        ...           X[rd % RFS] = (signed<XLEN>)res;
        ...      } else
        ...          raise(0,2);
        ...  }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''behavior: {
		...    if(rd % RFS) {
		...        signed<32> res = MEM[X[2] + uimm];
		...        X[rd % RFS] = (signed<XLEN>)res;
		...    } else
		...        raise(0,2);
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='behavior', transformer=IntermediateTransformer()).parse(data)
		"""
		pass

	@v_args(inline=True)
	def assembly(self, asm_syntax):
		"""
		>>> data = '''assembly:"{name(rd)}, {csr}, {zimm:#0x}";'''
		>>> Lark(GRAMMAR, parser='lalr', start='assembly', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''assembly:"{name(rd)}, {name(rs1)}, {name(rs2)}";'''
		>>> Lark(GRAMMAR, parser='lalr', start='assembly', transformer=IntermediateTransformer()).parse(data)
		>>> data = '''assembly:"{pred}, {succ} ({fm} , {name(rs1)}, {name(rd)})";'''
		>>> Lark(GRAMMAR, parser='lalr', start='assembly', transformer=IntermediateTransformer()).parse(data)
		"""
		refined_statement = re.sub(
			r"imm:.*",
			r"imm",
			re.sub(
				"name\\(([^)]+)\\)",
				r"\1",
				str(asm_syntax).strip('"').replace("{","").replace("}", "")
			)
		)

		return refined_statement

	@v_args(inline=True)
	def instruction(self, instruction_name: str, encoding: list, assembly: str = "", behavior: str = None) -> dict:
		"""
		>>> data = ''' JALR [[no_cont]] {
		... encoding: imm[11:0] :: rs1[4:0] :: 3'b000 :: rd[4:0] :: 7'b1100111;
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='instruction', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'name': 'jalr',
		'operands':
		[{'name': 'imm', 'bits': 12, 'bits_top': 11, 'bits_bottom': 0},
		 {'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}],
		'encoding':
		[{'type': 'operand', 'name': 'imm', 'bits': 12, 'bits_top': 11, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 3, 'pattern': '000'},
		 {'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 7, 'pattern': '1100111'}]}
		>>> data = '''CSRRW {
		... encoding: csr[11:0] :: rs1[4:0] :: 3'b001 :: rd[4:0] :: 7'b1110011;
		... assembly:"{name(rd)}, {csr}, {name(rs1)}";
		... behavior: {
		... unsigned<XLEN> xrs1 = X[rs1 % RFS];
		... if ((rd % RFS) != 0) {
		... unsigned<XLEN> xrd = CSR[csr];
		... CSR[csr] = xrs1;
		... // make sure Xrd is updated once CSR write succeeds
		... X[rd % RFS] = xrd;
		... } else {
		... CSR[csr] = xrs1;
		... }
		... }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='instruction', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'name': 'csrrw',
		'operands':
		[{'name': 'csr', 'bits': 12, 'bits_top': 11, 'bits_bottom': 0},
		 {'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}],
		'encoding':
		[{'type': 'operand', 'name': 'csr', 'bits': 12, 'bits_top': 11, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 3, 'pattern': '001'},
		 {'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 7, 'pattern': '1110011'}]}
		>>> data = '''FENCE {
		... encoding: fm[3:0] :: pred[3:0] :: succ[3:0] :: rs1[4:0] :: 3'b000 :: rd[4:0] :: 7'b0001111;
		... assembly:"{pred}, {succ} ({fm} , {name(rs1)}, {name(rd)})";
		... behavior: FENCE[fence] = pred << 4 | succ;
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='instruction', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'name': 'fence',
		'operands':
		[{'name': 'fm', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'name': 'pred', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'name': 'succ', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}],
		'encoding':
		[{'type': 'operand', 'name': 'fm', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'pred', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'succ', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 3, 'pattern': '000'},
		 {'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 7, 'pattern': '0001111'}]}
		>>> data = '''ECALL [[no_cont]] {
		...    encoding: 0b000000000000 :: 5'b00000 :: 3'b000 :: 5'b00000 :: 7'b1110011;
		...    behavior: raise(0, 11);
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='instruction', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'name': 'ecall',
		'operands': [],
		'encoding':
		[{'type': 'pattern', 'bits': 12, 'pattern': '000000000000'},
		 {'type': 'pattern', 'bits': 5, 'pattern': '00000'},
		 {'type': 'pattern', 'bits': 3, 'pattern': '000'},
		 {'type': 'pattern', 'bits': 5, 'pattern': '00000'},
		 {'type': 'pattern', 'bits': 7, 'pattern': '1110011'}]}
		>>> data = '''CLWSP {
		...     encoding: 3'b010 :: uimm[5:5] :: rd[4:0] :: uimm[4:2] :: uimm[7:6] :: 2'b10;
		...     assembly: "{name(rd)}, sp, {uimm:#05x}";
		...     behavior: {
		...         if(rd % RFS) {
		...             signed<32> res = MEM[X[2] + uimm];
		...             X[rd % RFS] = (signed<XLEN>)res;
		...         } else
		...             raise(0,2);
		...     }
		... }'''
		>>> Lark(GRAMMAR, parser='lalr', start='instruction', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'name': 'clwsp',
		'operands':
		[{'name': 'uimm', 'bits': 6, 'bits_top': 7, 'bits_bottom': 2},
		{'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}],
		'encoding':
		[{'type': 'pattern', 'bits': 3, 'pattern': '010'},
		{'type': 'operand', 'name': 'uimm', 'bits': 1, 'bits_top': 5, 'bits_bottom': 5},
		{'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		{'type': 'operand', 'name': 'uimm', 'bits': 3, 'bits_top': 4, 'bits_bottom': 2},
		{'type': 'operand', 'name': 'uimm', 'bits': 2, 'bits_top': 7, 'bits_bottom': 6},
		{'type': 'pattern', 'bits': 2, 'pattern': '10'}]}
		"""

		operands = list(filter(
			lambda x : "operand" in x.values(),
			list(encoding.values())[0]
		))

		operands = copy.deepcopy(operands)
		for operand in operands:
			operand.pop('type')

		operands_compacted = []
		for operand_source in operands:

			# Check if operand was already mentioned in this instruction
			operand = next((operand for operand in operands_compacted if operand['name'] == operand_source['name']), None)
			if operand is None:
				operand = copy.deepcopy(operand_source)
				operands_compacted.append(operand)

			# Increase upper and lower limit of the operand with new part
			operand['bits_top'] = max(operand['bits_top'], operand_source['bits_top'])
			operand['bits_bottom'] = min(operand['bits_bottom'], operand_source['bits_bottom'])
			operand['bits'] = operand['bits_top'] - operand['bits_bottom'] + 1


		return {
			"name": instruction_name.replace('.', '_').lower(),
			"operands": operands_compacted,
			"encoding": list(encoding.values())[0],
			"syntax": f"{instruction_name.lower()}{' '+assembly if assembly else ''}"}

	@v_args(inline=True)
	def encoding(self, *args) -> dict:
		"""
		>>> data = '''encoding: imm[20:20] :: imm[10:1] :: imm[11:11] :: imm[19:12] :: rd[4:0] :: 7'b1101111;'''
		>>> Lark(GRAMMAR, parser='lalr', start='encoding', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'encoding':
		[{'type': 'operand', 'name': 'imm', 'bits': 1, 'bits_top': 20, 'bits_bottom': 20},
		{'type':  'operand', 'name': 'imm', 'bits': 10, 'bits_top': 10, 'bits_bottom': 1},
		{'type':  'operand', 'name': 'imm', 'bits': 1, 'bits_top': 11, 'bits_bottom': 11},
		{'type':  'operand', 'name': 'imm', 'bits': 8, 'bits_top': 19, 'bits_bottom': 12},
		{'type':  'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		{'type':  'pattern', 'bits': 7, 'pattern': '1101111'}]}
		>>> data = '''encoding: imm[11:0] :: rs1[4:0] :: 3'b000 :: rd[4:0] :: 7'b1100111;'''
		>>> Lark(GRAMMAR, parser='lalr', start='encoding', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'encoding':
		[{'type': 'operand', 'name': 'imm', 'bits': 12, 'bits_top': 11, 'bits_bottom': 0},
		{'type':  'operand', 'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		{'type':  'pattern', 'bits': 3, 'pattern': '000'},
		{'type':  'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		{'type':  'pattern', 'bits': 7, 'pattern': '1100111'}]}
		>>> data = '''encoding: fm[3:0] :: pred[3:0] :: succ[3:0] :: rs1[4:0] :: 3'b000 :: rd[4:0] :: 7'b0001111;'''
		>>> Lark(GRAMMAR, parser='lalr', start='encoding', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'encoding':
		[{'type': 'operand', 'name': 'fm', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'pred', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'succ', 'bits': 4, 'bits_top': 3, 'bits_bottom': 0},
		 {'type': 'operand', 'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 3, 'pattern': '000'},
		 {'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		 {'type': 'pattern', 'bits': 7, 'pattern': '0001111'}]}
		>>> data = '''encoding: 0b000000000000 :: 5'b00000 :: 3'b000 :: 5'b00000 :: 7'b1110011;'''
		>>> Lark(GRAMMAR, parser='lalr', start='encoding', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		{'encoding':
		[{'type': 'pattern', 'bits': 12, 'pattern': '000000000000'},
		 {'type': 'pattern', 'bits': 5, 'pattern': '00000'},
		 {'type': 'pattern', 'bits': 3, 'pattern': '000'},
		 {'type': 'pattern', 'bits': 5, 'pattern': '00000'},
		 {'type': 'pattern', 'bits': 7, 'pattern': '1110011'}]}
		"""
		return {"encoding": [ x for x in args ]}

	@v_args(inline=True)
	def binary(self, number_of_bits: Token, bits: Token) -> dict:
		"""
		>>> data = '''7'b0010101'''
		>>> Lark(GRAMMAR, parser='lalr', start='binary', transformer=IntermediateTransformer()).parse(data)
 		{'type': 'pattern', 'bits': 7, 'pattern': '0010101'}
		>>> data = '''5'b10101'''
		>>> Lark(GRAMMAR, parser='lalr', start='binary', transformer=IntermediateTransformer()).parse(data)
 		{'type': 'pattern', 'bits': 5, 'pattern': '10101'}
		>>> data = '''1'b1'''
		>>> Lark(GRAMMAR, parser='lalr', start='binary', transformer=IntermediateTransformer()).parse(data)
 		{'type': 'pattern', 'bits': 1, 'pattern': '1'}
		>>> data = '''0b000000000000'''
		>>> Lark(GRAMMAR, parser='lalr', start='binary', transformer=IntermediateTransformer()).parse(data)
		{'type': 'pattern', 'bits': 12, 'pattern': '000000000000'}
		"""
		if number_of_bits != '0':
			assert(int(number_of_bits) == len(bits)), f"Incompatible pattern definition: {number_of_bits}b{''.join([x for x in bits])}"

		return {"type": "pattern",
	  		"bits": int(number_of_bits) if int(number_of_bits) != 0 else len(bits),
			"pattern" : ''.join([x for x in bits])}

	@v_args(inline=True)
	def register(self, name: str, upper_bit: int, lower_bit: int) -> dict:
		"""
		>>> data = '''rs2[4:0]'''
		>>> Lark(GRAMMAR, parser='lalr', start='register', transformer=IntermediateTransformer()).parse(data)
		{'type': 'operand', 'name': 'rs2', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}
		>>> data = '''csr[11:0]'''
		>>> Lark(GRAMMAR, parser='lalr', start='register', transformer=IntermediateTransformer()).parse(data)
		{'type': 'operand', 'name': 'csr', 'bits': 12, 'bits_top': 11, 'bits_bottom': 0}
		>>> data = '''rs1[4:0]'''
		>>> Lark(GRAMMAR, parser='lalr', start='register', transformer=IntermediateTransformer()).parse(data)
		{'type': 'operand', 'name': 'rs1', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}
		>>> data = '''rs1[2:0]'''
		>>> Lark(GRAMMAR, parser='lalr', start='register', transformer=IntermediateTransformer()).parse(data)
		{'type': 'operand', 'name': 'rs1', 'bits': 3, 'bits_top': 2, 'bits_bottom': 0}
		"""
		return {"type": "operand",
	  		"name": str(name),
			"bits": int(upper_bit) - int(lower_bit) + 1,
			"bits_top": int(upper_bit),
			"bits_bottom": int(lower_bit)}

	def instructions(self, list_of_instructions) -> list:
		"""
		>>> data = '''{
		... LUI {
		... encoding: imm[31:12] :: rd[4:0] :: 7'b0110111;
		... assembly: "{name(rd)}, {imm:#05x}";
		... behavior: if ((rd % RFS) != 0) X[rd % RFS] = (unsigned<XLEN>) ((signed) imm);
		... }
		... AUIPC {
		... encoding: imm[31:12] :: rd[4:0] :: 7'b0010111;
		... assembly: "{name(rd)}, {imm:#08x}";
		... behavior: if ((rd % RFS) != 0) X[rd % RFS] = PC+(signed)imm;
		... }}'''
		>>> Lark(GRAMMAR, parser='lalr', start='instructions', transformer=IntermediateTransformer()).parse(data) # doctest: +NORMALIZE_WHITESPACE
		[{'name': 'lui',
		 'operands':
		 [{'name': 'imm', 'bits': 20, 'bits_top': 31, 'bits_bottom': 12},
		  {'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}],
		 'encoding':
		  [{'type': 'operand', 'name': 'imm', 'bits': 20, 'bits_top': 31, 'bits_bottom': 12},
		   {'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		   {'type': 'pattern', 'bits': 7, 'pattern': '0110111'}]},
		{'name': 'auipc',
		 'operands':
		 [{'name': 'imm', 'bits': 20, 'bits_top': 31, 'bits_bottom': 12},
		  {'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0}],
		'encoding':
		 [{'type': 'operand', 'name': 'imm', 'bits': 20, 'bits_top': 31, 'bits_bottom': 12},
		  {'type': 'operand', 'name': 'rd', 'bits': 5, 'bits_top': 4, 'bits_bottom': 0},
		  {'type': 'pattern', 'bits': 7, 'pattern': '0010111'}]}]
		"""
		return list_of_instructions

def parse_segment(isa_segment: str) -> List[Dict[str,List[str]]]:
	return Lark(GRAMMAR, parser='lalr', start='instructions', transformer=IntermediateTransformer()).parse(isa_segment)

def isolate_isa_segment(core_data: List[str], isa: str) -> Union[str, None]:
	"""
	Scope based search. No regexes and no parsers (yet) needed.
	Starts from { and ends when this bracket is closed (}). Returns
	the slice that encloses the requested ISA extension if found. None otherwise
	"""
	segment_starting_index = None
	for index, entry in enumerate(core_data):
		if f"InstructionSet {isa} " in entry:
			segment_starting_index = index
			break

	# Can happen if you provide multiple files as sources and
	# ISA segments that belong to one of the aforementioned.
	# Hence, when called for the rest of the .core files it will
	# return None and the main loop will continue gracefully.
	if not segment_starting_index:
		return None

	segment_ending_index = None
	bracket_counter = 0
	for index, entry in enumerate(core_data[segment_starting_index:]):
		if '{' in entry and '}' not in entry:
			bracket_counter += 1

		elif '}' in entry and '{' not in entry:
			bracket_counter -= 1

		if bracket_counter == 0:
			segment_ending_index = segment_starting_index + index
			break

	"""
	Isolates the instructions ONLY inbetween two brackets {}. Note the final [:-1] slice to remove the trailing }
	matching the the previously opened 'InstructionSet XXXX extends YYYY {'
	"""
	return re.findall("instructions(.*)", '\n'.join(core_data[segment_starting_index:segment_ending_index + 1]), re.MULTILINE | re.DOTALL)[0][:-1]

def main():

	to_json = list()

	for core_file in arguments.core_dsl:

		source_data = subprocess.run(f'gcc -E -P -x c {core_file} -nostdinc',
			shell=True,
			check=True,
			universal_newlines=True,
			stdout=subprocess.PIPE,
			 stderr=subprocess.DEVNULL).stdout

		source_data = [ x.strip() for x in source_data.split('\n') if x.strip() ]

		for isa in arguments.isa:
			# trim the redundant information and isolate the instructions of the segment
			extracted_isa_segment = isolate_isa_segment(source_data, isa)

			if not extracted_isa_segment:
				continue

			isa_segment = parse_segment(extracted_isa_segment)

			if arguments.include_only_instructions:
				isa_segment = list(filter(
				lambda x : x['name'] in arguments.include_only_instructions,
				isa_segment))

			elif arguments.excluded_instructions:
				isa_segment = list(filter(
				lambda x : x['name'] not in arguments.excluded_instructions,
				isa_segment))

			for instruction in isa_segment:
				print(f"Generating instruction \033[1m\033[96m{instruction['name']}\033[0m")

			to_json += isa_segment

	with open(arguments.output_file, 'w') as output:
		output.write(json.dumps(to_json, indent=4))

if __name__ == "__main__":

	DSL2_ISA_SEGMENTS = [
		"RV32I","Zicsr","Zifencei","RVNMode","RVSMode","RVDebug", # RV32I.core_dsl
		"RV64I",                                                  # RV64I.core_dsl
		"RV32A","RV64A",                                          # RVA.core_dsl
		"RV32IC","RV32FC","RV32DC","RV64IC","RV128IC",            # RVC.core_dsl
		"RV32D","RV64D",                                          # RVD.core_dsl
		"RV32F","RV64F",                                          # RVF.core_dsl
		"RV32M","RV64M"]                                          # RVM.core_dsl
	parser = argparse.ArgumentParser()
	parser.add_argument('--core-descriptions', required=True, dest='core_dsl', nargs="+", help='Input CoreDSL2 file(s)')
	parser.add_argument('--isa', required=True, dest='isa', nargs='+', choices = DSL2_ISA_SEGMENTS, help='ISA code(s)')
	parser.add_argument('--output-file', required=True, action='store', dest='output_file', help='Output JSON filename')
	include_exclude = parser.add_mutually_exclusive_group()
	include_exclude.add_argument('--exclude-instructions', required=False, default=[], dest='excluded_instructions', nargs='+', help ='Omits specific instructions from ISA segment(s)')
	include_exclude.add_argument('--include-instructions', required=False, default=[], dest='include_only_instructions', nargs='+', help='Includes ONLY the mentioned instructions from the ISA segment(s)' )
	arguments = parser.parse_args()

	main()
