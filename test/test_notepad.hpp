    bool x = test_checked_modulus(
        unsigned_values<unsigned char>[6],
        unsigned_values<unsigned char>[4],
        '.'
    );
    bool rval = true;
    mp_for_each<
        mp_product<
            test_signed_pair,
            signed_test_types,
            signed_value_indices, signed_value_indices
        >
    >(
        [&](auto I){
            rval &= I.value;
        }
    );

    mp_for_each<
        mp_product<
            test_unsigned_pair,
            unsigned_test_types,
            unsigned_value_indices, unsigned_value_indices
        >
    >(
        [&](auto I){
            rval &= I.value;
        }
    );
