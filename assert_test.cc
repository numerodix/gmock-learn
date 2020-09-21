#include "_main.skel"

TEST(DISABLED_FirstTry, NoTitle) {
    // this test will be skipped
}

TEST(Asserting, WithCustomError) {
    ASSERT_EQ(1, 2) << "1 == 2 should hold, right?";
}

TEST(Asserting, TrueOrFalse_NonBoolInputs) {
    int age = 4;
    int *p = &age;
    int *q = nullptr;

    EXPECT_TRUE(true);
    EXPECT_TRUE(1);
    EXPECT_TRUE(1.1);
    EXPECT_TRUE(p);  // non null ptr
    EXPECT_TRUE(main);  // non null ptr

    EXPECT_FALSE(false);
    EXPECT_FALSE(0);
    EXPECT_FALSE(0.0);
    EXPECT_FALSE(q);  // null ptr
}

struct DummyCls {
    int age;
};

bool operator==(const DummyCls& lhs, const DummyCls& rhs) {
    return lhs.age == rhs.age;
}
bool operator!=(const DummyCls& lhs, const DummyCls& rhs) {
    return lhs.age != rhs.age;
}
// pretty print the class on assertion failure
std::ostream& operator<<(std::ostream& os, const DummyCls& dummy) {
    return os << "DummyCls<age: " << dummy.age << ">";
}

TEST(Asserting, Equality) {
    EXPECT_EQ(true, 1);

    // std::string works out of the box
    EXPECT_EQ(std::string("hi"), std::string("hi"));

    // char* needs _STREQ / _STRNE
    char msg1[] = "hi";
    char msg2[] = "hi";
    EXPECT_NE(msg1, msg2);  // different addresses
    EXPECT_STREQ(msg1, msg2);  // uses strcmp

    // case fold then compare
    EXPECT_STRCASEEQ(msg1, "HI");
    EXPECT_STRCASENE(msg1, "Hey");

    // equality over user defined type
    DummyCls dummy1{4};
    DummyCls dummy2{4};
    DummyCls dummy3{3};
    EXPECT_EQ(dummy1, dummy2);  // requires operator==
    EXPECT_NE(dummy1, dummy3);  // requires operator!=
    EXPECT_EQ(dummy1, dummy3);  // fails
}

TEST(Asserting, Throwing) {
    EXPECT_THROW(throw std::runtime_error("woops"), std::runtime_error);

    // expecting the wrong exception
    EXPECT_THROW(throw std::runtime_error("woops"), std::invalid_argument);

    // expected to throw but doesn't
    EXPECT_THROW(1+2, std::exception);

    // not supposed to throw
    EXPECT_NO_THROW(1+2);
    // not supposed to throw but does
    EXPECT_NO_THROW(throw std::runtime_error("damn"));
}

TEST(Asserting, Predicate) {
    // call predicate of one arg on the input, expect `true`
    ASSERT_PRED1([](int x) { return x == 0; }, 0);
    // call predicate of two args on the inputs, expect `true`
    ASSERT_PRED2([](int x, int y) { return x == y; }, 1, 1);
}