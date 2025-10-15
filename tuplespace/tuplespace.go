package tuplespace

import(
	"fmt"
	"sync"
)
//a tuple is a list of anything
type Tuple []interface{}

type TupleSpace struct {
    allTuples []Tuple //a list of tuples
	rwMut sync.RWMutex
	cond *sync.Cond

}

//CREATETS
//create an empty tuplespace
func  CreateTS() *TupleSpace{
	ts := &TupleSpace{allTuples: []Tuple{}}
	ts.cond = sync.NewCond(&ts.rwMut)
	return ts
}


//OUT
//writes a tuple to the tuplespace
//broadcasts to sleeping threads
func (ts *TupleSpace) Out(tuple []interface{}) error {
	ts.rwMut.Lock()
	defer ts.rwMut.Unlock()
	if len(tuple) == 0 {
		return fmt.Errorf("cannot insert empty tuple")
	}

	ts.allTuples = append(ts.allTuples, tuple)
	ts.cond.Broadcast() //notify sleeping threads
	return nil 
}

//RD
//Given a template tuple, we search the tuplespace for a match, we return the first match we find (not guarenteed to be the same each time)
//if no match is found we sleep the thread until the next -out happens. 
func (ts *TupleSpace) Rd(pattern []interface{}) ([]interface{}, error){
	ts.rwMut.Lock()
	defer ts.rwMut.Unlock()
	//this loop is for looping back up if we had to wait
	for{
		for _, tuple := range ts.allTuples {
			if len(pattern) != len(tuple) {
				continue
			}

			match := true
			for i := range pattern {
				if pattern[i] == nil {
					continue // wildcard
				}
				if ((pattern[i] != tuple[i]) && (tuple[i] != nil)){
					match = false
					break
				}
			}
			if match {
				return tuple, nil
			}
		}
		ts.cond.Wait()
	}
}

//this is the same as read but it deletes if a match is found
func (ts *TupleSpace) In(pattern []interface{}) ([]interface{}, error){
	ts.rwMut.Lock()
	defer ts.rwMut.Unlock()
	for{
		for i, tuple := range ts.allTuples {
			if len(pattern) != len(tuple) { 
				continue
			}

			match := true
			for j := range pattern {
				if pattern[j] == nil {
					continue 
				}
				if ((pattern[j] != tuple[j]) && (tuple[j] != nil)){
					match = false
					break
				}
			}

			if match {
				deleted := tuple //i must save this so that I can tell the client what i deleted, even when its gone
				ts.allTuples = append(ts.allTuples[:i], ts.allTuples[i+1:]...) //this is apparently how you delete things from lists in Go
				return deleted, nil
			}
		}
	ts.cond.Wait()
	}
}


