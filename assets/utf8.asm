 ; very bad source file					       
	nl = 10                ; 4 (+1)					       
	maxLen = 256                 ; 2 (+1)					       
	public getTitle   ; 2
 getTitle proc			 ; 3					       
	lea rax, titleStr   ; 1					       
	ret                               ; 5 (+2)					       
		  ; comment
	  getTitle endp     ; ASCII code
	public asmMain                ; ASCII code
		asmMain proc											; ASCII code					       
	lea rcx, prompt                          ; ASCII code					       
	   call printf                       ; ASCII code					       
	mov input, 0         ; ASCII code					       
	lea rcx, input					       
; comment
mov rdx, maxLen      ; ASCII code
	call readLine             ; ASCII code         					              
lea rcx, fmtStr   ; ASCII code					       
	lea rdx, input             ; ASCII code					       
		call printf            ; ASCII code					       
	add rsp, 56  ; ASCII code
	   ret                          ; ASCII code					       
asmMain endp					       
  end					       
