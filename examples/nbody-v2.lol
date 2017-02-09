HAI 1.2

OBTW
*** N-Body algorithm: propagate particles subject to Newtonian dynamics
*** written in LOLCODE with parallel extensions and other extensions.
TLDR

I HAS A little_time ITZ SRSLY A NUMBAR AN ITZ 0.001

I HAS A x ITZ SRSLY A NUMBAR
I HAS A y ITZ SRSLY A NUMBAR
I HAS A z ITZ SRSLY A NUMBAR

I HAS A vx ITZ SRSLY A NUMBAR
I HAS A vy ITZ SRSLY A NUMBAR
I HAS A vz ITZ SRSLY A NUMBAR

I HAS A ax ITZ SRSLY A NUMBAR
I HAS A ay ITZ SRSLY A NUMBAR
I HAS A az ITZ SRSLY A NUMBAR

I HAS A dx ITZ SRSLY A NUMBAR
I HAS A dy ITZ SRSLY A NUMBAR
I HAS A dz ITZ SRSLY A NUMBAR

WE HAS A pos_x ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32 AN IM SHARIN IT
WE HAS A pos_y ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32 AN IM SHARIN IT
WE HAS A pos_z ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32 AN IM SHARIN IT

I HAS A vel_x ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32
I HAS A vel_y ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32
I HAS A vel_z ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32

I HAS A tmp_pos_x ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32
I HAS A tmp_pos_y ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32
I HAS A tmp_pos_z ITZ SRSLY LOTZ A NUMBARS AN THAR IZ 32

I HAS A me ITZ SRSLY A NUMBR AN ITZ ME

VISIBLE "O HAI ITZ " me` " AN I HAS PARTICLZ 2 MUV"

HUGZ

IM IN YR loop UPPIN YR i TIL BOTH SAEM i AN 32

  pos_x'Z i R SUM OF ME AN WHATEVAR
  pos_y'Z i R SUM OF ME AN WHATEVAR
  pos_z'Z i R SUM OF ME AN WHATEVAR
  vel_x'Z i R QUOSHUNT OF SUM OF ME AN WHATEVAR AN 1000
  vel_y'Z i R QUOSHUNT OF SUM OF ME AN WHATEVAR AN 1000
  vel_z'Z i R QUOSHUNT OF SUM OF ME AN WHATEVAR AN 1000

IM OUTTA YR loop

IM IN YR big_loop UPPIN YR time TIL BOTH SAEM time AN 10

  IM IN YR outer_loop UPPIN YR i TIL BOTH SAEM i AN 32

    x R pos_x'Z i
    y R pos_y'Z i
    z R pos_z'Z i

    vx R vel_x'Z i
    vy R vel_y'Z i
    vz R vel_z'Z i

    ax R 0
    ay R 0
    az R 0

    IM IN YR inner_loop UPPIN YR j TIL BOTH SAEM j AN 32

      DIFFRINT i AN j, O RLY?
      YA RLY,

        dx R DIFF OF pos_x'Z i AN pos_x'Z j
        dy R DIFF OF pos_y'Z i AN pos_y'Z j
        dz R DIFF OF pos_z'Z i AN pos_z'Z j

        dx R SQUAR OF dx
        dy R SQUAR OF dy
        dz R SQUAR OF dz

        I HAS A r ITZ SRSLY A NUMBAR AN ITZ UNSQUAR OF ...
          SUM OF dx AN SUM OF dy AN dz

        I HAS A r3 ITZ SRSLY A NUMBAR AN ITZ PRODUKT OF r AN SQUAR OF r

        ax R SUM OF ax AN PRODUKT OF dx AN r3
        ay R SUM OF ay AN PRODUKT OF dy AN r3
        az R SUM OF az AN PRODUKT OF dz AN r3

      OIC

    IM OUTTA YR inner_loop

    IM IN YR inner_loop UPPIN YR k TIL BOTH SAEM k AN MAH FRENZ

      DIFFRINT k AN ME, O RLY?
        YA RLY,
          IM IN YR inner_inner_loop UPPIN YR j TIL BOTH SAEM j AN 32

BTW            TXT MAH BFF k AN STUFF,
					TXT MAH BFF k, dx R DIFF OF pos_x'Z i AN UR pos_x'Z j
               TXT MAH BFF k,  dy R DIFF OF pos_y'Z i AN UR pos_y'Z j
               TXT MAH BFF k,  dz R DIFF OF pos_z'Z i AN UR pos_z'Z j
BTW            TTYL

            dx R SQUAR OF dx
            dy R SQUAR OF dy
            dz R SQUAR OF dz

            I HAS A r ITZ SRSLY A NUMBAR AN ITZ UNSQUAR OF ...
              SUM OF dx AN SUM OF dy AN dz

            I HAS A r3 ITZ SRSLY A NUMBAR AN ITZ PRODUKT OF r AN SQUAR OF r

            I HAS A invr3 ITZ SRSLY A NUMBAR AN ITZ FLIP OF r3

            ax R SUM OF ax AN PRODUKT OF dx AN r3
            ay R SUM OF ay AN PRODUKT OF dy AN r3
            az R SUM OF az AN PRODUKT OF dz AN r3

          IM OUTTA YR inner_inner_loop
      OIC

    IM OUTTA YR inner_loop

    x R SUM OF x AN SUM OF PRODUKT OF vx AN little_time ...
      AN PRODUKT OF 0.5 AN PRODUKT OF ax AN SQUAR OF little_time
    y R SUM OF y AN SUM OF PRODUKT OF vy AN little_time ...
      AN PRODUKT OF 0.5 AN PRODUKT OF ay AN SQUAR OF little_time
    z R SUM OF z AN SUM OF PRODUKT OF vz AN little_time ...
      AN PRODUKT OF 0.5 AN PRODUKT OF az AN SQUAR OF little_time

    vx R SUM OF vx AN PRODUKT OF ax AN little_time
    vy R SUM OF vy AN PRODUKT OF ay AN little_time
    vz R SUM OF vz AN PRODUKT OF az AN little_time

    tmp_pos_x'Z i R x
    tmp_pos_y'Z i R y
    tmp_pos_z'Z i R z

    vel_x'Z i R vx
    vel_y'Z i R vy
    vel_z'Z i R vz

  IM OUTTA YR outer_loop

  HUGZ

  IM IN YR outer_loop UPPIN YR i TIL BOTH SAEM i AN 32

    pos_x'Z i R tmp_pos_x'Z i
    pos_y'Z i R tmp_pos_y'Z i
    pos_z'Z i R tmp_pos_z'Z i

  IM OUTTA YR outer_loop

  HUGZ

IM OUTTA YR big_loop

VISIBLE "O HAI ITZ " ME " AN MAH PARTICLZ IZ NOW:"
IM IN YR loop UPPIN YR i TIL BOTH SAEM i AN 32
  VISIBLE pos_x'Z i " " pos_y'Z i " " pos_z'Z i
IM OUTTA YR loop

KTHXBYE
