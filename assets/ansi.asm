 
 


 
   ;***************************************
; Do not indent source file description
  ; Test me: $(SolutionDir)assets\ansi.asm --encoding ansi
				;***************************************
 

 ;		   � ����������� ������ \			
	nl = 10                ; 4 (+1)
	maxLen = 256                 ; 2 (+1)
	public getTitle   ; 2
 getTitle proc			 ;-3
	lea rax, titleStr   ; 1
; ANSI comment: � ����������� ������
	ret                               ; 5 (+2)
	;not indented
		  ; not indented comment
	  getTitle ENDP     ;ASCII code
	public asmMain                ;.� ����������� ������
		asmMain PROC											;			ASCII code
	lea rcx, prompt                          ; � ����������� ������
	   call printf                       ;ASCII code
	   ;some comment
	mov input, 0         ; � ����������� ������
	lea rcx, input
	; indented
; indented comment
mov rdx, maxLen											  ;� ����������� ������
	call readLine             ;     ASCII code
lea rcx, fmtStr   ; � ����������� ������
	lea rdx, input             ;ASCII code
		call printf            ;ASCII code
	add rsp, 56  ; ASCII code
	   ret                          ; ASCII code
asmMain endp
  end





