 
 


 
   ;***************************************
; Do not indent source file description
  ; Test me: $(SolutionDir)assets\ansi.asm --encoding ansi
				;***************************************
 

 ;		     ŭŝ˙÷ĝ \			
	nl = 10                ; 4 (+1)
	maxLen = 256                 ; 2 (+1)
	public getTitle   ; 2
 getTitle proc			 ;-3
	lea rax, titleStr   ; 1
; ANSI comment:   ŭŝ˙÷ĝ
	ret                               ; 5 (+2)
	;not indented
		  ; not indented comment
	  getTitle ENDP     ;ASCII code
	public asmMain                ;.  ŭŝ˙÷ĝ
		asmMain PROC											;			ASCII code
	lea rcx, prompt                          ;   ŭŝ˙÷ĝ
	   call printf                       ;ASCII code
	   ;some comment
	mov input, 0         ;   ŭŝ˙÷ĝ
	lea rcx, input
	; indented
; indented comment
mov rdx, maxLen											  ;  ŭŝ˙÷ĝ
	call readLine             ;     ASCII code
lea rcx, fmtStr   ;   ŭŝ˙÷ĝ
	lea rdx, input             ;ASCII code
		call printf            ;ASCII code
	add rsp, 56  ; ASCII code
	   ret                          ; ASCII code
asmMain endp
  end





