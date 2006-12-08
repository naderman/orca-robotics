/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_OCM_ICONS_H
#define ORCA2_OCM_ICONS_H

namespace orcaqcm
{

/* XPM */
static const char * const ocm_platform_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 42 1",
"  c #2C2C2C",
". c #535353",
"X c gray33",
"o c #5F6060",
"O c #778AAD",
"+ c gray60",
"@ c #8193B5",
"# c #8C9EBE",
"$ c #9CABCA",
"% c #A8B8D5",
"& c #B0BFDB",
"* c #B7C6DF",
"= c #A6C7FF",
"- c #ACCBFF",
"; c #BCCAE4",
": c #B4D0FF",
"> c #BDD5FF",
", c #C0C0C0",
"< c #C3DDFF",
"1 c #C4DAFF",
"2 c #C6DBFF",
"3 c #C5DDFF",
"4 c #C7DEFF",
"5 c #CDDFFF",
"6 c #CBE1FF",
"7 c #CFE0FF",
"8 c #D0E4FF",
"9 c #D6E6FF",
"0 c #D8E6FF",
"q c #DAE9FF",
"w c #DEF0FF",
"e c #E1EBFF",
"r c #E0ECFF",
"t c #E8F0FF",
"y c #ECF3FF",
"u c #EEF4FF",
"i c #F0F6FF",
"p c #F5F9FF",
"a c #F6F8FF",
"s c #F9FBFF",
"d c gray100",
"f c None",
/* pixels */
"fff,ooooooooooo+",
"ff,o0aaaaaaaa$o.",
"ff+XXXXXXXXXXoO ",
"ffXsaappaaaas ; ",
"ffXpuuuiyiyip * ",
"ffXitttttttti & ",
"ffXyereeeeeey % ",
"ffXt09000090t $ ",
"ffXr55557755e $ ",
"ffX0422122120 # ",
"ffX9>>>>>>>>9 # ",
"ffX7::::::::8 @ ",
"ffX6--------6 O ",
"ffX4========4 Oo",
"ffXw<<<<4<<<w o,",
"ff+          +,f"
};

static const char * const ocm_component_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 40 1",
"  c #2C2C2C",
". c gray33",
"X c #E3B85D",
"o c #E4BA62",
"O c #E5BD68",
"+ c #E6BF6E",
"@ c #E7C171",
"# c #E7C275",
"$ c #E8C57A",
"% c #9A9A9A",
"& c #EAC883",
"* c #EBCC8A",
"= c #EBCF91",
"- c #ECD095",
"; c #EDD399",
": c #EED59E",
"> c #EFD7A2",
", c #F0D8A6",
"< c #F1DBAC",
"1 c #F1DEB3",
"2 c #F3DFB7",
"3 c #F2E0B7",
"4 c #F3E1BB",
"5 c #F3E1BC",
"6 c #F4E4C1",
"7 c #F6E7C8",
"8 c #F6E9CC",
"9 c #F7EAD0",
"0 c #F7ECD3",
"q c #F8EED8",
"w c #F9F0DB",
"e c #F9F1DF",
"r c #FBF3E4",
"t c #FBF6EA",
"y c #FCF8EF",
"u c #FDF9F2",
"i c #FDFBF6",
"p c #FEFDFA",
"a c gray100",
"s c None",
/* pixels */
"ssssssssssssssss",
"ssssssssssssssss",
"s%.............%",
"s.rrrrrrrrrrrrr ",
"%...%ppiiuyttrr ",
".iii yttrrewq09 ",
"%   %ewq0987665 ",
"s.q098766521<<5 ",
"s.q6431<<,>:;-5 ",
"%...%>:;-=**&&5 ",
".;;; **&&$$#@+5 ",
"%   %$#@+OOooX5 ",
"s.,,,,,,,,,,,,u ",
"s%             %",
"ssssssssssssssss",
"ssssssssssssssss"
};

static const char * const ocm_provided_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 11 1",
"  c #2C2C2C",
". c #414141",
"X c gray26",
"o c #727272",
"O c #959595",
"+ c gray59",
"@ c gray70",
"# c gainsboro",
"$ c #FEFEFE",
"% c gray100",
"& c None",
/* pixels */
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&%@o Xo@%&&&&&&&",
"%O      O%&&&&&&",
"@ X+##+  @&&&&&&",
"o +&&&&O o&&&&&&",
"  #&&&&#  oooooo",
"  #&&&&# X XX XX",
"o +&&&&+ o&&&&&&",
"#  +##+X @&&&&&&",
"$O      +%&&&&&&",
"&$@o Xo@%&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&"
};

static const char * const ocm_required_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 11 1",
"  c #2C2C2C",
". c #414141",
"X c gray26",
"o c #727272",
"O c #959595",
"+ c gray59",
"@ c gray70",
"# c gainsboro",
"$ c #FEFEFE",
"% c gray100",
"& c None",
/* pixels */
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&XXo@%&&&&&&&",
"&&&&    O%&&&&&&",
"&&&&##+X @&&&&&&",
"&&&&&&&+ o&&&&&&",
"&&&&&&&# Xoooooo",
"&&&&&&&#  XXX XX",
"&&&&&&&+ o&&&&&&",
"&&&&##+X @&&&&&&",
"&&&&    +%&&&&&&",
"&&&&XXo@%&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&"
};

static const char * const ocm_operation_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 11 1",
"  c #2C2C2C",
". c #414141",
"X c gray26",
"o c #727272",
"O c #959595",
"+ c gray59",
"@ c gray70",
"# c gainsboro",
"$ c #FEFEFE",
"% c gray100",
"& c None",
/* pixels */
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&%@o Xo@%&&&&&&&",
"%O      O%&&&&&&",
"@ X+##+  @&&&&&&",
"o +&&&&O o&&&&&&",
"  #&&&&# o&&&&&&",
"  #&&&&# o&&&&&&",
"o +&&&&+ o&&&&&&",
"#  +##+X @&&&&&&",
"$O      +%&&&&&&",
"&$@o Xo@%&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&",
"&&&&&&&&&&&&&&&&"
};

} // namespace

#endif
