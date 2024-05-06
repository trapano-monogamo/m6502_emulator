		jsr	main
main:	lda	#AA
		sta	$00
		bit	$00,x
		jmp	main
