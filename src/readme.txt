start date : 11 December 2025.


MazeGen's parsing guidelines.


file structure:
    file is split into 2 main sections :
        onebyte & twobyte opcodes.

    inside each are the follwing:
        A lot of "pri_opcd" elements. Inside each "pri_opcd" is a lot of shit. 
        The shit is structured as follows:
            <pri_opcd>
                <entry>
                    <opcd_ext> // these are opcode extension generated using the modrm.reg field.
                    <sec_opcd> // this denote the ModRM.RM value for this operand. ( yes, there are variants distinguished by R/M too. :) )
                    <syntax mod="mem" || mod ="nomem">
                        <menm> // mnemonic, name of the opcode.
                            <dst> & <src> // if you fail to find any <a> tags as <dst's> child, then the destination is a register. 
                                          // Get text between the tags ( thats where the register name should be. ) ( GetText() in TinyXML )
                                <a> // Operand addressing method.
                                <t> // Operand type.
                    <note><brief> // Addition information. brief nested in note. ( ideally, find this recursively. )


The following are some facts, informtion us about how does the MazeGen's .xml https://github.com/mazegen/x86reference/blob/master/x86reference.xml
translate to the intel's architecture software developer's manual.

1. If <entry> tag has attribute as [ attr = "invd" ], then instruction in invalid.
2. If <entry> tag has attribute as [ attr = "invd" ], and [ mode = "e" ], then instruction in invalid in 64 bit mode.
3. Need for immediate can be determined by the addressing mode ( I ) { or ( imm ) in coder's edition according to mazegen. } 
4. If <entry> tag has attribute as [ r = "yes" ], then ModRM byte is required.
5. If <entry> tag has a child as <opcd_ext>, then ModRM byte is required.
6. <opcd_ext> tag means opcode extension needs to be lookedup using the modrm.REG field.
7. If any of the operands of a instruction is ( Z ), then the 3 least significant bits of the opcode chooses a general purpose register.
    ( Mazegen won't create those extra entires, we need to create them manually. )
8. mode = "mem" means that ModRM.mod != 0b11 and nomem means ModRM.mod == 0b11. ( these are used in the opcode extension table. )
9. If tag <sec_opcd> appears, it too repesents "nomem" mode and the value in the tag represents the R/M corosponding to that specific entiry, which will be used
    to distinguish this variant of this instruction.
