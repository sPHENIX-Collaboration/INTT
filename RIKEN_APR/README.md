# Programs for RIKEN APR

## Author List Generator
We basically use the same author list in a year, but the first author should be the report writer.
The number of the institutes needs to be assigned for each case.
To vaoid such dull work, you can use this python script.

Run author_list_generator_JFY2023.py (JFY stands Japanese Fiscal Year) like:
```
[genki 08:11:24 RIKEN_APR] $ python3 author_list_generator_JFY2023.py
```
, so you'll be asked to write your family name. For example, 
```
Who are you?
Type your family name (eg. Nukazuka) > Nukazuka
```
then the author list for you is shown on your terminal:
```
\author{
	\Name{G.~Nukazuka},\institute{1}
	\Name{Y.~Akiba},\institute{1}
	\Name{J.~Bertaux},\institute{2}
	\Name{D.~Cacace},\institute{3}
	\Name{R.~G.~Cecato},\institute{4}
	\Name{A.~Enokizono},\institute{1}
	\Name{K.~Fujiki},\institute{1}\institute{5}
	\Name{M.~Fujiwara},\institute{6}
	\Name{T.~Hachiya},\institute{1}\institute{6}
	\Name{S.~Hasegawa},\institute{1}\institute{7}
	\Name{M.~Hata},\institute{6}
	\Name{B.~Hong},\institute{8}
	\Name{J.~Hwang},\institute{8}
	\Name{M.~Ikemoto},\institute{6}
	\Name{R.~Kan},\institute{6}
	\Name{M.~Kano},\institute{6}
	\Name{T.~Kato},\institute{1}\institute{5}
	\Name{T.~Kikuchi},\institute{1}\institute{5}
	\Name{T.~Kondo},\institute{9}
	\Name{C.~M.~Kuo},\institute{10}
	\Name{R.~S.~Lu},\institute{11}
	\Name{N.~Morimoto},\institute{6}
	\Name{I.~Nakagawa},\institute{1}
	\Name{R.~Nouicer},\institute{3}
	\Name{R.~Pisani},\institute{3}
	\Name{C.~W.~Shih},\institute{10}
	\Name{M.~Shimomura},\institute{6}
	\Name{R.~Shishikura},\institute{1}\institute{5}
	\Name{M.~Stojanovic},\institute{2}
	\Name{Y.~Sugiyama},\institute{6}
	\Name{W.~C.~Tang},\institute{10}
	\Name{Y.~Terasaka},\institute{6}
	\Name{H.~Tsujibata},\institute{6}
	\Name{M.~Watanabe},\institute{6}
	and
	\Name{X.~Wei},\institute{2}
}

\INSTITUTE{1}{RIKEN Nishina Center}
\INSTITUTE{2}{Department of Physics and Astronomy, Purdue University}
\INSTITUTE{3}{Physics Department, Brookhaven National Laboratory}
\INSTITUTE{4}{Instrumentation Division, Brookhaven National Laboratory}
\INSTITUTE{5}{Department of Physics, Rikkyo University}
\INSTITUTE{6}{Department of Mathematical and Physical Sciences, Nara Women's University}
\INSTITUTE{7}{Japan Atomic Energy Agency}
\INSTITUTE{8}{Department of Physics, Korea University}
\INSTITUTE{9}{Tokyo Metropolitan Industrial Technology Research Institute}
\INSTITUTE{10}{Department of Physics, National Central University}
\INSTITUTE{11}{Department of Physics, National Taiwan University}
```
The origian author list is in the script.
You can change it if needed.

#### ver JFY2024
The authors' e-mail addresses are printed at the bottom of the output. Please use it to circulate your draft. 
You can execute author_list_generator_JFY2024.py like a shell script file:
```
./author_list_generator_JFY2024.py
```

* Author: Genki Nukazuka (RIKEN), 12/25/2023.
