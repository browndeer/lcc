HAI 2.0

OBTW
	This performs a parallel Pi calculation across written in LOLCODE with
	parallel extensions. Each parallel processing element computes a piece of
	the total sum using the Leibniz formula for Pi:

	Pi/4 = Integral{dx/(1+x^2)}, x=0,1

	A final summation across all parallel processing elements is performed,
	demonstrating the concept of linear parallel reductions. A global barrier
	must occur (HUGZ) to guarantee that each processing element has completed
	calculating their partial sum (VAR). Each processing element accumulates the
	partial sum from every processing element (ME and MAH FRENZ). In this code,
	the precision of the calculated result improves as the total number of
	processing elements increases to machine precision. Alternatively, you can
	increase the number of integral steps, N.
TLDR

I HAS A N ITZ SRSLY A NUMBR AN ITZ 10
I HAS A H ITZ SRSLY A NUMBAR AN ITZ QUOSHUNT OF 1.0 AN PRODUKT OF N AN MAH FRENZ
I HAS A S ITZ SRSLY A NUMBR AN ITZ PRODUKT OF N AN ME
WE HAS A VAR ITZ SRSLY A NUMBAR AN IM SHARIN IT

IM IN YR LOOP UPPIN YR K TIL BOTH SAEM K AN N
	I HAS A X ITZ SRSLY A NUMBAR AN ITZ QUOSHUNT OF 4.0 ...
		AN SUM OF 1.0 AN SQUAR OF PRODUKT OF H AN ...
		SUM OF 0.5 AN SUM OF S AN K
	VAR R SUM OF VAR AN X
IM OUTTA YR LOOP

VAR R PRODUKT OF VAR AN H

HUGZ

I HAS A PIE ITZ SRSLY A NUMBAR

IM IN YR LOOP UPPIN YR K TIL BOTH SAEM K AN MAH FRENZ
	TXT MAH BFF K, PIE R SUM OF PIE AN UR VAR
IM OUTTA YR LOOP

BOTH SAEM 0 AN ME, O RLY?
YA RLY,
	VISIBLE "PIE IZ: " PIE
OIC

KTHXBYE
