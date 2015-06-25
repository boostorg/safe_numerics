#include <iostream>

constexpr int factorial (int n)
{
    return n > 0 ? n * factorial( n - 1 ) : 1;
}

int main(){
    int n;
    std::cin >> n;
    factorial( n );
    return 0;
}
