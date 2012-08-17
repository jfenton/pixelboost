// Copyright (c) The HLSL2GLSLFork Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef _CONSTANT_UNION_INCLUDED_
#define _CONSTANT_UNION_INCLUDED_


class constUnion
{
public:
   POOL_ALLOCATOR_NEW_DELETE(GlobalPoolAllocator)

   void setIConst(int i) { iConst = i; type = EbtInt; }
   void setFConst(float f) { fConst = f; type = EbtFloat; }
   void setBConst(bool b) { bConst = b; type = EbtBool; }

   int getIConst() const { return iConst; }
   float getFConst() const { return fConst; }
   bool getBConst() const { return bConst; }

   bool operator==(const int i) const { return (i == iConst); }
   bool operator==(const float f) const { return (f == fConst); }
   bool operator==(const bool b) const { return (b == bConst); }

   bool operator==(const constUnion& constant) const
   {
      if (constant.type != type)
         return false;

      switch (type)
      {
      case EbtInt:
         if (constant.iConst == iConst)
            return true;

         break;
      case EbtFloat:
         if (constant.fConst == fConst)
            return true;

         break;
      case EbtBool:
         if (constant.bConst == bConst)
            return true;

         break;
      }

      return false;
   }

   bool operator!=(const int i) const { return !operator==(i); }
   bool operator!=(const float f) const { return !operator==(f); }
   bool operator!=(const bool b) const { return !operator==(b); }
   bool operator!=(const constUnion& constant) const { return !operator==(constant); }

   bool operator>(const constUnion& constant) const
   {
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt:
         if (iConst > constant.iConst)
            return true;

         return false;
      case EbtFloat:
         if (fConst > constant.fConst)
            return true;

         return false;
      default:
         assert(false && "Default missing");
         return false;
      }

      return false;
   }

   bool operator<(const constUnion& constant) const
   {
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt:
         if (iConst < constant.iConst)
            return true;

         return false;
      case EbtFloat:
         if (fConst < constant.fConst)
            return true;

         return false;
      default:
         assert(false && "Default missing");
         return false;
      }

      return false;
   }

   constUnion operator+(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt: returnValue.setIConst(iConst + constant.iConst); break;
      case EbtFloat: returnValue.setFConst(fConst + constant.fConst); break;
      default: assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator-(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt: returnValue.setIConst(iConst - constant.iConst); break;
      case EbtFloat: returnValue.setFConst(fConst - constant.fConst); break;
      default: assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator*(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt: returnValue.setIConst(iConst * constant.iConst); break;
      case EbtFloat: returnValue.setFConst(fConst * constant.fConst); break; 
      default: assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator%(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt: returnValue.setIConst(iConst % constant.iConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator>>(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt: returnValue.setIConst(iConst >> constant.iConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator<<(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt: returnValue.setIConst(iConst << constant.iConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator&(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt:  returnValue.setIConst(iConst & constant.iConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator|(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt:  returnValue.setIConst(iConst | constant.iConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator^(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtInt:  returnValue.setIConst(iConst ^ constant.iConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator&&(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtBool: returnValue.setBConst(bConst && constant.bConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   constUnion operator||(const constUnion& constant) const
   {
      constUnion returnValue;
      assert(type == constant.type);
      switch (type)
      {
      case EbtBool: returnValue.setBConst(bConst || constant.bConst); break;
      default:     assert(false && "Default missing");
      }

      return returnValue;
   }

   TBasicType getType() const { return type; }

private:

   union
   {
      int iConst;  // used for ivec, scalar ints
      bool bConst; // used for bvec, scalar bools
      float fConst;   // used for vec, mat, scalar floats
   } ;

   TBasicType type;
};

#endif // _CONSTANT_UNION_INCLUDED_

