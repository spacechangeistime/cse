package reverse

import "strconv"

// Reverse digits of integer
func Int(v int) int {
	// String() reverses the string, defined in reverse.go
	i, _ := strconv.Atoi(String(strconv.Itoa(v)))
	return i
}
