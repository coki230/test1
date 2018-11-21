#include <REGX51.H>
#include <INTRINS.H>
// ��ɨ��
sbit shcp_h=P2^2;
sbit ds_h=P2^1;
sbit shcp=P2^0;
// ��ɨ��
sbit shcp_l=P2^3;
sbit ds_l=P2^4;

code int tab[64] = {
/*--  Fixedsys12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x10,0x20,0x10,0x20,0x11,0xFC,0x10,0x20,0xFC,0x20,0x24,0x20,0x27,0xFE,0x24,0x00,
0x24,0x20,0x48,0x20,0x29,0xFC,0x10,0x20,0x28,0x20,0x44,0x20,0x83,0xFE,0x00,0x00,
/*--  ����:  ��  --*/
/*--  Fixedsys12;  �������¶�Ӧ�ĵ���Ϊ����x��=16x16   --*/
0x00,0x40,0x00,0x40,0x78,0xA0,0x49,0x10,0x4A,0x08,0x4C,0x06,0x4B,0xF8,0x48,0x00,
0x48,0x00,0x4B,0xF8,0x7A,0x08,0x4A,0x08,0x02,0x08,0x02,0x08,0x03,0xF8,0x02,0x08,
};

/**
**********************************************
***��ȡ������Ԫ�ص����λ���ϵ�bitֵ**********
***clo��ɨ���У�row��ɨ���У��������濪ʼɨ�裩
***arr����Ҫȡֵ������************************
**********************************************
*/
int get_bit(int col, int row, int *arr) {
	// �����ڵ��ڼ���Ԫ��
	int arrIndex;
	// Ԫ�ص��ڼ�λ
	int valueBit;
	int cur, ret;
	int rowIndex, colIndex;
    // Ĭ�ϴ�0��ʼ����Ϊ����һ���1��ʼ��������Ҫת����ԭֵ��ȥ1
    col = col - 1;
    row = row - 1;
	
	rowIndex = row % 16;
	colIndex = col / 8;
	// ���colIndex����1��Ҳ���Ǵ��� 2 * 8,�Ѿ�����һ������������Ҫ���⴦��
	arrIndex = 2 * rowIndex + colIndex / 2 * 32 + colIndex % 2;
	valueBit = col % 8;
	// cur = arr[arrIndex + 2 * col];
	cur = arr[arrIndex];

	// ����Ҫ��λ�ƶ��������棬Ȼ��ֻȡ�����浽һλ��
	ret = cur >> (8 - valueBit -1);
	ret = ret & 0x01;
	return ret;

}



/**
**********************************************
***��ȡһ�е�ɨ������**********
**********************************************
*/


/*
 * ����λ�޷��ŵ�long�����Ϊ4���ֽڣ���Ϊ��4 * 8
 * 
 * 
 */
unsigned long get_row(int col_offset, int row, int *arr, int arrSize){
	unsigned long ret = 0;
	// ��ʾ��LED�ĳ��ȣ�����LED��ÿ��8λ
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
		
		// ���ɨ����д�������ĳ��ȣ�������ɨ��
		if (i > arrLen && col_scan == 1)
		{
			col_scan = 0;
		}
		// ���������� 
		ret = ret << 1;
		// �����ǰλ��ֵ�����Ϊ��1
		
		if (get_bit(col_scan, row, arr))
		{
			ret = ret | 0x01;
		}
		col_scan ++;
	}
	return ret;
}


// ��ɨ�躯��
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

// ��ɨ�躯��
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
		// ���ƶ���һ����3 * 8����
		for (j = 0;j < 24; j++ )
		{
			// ��ѭ����һ֡��ʾ����
			for(i = 0; i < 16; i++) {
				show_arr = get_row(j, i, tab, size);
				sendbyte_l(show_arr);
				sendbyte_h(i + 1);
				show();
			}
		}
	}
}
	
