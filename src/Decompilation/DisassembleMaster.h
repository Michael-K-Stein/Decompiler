#ifndef DISASSEMBLEMASTER_H_INCLUDED
#define DISASSEMBLEMASTER_H_INCLUDED

#include "Common.h"
#include "_00.h"
#include "ControlFlow_Conditional/JMP_Call.h"

typedef struct PrefixOut {
    int prefixOffset;

    bool Operand_Size_Override;
    bool Address_Size_Override;
};

PrefixOut WritePrefixes(unsigned char * opCodes, char * output, int *OpCodeOffset) {
    unsigned char * prefixes = (unsigned char *)calloc(sizeof(unsigned char), 4);

    int offset = 0;

    bool Operand_Size_Override = false;
    bool Address_Size_Override = false;

    /// Try loading the maximum of 4 prefix bytes
    for (int prefixInd = 0; prefixInd < 4; prefixInd++) {
        for (int prefType = 0; prefType < 11; prefType++){
            if (PREFIXES_LIST[prefType] == opCodes[prefixInd]) {
                prefixes[prefixInd] = opCodes[prefixInd];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (prefixes[i] != 0x00) {
            *OpCodeOffset+=1;
            //printf("0x%02X\n", prefixes[i]);
            switch (prefixes[i]) {
            case LOCK:
                memcpy(output + offset, &"LOCK ",5);
                offset += 5;
                break;
            case REPE:
                memcpy(output + offset, &"REPE ",5);
                offset += 5;
                break;
            case REPNE:
                memcpy(output + offset, &"REPNE ",6);
                offset += 6;
                break;
            case ES:
                memcpy(output + offset, &"ES ",3);
                offset += 3;
                break;
            case CS:
                memcpy(output + offset, &"CS ",3);
                offset += 3;
                break;
            case SS:
                memcpy(output + offset, &"SS ",3);
                offset += 3;
                break;
            case DS:
                memcpy(output + offset, &"DS ",3);
                offset += 3;
                break;
            case FS:
                memcpy(output + offset, &"FS ",3);
                offset += 3;
                break;
            case GS:
                memcpy(output + offset, &"GS ",3);
                offset += 3;
                break;
            case OPERAND_OVERRIDE:
                Operand_Size_Override = true;
                break;
            case ADDRESS_OVERRIDE:
                Address_Size_Override = true;
                break;
            }
        } else { break; }
    }

    PrefixOut prefOut;
    prefOut.prefixOffset = offset;
    prefOut.Address_Size_Override = Address_Size_Override;
    prefOut.Operand_Size_Override = Operand_Size_Override;

    return prefOut;
}

bool IsBetween(unsigned char Val, int incmin, int incmax) { return (Val >= incmin && Val <= incmax); }
int IsRegular(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x00, 0x03)) { memcpy(opCMDOutput, &"ADD", 3); return 1; }
    if (IsBetween(opCode[0], 0x08, 0x0D)) { memcpy(opCMDOutput, &"OR", 2); return 1; }
    if (IsBetween(opCode[0], 0x10, 0x15)) { memcpy(opCMDOutput, &"ADC", 3); return 1; }
    if (IsBetween(opCode[0], 0x18, 0x1D)) { memcpy(opCMDOutput, &"SBB", 3); return 1; }
    if (IsBetween(opCode[0], 0x20, 0x25)) { memcpy(opCMDOutput, &"AND", 3); return 1; }
    if (IsBetween(opCode[0], 0x28, 0x2D)) { memcpy(opCMDOutput, &"SUB", 3); return 1; }
    if (IsBetween(opCode[0], 0x30, 0x35)) { memcpy(opCMDOutput, &"XOR", 3); return 1; }
    if (IsBetween(opCode[0], 0xB0, 0xB8)) { memcpy(opCMDOutput, &"MOV", 3); return 1; }
    if (IsBetween(opCode[0], 0x88, 0x8B)) { memcpy(opCMDOutput, &"MOV", 3); return 1; }
    return 0;
}
int IsRegularIMM(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x80, 0x83)) { memcpy(opCMDOutput, &"ADD", 3); return 1; }
    return 0;
}
int IsOnlyIMM(unsigned char * opCode, char * opCMDOutput) {
    if (opCode[0] == 0x68) { memcpy(opCMDOutput, &"PUSH", 4); return 1; }
    if (opCode[0] == 0x6A) { memcpy(opCMDOutput, &"PUSH", 4); return 1; }
    return 0;
}
int IsOneByteOpCode(unsigned char * opCode, char * opCMDOutput, bool size_override) {
    switch (opCode[0]) {
        case 0x27: { memcpy(opCMDOutput, &"DAA", 3); return 1; break; }
        case 0x2F: { memcpy(opCMDOutput, &"DAS", 3); return 1; break; }
        case 0x37: { memcpy(opCMDOutput, &"AAA", 3); return 1; break; }
        case 0x3F: { memcpy(opCMDOutput, &"AAS", 3); return 1; break; }
        case 0x60: { memcpy(opCMDOutput, size_override ? &"PUSHA\0" : &"PUSHAD", 6); return 1; break; }
        case 0x61: { memcpy(opCMDOutput, size_override ? &"POPA\0" : &"POPAD", 5); return 1; break; }
        case 0x6C: { memcpy(opCMDOutput, &"INSB", 4); return 1; break; }
        case 0x6D: { memcpy(opCMDOutput, size_override ? &"INSW" : &"INSD", 4); return 1; break; }
        case 0x6E: { memcpy(opCMDOutput, &"OUTB", 4); return 1; break; }
        case 0x6F: { memcpy(opCMDOutput, size_override ? &"OUTW" : &"OUTD", 4); return 1; break; }
        case 0x9B: { memcpy(opCMDOutput, &"WAIT", 4); return 1; break; }
        case 0xCC: { memcpy(opCMDOutput, &"INT3", 4); return 1; break; }
        case 0xCE: { memcpy(opCMDOutput, &"INTO", 4); return 1; break; }
        case 0x98: { memcpy(opCMDOutput, size_override ? &"CBW\0" : &"CWDE", 4); return 1; break; }
        case 0x99: { memcpy(opCMDOutput, size_override ? &"CWD" : &"CDQ", 3); return 1; break; }
        case 0x9C: { memcpy(opCMDOutput, size_override ? &"PUSHF\0" : &"PUSHFD", 6); return 1; break; }
        case 0x9D: { memcpy(opCMDOutput, size_override ? &"POPF\0" : &"POPFD", 5); return 1; break; }
        case 0x9E: { memcpy(opCMDOutput, &"SAHF", 4); return 1; break; }
        case 0x9F: { memcpy(opCMDOutput, &"LAHF", 4); return 1; break; }
        case 0xA4: { memcpy(opCMDOutput, &"MOVSB", 5); return 1; break; }
        case 0xA5: { memcpy(opCMDOutput, size_override ? &"MOVSW" : &"MOVSD", 5); return 1; break; }
        case 0xAA: { memcpy(opCMDOutput, &"STOSB", 5); return 1; break; }
        case 0xAB: { memcpy(opCMDOutput, size_override ? &"STOSW" : &"STOSD", 5); return 1; break; }
        case 0xAC: { memcpy(opCMDOutput, &"LODSB", 5); return 1; break; }
        case 0xAD: { memcpy(opCMDOutput, size_override ? &"LODSW" : &"LODSD", 5); return 1; break; }
        case 0xAE: { memcpy(opCMDOutput, &"SCASB", 5); return 1; break; }
        case 0xAF: { memcpy(opCMDOutput, size_override ? &"SCASW" : &"SCASD", 5); return 1; break; }
        case 0xC3: { memcpy(opCMDOutput, &"RETN", 4); return 1; break; }
        case 0xCB: { memcpy(opCMDOutput, &"RETF", 4); return 1; break; }
        case 0xCF: { memcpy(opCMDOutput, size_override ? &"IRET\0" : &"IRETD", 5); return 1; break; }
        case 0xD7: { memcpy(opCMDOutput, &"SALC", 4); return 1; break; }
        case 0xD8: { memcpy(opCMDOutput, &"XLAT", 4); return 1; break; }
        case 0xEC: { memcpy(opCMDOutput, &"IN AL, DX", 9); return 1; break; }
        case 0xED: { memcpy(opCMDOutput, size_override ? &"IN AX, DX\0" : &"IN EAX, DX", 10); return 1; break; }
        case 0xEE: { memcpy(opCMDOutput, &"OUT AL, DX", 10); return 1; break; }
        case 0xEF: { memcpy(opCMDOutput, size_override ? &"OUT AX, DX\0" : &"OUT EAX, DX", 11); return 1; break; }
        case 0xF1: { memcpy(opCMDOutput, &"ICEBP", 5); return 1; break; }
        case 0xF4: { memcpy(opCMDOutput, &"HLT", 3); return 1; break; }
        case 0xF5: { memcpy(opCMDOutput, &"CMC", 3); return 1; break; }
        case 0xF8: { memcpy(opCMDOutput, &"CLC", 3); return 1; break; }
        case 0xF9: { memcpy(opCMDOutput, &"STC", 3); return 1; break; }
        case 0xFA: { memcpy(opCMDOutput, &"CLI", 3); return 1; break; }
        case 0xFB: { memcpy(opCMDOutput, &"STI", 3); return 1; break; }
        case 0xFC: { memcpy(opCMDOutput, &"CLD", 3); return 1; break; }
        case 0xFD: { memcpy(opCMDOutput, &"STD", 3); return 1; break; }
    }
}
int IsSingularOpCode(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x40, 0x47)) { memcpy(opCMDOutput, &"INC", 3); return 1; }
    if (IsBetween(opCode[0], 0x48, 0x4F)) { memcpy(opCMDOutput, &"DEC", 3); return 1; }
    if (IsBetween(opCode[0], 0x50, 0x57)) { memcpy(opCMDOutput, &"PUSH", 4); return 1; }
    if (IsBetween(opCode[0], 0x58, 0x5F)) { memcpy(opCMDOutput, &"POP", 3); return 1; }
    return 0;
}
int IsSegment(unsigned char * opCode, char * opCMDOutput) {
    int ret = 0;
    char * type = (char *)calloc(5, sizeof(char));
    char * seg = (char *)calloc(3, sizeof(char));

    if (opCode[0] == 0x0F) { return 0; } /// This is a two byte op-code

    sprintf(type, "%s", (opCode[0] % 2) ? &"POP\0" : &"PUSH" );

    if (opCode[0] >> 1 == 0b0000011) { memcpy(seg, &"ES", 2); ret = 1; }
    if (opCode[0] >> 1 == 0b0001011) { memcpy(seg, &"SS", 2); ret = 1; }
    if (opCode[0] >> 1 == 0b0000111) { memcpy(seg, &"CS", 2); ret = 1; }
    if (opCode[0] >> 1 == 0b0001111) { memcpy(seg, &"DS", 2); ret = 1; }

    sprintf(opCMDOutput, "%s %s", type, seg);

    if (!ret) { memcpy(opCMDOutput, &"\0\0\0\0\0\0\0\0", 8); }
    return ret;
}
int IsSpecial(unsigned char * opCode, char * opCMDOutput, bool size_override) {
    if (opCode[0] == 0x04) { memcpy(opCMDOutput, &"ADD AL", 6); return 1; }
    if (opCode[0] == 0x05) { if (!size_override) { memcpy(opCMDOutput, &"ADD EAX", 7); } else { memcpy(opCMDOutput, &"ADD AX", 6); } return 1; }
    if (IsBetween(opCode[0], 0x90, 0x97)) {memcpy(opCMDOutput, &"XCHG", 4); return 1; }
    return 0;
}
int IsSpecialA(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0xA0, 0xA3)) { memcpy(opCMDOutput, &"MOV", 3); return 1; }
    return 0;
}
int IsJMPCall(unsigned char * opCode, char * opCMDOutput) {
    if (opCode[0] == 0x9A) { memcpy(opCMDOutput, &"CALL FAR", 8); return 1; }
    if (opCode[0] == 0xE8) { memcpy(opCMDOutput, &"CALL", 4); return 1; }
    if (opCode[0] == 0xE9) { memcpy(opCMDOutput, &"JMP", 3); return 1; }
    if (opCode[0] == 0xEA) { memcpy(opCMDOutput, &"JMP FAR", 7); return 1; }
    if (opCode[0] == 0xEB) { memcpy(opCMDOutput, &"JMP", 3); return 1; }
    return 0;
}

