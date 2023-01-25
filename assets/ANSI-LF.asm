	; *****************************************************************
	; This sample was taken from https://github.com/leonhad/asm-samples
	; MIT License
	; Copyright (C) 2019 Leonardo Alves da Costa
	; -----------------------
	; File was modifed by making the code style and syntax as ugly as
	; possible to test ASM Formatter and udoing bad style
	; This comment block must not be indented
	; There should be blank line at the top of a file
	; Blank line should follow, test by removing it
	; *****************************************************************

	.MODEL flat, c

	.CODE
; Comment prior proc should stay here, procedure label should not be indented, semicolon; and trailing semicolons(1_23); ;;
			AVXPackedInt_16 proc


			; Blank lines after proc should be removed


	push ebp
	mov ebp, esp

	mov eax, [ebp+8]									         ; eax = *a, inline comment is misaligned
	mov	ecx, [ebp+12]; ecx = *b, inline comment is too close
	mov edx, [ebp+16]			;    	 edx = *c, inline comment has leading and trailing spaces    			  

; Multiple blank lines follow




		;Comment missing space after semicolon
			                    vmovdqa ymm0, ymmword ptr[eax]	; ymm0 = a, code is misaligned and has an incomplete tab
	vmovdqa ymm1, ymmword ptr[ecx]	; ymm1 = b

; Comment with trailing spaces and tabs    			  
	vpaddw ymm2, ymm0, ymm1		; ymm2 = ymm0 + ymm1
	vpaddsw ymm3, ymm0, ymm1	; ymm3 = ymm0 + ymm1, signed saturation

				      ; Comment with leading spaces and tabs
	vpsubw ymm4, ymm0, ymm1
	vpsubsw ymm5, ymm0, ymm1

		; Blank line follows this comment so it won't be indented

	vpminsw ymm6, ymm0, ymm1	; signed minimum
	vpmaxsw ymm7, ymm0, ymm1	; signed maximum

	; save results
	; The following comment contain ANSI characters
	; € ‚ƒ„…†‡ˆ‰Š‹Œ Žýþÿ÷ø \	
	; ÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ
	vmovdqa ymmword ptr[edx], ymm2
	vmovdqa ymmword ptr[edx+32], ymm3
	vmovdqa ymmword ptr[edx+64], ymm4
	vmovdqa ymmword ptr[edx+96], ymm5
	vmovdqa ymmword ptr[edx+128], ymm6
	vmovdqa ymmword ptr[edx+160], ymm7

	vzeroupper					; avoid performance penalties

	pop ebp  ;this comment contains additional semicolon; and trailing semicolons(1_23); ;;
	ret      ;  this comment has spaces after final semicolon;  		 	


	; Comment prior endp, endp should not be indented, blank line should follow endp
			AVXPackedInt_16 endp
AVXPackedInt_32 PROC
	push ebp
	mov ebp, esp

; Following sections of code contain surplus lines
	mov eax, [ebp+8]			; eax = *a
	mov	ecx, [ebp+12]			; ecx = *b
	mov edx, [ebp+16]			; edx = *c




	vmovdqa ymm0, ymmword ptr[eax]	; ymm0 = a
	vmovdqa ymm1, ymmword ptr[ecx]	; ymm1 = b




	vphaddd ymm2, ymm0, ymm1	;		horizontal add, leading tabs in comment
	vphsubd ymm3, ymm0, ymm1	; horizontal sub
	vpmulld ymm4, ymm0, ymm1	; signed mul
	vpsllvd ymm5, ymm0, ymm1	; logical shift left
	vpsravd ymm6, ymm0, ymm1	; arimethic  shift 		right, this comment 	contains surplus   spaces within comment

	; save results, scrambled code, leadign and trailing spaces
	              vmovdqa ymmword ptr[edx], ymm2
													vmovdqa ymmword ptr[edx+32], ymm3
	vmovdqa ymmword ptr[edx+64], ymm4
vmovdqa ymmword ptr[edx+96], ymm5
				    vmovdqa ymmword ptr[edx+128], ymm6

	vzeroupper					; avoid performance penalties

;a bunch of blank lines after endp should be replaced by single line
; blank lines before endp should be removed completely
	pop ebp
	ret



AVXPackedInt_32 ENDP




	END

; a bunch of blank lines follow that should be replaced by single blank line






