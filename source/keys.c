#include<stdbool.h>

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

#define MEM_IO   0x04000000
#define MEM_PAL  0x05000000
#define MEM_VRAM 0x06000000

#define REG_DISPCNT        (*((volatile u32 *)(MEM_IO)))
#define REG_DISPLAY_VCOUNT (*((volatile u32 *)(MEM_IO + 0x0006)))
#define REG_KEYINPUT       (*((volatile u32 *)(MEM_IO + 0x0130)))

#define KEY_DOWN_NOW(key)  (~(REG_KEYINPUT) & key)
#define KEY_PRESSED(key_curr, key_prev, key)  (key == key_curr && key != key_prev)

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_SELECT   0x0004
#define KEY_START    0x0008
#define KEY_RIGHT    0x0010
#define KEY_LEFT     0x0020
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080
#define KEY_R        0x0100
#define KEY_L        0x0200

#define KEY_MASK     0x03FF

#define CLR_RED      0x001F
#define CLR_GREEN    0x03E0
#define CLR_BLUE     0x7C00
#define CLR_PURPLE   0x7C1F
#define CLR_YELLOW   0x03FF
#define CLR_CYAN	 0x7FE0
#define CLR_WHITE    0x7FFF

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

static inline int clamp(int value, int min, int max)
{
	return (value < min ? min
	                    : (value > max ? max : value));
}

int main(){
	REG_DISPCNT = 0x0403; //bitmap mode 3, 16bpp, and bg2 enabled 

	volatile u16 *vram = (u16 *)(MEM_VRAM);

	int x = 120;
	int y = 80;

	u16 colors[] = {CLR_RED, CLR_GREEN, CLR_BLUE, CLR_PURPLE, CLR_YELLOW, CLR_CYAN, CLR_WHITE};

	int colorindex = 0;

	u16 old_color = 0;

	u16 key_curr = 0;
	u16 key_prev = 0;

	while(1){

		// Skip past the rest of any current V-Blank, then skip past the V-Draw

		while(REG_DISPLAY_VCOUNT >= 160);
		while(REG_DISPLAY_VCOUNT <  160);

		//get the currently pressed key and the key that was pressed in the last frame

		key_prev = key_curr;
		key_curr = ~(REG_KEYINPUT) & KEY_MASK;

		//drawing

		if(KEY_DOWN_NOW(KEY_A)){
			vram[y*SCREEN_WIDTH + x] = colors[colorindex];
		}else if(KEY_DOWN_NOW(KEY_B)){
			vram[y*SCREEN_WIDTH + x] = 0;
		}else {
			vram[y*SCREEN_WIDTH + x] = old_color;
		}

		if(KEY_DOWN_NOW(KEY_LEFT)){
			x--;
		}else if(KEY_DOWN_NOW(KEY_RIGHT)){
			x++;
		}else if(KEY_DOWN_NOW(KEY_UP)){
			y--;
		}else if(KEY_DOWN_NOW(KEY_DOWN)){
			y++;
		}

		x = clamp(x, 0, SCREEN_WIDTH - 1);
		y = clamp(y, 0, SCREEN_HEIGHT - 1);
		colorindex = clamp(colorindex, 0, 6);

		old_color = vram[y*SCREEN_WIDTH + x];
		vram[y*SCREEN_WIDTH + x] = colors[colorindex];

		//changing color

		if(KEY_PRESSED(key_curr, key_prev, KEY_R)){
			colorindex++;
		}else if (KEY_PRESSED(key_curr, key_prev, KEY_L)){
			colorindex--;
		}

	}

	return 0;
}
