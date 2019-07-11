/*
 * ExSuppFun.h
 *
 *  Created on: 11 июл. 2019 г.
 *      Author: BigBrother
 */

#ifndef EXFUNLIB_INC_EXSUPPFUN_H_
#define EXFUNLIB_INC_EXSUPPFUN_H_

#define FALSE 0
#define TRUE !FALSE

static inline uint8_t* Dec_Convert(uint8_t* buf, uint32_t value) {
                //0123456789
	int divider = 1000000000;
	unsigned char bNZflag=FALSE, minus=FALSE;		//  Флаги левых нулей и минуса
	unsigned char current_digit;

	if (value < 0) {		//    Если число value отрицательное
		minus=TRUE;
		value = -value;
	}
	while (divider) {
		current_digit = value / divider;
		if (current_digit || bNZflag) { //  Как только получили ненулевую цифру,
		  	if (minus) { 	      //  Если число отрицательное, то поставим -
		    	buf--;
				*buf++ = '-';
				minus=FALSE;
		  	}
			value %= divider;
			*buf++ = current_digit + '0';
			bNZflag = TRUE;				// это значит, что левые нули закончились
		} else {  			//  Вместо левых нулей - пробелы, чтобы выровнять вправо
		    *buf++ = ' ';
		}
		divider /= 10;
    }
	if (!bNZflag)
		*buf++ = '0';
	*buf = 0;				//  Это нуль-терминатор (признак окончания строки)
  return buf;
}


#endif /* EXFUNLIB_INC_EXSUPPFUN_H_ */
