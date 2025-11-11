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

static const uint16_t STACK_SIZE = 1024;

static uint8_t copy_buffer_size;

static void NcDrawCommandLine(void) {
    const uint8_t s = strlen(panel_a.short_path) - 1;
    char *last_char = &panel_a.short_path[s];
    *last_char = '>';
    DrawTextXY(0, TEXT_HEIGHT - 2, COLOR_COMMAND_LINE, panel_a.short_path + 1);
    *last_char = ' ';
    DrawInput(s, TEXT_HEIGHT - 2, TEXT_WIDTH - s, COLOR_COMMAND_LINE);
}

void NcDrawScreen(void) {
    if (panels_hidden)
        RestoreConsole();

    DrawTextXY(0, TEXT_HEIGHT - 1, COLOR_COMMAND_LINE, help);

    if (panels_hidden) {
        if (panel_a.total_bytes == 0)
            FindFiles();  // Для вычисления пути, который будет отображен в ком. строке
        NcDrawCommandLine();
        return;
    }

    PanelDrawBorder(0);
    PanelDrawBorder(PANEL_WIDTH);

    if (panel_a.total_bytes == 0)
        FindFiles();

    NcDrawCommandLine();
    PanelDrawTitle(COLOR_PANEL_TITLE_ACTIVE);
    PanelDrawFiles();
    PanelDrawFreeSpace();
    PanelShowCursor();

    PanelSwap();
    if (panel_a.total_bytes == 0) {
        if (panel_a.drive_user == panel_b.drive_user) {
            memcpy(panel_a.copy_start, panel_b.copy_start, panel_a.copy_end - panel_a.copy_start);
            memcpy(panel_a.files, panel_b.files, sizeof(panel_a.files[0]) * panel_a.count);
            PanelPreparePointers();
        } else {
            FindFiles();
        }
    }
    PanelDrawTitle(COLOR_PANEL_TITLE);
    PanelDrawFiles();
    PanelDrawFileInfo();
    PanelDrawFreeSpace();
    PanelSwap();
}

static void NcCommand(const char *text) {
    ExitScreen();
    panel_a.path[strlen(panel_a.path + 1)] = '>';
    puts(panel_a.path + 1);
    puts(text);
    CpmCommand(panel_a.drive_user, text);
}

static void NcExecute(void) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0)
        return;

    // Это папка
    const uint8_t attrib = PanelGetCursor()->attrib;
    if (attrib & ATTRIB_DIR) {
        find_files_select_dir = (PanelGetDirIndex() << ATTRIB_DIR_SHIFT) | ATTRIB_DIR;
        panel_a.drive_user = PanelGetDrive() | ((attrib << (4 - ATTRIB_DIR_SHIFT)) & 0xF0);
        panel_a.cursor_x = 0;
        panel_a.cursor_y = 0;
        panel_a.offset = 0;
        FindFiles();

        NcDrawCommandLine();
        PanelDrawTitle(COLOR_PANEL_TITLE_ACTIVE);
        PanelDrawFiles();
        PanelShowCursor();
        return;
    }

    char text[sizeof(panel_a.selected_name)];
    strcpy(text, panel_a.selected_name);

    char *ext = strchr(text, '.');
    if (ext && ext[1] == 'C' && ext[2] == 'O' && ext[3] == 'M') {
        ext[0] = 0;
        NcCommand(text);
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
    FindFiles();
    if (swap)
        PanelSwap();
    NcDrawScreen();
}

static void NcDriveChanged(uint8_t drive_user) {
    if (panel_a.drive_user == drive_user)
        FindFiles();
    if (panel_b.drive_user == drive_user) {
        PanelSwap();
        FindFiles();
        PanelSwap();
    }
    NcDrawScreen();
}

static void NcCopyMoveRename(bool rename) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0)
        return;

    // Исходный файл
    struct FileInfo *source_file = PanelGetCursor();
    struct FCB source;
    source.drive = PanelGetDrive() + 1;
    memcpy(source.name83, source_file->name83, sizeof(source_file->name83));

    // Окно
    const char *title = rename ? " Rename or move " : " Copy ";
    const uint8_t y = DrawWindow(WINDOW_X_CENTER, 8, title);  // Original: Copy | Rename
    DrawWindowText(y, "File");                          // Original: Copy | Rename or move
}

static void NcDelete(void) {
    // Выход, если папка пуста или файл не выбран
    if (panel_a.count == 0)
        return;

    // Диалог
    if (!DeleteWindow(panel_a.selected_name))
        return;

    // TODO: Проверить, что папка не пустая

    // Удаление файла
    struct FCB f;
    f.drive = PanelGetDrive() + 1;
    memcpy(f.name83, PanelGetCursor()->name83, sizeof(f.name83));
    CpmSetCurrentUser(PanelGetDirIndex());
    CpmDelete(&f); // TODO: Проверить ошибку

    // Обновить список файлов
    NcDriveChanged(panel_a.drive_user);
}

int main(int, char **) {
    const size_t unused_size = GetUnusedRam((void **)&panel_a.files, STACK_SIZE) / 2;
    panel_b.files = (void *)panel_a.files + unused_size;
    max_panel_files = unused_size / sizeof(panel_a.files[0]);

    copy_buffer_size = 1;
    uint8_t stop_size = unused_size / (128 * 2);
    if (stop_size > 64)
        stop_size = 64;  // Иначе будет переполнение
    while (copy_buffer_size <= stop_size)
        copy_buffer_size *= 2;

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
                        PanelDrawTitle(COLOR_PANEL_TITLE_ACTIVE);
                        PanelShowCursor();
                        NcDrawCommandLine();
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
