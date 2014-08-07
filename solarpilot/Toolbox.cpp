#include "Toolbox.h"
#include "math.h"
#include <stdio.h>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

// ----------- points and vectors -----------------------

void Point::Set(double _x, double _y, double _z)
{
	x = _x; 
	y = _y; 
	z = _z;
}

void Point::Set( Point &P )
{
	x = P.x; 
	y = P.y;
	z = P.z;
}

void Point::Add( Point &P )
{
	x+=P.x; 
	y+=P.y; 
	z+=P.z;
}

void Point::Add(double _x, double _y, double _z)
{
	x += _x; 
	y += _y; 
	z += _z;
}

void Point::Subtract( Point &P )
{
	x += -P.x; 
	y += -P.y; 
	z += -P.z;
}

double Point::operator [](const int &index){
	if(index==0) {return x;}
	else if(index==1) {return y;}
	else if(index==2) {return z;}
	else {throw spexception("Index out of range in Point()"); }	//range error
};



void Vect::Set(double _i, double _j, double _k)
{
	i=_i; 
	j=_j; 
	k=_k;
}

void Vect::Set( Vect &V )
{
	i = V.i; 
	j = V.j; 
	k = V.k;
}

void Vect::Add( Vect &V )
{
	i+=V.i; 
	j+=V.j; 
	k+=V.k;
}

void Vect::Subtract( Vect &V )
{ 
	i+=-V.i; 
	j+=-V.j; 
	k+=-V.k;
}

void Vect::Add(double _i, double _j, double _k)
{
	i+=_i; 
	j+=_j; 
	k+=_k;
}

double &Vect::operator[](int index){
	if( index == 0) return i;
	if( index == 1) return j;
	if( index == 2) return k;
	throw spexception("Index out of range in Vect()"); //range error
	return i;
};


	
PointVect::PointVect(const PointVect &v){
	x = v.x; y=v.y; z=v.z;
	i = v.i; j=v.j; k=v.k;
}
PointVect& PointVect::operator= (const PointVect &v) {
	x = v.x; y=v.y; z=v.z;
	i = v.i; j=v.j; k=v.k;
	return *this;
}
PointVect::PointVect(double px, double py, double pz, double vi, double vj, double vk) 
{
	x=px; y=py; z=pz; i=vi; j=vj; k=vk;
}

Point *PointVect::point()
{
	p.Set(x, y, z); 
	return &p;
};

Vect *PointVect::vect()
{
	v.Set(i, j, k); 
	return &v;
};
//-------------------------------------------------------

//------------------ DTObj class -------------------------------------
DTobj::DTobj()
{ 
	setZero();
}
	
void DTobj::setZero()
{
	_year=0;  
	_month=0;  
	_yday=0; 
	_mday=0; 
	_wday=0;  
	_hour=0;
	_min=0;  
	_sec=0;  
	_ms=0;
}
	
DTobj *DTobj::Now()
{
	struct tm now;
#ifdef _MSC_VER
	__time64_t long_time;
	// Get time as 64-bit integer.
	_time64( &long_time ); 
	// Convert to local time.
	_localtime64_s( &now, &long_time ); 
#else
	time_t long_time;
	// Get time as 64-bit integer.
	time( &long_time ); 
	// Convert to local time.
	now = *localtime(&long_time);
#endif
	_year=now.tm_year+1900;  
	_month=now.tm_mon;  
	_yday=now.tm_yday;  
	_mday=now.tm_mday;
	_wday=now.tm_wday; 
	_hour=now.tm_hour;
	_min=now.tm_min;  
	_sec=now.tm_sec;  
	_ms=0;

	return this;
}
//-------------------------------------------------------


//-----------------------DateTime class-------------------
//accessors
	//GETS
int DateTime::GetYear(){return _year;}
int DateTime::GetMonth(){return _month;}
int DateTime::GetYearDay(){return _yday;};
int DateTime::GetMonthDay(){return _mday;}
int DateTime::GetWeekDay(){return _wday;}
int DateTime::GetMinute(){return _min;};
int DateTime::GetSecond() {return _sec;};
int DateTime::GetMillisecond(){return _ms;};
// SETS
void DateTime::SetYear(const int val){_year=val; SetMonthLengths(_year);}
void DateTime::SetMonth(const int val){_month=val;}
void DateTime::SetYearDay(const int val){_yday=val;}
void DateTime::SetMonthDay(const int val){_mday=val;}
void DateTime::SetWeekDay(const int val){_wday=val;}
void DateTime::SetHour(const int val){_hour=val;}
void DateTime::SetMinute(const int val){_min=val;}
void DateTime::SetSecond(const int val){_sec=val;}
void DateTime::SetMillisecond(const int val){_ms=val;}

//Default constructor
DateTime::DateTime(){
	//Initialize all variables to the current date and time
	setDefaults();
}

DateTime::DateTime(double doy, double hour){
	//Convert from fractional hours to integer values for hr, min, sec
	int hr = int(floor(hour));
	double minutes = 60. * (hour - double(hr));
	int min = int(floor(minutes));
	int sec = int(60.*(minutes - double(min)));
	
	setZero();	//Zero values
	SetYearDay(int(doy+.001));	//day of the year
	SetHour(hr);
	SetMinute(min);
	SetSecond(sec);
}

