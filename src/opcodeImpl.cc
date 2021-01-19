#include "chip8.hh"


void chip8::initFunctionsTable() {
    opcodeTable[0] = &chip8::tableOc0;
    ocTable0[0] = &chip8::oc00E0;
    ocTable0[0xE] = &chip8::oc00EE;

    opcodeTable[0x1] = &chip8::oc1NNN;
    opcodeTable[0x2] = &chip8::oc2NNN;
    opcodeTable[0x3] = &chip8::oc3XKK;
    opcodeTable[0x4] = &chip8::oc4XKK;
    opcodeTable[0x5] = &chip8::oc5XY0;
    opcodeTable[0x6] = &chip8::oc6XKK;
    opcodeTable[0x7] = &chip8::oc7XKK;

    opcodeTable[0x8] = &chip8::tableOc8;
    ocTable8[0x0] = &chip8::oc8XY0;
    ocTable8[0x1] = &chip8::oc8XY1;
    ocTable8[0x2] = &chip8::oc8XY2;
    ocTable8[0x3] = &chip8::oc8XY3;
    ocTable8[0x4] = &chip8::oc8XY4;
    ocTable8[0x5] = &chip8::oc8XY5;
    ocTable8[0x6] = &chip8::oc8XY6;
    ocTable8[0x7] = &chip8::oc8XY7;
    ocTable8[0xE] = &chip8::oc8XY7;

    opcodeTable[0x9] = &chip8::oc9XY0;
    opcodeTable[0xA] = &chip8::ocANNN;
    opcodeTable[0xB] = &chip8::ocBNNN;
    opcodeTable[0xC] = &chip8::ocCXKK;
    opcodeTable[0xD] = &chip8::ocDXYN;

    opcodeTable[0xE] = &chip8::tableOcE;
    ocTableE[0x1] = &chip8::ocEXA1;
    ocTableE[0xE] = &chip8::ocEX9E;

    opcodeTable[0xF] = &chip8::tableOcF;
    ocTableF[0x7] = &chip8::ocFX07;
    ocTableF[0xA] = &chip8::ocFX07;
    ocTableF[0x15] = &chip8::ocFX07;
    ocTableF[0x18] = &chip8::ocFX07;
    ocTableF[0x1E] = &chip8::ocFX07;
    ocTableF[0x29] = &chip8::ocFX07;
    ocTableF[0x33] = &chip8::ocFX07;
    ocTableF[0x55] = &chip8::ocFX07;
    ocTableF[0x65] = &chip8::ocFX07;

}

void chip8::tableOc0() {
    
    (this->*ocTable0[getN(opcode)])();
}

void chip8::tableOc8() {
    (this->*ocTable8[getN(opcode)])();
}

void chip8::tableOcE() {
    (this->*ocTableE[getN(opcode)])();
}

void chip8::tableOcF() {
    (this->*ocTableF[getKK(opcode)])();
}


void chip8::oc00E0(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " CLS";
        return;
    }
    memset(display, 0, sizeof(display[0]) * DISPLAY_SIZE);
}

void chip8::oc00EE(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " RET";
        return;
    }
    pc = stack[--sp];
}

void chip8::oc1NNN(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " JP   $" << std::hex << getNNN(opcode);
        return;
    }
    pc = getNNN(opcode);
}   

void chip8::oc2NNN(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " CALL $" << std::hex << getNNN(opcode); 
        return;
    }
    
    stack[sp++] = pc;
    pc = getNNN(opcode);
}

void chip8::oc3XKK(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SE   V" << std::hex << getX(opcode) << ", $" << getKK(opcode);
        return;
    }
    if(registers[getX(opcode)] == getKK(opcode)) pc+=2;    
}

void chip8::oc4XKK(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SNE  V" << std::hex << getX(opcode) << ", $" << getKK(opcode);
        return;
    }
    if(registers[getX(opcode)] != getKK(opcode)) pc+=2;
}

void chip8::oc5XY0(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SE   V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    if(registers[getX(opcode)] == registers[getY(opcode)]) pc+=2;
}

void chip8::oc6XKK(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   V" << std::hex << getX(opcode) << ", $" << getKK(opcode);
        return;
    }
    registers[getX(opcode)] = getKK(opcode);
}

void chip8::oc7XKK(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " ADD  V" << std::hex << getX(opcode) << ", $" << getKK(opcode);
        return;
    }
    registers[getX(opcode)] += getKK(opcode);
}


void chip8::oc8XY0(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    registers[getX(opcode)] = registers[getY(opcode)];
}

void chip8::oc8XY1(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " OR  V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    registers[getX(opcode)] |= registers[getY(opcode)];
}

void chip8::oc8XY2(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " AND V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    registers[getX(opcode)] &= registers[getY(opcode)];
}

void chip8::oc8XY3(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " XOR V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    registers[getX(opcode)] ^= registers[getY(opcode)];
}

void chip8::oc8XY4(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " ADD V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    uint8_t x = registers[getX(opcode)], y = registers[getY(opcode)];
    if((x + y) > 255) registers[0xF] = 1;
    else registers[0xF] = 0;
    registers[getX(opcode)] = ((x+y) & 0xFFu);
}

