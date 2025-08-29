#include <vminstrs.hpp>

namespace vm::instrs {
profiler_t write = {
    "WRITE",
    mnemonic_t::write,
    {{// MOV REG, [VSP]
      LOAD_VALUE,
      // MOV REG, [VSP+OFFSET]
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, const std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_MOV &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[1].mem.base == vsp &&
               operands[1].mem.disp.size;
      },
      // ADD VSP, OFFSET
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, const std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_ADD &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[0].reg.value == vsp &&
               operands[1].type == ZYDIS_OPERAND_TYPE_IMMEDIATE;
      },
      // MOV [REG], REG
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, const std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_MOV &&
               operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[0].mem.base != vsp &&
               operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[1].reg.value != vsp;
      }}},
    [](zydis_reg_t& vip, zydis_reg_t& vsp,
       hndlr_trace_t& hndlr) -> std::optional<vinstr_t> {
      vinstr_t res{mnemonic_t::write};
      res.imm.has_imm = false;

      const auto mov_reg_reg = std::find_if(
          hndlr.m_instrs.begin(), hndlr.m_instrs.end(),
          [&](emu_instr_t& instr) -> bool {
            const auto& i = instr.m_instr;
            return i.mnemonic == ZYDIS_MNEMONIC_MOV &&
                   instr.operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                   instr.operands[0].mem.base != vsp &&
                   instr.operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER &&
                   instr.operands[1].reg.value != vsp;
          });

      res.stack_size = mov_reg_reg->operands[1].size;
      return res;
    }};
}