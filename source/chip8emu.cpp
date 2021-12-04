#include <iostream>
#include <string>
#include <fstream>
#include <vector>

uint8_t Vreg[16] = { 0x0 };
uint16_t Ireg = 0x0;
uint16_t Sreg[17] = { 0x0 };
uint16_t* SPointer = Sreg;
uint16_t PC = 0x200;

uint8_t delay_timer = 0x0;
uint8_t sound_timer = 0x0;

bool isExit = false;

bool vram[32][64];
std::string frame;

bool keys[16] = { false };

uint8_t ram[4096] = { 0x00 };

uint8_t font[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
					0x20, 0x60, 0x20, 0x20, 0x70, // 1
					0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
					0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
					0x90, 0x90, 0xF0, 0x10, 0x10, // 4
					0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
					0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
					0xF0, 0x10, 0x20, 0x40, 0x40, // 7
					0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
					0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
					0xF0, 0x90, 0xF0, 0x90, 0x90, // A
					0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
					0xF0, 0x80, 0x80, 0x80, 0xF0, // C
					0xE0, 0x90, 0x90, 0x90, 0xE0, // D
					0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
					0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F 


uint8_t rom[] = { 0x00, /*0xe0, 0xa2, 0x48, 0x60, 0x00, 0x61, 0x1e, 0x62, 0x00, 0xd2, 0x02, 0xd2, 0x12, 0x72, 0x08,
	0x32, 0x40, 0x12, 0x0a, 0x60, 0x00, 0x61, 0x3e, 0x62, 0x02, 0xa2, 0x4a, 0xd0, 0x2e, 0xd1, 0x2e,
	0x72, 0x0e, 0xd0, 0x2e, 0xd1, 0x2e, 0xa2, 0x58, 0x60, 0x0b, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x0a,
	0xa2, 0x67, 0xd0, 0x1f, 0x70, 0x0a, 0xa2, 0x76, 0xd0, 0x1f, 0x70, 0x03, 0xa2, 0x85, 0xd0, 0x1f,
	0x70, 0x0a, 0xa2, 0x94, 0xd0, 0x1f, 0x12, 0x46, 0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81,
	0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x80, 0x80, 0x80,
	0x80, 0x80, 0x80, 0x80, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81, 0x81, 0x81,
	0x81, 0x81, 0xff, 0xff*/ };

void print_state() {
	for (int i = 0; i <= 0xF; i++) {
		printf("%x\n", Vreg[i]);
	}
	printf("END\n");
}
void init() {
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			vram[i][j] = false;
		}
	}

	for (int i = 0; i < 80; i++) {
		ram[0xFF + i] = font[i];
	}
	
	char temp;
	int i = 0;
	//const char* filename = "2.ch8";
	
	/*std::ifstream romFile(filename, std::ios_base::in);

	while (romFile) {
		romFile >> temp;
		std::cout << temp;
		memory[0x200 + i] = temp;
		i++;
	}*/

	// Файл для чтения
	std::ifstream in("6", std::ios::binary);
	// Создаем вектор с соответствующим размером (запрашиваем его через
	// выставление указателя на конец файла)
	std::vector<unsigned char> v(in.seekg(0, std::ios::end).tellg());
	// Возвращаем указатель на место
	in.seekg(0, std::ios::beg);
	// Читаем файл в вектор
	in.read((char*)v.data(), v.size());

	for (int i = 0; i < v.size(); i++) {
		ram[0x200 + i] = v[i];
	}
	/*for (int i = 0; i < 164; i++) {
		memory[0x200 + i] = rom[i];
	}*/
}

uint16_t getNext() {
	uint16_t currentOpcode = ram[PC];
	currentOpcode <<= 8;
	PC++;
	currentOpcode |= ram[PC];
	PC++;
	//printf("%x : %x\n", PC, currentOpcode);
	return currentOpcode;
}

