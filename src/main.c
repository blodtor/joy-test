// основная библиотека SGDK
#include <genesis.h>
// подключаем библиотеку сгенерированную SGDK из resources.res
#include "resources.h"

/**
 * port - номер порта, нужно передовать ниже указанные константы
 *   PORT_1 = port 1
 *   PORT_2 = port 2
 * x - координата где выводить текст по X
 * y - координата где выводить текст по Y
 */
void drawJoyPortType(u16 port, u16 x, u16 y) {

	// фраза которую напишем перед типом порта
	char port_str[8] = "PortX:";

	// меняем X на реальный номер порта
	port_str[4] = '0' + port;

    // выводим на экран "Port0:" или "Port1:"
	VDP_drawText(port_str , x, y);

	// меняем позицию с которой надо выводить текст
	x += 7;

	switch(JOY_getPortType(port)) {
		case PORT_TYPE_MENACER:		VDP_drawText("Sega Menacer", x, y);	break;
		case PORT_TYPE_JUSTIFIER:	VDP_drawText("Konami Justifier", x, y);	break;
		case PORT_TYPE_MOUSE:		VDP_drawText("Sega MegaMouse", x, y); break;
		case PORT_TYPE_TEAMPLAYER:	VDP_drawText("Sega TeamPlayer", x, y); break;
		case PORT_TYPE_PAD:			VDP_drawText("Sega joypad", x, y); break;
		case PORT_TYPE_EA4WAYPLAY:	VDP_drawText("EA 4-Way Play", x, y); break;
		case PORT_TYPE_UNKNOWN:
		default:					VDP_drawText("Unidentified or no peripheral", x, y);
	}
}

/**
 *
 */
void drawJoypadType(u16 joy, u16 x, u16 y) {

	// фраза которую напишем перед типом контроллера
	char joy_str[6] = "JoyX:";

	// меняем X на реальный номер порта
	joy_str[3] = '0' + joy;

    // выводим на экран "Joy0:" или "Joy1:"
	VDP_drawText(joy_str , x, y);

	x+= 6;

	switch(JOY_getJoypadType(joy)) {
		 case JOY_TYPE_PAD3:        VDP_drawText("3 buttons joypad", x, y); break;
		 case JOY_TYPE_PAD6:        VDP_drawText("6 buttons joypad", x, y); break;
		 case JOY_TYPE_MOUSE:       VDP_drawText("Sega Mouse", x, y); break;
		 case JOY_TYPE_TRACKBALL:   VDP_drawText("Sega trackball", x, y); break;
		 case JOY_TYPE_MENACER:     VDP_drawText("Sega Menacer gun", x, y); break;
		 case JOY_TYPE_JUSTIFIER:   VDP_drawText("Sega Justifier gun", x, y); break;
		 case JOY_TYPE_UNKNOWN:
		 default :     				VDP_drawText("Unknow adaptater or not connected", x, y);
	}
}

