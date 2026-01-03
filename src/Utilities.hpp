#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#define PI 3.141592

template<class T>
T clamp(T value, T low, T high) {
	if (value < low)  return low;
	if (value > high) return high;
	return value;
}

template<class T>
T approach(T src, T dest, T speed) {
	if (dest > src) { src += speed; if (src > dest) return dest; }
	if (dest < src) { src -= speed; if (src < dest) return dest; }
	return src;
}

template<class T>
T approachSmooth(T src, T dest, T divisor, T margin) {
	src += (dest - src) / divisor;
	if (abs(dest - src) < margin) src = dest;
	return src;
}

template<class T>
T angleApproach(T src, T dest, T spd){
	T bound = PI*2;
	while( abs( dest-(src-bound) ) < abs( dest-src ) ){ src -= bound; }
	while( abs( dest-(src+bound) ) < abs( dest-src ) ){ src += bound; }
	if( dest > src ){ 
		src += spd;
		if( src > dest ){ return dest; }
	}
	if( dest < src ){ 
		src -= spd;
		if( src < dest ){ return dest; }
	}
	return src;
}

template<class T>
T angleApproachSmooth(T src, T dest, T divisor, T margin){
	T bound = PI*2;
	while( abs( dest-(src-bound) ) < abs( dest-src ) ){ src -= bound; }
	while( abs( dest-(src+bound) ) < abs( dest-src ) ){ src += bound; }
	if( abs(dest-src) < margin ){ return dest; }
	return src + ((dest-src)/divisor);
}



int randInt(int low, int high){
	return (rand()%(high-low)) + low;
}

float randFloat(float low, float high) {
	static const int percision = RAND_MAX;
	return low + ((high-low)*(rand()%percision)) / (float)percision;
}

#endif