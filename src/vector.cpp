// gelvector.h
// GELVector
// OpenGL object structure
// by Joe Flint

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "vector.h"
#include <math.h>
#include "SDL_opengl.h"

/** Default vector constructor.
 *  Vector points in the i + j + k direction (Components are < 1, 1, 1 >).
 */
Vector::Vector()
{
	x = 1.0;
	y = 1.0;
	z = 1.0;
}

/** Create a new vector with specific components.
 *  Components of the new vector are < xcomp, ycomp, zcomp >.
 *  @param xcomp X component of new vector
 *  @param ycomp Y component of new vector
 *  @param zcomp Z component of new vector
 */
Vector::Vector( double xcomp, double ycomp, double zcomp )
{
	// Set the components of the vector
	x = xcomp;
	y = ycomp;
	z = zcomp;
}

void Vector::glScale() const
{
	glScaled( x, y, z );
}

void Vector::glRotate() const
{
	glRotated( x, 1, 0, 0 );
	glRotated( y, 0, 1, 0 );
	glRotated( z, 0, 0, 1 );
}

void Vector::glNormal() const
{
	glNormal3d( x, y, z );
}

void Vector::glTranslate() const
{
	glTranslated( x, y, z );
}

/** Calculate the magnitude of the vector.
  * @return The magnitude of the vector
  */
double Vector::magnitude() const
{
	return sqrt( x * x + y * y + z * z );
}

/** Normalize the vector.
  * Modifies the current vector so that it's magnitude is 1.
  * Direction remains unchanged.
  */
void Vector::normalize()
{
	double m = magnitude();
	x /= m;
	y /= m;
	z /= m;
}

/** Multiply the vector by a scalar.
  * Multiply each component of the vector by the scalar s.
  * @param s The scalar to multiply the vector by
  */
void Vector::scale( double s )
{
	x *= s;
	y *= s;
	z *= s;
}

/** Compute a dot product.
  * Dot the current vector with the vector v.
  * @param v The vector with which to compute the dot product
  * @return The dot product of the vector with vector v
  */
double Vector::dot( Vector v ) const
{
	return ( x * v.x + y * v.y + z * v.z );
}

/** Compute a vector cross product.
  * Cross the current vector with the vector v.
  * @param v The vector with which to compute the cross product
  * @return The vector cross product of the vector with vector v
  */
Vector Vector::cross( Vector v ) const
{
	Vector c;
	c.x = y * v.z - z * v.y;
	c.y = -( x * v.z - z * v.x );
	c.z = x * v.y - y * v.x;
	return c;
}

Vector Vector::operator+( const Vector & rhs ) const
{
	Vector temp;
	temp.x = x + rhs.x;
	temp.y = y + rhs.y;
	temp.z = z + rhs.z;
	
	return temp;
}

Vector Vector::operator-( const Vector & rhs ) const
{
	Vector temp;
	temp.x = x - rhs.x;
	temp.y = y - rhs.y;
	temp.z = z - rhs.z;
	
	return temp;
}

const Vector & Vector::operator+=( const Vector & rhs )
{
	*this = *this + rhs;
	
	return *this;
}

const Vector & Vector::operator-=( const Vector & rhs )
{
	*this = *this - rhs;

	return *this;
}

const Vector Vector::operator*( const Vector & rhs ) const
{
	Vector temp = *this;
	temp.x *= rhs.x;
	temp.y *= rhs.y;
	temp.z *= rhs.z;
	
	return temp;
}

const Vector Vector::operator/( const Vector & rhs ) const
{
	Vector temp = *this;
	temp.x /= rhs.x;
	temp.y /= rhs.y;
	temp.z /= rhs.z;
	
	return temp;
}	

const Vector Vector::operator*( const double & rhs ) const
{
	Vector temp = *this;	
	temp.scale( rhs );

	return temp;
}

const Vector Vector::operator/( const double & rhs ) const
{
	Vector temp = *this;	
	temp.scale( 1 / rhs );

	return temp;
}

bool Vector::operator==( const Vector & rhs ) const
{
	return ( x == rhs.x && y == rhs.y && z == rhs.z );
}

bool Vector::operator!=( const Vector & rhs ) const
{
	return !operator==( rhs );
}
