.org	$8000

VALUE = $AA
CONST= $00
VAR =$0000

symbol=gj53i0e[}{\

	jsr	main
	bvc	end

main:
	lda	VALUE
loop:	sta	$00,x
	inx
	bvc	loop	; write accumulator up to $0F
	rts

sub:	.org	VAR	; something something

end:	.end
