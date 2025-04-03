import fractions

A4_FREQUENCY = 440.0
NAMES = ["C", "D", "E", "F", "G", "A", "B"]
INTERVALS = [
    fractions.Fraction(1, 1),
    fractions.Fraction(9, 8),
    fractions.Fraction(5, 4),
    fractions.Fraction(4, 3),
    fractions.Fraction(3, 2),
    fractions.Fraction(5, 3),
    fractions.Fraction(15, 8),
    fractions.Fraction(2, 1),
]

SYMBOLS = {
    -1: "♭",
    0: "",
    1: "♯",
}
