/*******************************************************************************
 * Copyright (c) 2016, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#ifndef NESTEDLOOP_INCL
#define NESTEDLOOP_INCL

#include "JitBuilder.hpp"
using namespace std;

//typedef void (MatMultFunctionType)(double *, double *, double *, int32_t);
typedef int32_t (MatMultFunctionType)(const char **, const char **, const char **,int32_t, int32_t);

class MatMult : public OMR::JitBuilder::MethodBuilder
   {
   private:
   OMR::JitBuilder::IlType *pDouble;
   OMR::JitBuilder::IlType *pString;
   OMR::JitBuilder::IlType *pStr;

   


   void Store2DString(OMR::JitBuilder::IlBuilder *bldr,
                OMR::JitBuilder::IlValue *base,
                OMR::JitBuilder::IlValue *first,
                OMR::JitBuilder::IlValue *second,
                OMR::JitBuilder::IlValue *N,
                OMR::JitBuilder::IlValue *value);

   

   OMR::JitBuilder::IlValue *Load2DString(OMR::JitBuilder::IlBuilder *bldr,
                                    OMR::JitBuilder::IlValue *base,
                                    OMR::JitBuilder::IlValue *first,
                                    OMR::JitBuilder::IlValue *second,
                                    OMR::JitBuilder::IlValue *N);



   public:
   MatMult(OMR::JitBuilder::TypeDictionary *);
   virtual bool buildIL();
   static void run(int32_t);
   };


#endif // !defined(MATMULT_INCL)
