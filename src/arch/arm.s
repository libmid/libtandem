.syntax unified
.arm
.text

.global __td_entry
.hidden __td_entry
__td_entry:
    mov r0, r4
    blx r5
    bkpt #0

.global __td_switch
.hidden __td_switch
__td_switch:
    str sp, [r0, #0]
    add r2, r0, #4
    stmia r2, {r4-r11, lr}
    add r2, r0, #40
    vstmia r2, {d8-d15}

    ldr sp, [r1, #0]
    add r2, r1, #4
    ldmia r2, {r4-r11, lr}
    add r2, r1, #40
    vldmia r2, {d8-d15}

    bx lr

