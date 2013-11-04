bool test_divide1();
bool test_divide2();
bool test_divide3();

int main(int argc, char * argv[]){
    bool result = true;
    result = test_divide1();
    result &= test_divide2();
    result &= test_divide3();
    return ! result ;
}
