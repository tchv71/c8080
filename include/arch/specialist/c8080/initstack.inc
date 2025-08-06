    asm {
        ld   sp, 089FFh
        ld   hl, 0C800h ; Reboot
        push hl
    }
