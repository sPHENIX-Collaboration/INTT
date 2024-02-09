#ifdef __CINT__

#pragma link C++ class foo + ;
#pragma link C++ struct foo::bar + ;
#pragma link C++ struct foo::bar_comparator + ;

#endif /* __CINT___ */