char * JCCCMD = { "JO\0\0JNO\0JB\0\0JNB\0JE\0\0JNE\0JBE\0JA\0\0JS\0\0JNS\0JPE\0JPO\0JL\0\0JGE\0JLE\0JG\0\0" };
int IsJCC(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x70, 0x7F)) { memcpy(opCMDOutput, JCCCMD + ((int)(opCode[0] - 0x70) * 4), 3); return 1; }
    if (opCode[0] == 0x0F && IsBetween(opCode[1], 0x80, 0x8F)) { memcpy(opCMDOutput, JCCCMD + ((int)(opCode[1] - 0x80) * 4), 3); return 1; }
    return 0;
}

int Disassemble(unsigned char * opCodes, char * output, bool _32bit, int TOTAL_FILE_OFFSET) {
    int OpCodeOffset = 0;
    int OpCodeTotal = 0;
    PrefixOut prefixOut = WritePrefixes(opCodes + OpCodeOffset, output, &OpCodeOffset);
    prefixOut.Address_Size_Override = prefixOut.Address_Size_Override ^ !_32bit;
    prefixOut.Operand_Size_Override = prefixOut.Operand_Size_Override ^ !_32bit;
    OpCodeTotal = OpCodeOffset;
    //printf("OpCodeOffset: %d\n", OpCodeOffset);

    bool d = (opCodes[OpCodeOffset] % 4) >> 1; // Add R/M to REG
    bool s = (opCodes[OpCodeOffset] % 2); // 32bit values

    char * reg = (char *)calloc(3, sizeof(char));  int reg_len_off = getRegisterName( ( opCodes[OpCodeOffset + 1] % 64 ) >> 3, s, prefixOut.Operand_Size_Override, reg  );
    char * rm = (char *)calloc(64, sizeof(char)); int rm_len_off = generateRM(opCodes + OpCodeOffset, rm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);

    char * opCMD = (char *)calloc(16, sizeof(char));
    if (IsRegular(opCodes + OpCodeOffset,opCMD)){
        sprintf(output, "%s %s, %s", opCMD, d ? reg : rm, d ? rm : reg);
        OpCodeOffset += rm_len_off + 1;
    } else if (IsRegularIMM(opCodes + OpCodeOffset, opCMD)) {
        char * imm = (char *)calloc(32, sizeof(char)); int imm_len_off = generateIMM(opCodes + OpCodeOffset + rm_len_off + 1, imm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override, opCodes[OpCodeOffset] % 4);
        sprintf(output, "%s %s, %s", opCMD, rm, imm);
        OpCodeOffset += rm_len_off + imm_len_off + 1;
    } else if (IsOnlyIMM(opCodes + OpCodeOffset, opCMD)) {
        char * imm = (char *)calloc(32, sizeof(char)); int imm_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override, (opCodes[OpCodeOffset] % 4) + 1);
        sprintf(output, "%s %s", opCMD, imm);
        OpCodeOffset += imm_len_off + 1;
    } else if (IsSingularOpCode(opCodes + OpCodeOffset, opCMD)) {
        sprintf(output, "%s %s", opCMD, SingleOpCMD(opCodes + OpCodeOffset, prefixOut.Operand_Size_Override));
        OpCodeOffset++;
    } else if (IsSegment(opCodes + OpCodeOffset, opCMD)) { sprintf(output, "%s", opCMD); OpCodeOffset++; }
    else if (IsSpecial(opCodes + OpCodeOffset, opCMD, prefixOut.Operand_Size_Override)) {
        if (IsBetween(opCodes[0], 0x90, 0x97)) {
            char * reg2 = (char *)calloc(4, sizeof(char));
            getRegisterName(opCodes[0] % 0x08, true, prefixOut.Operand_Size_Override, reg2);
            sprintf(output, "%s %s, %s", opCMD, prefixOut.Operand_Size_Override ? &"AX\0\0" : &"EAX\0", reg2);
            OpCodeOffset++;
        } else {
            char * imm = (char *)calloc(16, sizeof(char));
            int imm_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm, false, prefixOut.Operand_Size_Override, s);
            sprintf(output, "%s, %s", opCMD, imm);
            OpCodeOffset += 1 + imm_len_off;
        }
    } else if (IsSpecialA(opCodes + OpCodeOffset, opCMD)) {
        char * imm = (char *)calloc(16, sizeof(char));
        char * immF = (char *)calloc(18, sizeof(char));
        int imm_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm, false, prefixOut.Operand_Size_Override, true);
        sprintf(immF, "[%s]", imm);
        char * regs = {"AL\0AX\0EAX\0"};
        char * reg = (char*)calloc(4,sizeof(char));
        reg = &regs[s ? (prefixOut.Operand_Size_Override ? 3 : 6 ) : 0];
        sprintf(output, "%s %s, %s", opCMD, d ? immF : reg, d ? reg : immF);
        OpCodeOffset += 1 + imm_len_off;
    } else if (IsJMPCall(opCodes + OpCodeOffset, opCMD)) {
        char * disp = (char *)calloc(16, sizeof(char));
        OpCodeOffset += generateJMPCall(opCodes + OpCodeOffset, disp, prefixOut.Operand_Size_Override);
        sprintf(output, "%s %s", opCMD, disp);
    } else if (IsJCC(opCodes + OpCodeOffset, opCMD)) {
        char * disp = (char *)calloc(6, sizeof(char));
        bool near = false;
        OpCodeOffset++; if (opCodes[OpCodeOffset - 1] == 0x0F) { OpCodeOffset++; near = true; }
        OpCodeOffset += getDisplacementJcc(opCodes + OpCodeOffset, disp, TOTAL_FILE_OFFSET, near, prefixOut.Operand_Size_Override);
        sprintf(output, "%s %s", opCMD, disp);
    } else if (IsOneByteOpCode(opCodes + OpCodeOffset, opCMD,prefixOut.Operand_Size_Override)) {
        OpCodeOffset++;
        sprintf(output, "%s", opCMD);
    }

    //printf("\n\nREG: %s\n", reg);
    //printf("\nR/M: %s\n\n", rm);

    return OpCodeOffset;
    printf("\nOp codes used (in bytes): %d\n", OpCodeOffset);


    //printf("%s\n", output);

    //int codeOffset = Decomp_0x0X(opCodes + OpCodeOffset, output + prefixOut.prefixOffset, prefixOut.Operand_Size_Override, prefixOut.Address_Size_Override);
    //sprintf(dec, )
}

#endif // DISASSEMBLEMASTER_H_INCLUDED
