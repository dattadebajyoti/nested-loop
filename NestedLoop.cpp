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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "NestedLoop.hpp"


//===========================================
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <chrono> 
using namespace std;
using namespace std::chrono;

using vec = vector<string>;
using matrix = vector<vec>;
//=============================================
//store the matrix in a vector
matrix readCSV(string filename)
{
    char separator = ',';
    matrix result;
    string row, item;

    ifstream in(filename);
    while (getline(in, row))
    {
        vec R;
        stringstream ss(row);
        while (getline(ss, item, separator)) R.push_back(item);
        result.push_back(R);
    }
    in.close();
    return result;
}

//=================================================

void printMatrix(const matrix& M)
{
    for (vec row : M)
    {
        for (string s : row) cout << setw(12) << left << s << " ";  
        cout << '\n';
    }
}

//======================================================================

MatMult::MatMult(OMR::JitBuilder::TypeDictionary *types)
   : OMR::JitBuilder::MethodBuilder(types)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("matmult");

   
   pString = types->PointerTo(types->toIlType<char *>());
   pStr = types->toIlType<char *>();
   DefineParameter("A", pString);
   DefineParameter("B", pString);
   DefineParameter("R", pString);
   DefineParameter("N", Int32);
   DefineParameter("res", Int32);
   DefineReturnType(Int32);
   }

//==================Load and store the strings=====================================================   




void
MatMult::Store2DString(OMR::JitBuilder::IlBuilder *bldr,
                 OMR::JitBuilder::IlValue *base,
                 OMR::JitBuilder::IlValue *first,
                 OMR::JitBuilder::IlValue *second,
                 OMR::JitBuilder::IlValue *N,
                 OMR::JitBuilder::IlValue *value)
   {
   bldr->StoreAt(
   bldr->   IndexAt(pString,
               base,
               first),
            value);
   }

OMR::JitBuilder::IlValue *
MatMult::Load2DString(OMR::JitBuilder::IlBuilder *bldr,
                OMR::JitBuilder::IlValue *base,
                 OMR::JitBuilder::IlValue *first,
                 OMR::JitBuilder::IlValue *second,
                 OMR::JitBuilder::IlValue *N)
   {
   return
      bldr->LoadAt(pString,
      bldr->   IndexAt(pString,
                  base,
                  first));
   }






//=================================================BUILDIL for String=============================================
bool
MatMult::buildIL()
   {
   

   OMR::JitBuilder::IlValue *A_val, *B_val;

   //store no_res only if a value is not match for both arrays
   OMR::JitBuilder::IlValue *nores_ik = ConstString("Not Found");

   OMR::JitBuilder::IlValue *A = Load("A");

   OMR::JitBuilder::IlValue *B = Load("B");

   OMR::JitBuilder::IlValue *R = Load("R");

   OMR::JitBuilder::IlValue *N = Load("N");


   
   DefineLocal("result", Int32);
   Store("result", ConstInt32(0));

   OMR::JitBuilder::IlBuilder *aLoop=NULL;
   ForLoopUp("a", &aLoop,
             ConstInt32(1),
             Load("N"),
             ConstInt32(1));

   OMR::JitBuilder::IlBuilder *bLoop=NULL;
   aLoop->ForLoopUp("b", &bLoop,
   aLoop->          ConstInt32(1),
   aLoop->          Load("N"),
   aLoop->          ConstInt32(1));
   OMR::JitBuilder::IlBuilder *res=NULL, *nores=NULL;

   bLoop->           IfThenElse(&res, &nores,
   bLoop->              EqualTo(
   bLoop->                 LoadAt(pStr,
   bLoop->                    IndexAt(pStr,
                                       A,
   bLoop->                             Load("a"))),
   bLoop->                 LoadAt(pStr,
   bLoop->                    IndexAt(pStr,
                                       B,
   bLoop->                             Load("b")))
                     ));
   //If value matched
   res->             StoreAt(
   res->                IndexAt(pString,
                           R,
   res->                   Load("a")),
   res->                      LoadAt(pString,
   res->                       IndexAt(pString,
                                       A,
   res->                               Load("a")))
                     );
   res->             Store("result",Add(
                        Load("result"),
                        ConstInt32(1)
                     ));
   //If value not matched
   nores->           StoreAt(
   nores->              IndexAt(pString,
                           R,
   nores->                 Load("a")),
                              nores_ik
                     );
   
   

   Return(
      Load("result"));

   return true;
   }
//======================================================================================================






//print the array
void 
printStringMatrix(const char **S, int32_t M){
   for(int32_t i = 1; i < M; i++){
      cout<<S[i]<<endl;
   }
   cout<<"\n";
}



void
MatMult::run(int n)
   {
   //read matrices before initialising
   printf("Step 0: read matrices before initialising\n");
   //read csv into temp1 and temp2
   matrix temp1 = readCSV("data1_10000.csv");
   matrix temp2 = readCSV("data2_10000.csv");
   //================================================================================
   //define the 1-D array size
   const int32_t M_final=temp1.size();


  
   //================================================================================
   printf("Step 1: initialize JIT\n");
   bool initialized = initializeJit();
   if (!initialized)
      {
      fprintf(stderr, "FAIL: could not initialize JIT\n");
      exit(-1);
      }

   printf("Step 2: define matrices\n");
   
   const char* E_M[M_final];
   const char* E_M1[M_final];
   const char* R[M_final];

   for(int32_t i = 1; i < M_final; i++){
         E_M[i] = temp1[i][3].c_str();
         E_M1[i] = temp2[i][1].c_str();
   }


   printf("Step 3: define type dictionaries\n");
   OMR::JitBuilder::TypeDictionary types;

   printf("Step 4: compile MatMult method builder\n");
   MatMult method(&types);
   void *entry=0;
   
   int32_t rc = compileMethodBuilder(&method, &entry);
   if (rc != 0)
      {
      
      fprintf(stderr,"FAIL: compilation error %d\n", rc);
      exit(-2);
      }
  
   printf("Step 5: invoke MatMult compiled code\n");
   MatMultFunctionType *test = (MatMultFunctionType *)entry;
   int32_t res = 0;
   int32_t Count = test(E_M, E_M1, R, M_final, res);
  
   
   cout<<"print res: "<<Count<<endl;
   printStringMatrix(R, M_final);
   
   printf ("Step 6: shutdown JIT\n");
   shutdownJit();

   printf("PASS\n");
   }
