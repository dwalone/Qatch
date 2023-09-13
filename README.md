# Qatch, a simple quantum computing assembly language

## Build

g++ src/main.cpp src/engine/*.cpp -o executables/Linux/qatch

g++ src\\main.cpp src\\engine\\*.cpp -o executables\\Windows\\qatch.exe

## Run

executables/Linux/qatch examples/grover

executables\\Windows\\qatch examples\\grover

## Doc

`init 3`
    
initalises a circuit with 3 qubits

`H 1`

Applies Hadamard gate to qubit 1

`X 3`

Applies NOT gate to qubit 3

`CZ 1 | 3`

Applies Conditional-Z gate on qubit 1 conditional on qubit 3

```
def X3 $a $b $c
    ...
endef
```

defines a function called `X3` that takes 3 arguments, `$a`, `$b`, `$c`

```
for $i 1:3
    ...
endfor
```

defines a for loop that iterates over variable `$i` from 1->2->3

Please see `examples/grover` for a full script example