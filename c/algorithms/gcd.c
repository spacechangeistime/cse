#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int gcd_recursive(int a, int b) {
	if(b == 0)
		return a;
	return gcd_recursive(b, a%b);
}

int gcd(int a, int b) {
	while(b != 0) {
		int tmp = b;
		b = a%b;
		a = tmp;
	}
	return a;
}

int main() {
	srand(time(NULL));
	int a = rand()%100;
	int b = rand()%101;
	int i = gcd(a, b);
	int r = gcd_recursive(a, b);
	printf("a: %d, b: %d, gcd: %d, gcd_recursive: %d\n", a, b, i, r);
	return 0;
}
