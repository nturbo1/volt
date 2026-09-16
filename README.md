# VOLT - Versatile Optimizing Language Toolchain

## TODO
- Add tests for newly added string functions in `base/vlt_string.h`
- Consider loose coupling the scanner src logic from the scanner so that it will become easier to test the scanner without much worrying about the scanner src stuff like creating specific input files for tests. In other words, we should be able to replace the scanner src manager of the scanner without any difficulty, so that we can mock the scanner src manager. So, the scanner src manager obj should probably be passed as a parameter to the scanner creator function instead of it creating scanner src obj by itself.
