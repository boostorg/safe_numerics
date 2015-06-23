#include <iostream>

constexpr int factorial (int n)
{
    return n > 0 ? n * factorial( n - 1 ) : 1;
}

int main(){
    int n;
    std::cin >> n;
    constexpr int x = factorial( n );
    return 0;
}