//Fully specified constructor
DateTime::DateTime(DTobj &DT)
{
	_year=DT._year; _month=DT._month; _yday=DT._yday; _mday=DT._mday; _wday=DT._wday;
	_hour=DT._hour; _min=DT._min; _sec=DT._sec; _ms=DT._ms;
	//Initialize the month lengths array
	SetMonthLengths(_year);
}

void DateTime::setDefaults(){
	setZero();	//everything zeros but the day of year and year (hr=12 is noon)
	//Get the current year and set it as the default value
	DTobj N;	
	N.Now();
	//SetYear(N._year);
	SetYear(2011);	//Choose a non-leap year
	SetMonth(6);
	SetMonthDay(21);
	SetYearDay( GetDayOfYear() );	//Summer solstice
	SetHour(12);
	
}

void DateTime::SetDate(int year, int month, int day)
{
	//Given a year, month (1-12), and day of the month (1-N), set the values. Leave the time as it is
	SetYear(year); 
	SetMonth(month); 
	SetMonthDay(day); 
	SetMonthLengths(_year);
	SetYearDay( GetDayOfYear(year, month, day) );
}

void DateTime::SetMonthLengths(const int year){
	/* Calculate the number of days in each month and assign the result to the monthLength[] array.
	Provide special handling for 4-year, 100-year, and 400-year leap years.*/
	//Initialize the array of month lengths (accounting for leap year)
	for (int i=0; i<12; i+=2){ monthLength[i]=31; };
	for (int i=1; i<12; i+=2){ monthLength[i]=30; };
	//Given the year, determine how many days are in Feb.
	monthLength[1]=28; //Default
	if(year%4==0) {monthLength[1]=29;} //All years divisible by 4 are leap years
	if(year%100==0){ //All years divisible by 100 are not leap years, unless...
		if(year%400 == 0){monthLength[1]=29;} //.. the year is also divisible by 400.
		else {monthLength[1]=28;} 
	};
}

int DateTime::GetDayOfYear(){
	int val = GetDayOfYear(_year, _month, _mday);
	return val;
}

int DateTime::GetDayOfYear(int year, int month, int mday){
	//Return the day of the year specified by the class day/month/year class members
	//Day of the year runs from 1-365

	int doy=0;
 	if(month>2) { for (int i=0; i<month-1; i++) {doy+=monthLength[i];}; };
	doy+= mday; 
	return doy;
}

int DateTime::CalculateDayOfYear( int year, int month, int mday ){	//Static version
	
	int monthLength[12];
	//Initialize the array of month lengths (accounting for leap year)
	for (int i=0; i<12; i+=2){ monthLength[i]=31; };
	for (int i=1; i<12; i+=2){ monthLength[i]=30; };
	//Given the year, determine how many days are in Feb.
	monthLength[1]=28; //Default
	if(year%4==0) {monthLength[1]=29;} //All years divisible by 4 are leap years
	if(year%100==0){ //All years divisible by 100 are not leap years, unless...
		if(year%400 == 0){monthLength[1]=29;} //.. the year is also divisible by 400.
		else {monthLength[1]=28;} 
	};
	int doy=0;
 	if(month>2) { for (int i=0; i<month-1; i++) {doy+=monthLength[i];}; };
	doy+= mday; 
	return doy;

}

int DateTime::GetHourOfYear(){
//Return the hour of the year according to the values specified by the class members
	int doy = GetDayOfYear();
	int hr = (doy-1)*24 + _hour;
	return hr;
};

void DateTime::hours_to_date(double hours, double &month, double &day_of_month){
	/*
	Take hour of the year (0-8759) and convert it to month and day of the month. 
	If the year is not provided, the default is 2011 (no leap year)
	Month = 1-12
	Day = 1-365

	The monthLength[] array contains the number of days in each month. 
	This array is taken from the DateTime class and accounts for leap years.
	To modify the year to not include leap year days, change the year in the
	DateTime instance.

	*/
	double days = hours/24.;
	int dsum=0; 
	for(int i=0; i<12; i++){
		dsum += monthLength[i];
		if(days <= dsum){month = double(i)+1.; break;}
	}
	day_of_month = floor(days - (dsum - monthLength[int(month)-1]))+1.;

}

std::string DateTime::GetMonthName(int month){
	switch(month)
	{
	case 1:
		return "January";
	case 2:
		return "February";
	case 3:
		return "March";
	case 4:
		return "April";
	case 5:
		return "May";
	case 6:
		return "June";
	case 7:
		return "July";
	case 8:
		return "August";
	case 9:
		return "September";
	case 10:
		return "October";
	case 11:
		return "November";
	case 12:
		return "December";
	default:
		return "";
	}
}


//---------Weather data object class --------------------
//Copy constructor
WeatherData::WeatherData( const WeatherData &wd) : 
	_N_items( wd._N_items ),
	Day( wd.Day ),
	Hour( wd.Hour ),
	Month( wd.Month ),
	DNI( wd.DNI ),
	T_db( wd.T_db ),
	Pres( wd.Pres ),
	V_wind( wd.V_wind ),
	Step_weight( wd.Step_weight )
{
	//Recreate the pointer array
	initPointers();

}

WeatherData::WeatherData(){	
	initPointers();
};

