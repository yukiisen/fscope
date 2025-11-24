# Kiri
A fuzzy finder inspired by fzy that looks like telescope

It uses the same algorithm as fzy.

# Notes:
- This is not finished so the ui does blink a lot (will be fixed soon)
- There's some unsafe code in it (will be fixed as well)
- There's no signal handling yet.
- This thing differs from fzy in a few things:
    - It uses a simpler (dumber) copy of the algorithm
    - It has a different ui (will likeley be considered as bloat by that one person)
    - uses poll instead of select (I find it more enjoyable)

# Installation:
From source:
```
make
cp ./build/kiri ~/.local/bin/kiri // or somewhere else
```

# Usage:
It is quite similar to any other fuzzy finder so there's no need for this section

Check [fzy](https://github.com/jhawthorn/fzy) for more information.
