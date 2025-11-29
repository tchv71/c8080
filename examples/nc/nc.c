/*
 * NC shell for CP/M
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#define ARCH_MICRO80_COLOR
#include "nc.h"
#include <c8080/hal.h>
#include <c8080/keys.h>
#include <c8080/console.h>
#include <c8080/uint32tostring.h>
#include <c8080/touppercase.h>
#include <c8080/getunusedram.h>
#include <string.h>
#include <stdio.h>
#include <cpm.h>
#include "colors.h"
#include "windows.h"
#include "panel.h"
#include "dir.h"
#include "config.h"

//#ifdef ARCH_MICRO80_COLOR
#include "arch_micro80.h"
//#endif

#ifdef NC_SAVE_SCREEN
static struct SavedScreen saved_screen;
#endif

#ifdef NC_GLOB
static const uint8_t STATE_TAB = 1 << 4;
#ifdef NC_SAVE_SCREEN
static const uint8_t STATE_HIDDEN = 1 << 5;
#endif
#endif

static const uint16_t STACK_SIZE = 1024;

static uint8_t copy_buffer_size;  // Размер panel.buffer в 128 байтных блоках
static bool panels_hidden;

static const char help_64_no_fn[] =
    " ;\0Tab  \0"
    " 1\0Left \0"
    " 2\0Right\0"
    " 3\0View \0"
    " 4\0Edit \0"
    " 5\0Copy \0"
    " 6\0Ren  \0"
    " 7\0Mkdir\0"
    " 8\0Del  \0"
    " \0";

static void NcDrawCommandLine(void) {
    DrawInput(panel_a.short_path_size + 1, TEXT_HEIGHT - 2, TEXT_WIDTH - 1 - panel_a.short_path_size,
              COLOR_COMMAND_LINE);
}

static void NcDrawCommandLinePrefix(void) {
    DrawTextXY(0, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, panel_a.path + panel_a.short_path_skip);
    DrawTextXY(panel_a.short_path_size, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, ">");
}

static void NcDrawActivePanelTitleAndCommandLine(void) {
    NcDrawCommandLinePrefix();
    NcDrawCommandLine();
    PanelDrawTitle(COLOR_PANEL_TITLE_ACTIVE);
    PanelShowCursor();
}

static void NcDrawHelp(void) {
    const char *text = help_64_no_fn;
    uint8_t x = 0;
    uint8_t color = COLOR_COMMAND_LINE;
    do {
        DrawTextXY(x, TEXT_HEIGHT - 1, color, text);
        color ^= COLOR_COMMAND_LINE ^ COLOR_HELP_LINE;
        const uint8_t text_size = strlen(text);
        x += text_size;
        text += text_size + 1;
    } while (*text != 0);
}

void NcDrawScreen(void) {
    HideCursor();
    MoveCursor(0, 0);  // Что бы уменьшить вероятность прокрутки экрана из-за ошибок CP/M

#ifdef NC_SAVE_SCREEN
    if (panels_hidden) {
        RestoreScreen(&saved_screen);
    }
#endif

    NcDrawHelp();

#ifdef NC_SAVE_SCREEN
    if (panels_hidden) {
        if (panel_a.total_kb == 0)
            PanelReload();  // Для вычисления пути, который будет отображен в ком. строке
        NcDrawCommandLinePrefix();
        NcDrawCommandLine();
        return;
    }
#endif

    PanelDrawBorder(0);
    PanelDrawBorder(PANEL_WIDTH);

    if (panel_a.total_kb == 0)
        PanelReload();

    PanelDrawFiles();
    NcDrawActivePanelTitleAndCommandLine();
    PanelDrawFreeSpace();

    PanelSwap();
    if (panel_a.total_kb == 0)
        PanelReloadOrCopy();
    PanelDrawTitle(COLOR_PANEL_TITLE);
    PanelDrawFiles();
    PanelDrawFileInfo();
    PanelDrawFreeSpace();
    PanelSwap();
}

static void NcBeforeExit(void) {
    // Сохранение состояния
#ifdef NC_GLOB
    glob_drive_user_b = panel_b.drive_user;
    glob_state = (panel_x ? STATE_TAB : 0);
#ifdef NC_SAVE_SCREEN
    if (panels_hidden)
        glob_state |= STATE_HIDDEN;
#endif
    glob_a_cursor_x = panel_a.cursor_x;
    glob_a_cursor_y = panel_a.cursor_y;
    glob_a_offset = panel_a.offset;
    glob_b_cursor_x = panel_b.cursor_x;
    glob_b_cursor_y = panel_b.cursor_y;
    glob_b_offset = panel_b.offset;
#endif

    // Восстанавление экрана
#ifdef NC_SAVE_SCREEN
    RestoreScreen(&saved_screen);
#else
    ClearConsole();
#endif
}

static void NcCommand(const char *text) {
    NcBeforeExit();
    puts(panel_a.path);
    puts(">");
    puts(text);
    uint8_t d = panel_a.drive_user;
#ifdef NC_GLOB
    // TODO: Если указываем диск, то всегда будет корневая папка. А потом нужно будет вставить парсер пути.
    if (text[0] != 0 && text[1] == ':') {
        bios_user = 0xF0 + (d >> 4);
        d &= 0x0F;
    }
#endif
    CpmCommand(d, text);
}

static void NcExecute(void) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0)
        return;

    // Это папка
    const uint8_t attrib = PanelGetCursor()->attrib;
    if (attrib & ATTRIB_DIR_ALL) {
        if (attrib & ATTRIB_DIR_UP)
            panel_reload_select_dir = PanelGetDirIndex() << ATTRIB_DIR_SHIFT;
        panel_a.drive_user = PanelGetDrive() | ((attrib << (4 - ATTRIB_DIR_SHIFT)) & 0xF0);
        panel_a.cursor_x = 0;
        panel_a.cursor_y = 0;
        panel_a.offset = 0;
        PanelReload();

        PanelDrawFiles();
        NcDrawActivePanelTitleAndCommandLine();
        return;
    }

    char text[sizeof(panel_a.selected_name)];
    strcpy(text, panel_a.selected_name);

    char *ext = strchr(text, '.');
    if (ext && ext[1] == 'C' && ext[2] == 'O' && ext[3] == 'M') {
        ext[0] = 0;
        NcCommand(text);
        return;
    }

    // TODO: Запуск TXT, BAS файлов
}

static void NcSelectDrive(uint8_t offset) {
    const bool swap = panel_x != offset;
    const uint8_t result = SelectDriveWindow((swap ? panel_b.drive_user : panel_a.drive_user) & 0x0F, offset);
    if (result == 0xFF)
        return;
    if (swap)
        PanelSwap();
    panel_a.drive_user = result;
    panel_a.cursor_x = 0;
    panel_a.cursor_y = 0;
    panel_a.offset = 0;
    PanelReload();
    if (swap)
        PanelSwap();
    NcDrawScreen();
}

static void NcDriveChanged(uint8_t drive_user) {
    if (panel_a.drive_user == drive_user || drive_user == 0xFF)
        PanelReload();
    if (panel_b.drive_user == drive_user || drive_user == 0xFF) {
        PanelSwap();
        PanelReloadOrCopy();
        PanelSwap();
    }
    NcDrawScreen();
}

static void NcCopyMoveRename(bool rename) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0 || (PanelGetCursor()->attrib & ATTRIB_DIR_UP) != 0)
        return;

    // Исходный файл
    struct FileInfo *source_file = PanelGetCursor();
    struct FCB source;
    source.drive = PanelGetDrive() + 1;
    memcpy(source.name83, source_file->name83, sizeof(source_file->name83));

    // Окно
    const char *title = rename ? " Rename or move " : " Copy ";
    const uint8_t y = DrawWindow(WINDOW_X_CENTER, 8, title);  // Original: Copy | Rename
    DrawWindowText(y, "File");                                // Original: Copy | Rename or move
    DrawWindowText(y + 1, panel_a.selected_name);             // TODO: Можно добавить путь
    DrawWindowText(y + 2, "to");
    DrawButtons(y + 5, 0, rename ? "Rename\0Cancel\0" : "Copy\0Cancel\0");
    // Original: Copy\0Cancel | Rename/Move\0Cancel
    input[0] = 0;
    if (!RunInput(y + 3))
        return;

    ToUpperCase(input);

    struct FCB dest;
    uint8_t dest_drive_user = DirParsePathName(&dest, input, panel_b.drive_user, panel_a.drive_user);
    if (dest_drive_user == 0xFF) {
        ErrorWindow("Incorrect file name");
        return;
    }

    // Если имя файла не указано, то используется имя исходного файла
    if (dest.name83[0] == ' ')
        memcpy(dest.name83, source.name83, sizeof(dest.name83));

    // Для вывода на экран
    DirMakePathName(input, sizeof(input), dest_drive_user, &dest);

    // Есть ли файл в папке назначения?
    const uint8_t dest_user = dest_drive_user >> 4;
    CpmSetUser(dest_user);
    if (CpmSearchFirst(dest.drive, dest.name83, false) != NULL) {
        ErrorWindow("The file already exists");  // Original
        // В оригинале предлагается заменить файл
        // Если это сделаем, то нужно проверить, что мы не переносим файл сам в себя
        return;
    }

    // Копирование атрибут, всех 16 бит.
    CpmSetUser(PanelGetDirIndex());
    struct FCB *source_info = CpmSearchFirst(source.drive, source.name83, false);
    if (source_info == NULL) {
        ErrorWindow("Incorrect file name");
        return;
    }
    CpmSetAttrib(dest.name83, CpmGetAttrib(source_info->name83));

    // Переименование файла.
    // Средствами системы нельзя переносить файл между папками, т.к. нельзя изменять пользователя.
    // А может и можно.
    if (rename && panel_a.drive_user == dest_drive_user) {
        memcpy(source.rename, &dest, sizeof(dest.drive) + sizeof(dest.name83));
        CpmRename(&source);  // TODO: Error
        NcDriveChanged(dest_drive_user);
        return;
    }

    if (source_file->attrib & ATTRIB_DIR_ALL) {
        if (!rename || source.drive != dest.drive) {
            ErrorWindow("Can't copy the folder");
            return;
        }
    }

    if (CpmOpen(&source) == 0xFF) {
        ErrorWindow("Can't open the file");  // Original
        return;
    }

    CpmSetUser(dest_user);

    if (CpmCreate(&dest) == 0xFF) {
        ErrorWindow("Can't create the file");  // Original
        rename = false;                        // Не удалять исходный файл при ошибке
    } else {
        uint8_t y = DrawWindow(WINDOW_X_CENTER, 9, title);
        DrawWindowTextCenter(y, "File");
        // Original: Copying the file or directory | Renaming or Moving the file or directory
        DrawWindowTextCenter(y + 1, panel_a.selected_name);
        DrawWindowTextCenter(y + 2, "to");
        DrawWindowTextCenter(y + 3, input);
        DrawProgress(y + 4);
        DrawButtons(y + 6, 0, "Cancel\0");  // В оригинале нет кнопки

        panel_b.count = 0;  // Список файлов будет уничтожен

        uint16_t i = 0;
        for (;;) {
            CpmSetUser(PanelGetDirIndex());

            uint8_t *buffer = (void *)panel_b.files;
            uint8_t count = 0;
            uint8_t result;
            do {
                CpmSetDma(buffer);
                result = CpmRead(&source);
                if (result != 0) {
                    if (result == CPM_READ_EOF)
                        break;
                    ErrorWindow("Can't read the file");  // Original
                    rename = false;  // Не удалять исходный файл при ошибке
                    goto break2;
                }
                buffer += CPM_128_BLOCK;
                count++;
            } while (count < copy_buffer_size);

            CpmSetUser(dest_user);  // TODO: Error

            buffer = (void *)panel_b.files;
            while (count > 0) {
                CpmSetDma(buffer);
                if (CpmWrite(&dest) != 0) {
                    ErrorWindow("Can't write the file");  // Original
                    rename = false;  // Не удалять исходный файл при ошибке
                    goto break2;
                }
                buffer += CPM_128_BLOCK;
                count--;
            }

            if (result == CPM_READ_EOF)
                break;

            i += copy_buffer_size;
            DrawProgressNext(y + 4, i, source_file->blocks_128);

            if (CpmConsoleDirect(0xFF) == KEY_ESC) {
                rename = false;  // Не удалять исходный файл
                break;
            }
        }
        DrawProgressNext(y + 4, 1, 1);

    break2:
        CpmSetDma(DEFAULT_DMA);
        CpmSetUser(dest_user);

        if (CpmClose(&dest) == 0xFF) {
            rename = false;  // Не удалять исходный файл при ошибке
            ErrorWindow("Can't close the file");
        }
    }

    CpmSetUser(PanelGetDirIndex());

    if (CpmClose(&source) == 0xFF) {
        rename = false;  // Не удалять исходный файл при ошибке
        ErrorWindow("Can't close the file");
    }

    if (rename)
        if (CpmDelete(&source) == 0xFF)
            ErrorWindow("Can't delete the file");

    // Обновить все панели нужно в любом случае, потому что:
    // 1) panel_b.files использован как буфер
    // 2) при переносе из panel_a удален файл
    // 3) копирование могло быть в panel_a или panel_b (указано в dest_drive_user)
    NcDriveChanged(0xFF);
}

static void NcMakeDir(void) {
    if (!MakeDirWindow())
        return;

    ToUpperCase(input);

    static const char *errors[] = {"Folder creation limit", "Incorrect file name", "The file already exists",
                                   "Can't create the file", "Can't close the file"};

    const uint8_t drive_dir = DirMake(panel_a.drive_user, input);
    if (drive_dir >= DIR_MAKE_ERROR_LIMIT) {
        ErrorWindow(errors[(uint8_t)(drive_dir - DIR_MAKE_ERROR_LIMIT)]);
        return;
    }

    // TODO: Курсор на созданную папку. У нас есть для этого переменная.
    NcDriveChanged(drive_dir);
}

static void NcDelete(void) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0 || (PanelGetCursor()->attrib & ATTRIB_DIR_UP) != 0)
        return;

    // Диалог
    if (!DeleteWindow(panel_a.selected_name))
        return;

    struct FileInfo *c = PanelGetCursor();

    struct FCB f;
    f.drive = PanelGetDrive() + 1;
    memcpy(f.name83, c->name83, sizeof(f.name83));

    // Проверка, что папка не пустая
    if (c->attrib & ATTRIB_DIR_MASK) {
        CpmSetUser(GET_DIR_FROM_ATTRIB(c->attrib));
        if (CpmSearchFirst(f.drive, NULL, 0) != NULL) {
            ErrorWindow("Folder is not empty");
            return;
        }
    }

    // Удаление файла
    CpmSetUser(PanelGetDirIndex());
    CpmDelete(&f);  // TODO: Проверить ошибку

    // TODO: Выйти из удаленной папки
    // Обновить список файлов
    NcDriveChanged(panel_a.drive_user);
}

int main(int, char **) {
    const size_t panel_buffer_bytes = GetUnusedRam((void **)&panel_a.files, STACK_SIZE) / 2;
    panel_b.files = (void *)panel_a.files + panel_buffer_bytes;
    panel_files_max = panel_buffer_bytes / sizeof(panel_a.files[0]);

    // Вычисление круглого кол-ва 128 байтных буферов в panel_a.files
    uint8_t allow_loop = panel_buffer_bytes / (CPM_128_BLOCK * 2);  // Еще можно умножить на 2
    if (allow_loop < 2)
        return 1;  // Недостаточно памяти для copy_buffer_size и panel_a.files
    static const uint8_t MAX_ROUND_UINT8 = 128;
    copy_buffer_size = 1;
    do {
        copy_buffer_size *= 2;
    } while (copy_buffer_size <= allow_loop && copy_buffer_size < MAX_ROUND_UINT8 / 2);

    // CCP будет запускать A:NC вместо ожидания ввода команды пользователем
#ifdef NC_GLOB
    bios_dont_resart_nc = 0;
#endif

    // Что бы командер нижней строкой не закрывал полезные данные
    const uint16_t xy = GetCursorPosition();
    if (xy >= (TEXT_HEIGHT - 1) << 8) {
        CpmConsoleWrite('\n');
        MoveCursor(xy, (xy >> 8) - 1);
    }

    // Сохранение консоли
#ifdef NC_SAVE_SCREEN
    SaveScreen(&saved_screen);
#endif

    // Скрытие курсора
    HideCursor();

    // Для рисования ком. строки
    MakeString(spaces, ' ', sizeof(spaces) - 1);

#ifdef NC_GLOB
    // Текущий диск и папку в активную панель
    panel_a.drive_user = ((((bios_user & 0xF0) == 0xE0) ? bios_user : CpmGetUser()) << 4) | CpmGetDrive();

    bios_user = 0;

    // Восстановление состояния
    panel_a.cursor_x = glob_a_cursor_x;
    panel_a.cursor_y = glob_a_cursor_y;
    panel_a.offset = glob_a_offset;
    panel_b.cursor_x = glob_b_cursor_x;
    panel_b.cursor_y = glob_b_cursor_y;
    panel_b.offset = glob_b_offset;

#ifdef NC_SAVE_SCREEN
    panels_hidden = (glob_state & STATE_HIDDEN) != 0;
#endif

    if (glob_state & STATE_TAB) {
        if (panel_x != 0)
            panel_x = 0;
        else
            panel_x = TEXT_WIDTH / 2;
    }

    panel_b.drive_user = glob_drive_user_b;
    if ((panel_b.drive_user & 0x0F) >= DRIVE_COUNT)
        panel_b.drive_user = panel_a.drive_user;
#else
    // Текущий диск и папку в активную панель
    panel_a.drive_user = CpmGetDrive() | (CpmGetUser() << 4);
    panel_b.drive_user = panel_a.drive_user;
#endif

    NcDrawScreen();

    for (;;) {
        const uint8_t c = getchar();
        if (!panels_hidden) {
            if (input_pos == 0) {
                switch (c) {
                    case ';':
                        PanelDrawTitle(COLOR_PANEL_TITLE);
                        PanelHideCursor();
                        PanelSwap();
                        NcDrawActivePanelTitleAndCommandLine();
                        continue;
                    case '1':
                        NcSelectDrive(0);
                        continue;
                    case '2':
                        NcSelectDrive(PANEL_WIDTH);
                        continue;
                    case KEY_ENTER:
                        NcExecute();
                        continue;
                    case '5':
                        NcCopyMoveRename(false);
                        continue;
                    case '6':
                        NcCopyMoveRename(true);
                        continue;
                    case '7':
                        NcMakeDir();
                        continue;
                    case '8':
                        NcDelete();
                        continue;
                }
            }
            switch (c) {
                case KEY_LEFT:
                    PanelMoveCursorLeft();
                    continue;
                case KEY_RIGHT:
                    PanelMoveCursorRight();
                    continue;
                case KEY_DOWN:
                    PanelMoveCursorDown();
                    continue;
                case KEY_UP:
                    PanelMoveCursorUp();
                    continue;
            }
        }
        if (input_pos == 0) {
            switch (c) {
                case '0':
                    NcBeforeExit();
#ifdef NC_GLOB
                    bios_dont_resart_nc = 1;
#endif
                    return 0;
            }
        }

        switch (c) {
            case KEY_ENTER:
                NcCommand(input);
                continue;
            case 0x0A: {  // CTRL+ENTER
                uint8_t new_size = input_pos + strlen(panel_a.selected_name);
                if (new_size < sizeof(input) - 1) {
                    input_pos = new_size;
                    strcat(input, panel_a.selected_name);
                    NcDrawCommandLine();
                }
                continue;
            }
            case 'I' & 0x1F:  // CTRL+I
                panel_x = PANEL_WIDTH - panel_x;
                NcDrawScreen();
                continue;
#ifdef NC_SAVE_SCREEN
            case 'O' & 0x1F:  // CTRL+O
                panels_hidden = !panels_hidden;
                NcDrawScreen();
                continue;
#endif
        }
        ProcessInput(c);
        NcDrawCommandLine();
    }
}