void WeatherData::initPointers(){
	//On initialization, create a vector of pointers to all of the data arrays
	v_ptrs.resize(8); 
	v_ptrs.at(0) = &Day;
	v_ptrs.at(1) = &Hour;
	v_ptrs.at(2) = &Month;
	v_ptrs.at(3) = &DNI;
	v_ptrs.at(4) = &T_db;
	v_ptrs.at(5) = &Pres;
	v_ptrs.at(6) = &V_wind;
	v_ptrs.at(7) = &Step_weight;
	//Initialize the number of items
	_N_items = 0;
}

void WeatherData::resizeAll(int size, double val){ 
	for(unsigned int i=0; i<v_ptrs.size(); i++){
		v_ptrs.at(i)->resize(size, val); 
		_N_items = size;
	}
};

void WeatherData::getStep(int step, double &day, double &hour, double &dni, double &step_weight){
	//retrieve weather data from the desired time step
	day = Day.at(step);
	hour = Hour.at(step);
	dni = DNI.at(step);
	step_weight = Step_weight.at(step);
}

void WeatherData::getStep(int step, double &day, double &hour, double &month, double &dni, 
	double &tdb, double &pres, double &vwind, double &step_weight){
	double *args[] = {&day, &hour, &month, &dni, &tdb, &pres, &vwind, &step_weight};
	
	//retrieve weather data from the desired time step
	for(unsigned int i=0; i<v_ptrs.size(); i++){
		*args[i] = v_ptrs.at(i)->at(step);
	}
}

void WeatherData::setStep(int step, double day, double hour, double dni, double step_weight){
	Day.at(step) = day; 
	Hour.at(step) = hour;
	DNI.at(step) = dni;
	Step_weight.at(step) = step_weight;
}

void WeatherData::setStep(int step, double day, double hour, double month, double dni, 
	double tdb, double pres, double vwind, double step_weight){
	Day.at(step) = day;
	Hour.at(step) = hour;
	Month.at(step) = month;
	DNI.at(step) = dni;
	T_db.at(step) = tdb;
	Pres.at(step) = pres;
	V_wind.at(step) = vwind;
	Step_weight.at(step) = step_weight;
}

std::vector<std::vector<double>*> *WeatherData::getEntryPointers()
{
	return &v_ptrs;
}

//-----------------------Toolbox namespace--------------------

//misc
double Toolbox::round(double x){
	return fabs(x - ceil(x)) > 0.5 ? floor(x) : ceil(x);
}

void Toolbox::writeMatD(string dir, string name, matrix_t<double> &mat, bool clear){
	//write the data to a log file
	FILE *file;
	
	//ofstream file; 
	string path;
	path.append(dir);
	path.append("\\matrix_data_log.txt");


	if( clear ) {
		file = fopen(path.c_str(), "w");
	}
	else{
		file =	fopen(path.c_str(), "a");
	}

	int nr = mat.nrows();
	int nc = mat.ncols();
	
	fprintf(file, "%s\n", name.c_str());
	
	for (int i=0; i<nr; i++){
		for (int j=0; j<nc; j++){
			fprintf(file, "%e\t", mat.at(i,j));
		}
		fprintf(file, "%s", "\n");
	}
	fprintf(file, "%s", "\n");

	fclose(file);
}

void Toolbox::writeMatD(string dir, string name, block_t<double> &mat, bool clear){
	//write the data to a log file
	FILE *file;
	
	//ofstream file; 
	string path;
	path.append(dir);
	path.append("\\matrix_data_log.txt");


	if( clear ) {
		file =fopen(path.c_str(), "w");
	}
	else{
		file =fopen(path.c_str(), "a");
	}

	int nr = mat.nrows();
	int nc = mat.ncols();
	int nl = mat.nlayers();

	fprintf(file, "%s\n", name.c_str());
	
	for (int k=0; k<nl; k++){
		fprintf(file, "%i%s", k, "--\n");
		for (int i=0; i<nr; i++){
			for (int j=0; j<nc; j++){
				fprintf(file, "%e\t", mat.at(i,j,k));
			}
			fprintf(file, "%s", "\n");
		}
	}
	fprintf(file, "%s", "\n");

	fclose(file);
}

void Toolbox::swap(double &a, double &b){
	//Swap the values in A and B
	double xt = a;
	a = b;
	b = xt;
}

void Toolbox::swap(double *a, double *b){
	double xt = *a;
	*a = *b;
	*b = xt;
}

double Toolbox::atan3(double &x, double &y){
	double v = atan2(x,y); 
	return v < 0. ? v += 2.*acos(-1.) : v; 
}

void Toolbox::map_profiles(double *source, int nsource, double *dest, int ndest, double *weights){
	/* 
	Take a source array 'source(nsource)' and map the values to 'dest(ndest)'. 
	This method creates an integral-conserved map of values in 'dest' that may have a
	different number of elements than 'source'. The size of each node within 'source' 
	is optionally specified by the 'weights(nsource)' array. If all elements are of the
	same size, set weights=(double*)NULL or omit the optional argument.
	*/

	double *wsize;
	double wtot = 0.;
	if(weights != (double*)NULL){
		wsize = new double[nsource];
		for(int i=0; i<nsource; i++){
			wtot += weights[i]; //sum up weights		
			wsize[i] = weights[i];
		}
	}
	else{
		wsize = new double[nsource];
		for(int i=0; i<nsource; i++)
			wsize[i] = 1.;
		wtot = (double)nsource;
	}

	double delta_D = wtot/(double)ndest;

	int i=0;
	double ix = 0.;

	for(int j=0; j<ndest; j++){
		dest[j] = 0.;
		double 
			jx = (double)(j+1)*delta_D,
			jx0 = (double)j*delta_D;
		//From previous step
		if(ix-jx0>0)
			dest[j] += (ix-jx0)*source[i-1];
		//internal steps
		while(ix < jx ){
			ix += wsize[i];
			dest[j] += wsize[i] * source[i];
			i++;
		}
		//subtract overage
		if(ix > jx )
			dest[j] += -(ix-jx)*source[i-1];
		//Divide by length
		dest[j] *= 1./delta_D;
	}
	

	//Memory cleanup
	delete [] wsize;
	
}


