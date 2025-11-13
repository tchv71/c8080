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

#include "nc.h"
#include "tools.h"
#include "colors.h"
#include "windows.h"
#include "files.h"
#include "panel.h"
#include "dir.h"

static const uint16_t STACK_SIZE = 1024;

static uint8_t copy_buffer_size;  // Размер panel.buffer в 128 байтных блоках

static void NcDrawCommandLine(void) {
    DrawInput(panel_a.short_path_size + 1, TEXT_HEIGHT - 2, TEXT_WIDTH - 1 - panel_a.short_path_size,
              COLOR_COMMAND_LINE);
}

static void NcPanelDrawActiveTitleAndCommandLine(void) {
    DrawTextXY(0, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, panel_a.path + panel_a.short_path_skip);
    DrawTextXY(panel_a.short_path_size, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, ">");
    NcDrawCommandLine();
    PanelDrawTitle(COLOR_PANEL_TITLE_ACTIVE);
    PanelShowCursor();
}

void NcDrawScreen(void) {
    if (panels_hidden)
        RestoreConsole();

    DrawTextXY(0, TEXT_HEIGHT - 1, COLOR_COMMAND_LINE, help);

    if (panels_hidden) {
        if (panel_a.total_kb == 0)
            PanelReload();  // Для вычисления пути, который будет отображен в ком. строке
        NcDrawCommandLine();
        return;
    }

    PanelDrawBorder(0);
    PanelDrawBorder(PANEL_WIDTH);

    if (panel_a.total_kb == 0)
        PanelReload();

    PanelDrawFiles();
    NcPanelDrawActiveTitleAndCommandLine();
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

static void NcCommand(const char *text) {
    ExitScreen();
    puts(panel_a.path);
    puts(">");
    puts(text);
    CpmCommand(panel_a.drive_user, text);
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
        NcPanelDrawActiveTitleAndCommandLine();
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

    CpmSetCurrentUser(PanelGetDirIndex());

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

    static const char *errors[] = {
        "Folder creation limit",
        "Incorrect file name",
        "The file already exists",
        "Can't create the file",
        "Can't close the file"
    };

    const uint8_t drive_dir = DirMake(panel_a.drive_user, input);
    if (drive_dir >= DIR_MAKE_ERROR_LIMIT) {
        ErrorWindow(errors[drive_dir - DIR_MAKE_ERROR_LIMIT]);
        return;
    }

    // TODO: Курсор на созданную папку
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

    // Проверка, что папка не пустая
    if (c->attrib & ATTRIB_DIR_MASK) {
        CpmSetCurrentUser(GET_DIR_FROM_ATTRIB(c->attrib));
        struct FCB search_args;
        memset(&search_args, '?', sizeof(search_args));
        search_args.drive = PanelGetDrive() + 1;
        if (CpmSearchFirst(DEFAULT_DMA, &search_args) != NULL) {
            ErrorWindow("Folder is not empty");
            return;
        }
    }

    // Удаление файла
    struct FCB f;
    f.drive = PanelGetDrive() + 1;
    memcpy(f.name83, c->name83, sizeof(f.name83));
    CpmSetCurrentUser(PanelGetDirIndex());
    CpmDelete(&f);  // TODO: Проверить ошибку

    // Обновить список файлов
    NcDriveChanged(panel_a.drive_user);
}

int main(int, char **) {
    const size_t panel_buffer_bytes = GetUnusedRam((void **)&panel_a.files, STACK_SIZE) / 2;
    panel_b.files = (void *)panel_a.files + panel_buffer_bytes;
    panel_files_max = panel_buffer_bytes / sizeof(panel_a.files[0]);

    // Вычисление круглого кол-ва 128 байтных буферов в panel_a.files
    uint8_t allow_loop = panel_buffer_bytes / (CPM_128_BLOCK * 2); // Еще можно умножить на 2
    if (allow_loop < 2)
        return 1; // Недостаточно памяти для copy_buffer_size и panel_a.files
    static const uint8_t MAX_ROUND_UINT8 = 128;
    copy_buffer_size = 1;
    do {
        copy_buffer_size *= 2;
    } while (copy_buffer_size <= allow_loop && copy_buffer_size < MAX_ROUND_UINT8 / 2);

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
                        NcPanelDrawActiveTitleAndCommandLine();
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
        switch (c) {
            case KEY_ENTER:
                NcCommand(input);
                continue;
            case 0x0F:  // CTRL+O
                panels_hidden = !panels_hidden;
                NcDrawScreen();
                continue;
        }
        ProcessInput(c);
        NcDrawCommandLine();
    }
}