void executeNext(uint16_t instruction) {
	//print_state();
	switch (instruction & 0xF000)
	{
		case 0x0000:
			switch (instruction & 0x00FF)
			{
				case 0x00E0:
					for (int i = 0; i < 32; i++) {
						for (int j = 0; j < 64; j++) {
							vram[i][j] = false;
						}
					}
					break;
				case 0x00EE:
					PC = *SPointer;
					SPointer--;
			}
			break;
		case 0x1000:
			PC = (instruction & 0x0FFF);
			break;
		case 0x2000:
			if (*(SPointer++) == 0xFFFF) {
				SPointer--;
				isExit = true;
				return;
			}
			*SPointer = PC++;
			PC = (instruction & 0x0FFF);
			break;
		case 0x3000:
			if (Vreg[(instruction & 0x0F00) >> 8] == (instruction & 0x00FF)) {
				getNext();
			}
			break;
		case 0x4000:
			if (Vreg[(instruction & 0x0F00) >> 8] != (instruction & 0x00FF)) {
				getNext();
			}
			break;
		case 0x5000:
			if (Vreg[(instruction & 0x0F00) >> 8] == Vreg[(instruction & 0x00F0) >> 4]) {
				getNext();
			}
			break;
		case 0x6000:
			Vreg[(instruction & 0x0F00) >> 8] = (instruction & 0x00FF);
			break;
		case 0x7000:
			Vreg[(instruction & 0x0F00) >> 8] += (instruction & 0x00FF);
			break;
		case 0x8000:
			switch (instruction & 0x000F)
			{
				case 0x0:
					Vreg[(instruction & 0x0F00) >> 8] = Vreg[(instruction & 0x00F0) >> 4];
					break;
				case 0x1:
					Vreg[(instruction & 0x0F00) >> 8] |= Vreg[(instruction & 0x00F0) >> 4];
					break;
				case 0x2:
					Vreg[(instruction & 0x0F00) >> 8] &= Vreg[(instruction & 0x00F0) >> 4];
					break;
				case 0x3:
					Vreg[(instruction & 0x0F00) >> 8] ^= Vreg[(instruction & 0x00F0) >> 4];
					break;
				case 0x4:
					Vreg[0xF] = (Vreg[(instruction & 0x0F00) >> 8] + Vreg[(instruction & 0x00F0) >> 4] > 0xFF ? 1 : 0);
					Vreg[(instruction & 0x0F00) >> 8] = (uint16_t)Vreg[(instruction & 0x0F00) >> 8] + (uint16_t)Vreg[(instruction & 0x00F0) >> 4];
					break;
				case 0x5:
					Vreg[0xF] = (Vreg[(instruction & 0x0F00) >> 8] >= Vreg[(instruction & 0x00F0) >> 4] ? 1 : 0);
					Vreg[(instruction & 0x0F00) >> 8] = (uint16_t)Vreg[(instruction & 0x0F00) >> 8] - (uint16_t)Vreg[(instruction & 0x00F0) >> 4];
					break;
				case 0x6:
					Vreg[0xF] = (Vreg[(instruction & 0x0F00) >> 8] & 0x01);
					Vreg[(instruction & 0x0F00) >> 8] >>= 1;
					break;
				case 0x7:
					Vreg[0xF] = (Vreg[(instruction & 0x00F0) >> 4] >= Vreg[(instruction & 0x0F00) >> 8] ? 1 : 0);
					Vreg[(instruction & 0x00F0) >> 4] = (uint16_t)Vreg[(instruction & 0x00F0) >> 4] - (uint16_t)Vreg[(instruction & 0x0F00) >> 8];
					break;
				case 0xE:
					Vreg[0xF] = (Vreg[(instruction & 0x0F00) >> 8] & 0x80) >> 7;
					Vreg[(instruction & 0x0F00) >> 8] <<= 1;
			}
			break;
		case 0x9000:
			if (Vreg[(instruction & 0x0F00) >> 8] != Vreg[(instruction & 0x00F0) >> 4]) {
				getNext();
			}
			break;
		case 0xA000:
			Ireg = ram[(instruction & 0x0FFF)];
			break;
		case 0xB000:
			PC = (instruction & 0x0FFF) + Vreg[0];
			break;
		case 0xC000:
			Vreg[(instruction & 0x0F00) >> 8] = (uint8_t)(rand() % 0x100) & (instruction & 0x00FF);
			break;
		case 0xD000:
			for (int i = 0; i < (instruction & 0x000F); i++) {
				for (int j = 0; j < 0x8; j++) {
					if ((vram[Vreg[((instruction & 0x00F0) >> 4)] + i][Vreg[((instruction & 0x0F00) >> 8)] + j]) && (((ram[Ireg + i] >> (7 - j)) & 1) == 0)) {
						Vreg[0xF] = 1;
					}

					vram[Vreg[((instruction & 0x00F0) >> 4)] + i][Vreg[((instruction & 0x0F00) >> 8)] + j] = (ram[Ireg + i] >> (7 - j)) & 1;
				}
			}
			break;
		case 0xE000:
			switch (instruction & 0x00FF)
			{
				case 0x009E:
					break;
				case 0x00A1:
					break;
			}
			break;
		case 0xF000:
			switch (instruction & 0x00FF)
			{
				case 0x0007:
					Vreg[(instruction & 0x00FF) >> 8] = sound_timer;
					break;
				case 0x000A:
					while (isExit) {
						//updateKeys();
						for (int i = 0; i < 0xF; i++) {
							if (keys[i]) {
								Vreg[(instruction & 0x0F00) >> 8] = i;
								isExit = true;
							}
						}
					}
					isExit = false;
					break;
				case 0x0015:
					delay_timer = Vreg[(instruction & 0x0F00) >> 8];
					break;
				case 0x0018:
					sound_timer = Vreg[(instruction & 0x0F00) >> 8];
					break;
				case 0x001E:
					Ireg += Vreg[(instruction & 0x0F00) >> 8];
					break;
				case 0x0029:
					break;
				case 0x0033:
					break;
				case 0x0055:
					for (int i = 0; i <= ((instruction & 0x0F00) >> 8); i++) {
						 ram[Ireg + i] = Vreg[i];
					}
					break;
				case 0x0065:
					for (int i = 0; i <= ((instruction & 0x0F00) >> 8); i++) {
						Vreg[i] = ram[Ireg + i];
					}
			}
	}
}

void decreaseTimers() {
	if (delay_timer) {
		delay_timer--;
	}
	if (sound_timer) {
		sound_timer--;
		printf("\a");
	}
}

void drawDisplay() {
	

	std::string nextFrame;
	
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 64; j++) {
			char printed_char = (vram[i][j]) ? 0xB2 : 0x20;
			nextFrame += printed_char;
		}
		nextFrame += "\n";
	}

	for (int i = 0; i < 32*64 ; i++) {
		if (frame[i] != nextFrame[i]) {
			frame.erase();
			frame = nextFrame;
			break;
		}
		if (i == (32*64 - 1)) {
			return;
		}
	}

	system("cls");
	
	printf("%s", frame.data());
	printf("%d : %x\n", PC, rom[PC]);
}

int main() {
	init();

	while (!isExit) {
		decreaseTimers();
		executeNext(getNext());
		drawDisplay();
	}

	return 0;
}