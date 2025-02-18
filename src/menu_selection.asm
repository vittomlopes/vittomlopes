section .text
    global calcMenuY

calcMenuY:
    ; Parâmetro de entrada: rdi (selectedOption)
    ; Saída: rax (posição Y)

    mov eax, 350         ; Posição base do menu
    imul edi, edi, 40    ; Multiplica selectedOption por 40 (distância entre opções)
    add eax, edi         ; Soma a posição base
    ret
section .note.GNU-stack noexec