// точка входа в программу (игру)
int main(bool hardReset) {
	// инициализируем спрайтовый движок (выделяем место в VRAM под спрайты)
	SPR_init();

	// устанавливаем первой палитре цвета из палитры в файле rapuncel.png
	// 1 параметр - номер палитры (PAL0, PAL1, PAL2, PAL3)
	// 2 параметр - палитра из объекта изображения, имя которого мы указали в resources.res
	// 3 параметр - способ передачи (CPU, DMA, DMA_QUEUE, DMA_QUEUE_COPY)
	PAL_setPalette(PAL3, rapuncel.palette->data, DMA);

	// добавляем спрайт Рапунцель на экран
	// 1 параметр ссылка на ресурс из resources.res
	// 2 параметр координата x
	// 3 параметр координата y
	// 4 параметр атрибуты (TILE_ATTR)
	// возвращает указатель на struct Sprite который используем далее в коде
	Sprite *rapuncelSprite = SPR_addSprite(&rapuncel, 80, 80, TILE_ATTR(
			PAL3 // палитра
			,0// приоритет спрайта (спрайт с меньшим числом, будет перекрывать спрайт с большим)
			,FALSE// перевернуть по вертикали
			,FALSE// перевернуть по горизонтали
	));

	// выводим на экран "Joy TEST"
	// 1 параметр - строка которую хотим вывести на экран
	// 2 и 3 параметр - x и y на тайловой сетке, где выводить текст
	VDP_drawText("JOY TEST", 16, 5);

	drawJoyPortType(PORT_1, 0, 19);
	drawJoyPortType(PORT_2, 0, 20);

	drawJoypadType(JOY_1, 0, 22);
	drawJoypadType(JOY_2, 0, 23);
	drawJoypadType(JOY_3, 0, 24);
	drawJoypadType(JOY_4, 0, 25);

	// индекс последнего тайла в VRAM с которого можно добавить новые пользовательнские тайлы
	u16 ind = TILE_USER_INDEX;

	// устанавливаем 0 палитре цвета из палитры в файле sega.png
	// 1 параметр - номер палитры (PAL0, PAL1, PAL2, PAL3)
	// 2 параметр - палитра из объекта изображения, имя которого мы указали в resources.res
	// 3 параметр - способ передачи (CPU, DMA, DMA_QUEUE, DMA_QUEUE_COPY)
	PAL_setPalette(PAL0, bg_sega.palette->data, DMA);

	// загружаем тайлы (tileset) в VRAM из тайлсета определенного в resources.res файла bg/sega.png
	// 1 параметр - объект tileset, уникальные тайлы, определенные в resources.res
	// 2 параметр - индекс тайла в VRAM с которого будут загружены тайлы из tileset
	// 3 параметр - способ передачи данных (CPU, DMA, DMA_QUEUE, DMA_QUEUE_COPY)
	VDP_loadTileSet(&sega_tileset, ind, DMA);

	// создаем карту уровня на слое BG_B из карты тайлов определенный в resources.res (sega_map), используя макрос TILE_ATTR_FULL файла sega_map.tmx
	// 1 параметр - tilemap - карта индексов тайлов из тайлсета что рание грузили
	// 2 параметр - слой (BG_A или BG_B)
	// 3 параметр - атрибуты тайлов для tilemap (через макрос TILE_ATTR_FULL)
	//								1 параметр - палитра (PAL0, PAL1, PAL2, PAL3)
	//                              2 параметр - включить приоритеты тайлов
	//                              3 параметр - отоброзить тайлы по вертикали
	//                              4 параметр - отоброзить тайлы по горизонтали
	// 								5 параметр - индекс тайла в VRAM с которого будут наши тайлы
	Map *segaMap = MAP_create(&sega_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));

	// устанавливаем новый индекс последнего тайла в VRAM с которого можно добавить новые пользовательнские тайлы
	ind += sega_tileset.numTile;

	// Устанавливаем позицию на карте уровне игры в пикселах, какую часть уровня отображаем
	// 1 параметр - карта уровеня игры
	// 2 и 3 параметр - координаты x и y в пикселах на карте уровня
	MAP_scrollTo(segaMap, 0, 0);

	// куда смотрит Рапунцель. FALSE - вправо, TRUE - влево
	u8 hFlipRapuncel = FALSE;

	// что нажато на первом контроллере
	u16 joy1, joy2 = 0;

	// координата X спрайта Рапунцель
	s16 xRapuncel = 80;

	// количество циклов которое не нажимали на кнопки контроллера
	// на самом деле только 'влево' и 'вправо', нажатие остальных кнопок ни на что не влияет
	// когда достигнет 128 рапунцель начнет чисать волосы
	u8 time = 0;

	// Объявляем новый PCM семпл, его id будет 64 на основе файла sound/cherry.wav
	// 1 параметр - id сэмпла (значения от 1 до 63 используются для музыки, от 64 до 255 занимают звуки)
	// 2 параметр - sample  имя ресурса, который описали в resources.res - cherry_sfx
	// 3 параметр - len размер сэмпла в бфйтах (выделяемое место под сэмпл)
	XGM_setPCM(64, cherry_sfx, sizeof(cherry_sfx));

	do {
		// запоминаем в joy1 состояние кнопок первого контроллера (что нажато что нет)
		joy1 = JOY_readJoypad(JOY_1);

		// запоминаем в joy2 состояние кнопок первого контроллера (что нажато что нет)
		joy2 = JOY_readJoypad(JOY_2);

		// спрайт Рапунцель нарисавать как есть если hFlipRapuncel == FALSE (будет смотреть вправо)
		// если hFlipRapuncel == TRUE отобразить спрайт Рапунцель по горизониали (будет смотреть влево)
		SPR_setHFlip(rapuncelSprite, hFlipRapuncel);

		// меняем позицию спрайта Рапунцель X = xRapuncel, Y = 80
		SPR_setPosition(rapuncelSprite, xRapuncel, 80);

		if ((joy1 & BUTTON_C) || (joy2 & BUTTON_C)) {
			// нажата кнопка C на контроллере

			// проиграть звуковой эффект sound/cherry.wav
			// 1 параметр - id сэмпла который определили при вызове функции XGM_setPCM
			// 2 параметр - priority приоритет (значение от 0 до 15, где 0 меньший приоритет, а 15 — больший.
			//              Если звук уже проигрывается, то приоритет определяет, должен ли новый звук заменить старый
			// 3 параметр - channel задает отдельный канал, на котором может проигрываться 1 звук
			//              Канал SOUND_PCM_CH1 используется музыкой.
			//              Доступны каналы SOUND_PCM_CH1, SOUND_PCM_CH2, SOUND_PCM_CH3, SOUND_PCM_CH4
			XGM_startPlayPCM(64, 0, SOUND_PCM_CH2);
		}

		if (joy1 & BUTTON_A) {
			// проиграть музыку из Comix Zone - sound/comixzone.vgm
			// 1 параметр имя рессурса из файла resources.res
			XGM_startPlay(comixzone_vgm);
		}

		if (joy2 & BUTTON_A) {
			// проиграть музыку из Sonic - sound/sonic.vgm
			// 1 параметр имя рессурса из файла resources.res
			XGM_startPlay(sonic_vgm);
		}

		if ((joy1 & BUTTON_RIGHT) || (joy2 & BUTTON_RIGHT)) {
			// нажата кнопка 'вправо'
			// сбрасываем количество циклов которое не нажимали кнопки в 0
			time = 0;
			// чтоб Рапунцель смотрела вправо
			hFlipRapuncel = FALSE;
			// меняем позицию Рапунцель вправо на 1 (ходьба)
			xRapuncel += 1;

			if ((joy1 & BUTTON_B) || (joy2 & BUTTON_B)) {
				// нажата кнопка 'B' (и кнопка 'вправо')
				// меняем позицию Рапунцель вправо  еще на 2 (бег)
				xRapuncel += 2;
				// меняем анимацию спрайта Рапунцель на бег
				SPR_setAnim(rapuncelSprite, 3);
			} else {
				// кнопка 'B' НЕ нажата (только 'вправо' нажата)
				// меняем анимацию спрайта Рапунцель на ходьбу
				SPR_setAnim(rapuncelSprite, 2);
			}
		} else if ((joy1 & BUTTON_LEFT) || (joy2 & BUTTON_LEFT)) {
			// нажата кнопка 'влево'
			// сбрасываем количество циклов которое не нажимали кнопки в 0
			time = 0;
			// чтоб Рапунцель смотрела влево
			hFlipRapuncel = TRUE;
			// меняем позицию Рапунцель влево на 1 (ходьба)
			xRapuncel -= 1;

			if ((joy1 & BUTTON_B) || (joy2 & BUTTON_B)) {
				// нажата кнопка 'B' (и кнопка 'влево')
				// меняем позицию Рапунцель влево еще на 2 (бег)
				xRapuncel -= 2;
				// меняем анимацию спрайта Рапунцель на бег
				SPR_setAnim(rapuncelSprite, 3);
			} else {
				// кнопка 'B' НЕ нажата (только 'влево' нажата)
				// меняем анимацию спрайта Рапунцель на ходьбу
				SPR_setAnim(rapuncelSprite, 2);
			}
		} else {
			// кнопки 'влево' и 'вправо' не нажаты
			// количество циклов которое не нажимали кнопки увиличиваем на еденицу
			time++;

			if (time < 128) {
				// устанавливаем анимацию когда Рапунцель просто стоит с вытянутой сковородой
				SPR_setAnim(rapuncelSprite, 0);
			} else {
				// time стало 128 или больше 128
				// устанавливаем спрайту Рапунцель анимацию при которой она чешет волосы
				SPR_setAnim(rapuncelSprite, 1);
			}
		}

		// Обновляет и отображает спрайты на экране
		SPR_update();

		// ждем отрисовки кадра
		SYS_doVBlankProcess();

	  // цикл продолжается вечно
	} while (TRUE);

    return 0;
}
