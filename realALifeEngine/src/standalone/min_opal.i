//Minimal WRAPPING of the external class

/*
%typemap(in,checkfn="lua_isnumber") opal::real *INPUT(opal::real temp), opal::real &INPUT(opal::real temp)
%{ temp = (opal::real)lua_tonumber(L,$input);
   $1 = &temp; %}

%typemap(in, numinputs=0) opal::real *OUTPUT (opal::real temp)
%{ $1 = &temp; %}

%typemap(argout) opal::real *OUTPUT
%{  lua_pushnumber(L, (double) *$1); SWIG_arg++;%}

%typemap(in) opal::real *INOUT = opal::real *INPUT;
%typemap(argout) opal::real *INOUT = opal::real *OUTPUT;
*/
//SWIG_NUMBER_TYPEMAP(opal::real);
%apply opal::real *INOUT{opal::real *};
%apply opal::real &INPUT{opal::real &};
//%apply Vec3r *INOUT{Vec3r *};
//%apply Vec3r &INPUT{Vec3r &};


// Minimal Opal Wrapping 
%include "opal/Portability.h"
%include "opal/EventHandler.h"
%include "opal/CollisionEventHandler.h"
%include "opal/OpalMath.h"

%ignore opal::real;

/*
%ignore operator<<(std::ostream& o, const Vec3r& v);
//%ignore operator+(const Vec3r &u, const Vec3r &v);
//%rename(__add__) Vec3r::operator+;
%ignore operator<<(std::ostream& o, const Point3r& p);
%ignore operator<<(std::ostream& o, const Rayr& r);
%ignore operator<<(std::ostream& o, const Quaternion& q);
%ignore operator<<(std::ostream& o, const Matrix44r& m);

%include "opal/Vec3r.h"
%include "opal/Point3r.h"
%include "opal/Rayr.h"
%include "opal/Quaternion.h"
%include "opal/Matrix44r.h"
*/

namespace opal
{
    class Point3r
    {
        public:
            /// X-coordinate
            real x;

            /// Y-coordinate
            real y;

            /// Z-coordinate
            real z;

            Point3r()
            {
                x = 0;
                y = 0;
                z = 0;
            }

            Point3r(const Point3r & src)
            {
                x = src.x;
                y = src.y;
                z = src.z;
            }

            Point3r(real xx, real yy, real zz)
            {
                x = xx;
                y = yy;
                z = zz;
            }

            Point3r(const real * data)
            {
                x = data[0];
                y = data[1];
                z = data[2];
            }

            inline void set(real xx, real yy, real zz)
            {
                x = xx;
                y = yy;
                z = zz;
            }

            inline void set(real * data)
            {
                x = data[0];
                y = data[1];
                z = data[2];
            }

            inline real & operator[] (unsigned int i)
            {
                switch (i)
                {
                    case 0:
                        return x;
                    case 1:
                        return y;
                    case 2:
                        return z;
                    default:
                        assert(i < 3);
                        return z;
                }
            }

            inline const real & operator[] (unsigned int i) const
            {
                switch (i)
                {
                    case 0:
                        return x;
                    case 1:
                        return y;
                    case 2:
                        return z;
                    default:
                        assert(i < 3);
                        return z;
                }
            }

            inline void operator+=(const Vec3r & v)
            {
                x += v.x;
                y += v.y;
                z += v.z;
            }

            inline void operator-=(const Vec3r & v)
            {
                x -= v.x;
                y -= v.y;
                z -= v.z;
            }

            inline void operator*=(real scalar)
            {
                x *= scalar;
                y *= scalar;
                z *= scalar;
            }

            inline void operator/=(real scalar)
            {
                x /= scalar;
                y /= scalar;
                z /= scalar;
            }

            inline bool operator==(const Point3r & v) const
            {
                return (x == v.x && y == v.y && z == v.z);
            }

            inline bool operator!=(const Point3r & v) const
            {
                return (x != v.x || y != v.y || z != v.z);
            }
    };
    
    class Vec3r
    {
        public:
            /// x-coordinate
            real x;

            /// y-coordinate
            real y;

            /// z-coordinate
            real z;

            /// default - (0,0,0)
            OPAL_DECL Vec3r();

            OPAL_DECL Vec3r(const Vec3r & src);

            OPAL_DECL Vec3r(real xx, real yy, real zz);

            OPAL_DECL Vec3r(const real * data);

            OPAL_DECL virtual void OPAL_CALL set(real xx, real yy, real zz);

            OPAL_DECL virtual void OPAL_CALL set(real * data);

            OPAL_DECL virtual real & OPAL_CALL operator[] (unsigned int i);

            OPAL_DECL virtual const real & OPAL_CALL operator[] (unsigned int i) const;

            OPAL_DECL virtual real OPAL_CALL lengthSquared() const;

            OPAL_DECL virtual real OPAL_CALL length() const;

            OPAL_DECL virtual void OPAL_CALL normalize();

            OPAL_DECL virtual Vec3r OPAL_CALL unit() const;

            OPAL_DECL virtual void OPAL_CALL operator+=(const Vec3r & v);

            OPAL_DECL virtual void OPAL_CALL operator-=(const Vec3r & v);

            OPAL_DECL virtual void OPAL_CALL operator*=(const Vec3r & v);

            OPAL_DECL virtual void OPAL_CALL operator*=(real scalar);

            OPAL_DECL virtual void OPAL_CALL operator/=(real scalar);

            OPAL_DECL virtual void OPAL_CALL operator/=(const Vec3r & v);

            OPAL_DECL virtual bool OPAL_CALL operator==(const Vec3r & v) const;

            OPAL_DECL virtual bool OPAL_CALL operator!=(const Vec3r & v) const;

            OPAL_DECL virtual void OPAL_CALL operator=(const Vec3r & v);
    };
    
}

//%include "opal/Simulator.h"

