package reverse

import "strconv"

func Int(v int) int {
	i, _ := strconv.Atoi(String(strconv.Itoa(v)))
	return i
}
