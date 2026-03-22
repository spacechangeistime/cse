package main

import (
	"fmt"
	"golang.org/x/example/hello/reverse"
)

func main() {
	// reverse string and digits of integer number
	fmt.Println(reverse.String("Hello"), reverse.Int(12345))
}
