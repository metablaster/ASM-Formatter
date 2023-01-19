







 ;		   very bad source file
	nl = 10                ; 4 (+1)
	maxLen = 256                 ; 2 (+1)
	public getTitle   ; 2
 getTitle proc			 ;-3
	lea rax, titleStr   ; 1
	ret                               ; 5 (+2)
		  ; comment
	  getTitle endp     ;ASCII code
	public asmMain                ;.Οὐχὶ ταὐτὰ παρίσταταί μοι γιγνώσκειν, ὦ ἄνδρες ᾿Αθηναῖοι,
		asmMain proc											;			ASCII code
	lea rcx, prompt                          ;  Зарегистрируйтесь сейчас на Десятую Международную Конференцию по
	   call printf                       ;ASCII code
	   ;some comment
	mov input, 0         ; 4 bytes chars are 𒂕 𒃼
	lea rcx, input
; comment
mov rdx, maxLen											  ;ASCII code
	call readLine             ;     ASCII code
lea rcx, fmtStr   ; ASCII code
	lea rdx, input             ;ASCII code
		call printf            ;ASCII code
	add rsp, 56  ; ASCII code
	   ret                          ; ASCII code
asmMain endp
  end










