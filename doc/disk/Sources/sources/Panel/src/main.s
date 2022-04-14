    EXPORT main
        
    IMPORT _ZN3HAL4InitEv
    IMPORT _ZN8Keyboard4InitEv
    IMPORT _ZN8Keyboard6UpdateEv
    IMPORT _ZN9Interface6UpdateEv

    THUMB

    AREA ||i.main||, CODE, READONLY, ALIGN=1

main PROC
   
    BL  _ZN3HAL4InitEv             ; HAL::Init()
    BL  _ZN8Keyboard4InitEv        ; Keyboard::Init()
L1_10
    BL  _ZN8Keyboard6UpdateEv      ; Keyboard::Update()
    BL  _ZN9Interface6UpdateEv     ; Interface::Update()
    B   L1_10
    ENDP

    AREA ||.ARM.exidx||, LINKORDER=||i.main||, DATA, READONLY, SECTYPE={SHT_ARM_EXIDX}, ALIGN=2

    DCD      0x00000000
    RELOC 42, ||i.main||
    DCD      0x00000001

    END
    