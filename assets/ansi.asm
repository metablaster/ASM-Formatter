 
 
 
 ;		   � ����������� ������ \			
	nl = 10                ; 4 (+1)
	maxLen = 256                 ; 2 (+1)
	public getTitle   ; 2
 getTitle proc			 ;-3
	lea rax, titleStr   ; 1
; ANSI comment: � ����������� ������
	ret                               ; 5 (+2)
		  ; comment
	  getTitle ENDP     ;ASCII code
	public asmMain                ;.� ����������� ������
		asmMain proc											;			ASCII code
	lea rcx, prompt                          ; � ����������� ������
	   call printf                       ;ASCII code
	   ;some comment
	mov input, 0         ; � ����������� ������
	lea rcx, input
; comment
mov rdx, maxLen											  ;� ����������� ������
	call readLine             ;     ASCII code
lea rcx, fmtStr   ; � ����������� ������
	lea rdx, input             ;ASCII code
		call printf            ;ASCII code
	add rsp, 56  ; ASCII code
	   ret                          ; ASCII code
asmMain endp
  end





