- [C++](#c)
  - [Data Types](#data-types)
    - [struct](#struct)
    - [std::vector](#stdvector)
      - [definition](#definition)
      - [initialization](#initialization)
    - [std::map](#stdmap)
  - [Funtions](#funtions)
    - [std::clamp](#stdclamp)
  - [Documentation](#documentation)
  - [Nuances](#nuances)
    - [comparing floats](#comparing-floats)
      - [problem](#problem)
    - [solution](#solution)

# C++

## Data Types

### struct

If you define a struct like this:

```cpp
struct FillVariant : GrooveVariant 
{
    juce::String name;
    std::vector<DrumClip> clips;
    double length = 4;
};
```

You can initialize an instance of it with positional or designated initializers.
Designated initializers are more readable and look as follows:

```cpp
FillVariant fill0 = 
{
    .name = "fill0",
    .clips = {
        { ClipType::fill, 0,
            { /* stuff */ }
        }
    },
    .length = 4.0
};
```

>[!CAUTION]
>Designated initializers do not work with inherited structs!

### std::vector

A vector is a dynamic size array of arbitrary (templated) type.

#### definition

Definition:\
```std::vector<int> v;```\
for int type vector.

#### initialization

Using initializer list:\
```vector<type> v = {val1, val2, val3...};```


To add elements, std::vector has a .push_back() function:
```
int main() {
    vector<int> v;

    // Pushing Value one by one
    v.push_back(11);
    v.push_back(23);
    v.push_back(45);
    v.push_back(89);

    for (auto i : v)
        cout << i << " ";
    return 0;
}
```

### std::map

A map is an associative container that stores key value pairs sorted based on keys. Keys are unique.
Example:

```cpp
int main() {
    
    // Creating a map of integer keys
    // and string values
    map<int, string> m {{1, "Geeks"},
             {2,"For"}, {3,"Geeks"}};

    for (auto& p : m)
        cout << p.first << " " << p.second
        << "\n";
    return 0;
}
```

This will output:
```
1 Geeks
2 For
3 Geeks
```

## Funtions

Important or useful functions I stumbled upon:

### std::clamp

Clamps a value into a given range. If in range, the value is unchanged. If not, the value is pushed to minVal or pulled to maxVal:

```cpp
std::clamp(value, minVal, maxVal);
```

## Documentation

If you define data types, classes or functions, you might want to get information on hover like a description,  
parameters and return types etc.  
In C++ this is done via "Doxygen". The basic syntax is as follows:  
```/**``` opens a docstring and ```*/``` closes it. Every line in between starts with a ```*```.  
For newlines in the short description on hover, start each line with ```@brief``` and end it with a backslash  
if a new lone follows.
```@param``` is used for introducing the parameters and ```return``` for the return value.


```cpp
/**
 * @brief enum that translates instrument names to midi note ints.\
 * @brief available DrumTypes are:
 * @brief kick, snare, hat
 */
enum class DrumType
{
    kick = 36,
    snare = 38,
    hat = 42,
};
```

## Nuances

### comparing floats

#### problem

The following code:
```
#include <bits/stdc++.h>
using namespace std;

void compareFloatNum(double a, double b)
{
    if (a == b) {
        cout << "The numbers are equal"
             << endl;
    }
    else {
        cout << "The numbers are not equal"
             << endl;
    }
}

// Driver code
int main()
{
    double a = (0.3 * 3) + 0.1;
    double b = 1;
    compareFloatNum(a, b);
}
```
outputs:\
`The numbers are not equal`

### solution

The reason is rounding errors and precision. To correctly compare two floats:

```
#include <bits/stdc++.h>
using namespace std;

void compareFloatNum(double a, double b)
{

    // Correct method to compare
    // floating-point numbers
    if (abs(a - b) < 1e-9) {
        cout << "The numbers are equal "
             << endl;
    }
    else {
        cout << "The numbers are not equal "
             << endl;
    }
}

// Driver code
int main()
{
    double a = (0.3 * 3) + 0.1;
    double b = 1;
    compareFloatNum(a, b);
}
```

