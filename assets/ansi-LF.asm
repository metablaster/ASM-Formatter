 
 


 
   ;***************************************
; Do not indent source file description
  ; Test me: $(SolutionDir)assets\ansi.asm --encoding ansi
				;***************************************
 

 ;		   € ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø \			
	nl = 10                ; 4 (+1)
	maxLen = 256                 ; 2 (+1)
	public getTitle   ; 2
 getTitle proc			 ;-3
	lea rax, titleStr   ; 1
; ANSI comment: € ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø
	ret                               ; 5 (+2)
	;not indented
		  ; not indented comment
	  getTitle ENDP     ;ASCII code
	public asmMain                ;.€ ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø
		asmMain PROC											;			ASCII code
	lea rcx, prompt                          ; € ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø
	   call printf                       ;ASCII code
	   ;some comment
	mov input, 0         ; € ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø
	lea rcx, input
	; indented
; indented comment
mov rdx, maxLen											  ;€ ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø
	call readLine             ;     ASCII code
lea rcx, fmtStr   ; € ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø
	lea rdx, input             ;ASCII code
		call printf            ;ASCII code
	add rsp, 56  ; ASCII code
	   ret                          ; ASCII code
asmMain endp
  end





