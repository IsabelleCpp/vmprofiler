#include <vminstrs.hpp>

namespace vm::instrs {
profiler_t add = {
    "ADD",
    mnemonic_t::add,
    {{// MOV REG, [VSP]
      LOAD_VALUE,
      // MOV REG, [VSP+OFFSET]
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_MOV &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[1].mem.base == vsp &&
               operands[1].mem.disp.value;
      },
      // ADD REG, REG
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_ADD &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER;
      },
      // MOV [VSP+OFFSET], REG
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_MOV &&
               operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[0].mem.base == vsp &&
               operands[0].mem.disp.value &&
               operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER;
      },
      // PUSHFQ
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_PUSHFQ;
      },
      // POP [VSP]
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_POP &&
               operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[0].mem.base == vsp;
      }}},
    [](zydis_reg_t& vip, zydis_reg_t& vsp,
       hndlr_trace_t& hndlr) -> std::optional<vinstr_t> {
      vinstr_t res{mnemonic_t::add};

      // MOV REG, [VSP]
      const auto mov_reg_vsp = std::find_if(
          hndlr.m_instrs.begin(), hndlr.m_instrs.end(),
          [&](emu_instr_t& instr) -> bool {
            const auto& i = instr.m_instr;
            return i.mnemonic == ZYDIS_MNEMONIC_MOV &&
                   instr.operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
                   instr.operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                   instr.operands[1].mem.base == vsp;
          });

      // MOV [VSP+OFFSET], REG
      const auto mov_vsp_offset = std::find_if(
          hndlr.m_instrs.begin(), hndlr.m_instrs.end(),
          [&](emu_instr_t& instr) -> bool {
            const auto& i = instr.m_instr;
            return i.mnemonic == ZYDIS_MNEMONIC_MOV &&
                   instr.operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                   instr.operands[0].mem.base == vsp &&
                   instr.operands[0].mem.disp.value &&
                   instr.operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER;
          });

      res.stack_size = mov_vsp_offset->operands[1].size;
      res.imm.size = mov_reg_vsp->operands[1].size;
      return res;
    }};
}