/* Factorial of an integer x*/
int Toolbox::factorial(int x) {
	//if (floor(3.2)!=x) { cout << "Factorial must be an integer!" };
    
    int f = x;
    for (int i=1; i<x; i++) {
        int j=x-i;
        f=f*j;
	}
	if(f<1) f=1;	//Minimum of any factorial is 1
    return f;
}

double Toolbox::factorial_d(int x) {
	//returns the same as factorial, but with a doub value
	return double(factorial(x));
}

bool Toolbox::pointInPolygon( const vector< Point > &poly, const Point &pt) {
	/* This subroutine takes a polynomial array containing L_poly vertices (X,Y,Z) and a 
	single point (X,Y,Z) and determines whether the point lies within the polygon. If so, 
	the algorithm returns TRUE (otherwise FALSE) */
	
	//if polywind returns a number between -1 and 1, the point is in the polygon
	int wind = polywind(poly, pt);
	if (wind == -1 || wind == 1) { return true;}
	else {return false;}
}

vector<Point> Toolbox::projectPolygon(vector<Point> &poly, PointVect &plane) {
	/* Take a polygon with points in three dimensions (X,Y,Z) and project all points onto a plane defined 
	by the point-vector {x,y,z,i,j,k}. The subroutine returns a new polygon with the adjusted points all
	lying on the plane. The points are also assigned vector values corresponding to the normal vector
	of the plane that they lie in.*/

	//Declare variables
	double dist, A, B, C, D;
	Point pt; 

	//Declare a new polygon of type vector
	int Lpoly = poly.size(); 
	vector< Point > FPoly(Lpoly);
	

	//Determine the coefficients for the equation of the plane {A,B,C,D}
	A=plane.i; B=plane.j; C=plane.k;
	Vect uplane; uplane.Set(A,B,C); vectmag(uplane);
	D = -A*plane.x - B*plane.y - C*plane.z;

	for (int i=0; i<Lpoly; i++) {
		//Determine the distance between the point and the plane
		pt = poly.at(i);
		dist = -(A*pt.x + B*pt.y + C*pt.z + D)/vectmag(*plane.vect());
		//knowing the distance, now shift the point to the plane
		FPoly.at(i).x = pt.x+dist*A;
		FPoly.at(i).y = pt.y+dist*B;
		FPoly.at(i).z = pt.z+dist*C;
		
	}
	return FPoly;
}

int Toolbox::polywind( const vector<Point> &vt, const Point &pt) {
	/*
	Determine the winding number of a polygon with respect to a point. This helps
	calculate the inclusion/exclusion of the point inside the polygon. If the point is
	inside the polygon, the winding number is equal to -1 or 1.
	*/

	//Declarations
	int i, np, wind = 0, which_ign;
	double d0, d1, p0, p1, pt0, pt1; 
	
	/*The 2D polywind function can be mapped to 3D polygons by choosing a single dimension to 
	ignore. The best ignored dimension corresponds to the largest magnitude component of the
	normal vector to the plane containing the polygon.*/
	//Get the cross product of the first three points in the polygon to determine the planar normal vector
	Vect v1, v2;
	v1.Set((vt.at(0).x-vt.at(1).x),(vt.at(0).y - vt.at(1).y),(vt.at(0).z - vt.at(1).z));
	v2.Set((vt.at(2).x-vt.at(1).x),(vt.at(2).y - vt.at(1).y),(vt.at(2).z - vt.at(1).z));
	Vect pn = crossprod( v1, v2 );

	//PointVect ptX=vt.at(0);
	//if (pn.j >= pn.i) {which_ign=1;}
	//if (pn.k >= pn.j) {which_ign=2;}
	//if (pn.i >= pn.k) {which_ign=0;}
	which_ign = 1;
	if(fabs(pn.j) > fabs(pn.i)) {which_ign=1;}
	if(fabs(pn.k) > fabs(pn.j)) {which_ign=2;}
	if(fabs(pn.i) > fabs(pn.k)) {which_ign=0;}

	/* Return the winding number of a polygon (specified by a vector of vertex points vt) 
	around an arbitrary point pt.*/
	np = vt.size();
	switch (which_ign) {
	case 0:
		pt0 = pt.y; pt1 = pt.z;
		p0 = vt[np-1].y; p1 = vt[np-1].z;
		break;
	case 1:
		pt0 = pt.x; pt1 = pt.z;
		p0 = vt[np-1].x; p1 = vt[np-1].z;
		break;
	case 2:
		pt0 = pt.x; pt1 = pt.y;
		p0 = vt[np-1].x; p1 = vt[np-1].y;
	}

	for (i=0; i<np; i++) {
		switch (which_ign) {
		case 0:
			d0 = vt[i].y; d1 = vt[i].z;
			break;
		case 1:
			d0 = vt[i].x; d1 = vt[i].z;
			break;
		case 2:
			d0 = vt[i].x; d1 = vt[i].y;
		}

		if (p1 <= pt1) {
			if (d1 > pt1 && (p0-pt0)*(d1-pt1)-(p1-pt1)*(d0-pt0) > 0) wind++;
		}
		else {
			if (d1 <= pt1 && (p0-pt0)*(d1-pt1)-(p1-pt1)*(d0-pt0) < 0) wind--;
		}
		p0=d0;
		p1=d1;
	}
	return wind;
}

