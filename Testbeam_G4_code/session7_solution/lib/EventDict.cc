// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME EventDict
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
#include "Event.hh"

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

namespace ROOT {
   static void *new_Event(void *p = 0);
   static void *newArray_Event(Long_t size, void *p);
   static void delete_Event(void *p);
   static void deleteArray_Event(void *p);
   static void destruct_Event(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Event*)
   {
      ::Event *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::Event >(0);
      static ::ROOT::TGenericClassInfo 
         instance("Event", ::Event::Class_Version(), "Event.hh", 8,
                  typeid(::Event), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::Event::Dictionary, isa_proxy, 4,
                  sizeof(::Event) );
      instance.SetNew(&new_Event);
      instance.SetNewArray(&newArray_Event);
      instance.SetDelete(&delete_Event);
      instance.SetDeleteArray(&deleteArray_Event);
      instance.SetDestructor(&destruct_Event);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Event*)
   {
      return GenerateInitInstanceLocal((::Event*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Event*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
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
atomic_TClass_ptr Event::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *Event::Class_Name()
{
   return "Event";
}

//______________________________________________________________________________
const char *Event::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int Event::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *Event::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *Event::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetClass(); }
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

//______________________________________________________________________________
void Event::Streamer(TBuffer &R__b)
{
   // Stream an object of class Event.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(Event::Class(),this);
   } else {
      R__b.WriteClassBuffer(Event::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Event(void *p) {
      return  p ? new(p) ::Event : new ::Event;
   }
   static void *newArray_Event(Long_t nElements, void *p) {
      return p ? new(p) ::Event[nElements] : new ::Event[nElements];
   }
   // Wrapper around operator delete
   static void delete_Event(void *p) {
      delete ((::Event*)p);
   }
   static void deleteArray_Event(void *p) {
      delete [] ((::Event*)p);
   }
   static void destruct_Event(void *p) {
      typedef ::Event current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Event

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
   static TClass *vectorlEintmUgR_Dictionary();
   static void vectorlEintmUgR_TClassManip(TClass*);
   static void *new_vectorlEintmUgR(void *p = 0);
   static void *newArray_vectorlEintmUgR(Long_t size, void *p);
   static void delete_vectorlEintmUgR(void *p);
   static void deleteArray_vectorlEintmUgR(void *p);
   static void destruct_vectorlEintmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int*>*)
   {
      vector<int*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int*>", -2, "vector", 469,
                  typeid(vector<int*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int*>) );
      instance.SetNew(&new_vectorlEintmUgR);
      instance.SetNewArray(&newArray_vectorlEintmUgR);
      instance.SetDelete(&delete_vectorlEintmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintmUgR);
      instance.SetDestructor(&destruct_vectorlEintmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int*> >()));

      ::ROOT::AddClassAlternate("vector<int*>","std::__1::vector<int*, std::__1::allocator<int*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<int*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<int*>*)0x0)->GetClass();
      vectorlEintmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<int*> : new vector<int*>;
   }
   static void *newArray_vectorlEintmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<int*>[nElements] : new vector<int*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintmUgR(void *p) {
      delete ((vector<int*>*)p);
   }
   static void deleteArray_vectorlEintmUgR(void *p) {
      delete [] ((vector<int*>*)p);
   }
   static void destruct_vectorlEintmUgR(void *p) {
      typedef vector<int*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<int*>

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

namespace ROOT {
   static TClass *vectorlEEventgR_Dictionary();
   static void vectorlEEventgR_TClassManip(TClass*);
   static void *new_vectorlEEventgR(void *p = 0);
   static void *newArray_vectorlEEventgR(Long_t size, void *p);
   static void delete_vectorlEEventgR(void *p);
   static void deleteArray_vectorlEEventgR(void *p);
   static void destruct_vectorlEEventgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<Event>*)
   {
      vector<Event> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<Event>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<Event>", -2, "vector", 469,
                  typeid(vector<Event>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEEventgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<Event>) );
      instance.SetNew(&new_vectorlEEventgR);
      instance.SetNewArray(&newArray_vectorlEEventgR);
      instance.SetDelete(&delete_vectorlEEventgR);
      instance.SetDeleteArray(&deleteArray_vectorlEEventgR);
      instance.SetDestructor(&destruct_vectorlEEventgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<Event> >()));

      ::ROOT::AddClassAlternate("vector<Event>","std::__1::vector<Event, std::__1::allocator<Event> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<Event>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEEventgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<Event>*)0x0)->GetClass();
      vectorlEEventgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEEventgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEEventgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<Event> : new vector<Event>;
   }
   static void *newArray_vectorlEEventgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<Event>[nElements] : new vector<Event>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEEventgR(void *p) {
      delete ((vector<Event>*)p);
   }
   static void deleteArray_vectorlEEventgR(void *p) {
      delete [] ((vector<Event>*)p);
   }
   static void destruct_vectorlEEventgR(void *p) {
      typedef vector<Event> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<Event>

namespace ROOT {
   static TClass *vectorlEEventmUgR_Dictionary();
   static void vectorlEEventmUgR_TClassManip(TClass*);
   static void *new_vectorlEEventmUgR(void *p = 0);
   static void *newArray_vectorlEEventmUgR(Long_t size, void *p);
   static void delete_vectorlEEventmUgR(void *p);
   static void deleteArray_vectorlEEventmUgR(void *p);
   static void destruct_vectorlEEventmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<Event*>*)
   {
      vector<Event*> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<Event*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<Event*>", -2, "vector", 469,
                  typeid(vector<Event*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEEventmUgR_Dictionary, isa_proxy, 4,
                  sizeof(vector<Event*>) );
      instance.SetNew(&new_vectorlEEventmUgR);
      instance.SetNewArray(&newArray_vectorlEEventmUgR);
      instance.SetDelete(&delete_vectorlEEventmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEEventmUgR);
      instance.SetDestructor(&destruct_vectorlEEventmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<Event*> >()));

      ::ROOT::AddClassAlternate("vector<Event*>","std::__1::vector<Event*, std::__1::allocator<Event*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<Event*>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEEventmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<Event*>*)0x0)->GetClass();
      vectorlEEventmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEEventmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEEventmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<Event*> : new vector<Event*>;
   }
   static void *newArray_vectorlEEventmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<Event*>[nElements] : new vector<Event*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEEventmUgR(void *p) {
      delete ((vector<Event*>*)p);
   }
   static void deleteArray_vectorlEEventmUgR(void *p) {
      delete [] ((vector<Event*>*)p);
   }
   static void destruct_vectorlEEventmUgR(void *p) {
      typedef vector<Event*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<Event*>

namespace {
  void TriggerDictionaryInitialization_EventDict_Impl() {
    static const char* headers[] = {
"Event.hh",
0
    };
    static const char* includePaths[] = {
"/Users/genki/soft/ROOT/6.24.00/root/include/",
"/Users/genki/Nextcloud/Physics/sPhenix/INTT/repo/INTT/Testbeam_G4_code/session7_solution/lib/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "EventDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$TrackMC.hh")))  __attribute__((annotate("$clingAutoload$Event.hh")))  TrackMC;
namespace std{inline namespace __1{template <class _Tp> class __attribute__((annotate("$clingAutoload$iosfwd")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}}
class __attribute__((annotate("$clingAutoload$Event.hh")))  Event;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "EventDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "Event.hh"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"Event", payloadCode, "@",
"TrackMC", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("EventDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_EventDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_EventDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_EventDict() {
  TriggerDictionaryInitialization_EventDict_Impl();
}
