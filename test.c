#include <unistd.h>

int main(void) {
	write(1, "bonjour\n", 8);
	return (0);
}
