asm {
        pop  de
        ld   a, (7)
        sub  8
        ld   h, a
        ld   l, 0
        ld   sp, hl
        ld   hl, 0
        push de

        ;call 0F830h ; Get RAM top
        ;ld   sp, hl
        ;ld   hl, 0F86Ch ; Monitor
        ;push hl
}