void chip8::oc8XY5(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SUB V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    uint8_t x = registers[getX(opcode)], y = registers[getY(opcode)];
    if(x > y) registers[0xF] = 1;
    else registers[0xF] = 0;
    registers[getX(opcode)] -= registers[getY(opcode)];
}

// according to : https://github.com/mattmikolay/chip-8/issues/4
void chip8::oc8XY6() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SHR V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    registers[0xF] = (registers[getY(opcode)] & 0x1);
    registers[getX(opcode)] = registers[getY(opcode)] >> 1;
}

void chip8::oc8XY7() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SUBN V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    uint8_t x = registers[getX(opcode)], y = registers[getY(opcode)];
    if(y > x)registers[0xF] = 1;
    else registers[0xF] = 0;
    registers[getX(opcode)] = y - x;;
}

void chip8::oc8XYE() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SHL V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    uint8_t x = getX(opcode);
    registers[0xF] = (registers[x] & 0x80u) >> 7u;
    registers[x] = registers[getY(opcode)] << 1; 

}

void chip8::oc9XY0() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SNE V" << std::hex << getX(opcode) << ", V" << getY(opcode);
        return;
    }
    if(registers[getX(opcode)] != registers[getY(opcode)]) pc +=2;
}

void chip8::ocANNN() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   I" << std::hex << ", $" << getNNN(opcode); 
        return;
    }

    I = getNNN(opcode);
}

void chip8::ocBNNN() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " JP  V0" << std::hex << ", $" << getNNN(opcode); 
        return;
    }
    pc = registers[0] + getNNN(opcode);
}

void chip8::ocCXKK() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " RND  V" << std::hex << getX(opcode) << ", $" << getKK(opcode); 
        return;
    }
    // TODO : Check this opcode, seems weird to do it this way
    std::random_device engine;
    registers[getX(opcode)] = (rand()%0xFFu) & getKK(opcode);
}

void chip8::ocDXYN() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " DRW  V" << std::hex << getX(opcode) << ", V" << getY(opcode) << ", $" << getN(opcode); 
        return;
    }
	uint8_t Vx = getX(opcode);
	uint8_t Vy = getY(opcode);
	uint8_t height = getN(opcode);

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % DISPLAY_WIDTH;
	uint8_t yPos = registers[Vy] % DISPLAY_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = mem[I + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &display[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void chip8::ocEX9E() {
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SKP  V" << std::hex << getX(opcode);
        return;
    }
    if(keyboard[registers[getX(opcode)]]) pc+=2;
}

void chip8::ocEXA1(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SKNP V" << std::hex << getX(opcode);
        return;
    }

    if(!keyboard[registers[getX(opcode)]]) pc+=2;
}

void chip8::ocFX07(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   V" << std::hex << getX(opcode) << ", DT";
        return;
    }
    registers[getX(opcode)] = delayTimer;
}

void chip8::ocFX0A(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " SKP  V" << std::hex << getX(opcode) << ", K";
        return;
    }
    for(int i = 0; i < 0xF; i++){
        if(keyboard[i]) {
            registers[getX(opcode)] = i;
            return;
        }
    }
    pc-=2;
}

void chip8::ocFX15(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   DT, V" << std::hex << getX(opcode);
        return;
    }
    delayTimer = registers[getX(opcode)];
}

void chip8::ocFX18(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   ST, V" << std::hex << getX(opcode);
        return;
    }
    soundTimer = registers[getX(opcode)];
}

void chip8::ocFX1E(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " ADD  I, V" << std::hex << getX(opcode);
        return;
    }
    I += registers[getX(opcode)];
}

void chip8::ocFX29(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD    F, V" << std::hex << getX(opcode);
        return;
    }
    I = FONT_START_ADDR + (registers[getX(opcode)] * 5);
}

void chip8::ocFX33(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   B, V" << std::hex << getX(opcode);
        return;
    }
    	// Ones-place
    // uint8_t v = registers[getX(opcode)];
	// mem[I + 2] = v % 10;
	// v /= 10;

	// // Tens-place
	// mem[I + 1] = v % 10;
	// v /= 10;

	// // Hundreds-place
	// mem[I] = v % 10;
    uint8_t v = registers[getX(opcode)];
    mem[I]     = v/ 100;
    mem[I + 1] = (v / 10) % 10;
    mem[I + 2] = (v % 100) % 10;	
}

void chip8::ocFX55(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD  [I], V" << std::hex << getX(opcode);
        return;
    }
    for(int i = 0; i < getX(opcode) + 1; ++i ) {
        mem[I + i] = registers[i];
    }
    
}

void chip8::ocFX65(){
    if(disassF) {
        disassFile << "$" << std::hex << (pc & 0xFFF) << " LD   V" << std::hex << getX(opcode) << ", [I]";
        return;
    }
    for(int i = 0; i <= getX(opcode); ++i ) {
        registers[i] = mem[I + i];
    }
}