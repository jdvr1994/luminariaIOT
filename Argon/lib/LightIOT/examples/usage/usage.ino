// Example usage for LightIOT library by Juan David Velasquez Rosero.

#include "LightIOT.h"

// Initialize objects from the lib
LightIOT lightIOT;

void setup() {
    // Call functions on initialized library objects that require hardware
    lightIOT.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    lightIOT.process();
}
