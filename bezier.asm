; rdi = r7 - coordinations of points array
; rsi = r6 - pixel array
; rdx = r2 - width
; xmm0 - step

section .data
    align 16
    step dd 0.0, 0.0, 0.0, 0.0
    align 16
    zero dd 0.0, 0.0, 0.0, 0.0
    align 16
    one dd 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000

section .text
    global asmDrawBezier
    
asmDrawBezier:
    push rbp                    ; push calling procedure frame pointer
    mov rbp, rsp                ; set new frame pointer
    
;-----------------------------------------------------------------
    
    ; initialize registers before loop
    
    movss [step+4], xmm0        ; xx | xx | step | xx
    movss [step+12], xmm0       ; step | xx | step | xx  
    movshdup xmm5, [step]       ; set register xmm5 as step | step | step | step
    
    movaps xmm0, [zero]         ; actual_t | actual_t | actual_t | actual_t
    
    movaps xmm4, [one]          ; constant which equals one,
                                ; useful for determining end of loop
    
    movups xmm6, [rdi]          ; p2 | p1
    movups xmm7, [rdi+16]       ; p4 | p3
    
    ;-------------------------------------------------------------

loop:
    
    movaps xmm1, xmm6           ; p2 | p1
    movaps xmm2, xmm7           ; p4 | p3

    ; first iteration
    
    movhlps xmm3, xmm1          ; xx | p2
    movlhps xmm3, xmm2          ; p3 | p2
    subps xmm3, xmm1            ; p3-p2 | p2-p1
    mulps xmm3, xmm0            ; *t | *t
    addps xmm1, xmm3            ; +p2 | +p1
    
    movups xmm3, [rdi+24]       ; p5 | p4
    subps xmm3, xmm2            ; p5-p4 | p4-p3
    mulps xmm3, xmm0            ; *t | *t
    addps xmm2, xmm3            ; +p4 | +p3

    ; second iteration
    
    movhlps xmm3, xmm1          ; xx | p2
    movlhps xmm3, xmm2          ; p3 | p2
    subps xmm3, xmm1            ; p3-p2 | p2-p1
    mulps xmm3, xmm0            ; *t | *t
    addps xmm1, xmm3            ; +p2 | +p1
    
    movhlps xmm3, xmm2          ; xx | p4
    subps xmm3, xmm2            ; xx | p4-p3
    mulps xmm3, xmm0            ; xx | *t
    addps xmm2, xmm3            ; xx | +p3
    
    ; third iteration
    
    movhlps xmm3, xmm1          ; xx | p2
    movlhps xmm3, xmm2          ; p3 | p2
    subps xmm3, xmm1            ; p3-p2 | p2-p1
    mulps xmm3, xmm0            ; *t | *t
    addps xmm1, xmm3            ; +p2 | +p1
    
    ; fourth iteration
    
    movhlps xmm3, xmm1          ; xx | p2
    subps xmm3, xmm1            ; xx | p2-p1
    mulps xmm3, xmm0            ; xx | *t
    addps xmm1, xmm3            ; xx | +p1 := xx | result := xx | [r.y][r.x]
    
    ; calculate position in the pixel array
    
    shufps xmm1, xmm1, 0x50     ; [r.y][r.y] | [r.x][r.x] 
    movhlps xmm2, xmm1          ; xx | [r.y][r.y]
    cvtss2si r8, xmm1           ; convert r.x to integer and store
    cvtss2si r9, xmm2           ; convert r.y to integer and store
    imul r9, rdx                ; r.y * width
    add r8, r9                  ; r.x + r.y*width
    
    ; put colors in the pixel array
    
    imul r8, 3
    add r8, rsi
    mov BYTE [r8], 0x00         ; put R from RGB component
    mov BYTE [r8+1], 0xC0       ; pug G component
    mov BYTE [r8+2], 0xC0       ; put B component
    
    ; increment loop variables
    
    addps xmm0, xmm5            ; make step
    comiss xmm0, xmm4           ; compare actual t with value one
    jnbe end                    ; if flags ZF, CF are set to 0
                                ; which means that actual t is greater than 1
                                ; jump to end
    jmp loop                    ; else go back to the beginning of the loop

;-----------------------------------------------------------------

end:
    mov rsp, rbp                ; restore stack pointer
    pop rbp                      ; restore calling procedure frame pointer
    ret