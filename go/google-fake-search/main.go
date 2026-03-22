package main

import (
	"fmt"
	"math/rand"
	"time"
)

type Result string
type Search func(query string) Result

func fakeSearch(kind string) Search {
	return func(query string) Result {
		c := make(chan Result)
		go func() {
			time.Sleep(time.Duration(rand.Intn(100)) * time.Millisecond)
			c <- Result(fmt.Sprintf("%v result %v\n", kind, query))
		}()
		return <-c
	}
}

var (
	Web1   = fakeSearch("Web")
	Web2   = fakeSearch("Web")
	Image1 = fakeSearch("Image")
	Image2 = fakeSearch("Image")
	Video1 = fakeSearch("Youtube")
	Video2 = fakeSearch("Youtube")
	Map1   = fakeSearch("Maps")
	Map2   = fakeSearch("Maps")
)

func First(query string, replicas ...Search) Result {
	c := make(chan Result)
	for i, _ := range replicas {
		go func() { c <- replicas[i](query) }()
	}
	return <-c
}

func Google(query string) []Result {
	c := make(chan Result)
	go func() { c <- First(query, Web1, Web2) }()
	go func() { c <- First(query, Image1, Image2) }()
	go func() { c <- First(query, Video1, Video2) }()
	go func() { c <- First(query, Map1, Map2) }()
	var results []Result
	timeout := time.After(time.Duration(80 * time.Millisecond))
	for i := 0; i < 4; i++ {
		select {
		case result := <-c:
			results = append(results, result)
		case <-timeout:
			fmt.Println("timeout")
		}
	}
	return results
}

func main() {
	rand.Seed(time.Now().UnixNano())
	start := time.Now()
	results := Google("golang")
	elapsed := time.Since(start)
	fmt.Printf("%v\nin %v\n", results, elapsed)
}
