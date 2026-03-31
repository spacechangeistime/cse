package main

import (
	"container/heap"
	"fmt"
	"math/rand"
)

type Value struct {
	V int
	C <-chan int
}

type MinHeap []Value

func (h MinHeap) Len() int           { return len(h) }
func (h MinHeap) Less(i, j int) bool { return h[i].V < h[j].V }
func (h MinHeap) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *MinHeap) Push(val any) {
	*h = append(*h, val.(Value))
}
func (h *MinHeap) Pop() any {
	old := *h
	n := len(old)
	val := old[n-1]
	*h = old[0 : n-1]
	return val
}

func mergeChannelsSort(channels []<-chan int) <-chan int {
	out := make(chan int)
	h := &MinHeap{}
	heap.Init(h)
	for i, _ := range channels {
		if v, ok := <-channels[i]; ok {
			heap.Push(h, Value{V: v, C: channels[i]})
		}
	}
	go func() {
		defer close(out)
		for h.Len() > 0 {
			val := heap.Pop(h).(Value)
			out <- val.V
			ok := false
			if val.V, ok = <-val.C; ok {
				heap.Push(h, val)
			}
		}
	}()
	return out
}

var N int = 3

func main() {
	channels := make([]<-chan int, N)
	for i := 0; i < N; i++ {
		ch := make(chan int)
		channels[i] = ch
		go func() {
			defer close(ch)
			for j := rand.Intn(10); j < 10; j++ {
				ch <- j + i*7
			}
		}()
	}
	out := mergeChannelsSort(channels)
	for v := range out {
		fmt.Printf("%v, ", v)
	}
}
