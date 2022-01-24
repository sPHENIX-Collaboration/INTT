// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TrackMCDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "TrackMC.hh"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_TrackMC(void *p = 0);
   static void *newArray_TrackMC(Long_t size, void *p);
   static void delete_TrackMC(void *p);
   static void deleteArray_TrackMC(void *p);
   static void destruct_TrackMC(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TrackMC*)
   {
      ::TrackMC *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TrackMC >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TrackMC", ::TrackMC::Class_Version(), "TrackMC.hh", 10,
                  typeid(::TrackMC), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TrackMC::Dictionary, isa_proxy, 4,
                  sizeof(::TrackMC) );
      instance.SetNew(&new_TrackMC);
      instance.SetNewArray(&newArray_TrackMC);
      instance.SetDelete(&delete_TrackMC);
      instance.SetDeleteArray(&deleteArray_TrackMC);
      instance.SetDestructor(&destruct_TrackMC);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TrackMC*)
   {
      return GenerateInitInstanceLocal((::TrackMC*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TrackMC*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TrackMC::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TrackMC::Class_Name()
{
   return "TrackMC";
}

//______________________________________________________________________________
const char *TrackMC::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TrackMC*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TrackMC::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TrackMC*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TrackMC::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TrackMC*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TrackMC::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TrackMC*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TrackMC::Streamer(TBuffer &R__b)
{
   // Stream an object of class TrackMC.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TrackMC::Class(),this);
   } else {
      R__b.WriteClassBuffer(TrackMC::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TrackMC(void *p) {
      return  p ? new(p) ::TrackMC : new ::TrackMC;
   }
   static void *newArray_TrackMC(Long_t nElements, void *p) {
      return p ? new(p) ::TrackMC[nElements] : new ::TrackMC[nElements];
   }
   // Wrapper around operator delete
   static void delete_TrackMC(void *p) {
      delete ((::TrackMC*)p);
   }
   static void deleteArray_TrackMC(void *p) {
      delete [] ((::TrackMC*)p);
   }
   static void destruct_TrackMC(void *p) {
      typedef ::TrackMC current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TrackMC

namespace ROOT {
   static TClass *vectorlEvectorlETrackMCgRsPgR_Dictionary();
   static void vectorlEvectorlETrackMCgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEvectorlETrackMCgRsPgR(void *p = 0);
   static void *newArray_vectorlEvectorlETrackMCgRsPgR(Long_t size, void *p);
   static void delete_vectorlEvectorlETrackMCgRsPgR(void *p);
   static void deleteArray_vectorlEvectorlETrackMCgRsPgR(void *p);
   static void destruct_vectorlEvectorlETrackMCgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<vector<TrackMC> >*)
   {
      vector<vector<TrackMC> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<vector<TrackMC> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<vector<TrackMC> >", -2, "vector", 469,
                  typeid(vector<vector<TrackMC> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEvectorlETrackMCgRsPgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<vector<TrackMC> >) );
      instance.SetNew(&new_vectorlEvectorlETrackMCgRsPgR);
      instance.SetNewArray(&newArray_vectorlEvectorlETrackMCgRsPgR);
      instance.SetDelete(&delete_vectorlEvectorlETrackMCgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEvectorlETrackMCgRsPgR);
      instance.SetDestructor(&destruct_vectorlEvectorlETrackMCgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<vector<TrackMC> > >()));

      ::ROOT::AddClassAlternate("vector<vector<TrackMC> >","std::__1::vector<std::__1::vector<TrackMC, std::__1::allocator<TrackMC> >, std::__1::allocator<std::__1::vector<TrackMC, std::__1::allocator<TrackMC> > > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<vector<TrackMC> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEvectorlETrackMCgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<vector<TrackMC> >*)0x0)->GetClass();
      vectorlEvectorlETrackMCgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEvectorlETrackMCgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEvectorlETrackMCgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<vector<TrackMC> > : new vector<vector<TrackMC> >;
   }
   static void *newArray_vectorlEvectorlETrackMCgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<vector<TrackMC> >[nElements] : new vector<vector<TrackMC> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEvectorlETrackMCgRsPgR(void *p) {
      delete ((vector<vector<TrackMC> >*)p);
   }
   static void deleteArray_vectorlEvectorlETrackMCgRsPgR(void *p) {
      delete [] ((vector<vector<TrackMC> >*)p);
   }
   static void destruct_vectorlEvectorlETrackMCgRsPgR(void *p) {
      typedef vector<vector<TrackMC> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<vector<TrackMC> >

namespace ROOT {
   static TClass *vectorlEpairlETVector3mUcOTVector3mUgRsPgR_Dictionary();
   static void vectorlEpairlETVector3mUcOTVector3mUgRsPgR_TClassManip(TClass*);
   static void *new_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p = 0);
   static void *newArray_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(Long_t size, void *p);
   static void delete_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p);
   static void deleteArray_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p);
   static void destruct_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<pair<TVector3*,TVector3*> >*)
   {
      vector<pair<TVector3*,TVector3*> > *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<pair<TVector3*,TVector3*> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<pair<TVector3*,TVector3*> >", -2, "vector", 469,
                  typeid(vector<pair<TVector3*,TVector3*> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEpairlETVector3mUcOTVector3mUgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<pair<TVector3*,TVector3*> >) );
      instance.SetNew(&new_vectorlEpairlETVector3mUcOTVector3mUgRsPgR);
      instance.SetNewArray(&newArray_vectorlEpairlETVector3mUcOTVector3mUgRsPgR);
      instance.SetDelete(&delete_vectorlEpairlETVector3mUcOTVector3mUgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlEpairlETVector3mUcOTVector3mUgRsPgR);
      instance.SetDestructor(&destruct_vectorlEpairlETVector3mUcOTVector3mUgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<pair<TVector3*,TVector3*> > >()));

      ::ROOT::AddClassAlternate("vector<pair<TVector3*,TVector3*> >","std::__1::vector<std::__1::pair<TVector3*, TVector3*>, std::__1::allocator<std::__1::pair<TVector3*, TVector3*> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<pair<TVector3*,TVector3*> >*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEpairlETVector3mUcOTVector3mUgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<pair<TVector3*,TVector3*> >*)0x0)->GetClass();
      vectorlEpairlETVector3mUcOTVector3mUgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEpairlETVector3mUcOTVector3mUgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<pair<TVector3*,TVector3*> > : new vector<pair<TVector3*,TVector3*> >;
   }
   static void *newArray_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<pair<TVector3*,TVector3*> >[nElements] : new vector<pair<TVector3*,TVector3*> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p) {
      delete ((vector<pair<TVector3*,TVector3*> >*)p);
   }
   static void deleteArray_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p) {
      delete [] ((vector<pair<TVector3*,TVector3*> >*)p);
   }
   static void destruct_vectorlEpairlETVector3mUcOTVector3mUgRsPgR(void *p) {
      typedef vector<pair<TVector3*,TVector3*> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<pair<TVector3*,TVector3*> >

namespace ROOT {
   static TClass *vectorlETrackMCgR_Dictionary();
   static void vectorlETrackMCgR_TClassManip(TClass*);
   static void *new_vectorlETrackMCgR(void *p = 0);
   static void *newArray_vectorlETrackMCgR(Long_t size, void *p);
   static void delete_vectorlETrackMCgR(void *p);
   static void deleteArray_vectorlETrackMCgR(void *p);
   static void destruct_vectorlETrackMCgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TrackMC>*)
   {
      vector<TrackMC> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TrackMC>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TrackMC>", -2, "vector", 469,
                  typeid(vector<TrackMC>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETrackMCgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TrackMC>) );
      instance.SetNew(&new_vectorlETrackMCgR);
      instance.SetNewArray(&newArray_vectorlETrackMCgR);
      instance.SetDelete(&delete_vectorlETrackMCgR);
      instance.SetDeleteArray(&deleteArray_vectorlETrackMCgR);
      instance.SetDestructor(&destruct_vectorlETrackMCgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TrackMC> >()));

      ::ROOT::AddClassAlternate("vector<TrackMC>","std::__1::vector<TrackMC, std::__1::allocator<TrackMC> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TrackMC>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETrackMCgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TrackMC>*)0x0)->GetClass();
      vectorlETrackMCgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETrackMCgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETrackMCgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TrackMC> : new vector<TrackMC>;
   }
   static void *newArray_vectorlETrackMCgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TrackMC>[nElements] : new vector<TrackMC>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETrackMCgR(void *p) {
      delete ((vector<TrackMC>*)p);
   }
   static void deleteArray_vectorlETrackMCgR(void *p) {
      delete [] ((vector<TrackMC>*)p);
   }
   static void destruct_vectorlETrackMCgR(void *p) {
      typedef vector<TrackMC> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TrackMC>

namespace ROOT {
   static TClass *vectorlETrackMCmUgR_Dictionary();
   static void vectorlETrackMCmUgR_TClassManip(TClass*);
   static void *new_vectorlETrackMCmUgR(void *p = 0);
   static void *newArray_vectorlETrackMCmUgR(Long_t size, void *p);
   static void delete_vectorlETrackMCmUgR(void *p);
   static void deleteArray_vectorlETrackMCmUgR(void *p);
   static void destruct_vectorlETrackMCmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TrackMC*>*)
   {
      vector<TrackMC*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TrackMC*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TrackMC*>", -2, "vector", 469,
                  typeid(vector<TrackMC*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETrackMCmUgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<TrackMC*>) );
      instance.SetNew(&new_vectorlETrackMCmUgR);
      instance.SetNewArray(&newArray_vectorlETrackMCmUgR);
      instance.SetDelete(&delete_vectorlETrackMCmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlETrackMCmUgR);
      instance.SetDestructor(&destruct_vectorlETrackMCmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TrackMC*> >()));

      ::ROOT::AddClassAlternate("vector<TrackMC*>","std::__1::vector<TrackMC*, std::__1::allocator<TrackMC*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TrackMC*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETrackMCmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TrackMC*>*)0x0)->GetClass();
      vectorlETrackMCmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETrackMCmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETrackMCmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TrackMC*> : new vector<TrackMC*>;
   }
   static void *newArray_vectorlETrackMCmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TrackMC*>[nElements] : new vector<TrackMC*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETrackMCmUgR(void *p) {
      delete ((vector<TrackMC*>*)p);
   }
   static void deleteArray_vectorlETrackMCmUgR(void *p) {
      delete [] ((vector<TrackMC*>*)p);
   }
   static void destruct_vectorlETrackMCmUgR(void *p) {
      typedef vector<TrackMC*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TrackMC*>

namespace ROOT {
   static TClass *vectorlETVector3mUgR_Dictionary();
   static void vectorlETVector3mUgR_TClassManip(TClass*);
   static void *new_vectorlETVector3mUgR(void *p = 0);
   static void *newArray_vectorlETVector3mUgR(Long_t size, void *p);
   static void delete_vectorlETVector3mUgR(void *p);
   static void deleteArray_vectorlETVector3mUgR(void *p);
   static void destruct_vectorlETVector3mUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TVector3*>*)
   {
      vector<TVector3*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TVector3*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TVector3*>", -2, "vector", 469,
                  typeid(vector<TVector3*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETVector3mUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TVector3*>) );
      instance.SetNew(&new_vectorlETVector3mUgR);
      instance.SetNewArray(&newArray_vectorlETVector3mUgR);
      instance.SetDelete(&delete_vectorlETVector3mUgR);
      instance.SetDeleteArray(&deleteArray_vectorlETVector3mUgR);
      instance.SetDestructor(&destruct_vectorlETVector3mUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TVector3*> >()));

      ::ROOT::AddClassAlternate("vector<TVector3*>","std::__1::vector<TVector3*, std::__1::allocator<TVector3*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TVector3*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETVector3mUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TVector3*>*)0x0)->GetClass();
      vectorlETVector3mUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETVector3mUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETVector3mUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TVector3*> : new vector<TVector3*>;
   }
   static void *newArray_vectorlETVector3mUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TVector3*>[nElements] : new vector<TVector3*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETVector3mUgR(void *p) {
      delete ((vector<TVector3*>*)p);
   }
   static void deleteArray_vectorlETVector3mUgR(void *p) {
      delete [] ((vector<TVector3*>*)p);
   }
   static void destruct_vectorlETVector3mUgR(void *p) {
      typedef vector<TVector3*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TVector3*>

namespace {
  void TriggerDictionaryInitialization_TrackMCDict_Impl() {
    static const char* headers[] = {
"TrackMC.hh",
0
    };
    static const char* includePaths[] = {
"/Users/genki/soft/ROOT/6.24.00/root/include/",
"/Users/genki/Nextcloud/Physics/sPhenix/INTT/repo/INTT/Testbeam_G4_code/session7_solution/lib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "TrackMCDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$TrackMC.hh")))  TrackMC;
namespace std{inline namespace __1{template <class _Tp> class __attribute__((annotate("$clingAutoload$iosfwd")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}}
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "TrackMCDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "TrackMC.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"TrackMC", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("TrackMCDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_TrackMCDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_TrackMCDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_TrackMCDict() {
  TriggerDictionaryInitialization_TrackMCDict_Impl();
}
