
// enum KEY_VK {
// 	VK_LEFT = 0,
// 	VK_RIGHT = 10
// };

uint8_t keyboard[8] {0,0,0,0,0,0,0,0};
bool key[16] {false};               // 锟斤拷一锟斤拷扫锟借被锟斤拷锟铰的帮拷锟斤拷
bool key_pressed_flag[16] {false};  // 锟斤拷锟轿帮拷锟斤拷

void key_input(void) {
	// 0x1
	key[0] = ege::keystate('Q');
	key[1] = ege::keystate('W');
	key[2] = ege::keystate('E');
	key[3] = ege::keystate('R');
	key[4] = ege::keystate('T');
	// 0x2
	key[5] = ege::keystate('A');
	key[6] = ege::keystate('S');
	key[7] = ege::keystate('D');
	key[8] = ege::keystate('F');
	key[9] = ege::keystate('G');
	// 0x4
	key[10] = ege::keystate('Z');
	key[11] = ege::keystate('X');
	key[12] = ege::keystate('C');
	key[13] = ege::keystate('V');
	key[14] = ege::keystate('B');

	for (int i = 0; i < 15; i++) {
		if (key[i] == false) {
			key_pressed_flag[i] = false;
		}
	}
}



bool key_pressed_func (int id_of_key, bool _pressed_key_ = true) {
	if (key[id_of_key] && !key_pressed_flag[id_of_key]) {
		key_pressed_flag[id_of_key] = _pressed_key_;
		return true;
	} else {
		return false;
	}
}