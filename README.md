# IKPProjekat

A32. Logički

Potrebno je razviti BattleshipEngine servis čiji zadatak je da pruži podršku za igru „Potapanje brodova“. 
Pored BattleshipEngine servisa, neophodno je implementirati klijentske procese koji predstavljaju igrače. 
Igrač može biti računar ili čovek. U toku partije aktivna su dva igrača. 
Prilikom startovanja igre, potrebno je da svaki igrač unese pozicije svojih brodova. Tabla je veličine 
10x10. Svaki igrač ima 5 brodova:

- 1 brod dužine 5 polja
- 1 brod dužine 4 polja
- 2 broda dužine 3 polja 
- 1 brod dužine 2 polja

Nakon toga, igrači naizmenično gađaju polja table protivničkog igrača u nameri da potope protivničke 
brodove. U slučaju da je igrač uspeo da pogodi protivnički brod, ima pravo na još jedno gađanje.
Brod je potopljen kad su pogođene sve pozicije na kojima se nalazi. Svaka pozicija na tabli može imati 
stanja NEPOZNATO, POGODAK ili PROMAŠAJ. U zavisnosti od stanja pozicije, na tabli će biti prikazan 
odgovarajući simbol za tu poziciju (~, O ili X).
Igrač u svakom trenutku vidi stanje svoje table i stanje onih polja protivničke table koje je gađao do tada. 
Pobednik je onaj igrač koji prvi potopi sve protivničke brodove.
Koristiti retku matricu. 
Asinhrono javljanje statusa pogodaka i statusa igre. 
Omogućiti IU da klijent može da nastavi igru. 
Igrač ima predefinisano vreme da igra, ili u suprotnom gubi red.

Više informacija na:
https://en.wikipedia.org/wiki/Battleship_(game)