Vect Toolbox::crossprod(const Vect &A, const Vect &B) {
	/* Calculate the cross product of two vectors. The magnitude of the cross product
	represents the area contained in a parallelogram bounded by the multipled vectors.*/
	Vect res; 
	res.i = A.j*B.k - A.k*B.j;
	res.j = A.k*B.i - A.i*B.k;
	res.k = A.i*B.j - A.j*B.i;
	return res;
};

void Toolbox::unitvect(Vect &A) {
	/*Take a vector that may or may not be in unit vector form and scale the magnitude to 
	make it a unit vector*/
	double M = vectmag(A);
	if(M==0.){A.i=0; A.j=0; A.k=0; return;}
	A.i /= M; A.j /= M; A.k /= M; return;
}

double Toolbox::dotprod(const Vect &A, const Vect &B)
{
	return (A.i * B.i + A.j * B.j + A.k * B.k); 
}

double Toolbox::dotprod(const Vect &A, const Point &B)
{
	return (A.i * B.x + A.j * B.y + A.k * B.z);
}

double Toolbox::vectmag(const Vect &A) 
{
	return sqrt(pow(A.i,2) + pow(A.j,2) + pow(A.k,2));
}

double Toolbox::vectmag(const Point &P){
	return sqrt( pow(P.x,2) + pow(P.y,2) + pow(P.z,3) );
}

double Toolbox::vectmag(double i, double j, double k){
	return sqrt( pow(i,2) + pow(j,2) + pow(k,2) );
}

double Toolbox::vectangle(const Vect &A, const Vect&B) 
{
	//Determine the angle between two vectors
	return acos(dotprod(A,B)/(vectmag(A)*vectmag(B)));
}

void Toolbox::rotation(double theta, int axis, Vect &V){
	Point p;
	p.Set(V.i, V.j, V.k);
	Toolbox::rotation(theta, axis, p);
	V.Set(p.x, p.y, p.z);
}

void Toolbox::rotation(double theta, int axis, Point &P){
	/*
	This method takes a point, a rotation angle, and the axis of rotation and 
	rotates the point about the origin in the specified direction. 

	The inputs are:
	theta	| [rad]	| Angle of rotation
	axis	| none	| X=0, Y=1, Z=2 : Axis to rotate about

	The method returns a modified point "P" that has been rotated according to the inputs.

	Rotation is clockwise about each axis (left hand rule). In other words, 
	positive rotation for each axis is defined by the apparent motion when positive end 
	of the axis points toward the viewer. 
	*/

	//the 3x3 rotation matrix
	vector<Vect> MR(3);
	double 
		costheta = cos(theta),
		sintheta = sin(theta);
	switch(axis)
	{
		/*
		The rotation vectors are entered in as the transverse for convenience of multiplication.
		The correct matrix form for each are:

		X axis
		[1,					0,				0,
		 0,				cos(theta),		-sin(theta),
		 0,				sin(theta),		cos(theta)]

		Y axis
		[cos(theta),		0,			sin(theta),
			0,				1,				0,
		 -sin(theta),		0,			cos(theta)]

		Z axis
		[cos(theta),	-sin(theta),		0,
		 sin(theta),	cos(theta),			0,
			0,				0,				1	]

		*/
	case 0:	//X axis
		//Fill in the x-rotation matrix for this angle theta
		MR.at(0).Set(1., 0., 0.);
		MR.at(1).Set(0., costheta, sintheta);
		MR.at(2).Set(0., -sintheta, costheta);
		break;
	case 1:	//Y axis
		//Fill in the y-rotation matrix for this angle theta
		MR.at(0).Set(costheta, 0., -sintheta);
		MR.at(1).Set(0., 1., 0.);
		MR.at(2).Set(sintheta, 0., costheta);
		break;
	case 2:	//Z axis
		//Fill in the z-rotation matrix for this angle theta
		MR.at(0).Set(costheta, sintheta, 0.);
		MR.at(1).Set(-sintheta, costheta, 0.);
		MR.at(2).Set(0., 0., 1.);
		break;
	default:
		;
	}

	//Create a copy of the point
	Point Pc(P);
	
	//Do the rotation. The A matrix is the rotation vector and the B matrix is the point vector
	P.x = dotprod(MR.at(0), Pc);
	P.y = dotprod(MR.at(1), Pc);
	P.z = dotprod(MR.at(2), Pc);
	return;

}

