#include <vminstrs.hpp>

namespace vm::instrs {
profiler_t nor = {
    "NOR",
    mnemonic_t::nor,
    {{// MOV REG, [VSP]
      LOAD_VALUE,
      // MOV REG, [VSP+OFFSET]
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_MOV &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[1].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[1].mem.base == vsp &&
               operands[1].mem.disp.size;
      },
      // NOT REG
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_NOT &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER;
      },
      // OR REG, REG
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_OR &&
               operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
               operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER;
      },
      // MOV [VSP+OFFSET], REG
      [](const zydis_reg_t vip, const zydis_reg_t vsp,
         const zydis_decoded_instr_t& instr, std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT>& operands) -> bool {
        return instr.mnemonic == ZYDIS_MNEMONIC_MOV &&
               operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
               operands[0].mem.base == vsp &&
               operands[0].mem.disp.size &&
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
      vinstr_t res{mnemonic_t::nand};
      res.imm.has_imm = false;

      // MOV [VSP+OFFSET], REG
      const auto mov_vsp_reg = std::find_if(
          hndlr.m_instrs.begin(), hndlr.m_instrs.end(),
          [&](emu_instr_t& instr) -> bool {
            const auto& i = instr.m_instr;
            return i.mnemonic == ZYDIS_MNEMONIC_MOV &&
                   instr.operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
                   instr.operands[0].mem.base == vsp &&
                   instr.operands[0].mem.disp.size &&
                   instr.operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER;
          });

      res.stack_size = mov_vsp_reg->operands[1].size;
      return res;
    }};
}