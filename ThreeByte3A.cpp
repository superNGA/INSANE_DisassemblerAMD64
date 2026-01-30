    // 0x0
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x0].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x0,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x1
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x1].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x3
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x3].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x3].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x3].InsertVarient(10);
    {
        // 0x3
        // valignd/q Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x3].m_pVarients[10]->Init(
            /*szName         = */"valignd/q",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x3,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x4
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x8].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x8,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x8].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x8].InsertVarient(10);
    {
        // 0x8
        // vroundps Vx,Wx,Ib (66) 
        m_EVEXThreeByteOpCodes_3A[0x8].m_pVarients[10]->Init(
            /*szName         = */"vroundps",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x8,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x8].InsertVarient(10);
    {
        // 0x8
        // vrndscaleps Vx,Wx,Ib (66),(evo) 
        m_EVEXThreeByteOpCodes_3A[0x8].m_pVarients[10]->Init(
            /*szName         = */"vrndscaleps",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x8,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x8].InsertVarient(0);
    {
        // 0x8
        // vrndscaleph Vx,Wx,Ib (evo)
        m_EVEXThreeByteOpCodes_3A[0x8].m_pVarients[0]->Init(
            /*szName         = */"vrndscaleph",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x8,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x9
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x9].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x9,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x9].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x9].InsertVarient(10);
    {
        // 0x9
        // vroundpd Vx,Wx,Ib (66) 
        m_EVEXThreeByteOpCodes_3A[0x9].m_pVarients[10]->Init(
            /*szName         = */"vroundpd",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x9,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x9].InsertVarient(10);
    {
        // 0x9
        // vrndscalepd Vx,Wx,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0x9].m_pVarients[10]->Init(
            /*szName         = */"vrndscalepd",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x9,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0xA
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xA].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xA,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xA].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xA].InsertVarient(10);
    {
        // 0xA
        // vrndscaless Vx,Hx,Wx,Ib (66),(evo) 
        m_EVEXThreeByteOpCodes_3A[0xA].m_pVarients[10]->Init(
            /*szName         = */"vrndscaless",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xA,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }
    m_EVEXThreeByteOpCodes_3A[0xA].InsertVarient(0);
    {
        // 0xA
        // vrndscalesh Vx,Hx,Wx,Ib (evo)
        m_EVEXThreeByteOpCodes_3A[0xA].m_pVarients[0]->Init(
            /*szName         = */"vrndscalesh",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xA,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0xB
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xB].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xB,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xB].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xB].InsertVarient(10);
    {
        // 0xB
        // vrndscalesd Vx,Hx,Wx,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0xB].m_pVarients[10]->Init(
            /*szName         = */"vrndscalesd",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xB,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0xC
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xC].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xC,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xC].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xC].InsertVarient(10);
    {
        // 0xC
        // vblendps Vx,Hx,Wx,Ib (66)
        m_EVEXThreeByteOpCodes_3A[0xC].m_pVarients[10]->Init(
            /*szName         = */"vblendps",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0xD
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xD].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xD,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xD].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xD].InsertVarient(10);
    {
        // 0xD
        // vblendpd Vx,Hx,Wx,Ib (66)
        m_EVEXThreeByteOpCodes_3A[0xD].m_pVarients[10]->Init(
            /*szName         = */"vblendpd",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xD,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0xE
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xE].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xE,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xF
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xF].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xF,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x10
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x10].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x10,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x11
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x11].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x11,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x12
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x12].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x12,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x13
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x13].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x13,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x14
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x14].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x14,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x15
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x15].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x15,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x16
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x16].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x16,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x17
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x17].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x17,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x18
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x18].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x18,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x18].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x18].InsertVarient(10);
    {
        // 0x18
        // vinsertf32x4/64x2 Vqq,Hqq,Wqq,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0x18].m_pVarients[10]->Init(
            /*szName         = */"vinsertf32x4/64x2",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x18,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_qq),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x19
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x19].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x19,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x19].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x19].InsertVarient(10);
    {
        // 0x19
        // vextractf32x4/64x2 Wdq,Vqq,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0x19].m_pVarients[10]->Init(
            /*szName         = */"vextractf32x4/64x2",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x19,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_W, OperandType_dq),
            /*operand2       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x1A
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x1A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x1A].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x1A].InsertVarient(10);
    {
        // 0x1A
        // vinsertf32x8/64x4 Vqq,Hqq,Wqq,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x1A].m_pVarients[10]->Init(
            /*szName         = */"vinsertf32x8/64x4",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x1A,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_qq),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x1B
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x1B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x1B].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x1B].InsertVarient(10);
    {
        // 0x1B
        // vextractf32x8/64x4 Wdq,Vqq,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x1B].m_pVarients[10]->Init(
            /*szName         = */"vextractf32x8/64x4",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x1B,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_W, OperandType_dq),
            /*operand2       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x1C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x1C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x1D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x1D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x1D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x1E
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x1E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x1E].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x1E].InsertVarient(10);
    {
        // 0x1E
        // vpcmpud/q Vk,Hd,Wd,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x1E].m_pVarients[10]->Init(
            /*szName         = */"vpcmpud/q",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x1E,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_k),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_d),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_d),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x1F
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x1F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x1F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x1F].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x1F].InsertVarient(10);
    {
        // 0x1F
        // vpcmpd/q Vk,Hd,Wd,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x1F].m_pVarients[10]->Init(
            /*szName         = */"vpcmpd/q",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x1F,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_k),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_d),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_d),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x20
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x20].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x20,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x21
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x21].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x21,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x22
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x22].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x22,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x23
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x23].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x23,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x23].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x23].InsertVarient(10);
    {
        // 0x23
        // vshuff32x4/64x2 Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x23].m_pVarients[10]->Init(
            /*szName         = */"vshuff32x4/64x2",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x23,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x24
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x24].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x24,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x25
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x25].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x25,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x25].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x25].InsertVarient(10);
    {
        // 0x25
        // vpternlogd/q Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x25].m_pVarients[10]->Init(
            /*szName         = */"vpternlogd/q",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x25,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x26
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x26].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x26,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x26].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x26].InsertVarient(10);
    {
        // 0x26
        // vgetmantps/d Vx,Wx,Ib (66),(ev) 
        m_EVEXThreeByteOpCodes_3A[0x26].m_pVarients[10]->Init(
            /*szName         = */"vgetmantps/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x26,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x26].InsertVarient(0);
    {
        // 0x26
        // vgetmantph Vx,Wx,Ib (ev)
        m_EVEXThreeByteOpCodes_3A[0x26].m_pVarients[0]->Init(
            /*szName         = */"vgetmantph",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x26,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x27
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x27].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x27,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x27].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x27].InsertVarient(10);
    {
        // 0x27
        // vgetmantss/d Vx,Hx,Wx,Ib (66),(ev) 
        m_EVEXThreeByteOpCodes_3A[0x27].m_pVarients[10]->Init(
            /*szName         = */"vgetmantss/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x27,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x27].InsertVarient(0);
    {
        // 0x27
        // vgetmantsh Vx,Hx,Wx,Ib (ev)
        m_EVEXThreeByteOpCodes_3A[0x27].m_pVarients[0]->Init(
            /*szName         = */"vgetmantsh",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x27,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x28
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x28].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x28,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x29
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x29].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x29,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x2F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x2F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x2F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x30
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x30].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x30,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x31
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x31].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x31,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x32
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x32].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x32,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x33
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x33].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x33,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x34
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x34].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x34,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x35
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x35].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x35,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x36
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x36].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x36,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x37
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x37].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x37,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x38
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x38].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x38,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x38].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x38].InsertVarient(10);
    {
        // 0x38
        // vinserti32x4/64x2 Vqq,Hqq,Wqq,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0x38].m_pVarients[10]->Init(
            /*szName         = */"vinserti32x4/64x2",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x38,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_qq),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x39
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x39].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x39,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x39].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x39].InsertVarient(10);
    {
        // 0x39
        // vextracti32x4/64x2 Wdq,Vqq,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0x39].m_pVarients[10]->Init(
            /*szName         = */"vextracti32x4/64x2",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x39,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_W, OperandType_dq),
            /*operand2       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x3A
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x3A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x3A].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x3A].InsertVarient(10);
    {
        // 0x3A
        // vinserti32x8/64x4 Vqq,Hqq,Wqq,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x3A].m_pVarients[10]->Init(
            /*szName         = */"vinserti32x8/64x4",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x3A,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_qq),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x3B
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x3B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x3B].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x3B].InsertVarient(10);
    {
        // 0x3B
        // vextracti32x8/64x4 Wdq,Vqq,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x3B].m_pVarients[10]->Init(
            /*szName         = */"vextracti32x8/64x4",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x3B,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_W, OperandType_dq),
            /*operand2       = */Operand_t( OperandMode_V, OperandType_qq),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x3C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x3C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x3C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x3D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x3D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x3D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x3E
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x3E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x3E].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x3E].InsertVarient(10);
    {
        // 0x3E
        // vpcmpub/w Vk,Hk,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x3E].m_pVarients[10]->Init(
            /*szName         = */"vpcmpub/w",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x3E,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_k),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_k),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x3F
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x3F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x3F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x3F].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x3F].InsertVarient(10);
    {
        // 0x3F
        // vpcmpb/w Vk,Hk,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x3F].m_pVarients[10]->Init(
            /*szName         = */"vpcmpb/w",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x3F,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_k),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_k),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x40
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x40].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x40,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x40].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x40].InsertVarient(10);
    {
        // 0x40
        // vdpps Vx,Hx,Wx,Ib (66)
        m_EVEXThreeByteOpCodes_3A[0x40].m_pVarients[10]->Init(
            /*szName         = */"vdpps",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x40,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x41
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x41].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x41,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x42
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x42].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x42,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x42].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x42].InsertVarient(10);
    {
        // 0x42
        // vdbpsadbw Vx,Hx,Wx,Ib (66),(evo)
        m_EVEXThreeByteOpCodes_3A[0x42].m_pVarients[10]->Init(
            /*szName         = */"vdbpsadbw",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x42,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x43
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x43].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x43,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x43].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x43].InsertVarient(10);
    {
        // 0x43
        // vshufi32x4/64x2 Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x43].m_pVarients[10]->Init(
            /*szName         = */"vshufi32x4/64x2",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x43,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x44
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x44].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x44,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x44].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x44].InsertVarient(10);
    {
        // 0x44
        // vpclmulqdq Vx,Hx,Wx,Ib (66)
        m_EVEXThreeByteOpCodes_3A[0x44].m_pVarients[10]->Init(
            /*szName         = */"vpclmulqdq",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x44,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x45
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x45].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x45,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x46
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x46].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x46,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x47
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x47].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x47,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x48
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x48].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x48,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x49
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x49].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x49,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x4A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x4B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x4C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x4D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x4E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x4F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x4F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x4F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x50
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x50].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x50,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x50].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x50].InsertVarient(10);
    {
        // 0x50
        // vrangeps/d Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x50].m_pVarients[10]->Init(
            /*szName         = */"vrangeps/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x50,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x51
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x51].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x51,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x51].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x51].InsertVarient(10);
    {
        // 0x51
        // vrangess/d Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x51].m_pVarients[10]->Init(
            /*szName         = */"vrangess/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x51,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x52
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x52].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x52,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x53
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x53].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x53,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x54
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x54].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x54,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x54].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x54].InsertVarient(10);
    {
        // 0x54
        // vfixupimmps/d Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x54].m_pVarients[10]->Init(
            /*szName         = */"vfixupimmps/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x54,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x55
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x55].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x55,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x55].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x55].InsertVarient(10);
    {
        // 0x55
        // vfixupimmss/d Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x55].m_pVarients[10]->Init(
            /*szName         = */"vfixupimmss/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x55,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x56
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x56].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x56,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x56].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x56].InsertVarient(10);
    {
        // 0x56
        // vreduceps/d Vx,Wx,Ib (66),(ev) 
        m_EVEXThreeByteOpCodes_3A[0x56].m_pVarients[10]->Init(
            /*szName         = */"vreduceps/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x56,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x56].InsertVarient(0);
    {
        // 0x56
        // vreduceph Vx,Wx,Ib (ev)
        m_EVEXThreeByteOpCodes_3A[0x56].m_pVarients[0]->Init(
            /*szName         = */"vreduceph",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x56,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x57
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x57].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x57,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x57].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x57].InsertVarient(10);
    {
        // 0x57
        // vreducess/d Vx,Hx,Wx,Ib (66),(ev) 
        m_EVEXThreeByteOpCodes_3A[0x57].m_pVarients[10]->Init(
            /*szName         = */"vreducess/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x57,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x57].InsertVarient(0);
    {
        // 0x57
        // vreducesh Vx,Hx,Wx,Ib (ev)
        m_EVEXThreeByteOpCodes_3A[0x57].m_pVarients[0]->Init(
            /*szName         = */"vreducesh",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x57,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x58
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x58].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x58,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x59
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x59].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x59,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x5F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x5F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x5F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x60
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x60].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x60,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x61
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x61].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x61,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x62
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x62].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x62,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x63
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x63].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x63,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x64
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x64].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x64,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x65
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x65].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x65,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x66
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x66].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x66,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x66].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x66].InsertVarient(10);
    {
        // 0x66
        // vfpclassps/d Vk,Wx,Ib (66),(ev) 
        m_EVEXThreeByteOpCodes_3A[0x66].m_pVarients[10]->Init(
            /*szName         = */"vfpclassps/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x66,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_k),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x66].InsertVarient(0);
    {
        // 0x66
        // vfpclassph Vx,Wx,Ib (ev)
        m_EVEXThreeByteOpCodes_3A[0x66].m_pVarients[0]->Init(
            /*szName         = */"vfpclassph",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x66,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x67
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x67].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x67,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x67].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x67].InsertVarient(10);
    {
        // 0x67
        // vfpclassss/d Vk,Wx,Ib (66),(ev) 
        m_EVEXThreeByteOpCodes_3A[0x67].m_pVarients[10]->Init(
            /*szName         = */"vfpclassss/d",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x67,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_k),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }
    m_EVEXThreeByteOpCodes_3A[0x67].InsertVarient(0);
    {
        // 0x67
        // vfpclasssh Vx,Wx,Ib (ev)
        m_EVEXThreeByteOpCodes_3A[0x67].m_pVarients[0]->Init(
            /*szName         = */"vfpclasssh",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x67,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0x68
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x68].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x68,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x69
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x69].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x69,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x6F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x6F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x6F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x70
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x70].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x70,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x70].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x70].InsertVarient(10);
    {
        // 0x70
        // vpshldw Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x70].m_pVarients[10]->Init(
            /*szName         = */"vpshldw",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x70,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x71
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x71].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x71,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x71].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x71].InsertVarient(10);
    {
        // 0x71
        // vpshldd/q Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x71].m_pVarients[10]->Init(
            /*szName         = */"vpshldd/q",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x71,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x72
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x72].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x72,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x72].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x72].InsertVarient(10);
    {
        // 0x72
        // vpshrdw Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x72].m_pVarients[10]->Init(
            /*szName         = */"vpshrdw",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x72,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x73
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0x73].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0x73,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0x73].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0x73].InsertVarient(10);
    {
        // 0x73
        // vpshrdd/q Vx,Hx,Wx,Ib (66),(ev)
        m_EVEXThreeByteOpCodes_3A[0x73].m_pVarients[10]->Init(
            /*szName         = */"vpshrdd/q",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0x73,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0x74
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x74].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x74,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x75
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x75].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x75,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x76
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x76].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x76,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x77
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x77].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x77,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x78
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x78].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x78,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x79
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x79].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x79,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x7F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x7F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x7F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x80
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x80].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x80,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x81
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x81].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x81,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x82
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x82].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x82,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x83
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x83].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x83,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x84
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x84].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x84,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x85
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x85].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x85,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x86
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x86].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x86,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x87
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x87].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x87,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x88
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x88].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x88,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x89
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x89].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x89,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x8A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x8A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x8B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x8B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x8C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x8C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x8D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x8D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x8E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x8E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x8F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x8F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x8F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x90
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x90].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x90,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x91
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x91].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x91,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x92
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x92].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x92,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x93
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x93].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x93,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x94
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x94].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x94,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x95
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x95].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x95,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x96
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x96].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x96,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x97
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x97].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x97,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x98
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x98].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x98,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x99
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x99].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x99,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x9A
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x9A].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9A,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x9B
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x9B].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9B,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x9C
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x9C].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9C,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x9D
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x9D].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9D,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x9E
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x9E].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9E,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0x9F
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0x9F].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0x9F,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA0
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA0].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA0,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA1
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA1].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA1,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA2
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA2].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA2,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA3
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA3].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA3,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA4
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA4].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA5
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA5].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA6
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA6].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA6,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA7
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA7].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA7,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA8
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA8].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA8,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xA9
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xA9].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xA9,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xAA
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xAA].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAA,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xAB
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xAB].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAB,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xAC
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xAC].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAC,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xAD
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xAD].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAD,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xAE
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xAE].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAE,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xAF
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xAF].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xAF,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB0
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB0].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB0,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB1
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB1].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB1,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB2
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB2].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB2,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB3
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB3].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB3,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB4
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB4].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB5
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB5].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB6
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB6].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB6,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB7
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB7].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB7,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB8
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB8].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB8,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xB9
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xB9].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xB9,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xBA
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xBA].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBA,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xBB
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xBB].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBB,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xBC
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xBC].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBC,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xBD
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xBD].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBD,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xBE
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xBE].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBE,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xBF
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xBF].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xBF,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC0
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC0].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC0,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC1
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC1].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC1,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC2
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xC2].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xC2,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xC2].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xC2].InsertVarient(0);
    {
        // 0xC2
        // vcmpph Vx,Hx,Wx,Ib (ev) 
        m_EVEXThreeByteOpCodes_3A[0xC2].m_pVarients[0]->Init(
            /*szName         = */"vcmpph",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC2,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }
    m_EVEXThreeByteOpCodes_3A[0xC2].InsertVarient(3);
    {
        // 0xC2
        // vcmpsh Vx,Hx,Wx,Ib (F3),(ev)
        m_EVEXThreeByteOpCodes_3A[0xC2].m_pVarients[3]->Init(
            /*szName         = */"vcmpsh",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xC2,
            /*nOperands      = */4,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_VXY, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand4       = */Operand_t( OperandMode_I, OperandType_b));
            
    }

    // 0xC3
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC3].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC3,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC4
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC4].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC4,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC5
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC5].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC5,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC6
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC6].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC6,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC7
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC7].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC7,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC8
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC8].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC8,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xC9
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xC9].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xC9,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xCA
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xCA].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCA,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xCB
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xCB].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCB,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xCC
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xCC].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCC,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xCD
    // This opcode does not repesent a valid VEX encodable intruction.
    m_EVEXThreeByteOpCodes_3A[0xCD].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */false,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */false,
        /*iByte          = */0xCD,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    // 0xCE
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xCE].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xCE,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xCE].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xCE].InsertVarient(10);
    {
        // 0xCE
        // vgf2p8affineqb Vx,Wx,Ib (66)
        m_EVEXThreeByteOpCodes_3A[0xCE].m_pVarients[10]->Init(
            /*szName         = */"vgf2p8affineqb",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xCE,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

    // 0xCF
    // Parent instruction. Split type [ Prefix Split]
    m_EVEXThreeByteOpCodes_3A[0xCF].Init(
        /*szName         = */"xx_INVALID_xx",
        /*bValidOpcd     = */true,
        /*bEscapeOpcd    = */false,
        /*bModrmRequired = */true,
        /*iByte          = */0xCF,
        /*nOperands      = */0,
        /*operand1       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand2       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand3       = */Operand_t( OperandMode_Invalid, OperandType_Invalid),
        /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
        
    m_EVEXThreeByteOpCodes_3A[0xCF].InitVarientType(OpCodeDesc_t::VarientType_t::VarientKey_LegacyPrefix);
    m_EVEXThreeByteOpCodes_3A[0xCF].InsertVarient(10);
    {
        // 0xCF
        // vgf2p8affineinvqb Vx,Wx,Ib (66)
        m_EVEXThreeByteOpCodes_3A[0xCF].m_pVarients[10]->Init(
            /*szName         = */"vgf2p8affineinvqb",
            /*bValidOpcd     = */true,
            /*bEscapeOpcd    = */false,
            /*bModrmRequired = */true,
            /*iByte          = */0xCF,
            /*nOperands      = */3,
            /*operand1       = */Operand_t( OperandMode_V, OperandType_x),
            /*operand2       = */Operand_t( OperandMode_W, OperandType_x),
            /*operand3       = */Operand_t( OperandMode_I, OperandType_b),
            /*operand4       = */Operand_t( OperandMode_Invalid, OperandType_Invalid));
            
    }