bool Toolbox::plane_intersect(Point &P, Vect &N, Point &C, Vect &L, Point &Int){
	/*
	Determine the intersection point of a line and a plane. The plane is 
	defined by:
	P	| Point on the plane
	N	| Normal unit vector to the plane
	The line/vector is defined by:
	C	| (x,y,z) coordinate of the beginning of the line
	L	| Unit vector pointing along the line

	The distance 'd' along the unit vector is given by the equation:
	d = ((P - C) dot N)/(L dot N)

	The method fills in the values of a point "Int" that is the intersection. 

	In the case that the vector does not intersect with the plane, the method returns FALSE and
	the point Int is not modified. If an intersection is found, the method will return TRUE.
	*/

	double PC[3], LdN, PCdN, d;
	int i;
	//Create a vector between P and C
	for(i=0; i<3; i++){PC[i] = P[i]-C[i];}
	//Calculate the dot product of L and N
	LdN = 0.;
	for(i=0; i<3; i++){
		LdN += L[i]*N[i];
	}
	//Calculate the dot product of (P-C) and N
	PCdN = 0.;
	for(i=0; i<3; i++){
		PCdN += PC[i]*N[i];
	}

	if(LdN == 0.) return false; //Line is parallel to the plane
	d = PCdN / LdN;	//Multiplier on unit vector that intersects the plane
		
	//Calculate the coordinates of intersection
	Int.x = C.x + d*L.i;
	Int.y = C.y + d*L.j;
	Int.z = C.z + d*L.k;

	return true;

}

bool Toolbox::line_norm_intersect(Point &line_p0, Point &line_p1, Point &P, Point &I, double &rad){
	/* 
	Note: 2D implementation (no Z component)

	Given two points that form a line segment (line_p0 and line_p1) and an external point 
	NOT on the line (P), return the location of the intersection (I) between a second line
	that is normal to the first and passes through P. Also return the corresponding radius 'rad'
	||P I||. 

    (line_p0)       (I)                 (line_p1)
	O--------------X----------------------------O
                   |_|
                   |
                   |
                   O
                    (P)

	If the normal to the line segment lies outside of the segment, the method returns 
	FALSE, otherwise its TRUE. In the first case, 'I' is equal to the segment point closest 
	to 'P', otherwise it is the intersection point.

	Solve for 'I' using the derivative of the distance formula (r(x,y)) between I and P. I is the point
	where dr(x,y)/dx = 0.

	*/

	//Check to see if the 'x' components of p0 and p1 are the same, which is undefined slope.
	if(line_p0.x == line_p1.x){
		//check for containment
		double Iyr = (P.y - line_p0.y)/(line_p1.y - line_p0.y);
		if(Iyr < 0.){	//out of bounds on the p0 side
			I.Set(line_p0.x, line_p0.y, 0.);
			rad = vectmag(I.x - P.x, I.y - P.y, 0.);
			return false;
		}
		else if(Iyr > 1.){	//out of bounds on the p1 side
			I.Set(line_p1.x, line_p1.y, 0.);
			rad = vectmag(I.x - P.x, I.y - P.y, 0.);
			return false;
		}
		I.Set(line_p0.x, P.y, 0.);
	}

	double 
		drdx = (line_p1.y - line_p0.y)/(line_p1.x - line_p0.x),
		drdx_sq = pow(drdx,2);
	I.x = (P.x + P.y * drdx - line_p0.y*drdx + line_p0.x*drdx_sq)/(1. + drdx_sq);
	//check for containment
	double Ixr = (I.x - line_p0.x)/(line_p1.x - line_p0.x);
	if(Ixr < 0.){	//outside the bounds on the p0 side
		I.x = line_p0.x;
		I.y = line_p0.y;
		rad = vectmag(I.x - P.x, I.y - P.y, 0.);
		return false;
	}
	else if(Ixr > 1.){	//outside the bounds on the p1 side
		I.x = line_p1.x;
		I.y = line_p1.y;
		rad = vectmag(I.x - P.x, I.y - P.y, 0.);
		return false;
	}
	//in bounds
	I.y = line_p0.y + (I.x - line_p0.x)*drdx;
	rad = vectmag(I.x - P.x, I.y - P.y, 0.);
	return true;

}

void Toolbox::ellipse_bounding_box(double &A, double &B, double &phi, double sides[4], double cx, double cy){
	/* 
	This algorithm takes an ellipse in a plane at location {cx,cy} and with unrorated X axis 
	size A, unrotated Y axis size B, and with angle of rotation 'phi' [radians] and calculates 
	the bounding box edge locations in the coordinate system of the plane.

	This method fills the 'sides' array with values for:
	sides[0]	|	x-axis minimum
	sides[1]	|	x-axis maximum
	sides[2]	|	y-axis minimum
	sides[3]	|	y-axis maximum

	Reference:
	http://stackoverflow.com/questions/87734/how-do-you-calculate-the-axis-aligned-bounding-box-of-an-ellipse
	
	Governing equations are:
	x = cx + A*cos(t)*cos(phi) - b*sin(t)*sin(phi)
	y = cy + b*sin(t)*cos(phi) - a*cos(t)*sin(phi)
	
	where 't' is an eigenvalue with repeating solutions of dy/dt=0
	
	For X values:
	0 = dx/dt = -A*sin(t)*cos(phi) - B*cos(t)*sin(phi)
	--> tan(t) = -B*tan(phi)/A
	--> t = atan( -B*tan(phi)/A )

	for Y values:
	0 = dy/dt = B*cos(t)*cos(phi) - A*sin(t)*sin(phi)
	--> tan(t) = B*cot(phi)/A
	--> t = aan( B*cot(phi)/A )
	
	*/
	double pi = acos(-1.);

	//X first
	//double tx = atan( -B*tan(phi)/A );
	double tx = atan2( -B*tan(phi), A);
	//plug back into the gov. equation
	double txx = A*cos(tx)*cos(phi) - B*sin(tx)*sin(phi);
	sides[0] = cx + txx/2.;
	sides[1] = cx - txx/2.;
	//enforce orderedness
	if(sides[1] < sides[0]) swap(&sides[0], &sides[1]);

	//Y next
	double ty = atan2( -B, tan(phi)*A );
	double tyy = B*sin(ty)*cos(phi) - A*cos(ty)*sin(phi);
	sides[2] = cy + tyy/2.;
	sides[3] = cy - tyy/2.;
	if(sides[3] < sides[2]) swap(&sides[3], &sides[2]);
	


}

