/*
 * ssd1306_driver.h
 *
 *  Created on: Jun 15, 2026
 *      Author: sabariyogesh
 */

#ifndef INC_SSD1306_DRIVER_H_
#define INC_SSD1306_DRIVER_H_

/*
 * ssd1306_driver.h
 *
 *  Created on: Jun 14, 2026
 *      Author: sabariyogesh
 */
//
//#ifndef SSD1306_DRIVER_H_
//#define SSD1306_DRIVER_H_

void SSD1306_Init(void);
void SSD1306_Fill(void);
void Set_Cursor(uint8_t page_addr,uint8_t col_start_addr);
void OLED_WriteChar(char x);
void OLED_WriteString(char *str);
void Set_H_Scrolling(uint8_t start_page_addr,uint8_t end_page_addr,uint8_t time_interval);
void Set_Contrast(uint8_t contrast);




//#endif /* SSD1306_DRIVER_H_ */


#endif /* INC_SSD1306_DRIVER_H_ */
