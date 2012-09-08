// vector.h
// Vector
// by Joe Flint
// Vector object
// constists of a single point and three components

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Apparently gcc 3.4 uses _VECTOR_H within STL vector
#ifndef _BRUTALCHESS_VECTOR_H
#define _BRUTALCHESS_VECTOR_H

/** A magnitude and a direction.
  * Describes a direction in space.
  */
class Vector
{
	public:
		Vector();
		Vector( double xcomp, double ycomp, double zcomp );
		void glScale() const;
		void glRotate() const;
		void glNormal() const;
		void glTranslate() const;
		double magnitude() const;
		void normalize();
		void scale( double s );
		double dot( Vector v ) const;
		Vector cross( Vector v ) const;
	
		Vector operator+( const Vector & rhs ) const;
		Vector operator-( const Vector & rhs ) const;

		const Vector & operator+=( const Vector & rhs );
		const Vector & operator-=( const Vector & rhs );

		const Vector operator*( const Vector & rhs ) const;
		const Vector operator/( const Vector & rhs ) const;
		const Vector operator*( const double & rhs ) const;
		const Vector operator/( const double & rhs ) const;
	
		bool operator==( const Vector & rhs ) const;
		bool operator!=( const Vector & rhs ) const;
	
		/** x component of the vector */
		double x; 
		/** y component of the vector */
		double y; 
		/** z component of the vector */
		double z;
};
#endif