Point Toolbox::rotation_arbitrary(double theta, Vect &axis, Point &axloc, Point &pt){
	/* 
	Rotation of a point 'pt' about an arbitrary axis with direction 'axis' centered at point 'axloc'. 
	The point is rotated through 'theta' radians.
	http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/
	
	Returns the rotated Point location
	*/

	double
		a = axloc.x,	//Point through which the axis passes
		b = axloc.y,
		c = axloc.z,
		x = pt.x,		//Point that we're rotating
		y = pt.y,
		z = pt.z,
		u = axis.i,		//Direction of the axis that we're rotating about
		v = axis.j,
		w = axis.k;
	Point fin;

	double 
		sinth = sin(theta),
		costh = cos(theta);

	fin.x = (a*(pow(v,2)+pow(w,2)) - u*(b*v + c*w - u*x - v*y - w*z))*(1.-costh) + x*costh + (-c*v + b*w - w*y + v*z)*sinth;
	fin.y = (b*(pow(u,2)+pow(w,2)) - v*(a*u + c*w - u*x - v*y - w*z))*(1.-costh) + y*costh + (c*u - a*w + w*x - u*z)*sinth;
	fin.z = (c*(pow(u,2)+pow(v,2)) - w*(a*u + b*v - u*x - v*y - w*z))*(1.-costh) + z*costh + (-b*u + a*v - v*x + u*y)*sinth;

	return fin;
}

double Toolbox::ZRotationTransform(Vect &normal_vect){
	/* 
	When a heliostat position is transformed using the SolTrace convention, the heliostat 
	ends up such that the horizontal (x) axis is not parallel with the ground XZ plane.

	This is caused by a mismatch between conventional rotation about (1) the y axis and (2) the 
	rotated y axis, and azimuth-elevation rotation about (1) the x axis and (2) the original z axis.

	This method calculates the angle of rotation about the modified z-axis in order to restore 
	the azimuth-elevation positioning.
	
	Rotations are assumed to be:
		(1) CCW about Y axis
		(2) CW about X' axis
		(3) CW about Z'' axis


	*/
	double Pi = acos(-1.);
	double az = atan3(normal_vect.i,normal_vect.j);
	double el = asin(normal_vect.k);

	//Calculate Euler angles
	double alpha = atan2(normal_vect.i, normal_vect.k);		//Rotation about the Y axis
	double bsign = normal_vect.j > 0. ? 1. : -1.;
	double beta = -bsign*acos( ( pow(normal_vect.i,2) + pow(normal_vect.k,2) )/ max(sqrt(pow(normal_vect.i,2) + pow(normal_vect.k,2)), 1.e-8) );	//Rotation about the modified X axis

	//Calculate the modified axis vector
	Vect modax;
	modax.Set( cos(alpha), 0., -sin(alpha) );

	//Rotation references - axis point
	Point axpos;
	axpos.Set(0.,0.,0.);	//Set as origin
	//Point to rotate
	Point pbase;
	pbase.Set(0., -1., 0.);	//lower edge of heliostat
	//Rotated point
	Point prot = Toolbox::rotation_arbitrary(beta, modax, axpos, pbase);

	//Azimuth/elevation reference vector (vector normal to where the base of the heliostat should be)
	Vect azelref;
	azelref.Set( sin(az)*sin(el), cos(az)*sin(el), -cos(el) );

	//Calculate the angle between the rotated point and the azel ref vector
	//double gamma = acos( Toolbox::dotprod(azelref, prot) );
	/* 
	the sign of the rotation angle is determined by whether the 'k' component of the cross product
	vector is positive or negative. 
	*/
	Vect protv;
	protv.Set(prot.x, prot.y, prot.z);
	unitvect(protv);
	Vect cp = crossprod(protv, azelref);
	double gamma = asin( vectmag( cp ));
	double gsign = (cp.k > 0. ? 1. : -1.) * (normal_vect.j > 0. ? 1. : -1.);
	
	return gamma * gsign;


}

double Toolbox::ZRotationTransform(double Az, double Zen){
	/* 
	Overload for az-zen specification

	Notes:
	The Azimuth angle should be [-180,180], zero is north, +east, -west.
	Az and Zen are both in Radians.
	*/
	//Calculate the normal vector to the heliostat based on elevation and azimuth
	double Pi = acos(-1.);
	double el = Pi/2.-Zen;
	double az = Az+Pi;	//Transform to 0..360 (in radians)
	Vect aim;
	aim.Set( sin(az)*cos(el), cos(az)*cos(el), sin(el) );
	return ZRotationTransform(aim);
}

double Toolbox::intersect_fuv(double U, double V){
	/* 
	Helper function for intersect_ellipse_rect()
	*/
	double 
		u2 = sqrt(1.-pow(U,2)),
		v2 = sqrt(1.-pow(V,2));
	return asin(u2 * v2 - U*V) - U*u2 - V*v2 + 2.*U*V;
}

