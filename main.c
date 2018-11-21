#include <REGX51.H>
#include <INTRINS.H>
// 行扫描
sbit shcp_h=P2^2;
sbit ds_h=P2^1;
sbit shcp=P2^0;
// 列扫描
sbit shcp_l=P2^3;
sbit ds_l=P2^4;

code int tab[64] = {
/*--  Fixedsys12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x10,0x20,0x10,0x20,0x11,0xFC,0x10,0x20,0xFC,0x20,0x24,0x20,0x27,0xFE,0x24,0x00,
0x24,0x20,0x48,0x20,0x29,0xFC,0x10,0x20,0x28,0x20,0x44,0x20,0x83,0xFE,0x00,0x00,
/*--  文字:  哈  --*/
/*--  Fixedsys12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x40,0x00,0x40,0x78,0xA0,0x49,0x10,0x4A,0x08,0x4C,0x06,0x4B,0xF8,0x48,0x00,
0x48,0x00,0x4B,0xF8,0x7A,0x08,0x4A,0x08,0x02,0x08,0x02,0x08,0x03,0xF8,0x02,0x08,
};

/**
**********************************************
***获取数组内元素的相对位置上的bit值**********
***clo：扫描列；row：扫描行（从最左面开始扫描）
***arr：需要取值的数组************************
**********************************************
*/
int get_bit(int col, int row, int *arr) {
	// 数组内到第几个元素
	int arrIndex;
	// 元素到第几位
	int valueBit;
	int cur, ret;
	int rowIndex, colIndex;
    // 默认从0开始，因为行列一般从1开始，所以需要转换，原值减去1
    col = col - 1;
    row = row - 1;
	
	rowIndex = row % 16;
	colIndex = col / 8;
	// 如果colIndex大于1，也就是大于 2 * 8,已经换下一个字了所以需要特殊处理
	arrIndex = 2 * rowIndex + colIndex / 2 * 32 + colIndex % 2;
	valueBit = col % 8;
	// cur = arr[arrIndex + 2 * col];
	cur = arr[arrIndex];

	// 把需要到位移动到最右面，然后只取最右面到一位数
	ret = cur >> (8 - valueBit -1);
	ret = ret & 0x01;
	return ret;

}



/**
**********************************************
***获取一行的扫描数据**********
**********************************************
*/


/*
 * 返回位无符号的long，最多为4个字节，即为：4 * 8
 * 
 * 
 */
unsigned long get_row(int col_offset, int row, int *arr, int arrSize){
	unsigned long ret = 0;
	// 显示的LED的长度，三个LED，每个8位
	int ledLen = 3 * 8;
	int arrLen = arrSize * 8;
	int col_scan = 1;
	int col_start, col_end;
	int i;
	if(col_offset <= ledLen) {
		col_start = 1;
	} else {
		col_start = col_offset - ledLen;
	}
	col_end = col_offset + 1;
	col_scan = col_start;

	for(i = col_start; i < col_end; i++) {
		
		// 如果扫描的行大于数组的长度，则重新扫描
		if (i > arrLen && col_scan == 1)
		{
			col_scan = 0;
		}
		// 先整体右移 
		ret = ret << 1;
		// 如果当前位有值则最后为置1
		
		if (get_bit(col_scan, row, arr))
		{
			ret = ret | 0x01;
		}
		col_scan ++;
	}
	return ret;
}


// 行扫描函数
void sendbyte_h(int index){
	int i;
	for(i = 0; i < 16; i++) {
		shcp_h = 0;
		if ((16 - index) == i)
		{
			ds_h = 0;
		} else {
			ds_h = 1;
		}
		shcp_h = 1;
	}
	
}

// 列扫描函数
void sendbyte_l(unsigned long show_arr){
	int j;
	for(j = 0; j < 24; j++) {
		shcp_l = 0;
		ds_l = show_arr & 0x01;
		show_arr = show_arr >> 1;
		shcp_l = 1;
	}
	

}
void show(){
	shcp = 0;
	_nop_();
	shcp = 1;
}

void main() {
	int i, j;
	unsigned long show_arr;
	int size = sizeof(tab) / sizeof(tab[0]); 
	int delay = 500;
	while(1){
		// 行移动，一共有3 * 8个点
		for (j = 0;j < 24; j++ )
		{
			// 列循环，一帧显示数据
			for(i = 0; i < 16; i++) {
				show_arr = get_row(j, i, tab, size);
				sendbyte_l(show_arr);
				sendbyte_h(i + 1);
				show();
			}
		}
	}
}
	