double Toolbox::intersect_ellipse_rect(double rect[4], double ellipse[2]){
	/* 
	Calculate the area of intersection of a rectangle and an ellipse where the sides of the 
	rectangle area parallel with the axes of the ellipse. 

	{rect[0], rect[1]} = Point location of center of rectangle
	{rect[2], rect[3]} = Rectangle width, Rectangle height
	
	{ellipse[0], ellipse[1]} = Ellipse width and height

	(A.D. Groves, 1963. Area of intersection of an ellipse and a rectangle. Ballistic Research Laboratories.)
	*/

	//Unpack
	double
		a = rect[0] - rect[2]/2.,	//Lower left corner X location
		b = rect[1] - rect[3]/2.,	//Lower left corner Y location
		c = rect[2],	//Rect width
		d = rect[3];	//Rect height
	double
		w = ellipse[0],	//Ellipse width
		h = ellipse[1];	//Ellipse height

	//Construct 4 separate possible rectangles
	double A[4], B[4], C[4], D[4];
	for(int i=1; i<5; i++){
		A[i-1] = max(0., pow(-1, (pow(i,2)-i)/2)*rect[0] - c/2.);
		B[i-1] = max(0., pow(-1, (pow(i,2)+i-2)/2)*rect[1] - d/2.);
		C[i-1] = max(0., pow(-1, (pow(i,2)-i)/2)*rect[0]+c/d-A[i-1]);
		D[i-1] = max(0., pow(-1, (pow(i,2)+i-2)/2)*rect[1]+d/2.-B[i-1]);
	}

	double atot=0.;
	for(int i=0; i<4; i++){
		if(C[i] == 0. || D[i] == 0.) continue;	//No area if width or height are 0

		//Calculate vertex radii
		double V[4];
		V[0] = pow(A[i]/w,2) + pow(B[i]/h,2);
		V[1] = pow(A[i]/w,2) + pow((B[i]+D[i])/h,2);
		V[2] = pow((A[i]+C[i])/w,2) + pow((B[i]+D[i])/h,2);
		V[3] = pow((A[i]+C[i])/w,2) + pow(B[i]/h,2);

		//Seven cases
		if(pow(A[i]/w,2) + pow(B[i]/h,2) >= 1.){
			continue;	//no area overlap
		}
		else if(V[0] < 1. && V[1] >= 1 && V[3] >= 1.){
			//Lower left vertex is the only one in the ellipse
			atot += w*h/2.*intersect_fuv(A[i]/w, B[i]/h);
		}
		else if(V[3] < 1. && V[1] >= 1.){
			//Lower edge inside ellipse, upper edge outside
			atot += w*h/2. * (intersect_fuv(A[i]/w, B[i]/h) - intersect_fuv((A[i]+C[i])/w, B[i]/h));
		}
		else if(V[1] < 1. && V[3] >= 1.){
			//Left edge inside, right edge outside
			atot += w*h/2. * (intersect_fuv(A[i]/w, B[i]/h) - intersect_fuv(A[i]/w, (B[i]+D[i])/h));
		}
		else if(V[1] < 1. && V[3] < 1. && V[2] > 1.){
			//All vertices inside ellipse except upper right corner
			atot += w*h/2.*(intersect_fuv(A[i]/w, B[i]/h) - intersect_fuv((A[i]+C[i])/w, B[i]/h) - intersect_fuv(A[i]/w, (B[i]+D[i])/h));
		}
		else if(V[2] < 1.){
			//All vertices inside the ellipse
			atot += w*h;
		}
		else{
			continue;	//Error
		}
	}
	return atot;

}

string Toolbox::getDelimiter(std::string &text){
	if( text.c_str() == "") return ",";
	//Find the type of delimiter
	vector<string> delims;
	delims.push_back(",");
	delims.push_back(" ");
	delims.push_back("\t");
	delims.push_back(";");
	string delim = "\t";	//initialize
	int ns=0;
	for(int i=0; i<4; i++){
		vector<string> data = Toolbox::split(text, delims[i]);
		if(data.size()>ns){ delim = delims[i]; ns = data.size(); }	//pick the delimiter that returns the most entries
	}
	return delim;
}

vector< string > Toolbox::split( const string &str, const string &delim, bool ret_empty, bool ret_delim )
{
	//Take a string with a delimiter and return a vector of separated values
	vector< string > list;

	char cur_delim[2] = {0,0};
	string::size_type m_pos = 0;
	string token;
	int dsize = delim.size();
	
	while (m_pos < str.length())
	{
		//string::size_type pos = str.find_first_of(delim, m_pos);
		string::size_type pos = str.find(delim, m_pos);
		if (pos == string::npos)
		{
			cur_delim[0] = 0;
			token.assign(str, m_pos, string::npos);
			m_pos = str.length();
		}
		else
		{
			cur_delim[0] = str[pos];
			string::size_type len = pos - m_pos;			
			token.assign(str, m_pos, len);
			//m_pos = pos + 1;
			m_pos = pos + dsize;
		}
		
		if (token.empty() && !ret_empty)
			continue;

		list.push_back( token );
		
		if ( ret_delim && cur_delim[0] != 0 && m_pos < str.length() )
			list.push_back( string( cur_delim ) );
	}
	
	return list;
}