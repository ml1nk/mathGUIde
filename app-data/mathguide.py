# -*- coding: UTF-8 -*-

# mathGUIde  Version 2.1"
# Copyright © 2004-2011 Hartmut Ring"
# http://www.math.uni-siegen.de/ring/mathGUIde/
# This file is for internal use only by the mathGUIde GUI.
#
# mathGUIde is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with
# this program; if not, see http://www.gnu.org/licenses/.""")
#............................................................


import operator, random, math, re, inspect, functools

class _MathGUIdeError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

_pow = pow # import everything from math except pow:
from math import *
pow = _pow

# make original Python functions floor and ceil invisible:
_floor, _ceil = floor, ceil

def _cmp(a,b):
    return int(a > b) - int(a < b)

#............................................................
#begin

# Change floor and ceil so that they map from R-->Z
# instead of R-->R as given by Python.

def floor(x):
    """ <b>floor(x)</b><br/>
        <i>highest integer n &lt;= x</i><br/>
    ----de----
        <b>floor(x)</b><br/>
        <i>größte ganze Zahl n &lt;= x (Gauß-Klammer)</i><br/>
    """
    return int(_floor(x))

def ceil(x):
    """ <b>floor(x)</b><br/>
        <i>smallest integer n >= x</i><br/>
    ----de----
        <b>ceil(x)</b><br/>
        <i>kleinste ganze Zahl n >= x</i><br/>
    """
    return int(_ceil(x))

#............................................................

def copy(x):
    """ <b>copy(x)</b><br/>
        <i>independent copy ("clone") of x</i>
    ----de----
        <b>copy(x)</b><br/>
        <i>unabhängige Kopie</i>
    """
    try:
        c = x.copy()
    except:
        c = x
    return c

#===============================================================

def fromTo(a, b, step=1):
    """ <b>fromTo(a, b, step=1)</b><br/>
        <i>Iterator over equidistant integers from <var>a</var> to <var>b</var></i><br/>
        If no <var>step</var> is given, <var>step</var> is set to 1.<br/>
        <b>Examples</b>:<pre>
    list(fromTo(3, 6))               # --->  [3,4,5,6]
    list(fromTo(3, 6, 2))            # --->  [3,5]
    list(fromTo(6, 3))               # --->  []
    list(fromTo(6, 3, -2))           # --->  [6,4]
    [2*i for i in fromTo(6, 3, -2)]  # --->  [12,8]
    sum(fromTo(1, 10))               # --->  55</pre>
    ----de----
        <b>fromTo(a, b, step=1)</b><br/>
        <i>Iterator über die ganzen Zahlen von a bis einschl. b im Abstand step</i><br/>
        Wenn <var>step</var> nicht übergeben wird, wird <var>step</var> auf 1 gesetzt.<br/>
        <b>Beispiele</b>:<pre>
    list(fromTo(3, 6))               # --->  [3,4,5,6]
    list(fromTo(3, 6, 2))            # --->  [3,5]
    list(fromTo(6, 3))               # --->  []
    list(fromTo(6, 3, -2))           # --->  [6,4]
    [2*i for i in fromTo(6, 3, -2)]  # --->  [12,8]
    sum(fromTo(1, 10))               # --->  55</pre>
    """
    if step > 0:
        return range(a, b+1, step)
    else:
        return range(a, b-1, step)

#===============================================================
#  Elementary Functions
#---------------------------------------------------------------

def isInteger(n):
    """ <b>isInteger(n)</b><br/>
        returns <code>True</code> if and only if n is of type <code>int</code>.
    ----de----
        <b>isInteger(n)</b><br/>
        gibt <code>True</code> zurück genau dann, wenn n vom Typ <code>int</code> ist.
    """
    return type(n) == int or type(n) == Rational and n.q == 1

def isNatural(n):
    return isInteger(n) and n > 0

#............................................................

def sum(obj, start=None, end=None, step=1):
    """ <b>sum(obj, start=None, end=None, step=1)</b><br/>
        <i>The sum of the elements of iterable obj or of all obj(i) for i in fromTo(start,end,step)</i><br/>
        The element type must define the operator + (e.g. numbers or strings).<br/>
        <b>Examples</b>:<br/>
        <code>sum([i^2 for i n fromTo(1,10)])</code><br/>
        <code>sum(fibonacci, 0, 10)</code><br/>
        <code>sum(lambda i:i^2, 2, 10, 2)</code>
    ----de----
        <b>sum(obj, start=None, end=None, step=1)</b><br/>
        <i>Summe der Elemente des iterierbaren Objekts a bzw. aller obj(i) für i in fromTo(start,end,step)</i><br/>
        Die Elemente müssen mit dem Operator + verknüpfbar sein
        (z. B. Zahlen oder Strings).<br/>
        <b>Beispiele</b>:<br/>
        <code>sum([i^2 for i n fromTo(1,10)])</code><br/>
        <code>sum(fibonacci, 0, 10)</code><br/>
        <code>sum(lambda i:i^2, 2, 10, 2)</code>
    """
    s = 0 # for an empty iteration the element type ist assumed to be numeric.
    if str(type(obj)) != "<class 'function'>":
        # obj is sequence or iterator or generator
        for i in obj:
            if s == 0: # adding to 0 would not work for types with neutral element != 0 (e.g. for a list of strings).
                s = i
            else:
                s += i
    else: # obj is a function
        for i in fromTo(start, end, step):
            if s == None:
                s = obj(i)
            else:
                s += obj(i)
    return s

#............................................................

def product(obj, start=None, end=None, step=1):
    """ <b>product(obj, start=None, end=None, step=1)</b><br/>
        <i>The product of the elements of iterable obj or of all obj(i) for i in fromTo(start,end,step)</i><br/>
        The element type must define the operator *.<br/>
        <b>Examples</b>:<br/>
        <code>product([i^2 for in fromTo(1,10)])</code><br/>
        <code>product(fibonacci, 0, 10)</code><br/>
        <code>product(lambda i:i^2, 2, 10, 2)</code>
    ----de----
        <b>product(obj, start=None, end=None, step=1)</b><br/>
        <i>Product der Elemente des iterierbaren Objekts a bzw. aller obj(i) für i in fromTo(start,end,step)</i><br/>
        Die Elemente müssen mit dem Operator * verknüpfbar sein<br/>
        <b>Beispiele</b>:<br/>
        <code>product([i^2 for in fromTo(1,10)])</code><br/>
        <code>product(fibonacci, 0, 10)</code><br/>
        <code>product(lambda i:i^2, 2, 10, 2)</code>
    """
    s = 1 # for an empty iteration the element type ist assumed to be numeric.
    if str(type(obj)) != "<class 'function'>":
        # obj is sequence or iterator or generator
        for i in obj:
            if s == 1:  # multiplying to 1 would not work for types with neutral element != 1
                s = i
            else:
                s *= i
    else: # obj is a function
        for i in fromTo(start, end, step):
            if s == None:
                s = obj(i)
            else:
                s *= obj(i)
    return s

#............................................................

def closure(elements, operation):
    """ <b>closure(elements, operation)</b><br/>
        <i>The closure of the elements under the binary operation</i><br/>
        operation must be given as a function with two parameters.
    ----de----
        <b>closure(elements, operation)</b><br/>
        <i>Der Abschluss der Menge elements unter der binären Operation operation</i><br/>
        operation muss eine function mit zwei Parametern sein.
    """
    closureList = list(elements)
    stringList = [str(x) for x in closureList]
    more = True
    while more:
        more = False
        for x in closureList:
            for y in closureList:
                e = operation(x,y)
                s = str(e)
                if s not in stringList:
                    closureList.append(e)
                    stringList.append(s)
                    more = True
    closureList.sort(key=str)
    return closureList

#............................................................

def join(l, filler=""):
    """ <b>join(l, filler="")</b><br/>
        <i>Joins the elements of <code>l</code> to a string</i><br/>
        Between each two adjacent elements <code>filler</code> is inserted.<br/>
        This is a replacement for the string method <code>join()</code>.<br/>
        In addition to the string method the list elements are converted to strings.<br/>
        <b>Example</b>:<br/>
        <code>join(fromTo(1,5), "-")</code>
        returns <code>"1-2-3-4-5"</code>.
    ----de----
        <b>join(l, filler="")</b><br/>
        <i>verbindet die Elemente von <code>l</code> zu einem String</i><br/>
        Zwischen je zwei Elemente wird <code>filler</code> eingefügt.<br/>
        Ersatz für die schwer lesbare String-Methode join()
        bzw. für die Funktion string.join().<br/>
        Im Gegensatz zu diesen Funktionen werden zusätzlich
        die Listenelemente zu Strings konvertiert.<br/>
        <b>Beispiel</b>:<br/>
        <code>join(fromTo(1,5), "-")</code>
        liefert <code>"1-2-3-4-5"</code>.
    """
    return filler.join([str(x) for x in l])


#===============================================================
#  Algorithms of Elementary Number Theory
#---------------------------------------------------------------

#---------------------------------------------------------------
#  Large random numbers

def rand(n):
    """ <b>rand(n)</b><br/>
        Integer random number from the range 0 ... <var>n</var>-1<br/>
        <var>n</var> must be a natural number (int).
    ----de----
        <b>rand(n)</b><br/>
        Zufallszahl im Bereich 0 ... <var>n</var>-1<br/>
        <var>n</var> kann eine beliebige natürliche Zahl sein (int).
    """
    return random.randrange(n)

#---------------------------------------------------------------
#  Conversion between positional notation systems

class NumeralSystem: # conversions between numeral systems # Umrechnung zwischen Stellenwertsystemen
    """ Class <code>NumeralSystem</code>: Numeral Systems<br/>
        This is a static class (only static methods available).
    ----de----
        Klasse <code>NumeralSystem</code>: Stellenwertsysteme<br/>
        Statische Klasse: nur Klassenmethoden.
    """
    #............................................................
    _staticMethods = ("toBase", "binary", "fromBase")

    @staticmethod
    def toBase(n, b, ascending=False):
        """ <b>toBase(n, b)</b><br/>
            <i><var>n</var> in the base <var>b</var> numeral system</i><br/>
            The result is returned as a list of the digits.<br/>
            <b>See also</b> <a href="#binary"><code>binary</code></a>,
                            <a href="#fromBase"><code>fromBase</code></a>.
        ----de----
            <b>toBase(n, b)</b><br/>
            <i><var>n</var> im Stellenwertsystem zur Basis <var>b</var></i><br/>
            Das Ergebnis wird als Liste der Stellen ausgegeben.<br/>
            <b>Siehe auch</b> <a href="#binary"><code>binary</code></a>,
                              <a href="#fromBase"><code>fromBase</code></a>.
        """
        v = []
        while n > 0:
            if ascending:
                v.append(n % b)    # insert n % b at the end
            else:
                v.insert(0, n % b) # insert n % b at the beginning
            n //= b
        return v

    @staticmethod
    def binary(n, ascending=False):
        """ <b>binary(n)</b><br/>
            <i>Binary number representation of <var>n</var> as list</i><br/>
            Abbreviation for <code>toBase(n,2)</code><br/>
            <b>Example</b>:<br/>
            <code>NumeralSystem.binary(11)</code> returns <code>[1, 0, 1, 1]</code>.<br/>
            <b>See also</b> <a href="#toBase"><code>toBase</code></a>,
                            <a href="#fromBase"><code>fromBase</code></a>.
        ----de----
            <b>binary(n)</b><br/>
            <i>Binärdarstellung von <var>n</var> als Liste</i><br/>
            Kurzform für <code>toBase(n,2)</code><br/>
            <b>Beispiel</b>:<br/>
            <code>NumeralSystem.binary(11)</code> gibt <code>[1, 0, 1, 1]</code> zurück.<br/>
            <b>Siehe auch</b> <a href="#toBase"><code>toBase</code></a>,
                              <a href="#fromBase"><code>fromBase</code></a>.
        """
        return NumeralSystem.toBase(n, 2, ascending)

    @staticmethod
    def fromBase(v, b, ascending=False):
        """ <b>fromBase(v, b)</b><br/>
            <i>Converts <var>v</var> from the base <var>b</var> numeral system to a number</i><br/>
            <var>v</var> is expected as list of digits.<br/>
            <b>See also</b> <a href="#toBase"><code>toBase</code></a>,
                            <a href="#binary"><code>binary</code></a>.
        ----de----
            <b>fromBase(v, b)</b><br/>
            <i><var>v</var> aus dem Stellenwertsystem zur Basis <var>b</var> als Zahl</i><br/>
            <var>v</var> wird als Liste der Stellen erwartet.<br/>
            <b>Siehe auch</b> <a href="#toBase"><code>toBase</code></a>,
                              <a href="#binary"><code>binary</code></a>.
        """
        n = 0
        if ascending:
            for i in fromTo(len(v)-1, 0, -1):
                n = b * n + i
        else:
            for i in v:
                n = b * n + i
        return n

#---------------------------------------------------------------
#  Integer functions

def binomial(n, k):
    """ <b>binomial(n, k)</b><br/>
        <i>Binomial coefficient</i><br/>
        n must be a nonnegative integer and k in the range 0 .. n.
    ----de----
        <b>binomial(n, k)</b><br/>
        <i>Binomialkoeffizient (n über k)</i><br/>
        n muss nichtnegative ganze Zahl sein, k im Bereich 0 .. n.
    """
    if k > n//2:
        k = n - k
    b = 1
    for i in fromTo(1, k):
        b = b * (n+1-i) // i
    return b

#............................................................

def factorial(n):
    """ <b>factorial(n)</b><br/>
        <i>n! = 1*2*3* ... *n (factorial of n)</i>
    ----de----
        <b>factorial(n)</b><br/>
        <i>n! = 1*2*3* ... *n (n Fakultät)</i>
    """
    f = 1
    for i in fromTo(2, n):
        f *= i
    return f

#............................................................

def fibonacci(n):
    """ <b>fibonacci(n)</b><br/>
        <i><var>n</var>-th element of the Fibonacci series</i>
        fibonacci(0) = 0<br/>
        fibonacci(1) = 1<br/>
        fibonacci(n) = fibonacci(n-1) + fibonacci(n-2) für n >= 2
    ----de----
        <b>fibonacci(n)</b><br/>
        <i><var>n</var>-tes Element der Fibonacci-Folge</i>
        fibonacci(0) = 0<br/>
        fibonacci(1) = 1<br/>
        fibonacci(n) = fibonacci(n-1) + fibonacci(n-2) für n >= 2
    """
    a, b = 0, 1
    while n > 0:
        a, b = b, a+b
        n -= 1
    return a

#---------------------------------------------------------------
#  Elementary number theoretic functions
#  Funktionen der elementaren Zahlentheorie

def gcd(a,b):
    """ <b>gcd(a,b)</b><br/>
        <i>greatest common divisor of a and b</i><br/>
        Calculated using the Euclidean algorithm<br/>
        <b>See also</b> <a href="#lcm"><code>lcm</code></a>
    ----de----
        <b>gcd(a,b)</b><br/>
        <i>Größter gemeinsamer Teiler von a und b</i><br/>
        Berechnung mit dem euklidischen Algorithmus<br/>
        <b>Siehe auch</b> <a href="#lcm"><code>lcm</code></a>
    """
    while b != 0:
        a, b = b, a % b
    return a

#............................................................

def _doc_gcdBinary(a,b):
    """ Binary Euclidean algorithm
        For optimization replace:
        x //= 2     by  x >>= 1
        x *= 2      by  x <<= 1
        x % 2 == 0  by  x &amp; 1 == 0
    ----de----
        Binärer Euklidischer Algorithmus
        Zur Optimierung sollte ersetzt werden:
        x //= 2     durch  x >>= 1
        x *= 2      durch  x <<= 1
        x % 2 == 0  durch  x &amp; 1 == 0
    """
    assert a >= b
    g = 1
    while a % 2 == 0 and b % 2 == 0:
        # größte gemeinsame Zweierpotenz:
        a //= 2
        b //= 2
        g *= 2
    while a != 0:
        while a % 2 == 0:
            a //= 2
        while b % 2 == 0:
            b //= 2
        t = abs(a-b) // 2
        if a >= b:
            a = t
        else:
            b = t
    return g * b

#............................................................

def _doc_gcdRecursive(a,b):
    """ recursive version of gcd
    ----de----
        rekursive Variante von gcd
    """
    if b == 0:
        return a
    else:
        return _doc_gcdRecursive(b, a % b)

#............................................................

def gcdExt(a,b):
    """ <b>gcdExt(a,b)</b><br/>
        <i>Extended Euclidean algorithm.</i><br/>
        Returns (d, x, y) with d == gcd(a,b) and x*a + y*b == d
    ----de----
        <b>gcdExt(a,b)</b><br/>
        <i>Erweiterter Euklidischer Algorithmus.</i><br/>
        Gibt (d, x, y) zurück mit d == gcd(a,b) und  x*a + y*b == d
    """
    if b == 0:                 # Basis for Math. Induction over b
        return a, 1, 0         # gcd(a,b) == 1*a + 0*b
    else:
        q, r = divmod(a, b)    # r < b
        d, z, x = gcdExt(b, r) # d == z*b + x*r (Induction assumption)
                               #   == z*b + x*(a - b*q)
                               #   == x*a + (z - x*q)*b
        y = z - x * q          # d == x*a + y*b
        return d, x, y

#............................................................

def lcm(a, b):
    """ <b>lcm(a,b)</b><br/>
        <i>Least common multiple of <var>a</var> and <var>b</var></i><br/>
        <b>See also</b> <a href="#gcd"><code>gcd</code></a>
    ----de----
        <b>lcm(a,b)</b><br/>
        <i>Kleinstes gemeinsames Vielfaches von <var>a</var> und <var>b</var></i><br/>
        <b>Siehe auch</b> <a href="#gcd"><code>gcd</code></a>
    """
    assert a > 0 and b > 0
    return (a * b) // gcd(a, b)

#............................................................

def _chinese2(mod1, mod2):
    m, n = mod1.m, mod2.m
    a, b = mod1.n, mod2.n
    d, x, y = gcdExt(m,n)
    assert d == 1    # m*x + n*y == 1
    x1 = x % n       # m*x1 % n == 1
    y1 = y % m       # n*y1 % m == 1
    c = n * y1       # c % m == 1
                     # c % n == 0
    d = m * x1       # d % n == 1
                     # d % m == 0
                     # (a*c + b*d) % m == a
                     # (a*c + b*d) % n == b
    return Mod(a*c + b*d, m*n)

def chinese(mod1, *mod2):
    """ <b>chinese(mod1, mod2,...)</b><br/>
        <i>Mod object from mod1, mod2, ... according to the Chinese remainder theorem</i><br/>
        Requires that mod1, mod2, ... are Mod objects with pairwise coprime moduli.<br/>
        <code>chinese(Mod(a1, m1), Mod(a2, m2), ...)</code><br/>
        returns Mod(x, m1*m2*...) with 0 &lt;= <code>x</code> &lt; m1*m2*... and<br/>
        <code>x % m1 == a1</code><br/>
        <code>x % m2 == a2</code><br/>, ...<br/>
        <b>Example</b>:<pre>
    chinese(Mod(1,5), Mod(2,7))  # ---> Mod(16,35)</pre>
        (16 mod 5 = 1, 16 mod 7 = 2).<br/>
        <b>See also</b> <a href="Mod.html"><code>Mod</code></a>
    ----de----
        <b>chinese(mod1, mod2,...)</b><br/>
        <i>Mod-Objekt zu mod1 mod2, ... nach Chinesischem Restsatz</i><br/>
        Voraussetzung: mod1, mod2, ... sind Mod-Objekte mit paarweise
        teilerfremden Moduln.<br/>
        <code>chinese(Mod(a1, m1), Mod(a2, m2, ...)</code><br/>
        berechnet Mod(x, m1*m2*...) mit 0 &lt;= <code>x</code> &lt; m1*m2*... und<br/>
        <code>x % m1 == a1</code><br/>
        <code>x % m2 == a2</code><br/>, ...<br/>
        <b>Beispiel</b>:<pre>
    chinese(Mod(1,5), Mod(2,7))  # ---> Mod(16,35)</pre>
        (16 mod 5 = 1, 16 mod 7 = 2).<br/>
        <b>Siehe auch</b> <a href="Mod.html"><code>Mod</code></a>
    """
    r = mod1.copy()
    for m in mod2:
        r = _chinese2(r, m)
    return r

#............................................................

def eulerPhi(n):
    """ <b>eulerPhi(n)</b><br/>
        <i>Euler's totient function</i><br/>
        Number of positive integers less than or equal to <var>n</var> that are coprime to <var>n</var>
    ----de----
        <b>eulerPhi(n)</b><br/>
        <i>Eulersche Phi-Funktion</i><br/>
        Anzahl der zu <var>n</var> teilerfremden Zahlen im Bereich von 1 bis <var>n</var>
    """
    #--- directly following the definition:
    # return sum([int(gcd(i,n) == 1) for i in Range(n)])

    #--- more efficient:
    for p in factors(n):
        n = n*(p-1)//p
    return n

#............................................................

def expMod(a, x, m):
    """ <b>expMod(a, x, m)</b><br/>
        <i>Calculates (a**x) % m efficiently.</i><br/>
        Equivalent to the Python standard function <code>pow</code>.
    ----de----
        <b>expMod(a, x, m)</b><br/>
        <i>Berechnet (a**x) % m effizient.</i><br/>
        Synonym für die Python-Standardfunktion <code>pow</code>.
    """
    return pow(a, x, m)  # Python standard function pow

#............................................................
# The following two implementations are for documentation only:
# Die folgenden beiden Implementierungen dienen nur der Dokumentation:

def _doc_expMod_recursive(a, x, n):
    """ recursive implementation of expMod
        (Stack overflow possible!)
    ----de----
        rekursive Implementierung von expMod
        (Stack-Überlauf möglich!)
    """
    if x == 1:
        return a % n
    elif x % 2 == 1:
        return (a * _doc_expMod_recursive(a,x-1,n)) % n
    else:
        t = _doc_expMod_recursive(a, x//2, n)
        return (t * t) % n

def _doc_expMod_iter(a,x,n):
    """ iterative implementation of expMod
    ----de----
        iterative Implementierung von expMod
    """
    b, c = 1, a      # TODO: verification
    while x > 1:
        if x % 2 == 1:
            b = (b*c) % n
        c = (c*c) % n
        x = x // 2
    return (b*c) % n

#............................................................

def _divisor(n):
    """ Pollard-Rho Algorithm (see Cormen, p. 845)
    """
    i, k = 1, 2
    x = rand(n)
    y = x
    while i < n:
        i += 1
        x = (x*x-1) % n
        d = gcd(y-x, n)
        if d != 1 and d != n:
            return d
        if i == k:
            y = x
            k = 2 * k

#............................................................

def isPrime(n,s=20):
    """ <b>isPrime(n)</b><br/>
        <i>Returns bool (is <var>n</var> a prime number?)</i><br/>
        Implementation using Miller Rabin test<br/>
        (see. Cormen, Leiserson, Rivest: Algorithms, p. 841)<br/>
        <b>See also</b> <a href="#prime"><code>prime</code></a>,
                        <a href="#nextPrime"><code>nextPrime</code></a>.
    ----de----
        <b>isPrime(n)</b><br/>
        <i>Rückgabewert bool (ist <var>n</var> Primzahl?)</i><br/>
        Implementierung mit Miller-Rabin-Test<br/>
        (vgl. Cormen, Leiserson, Rivest: Algorithms, S. 841)<br/>
        <b>Siehe auch</b> <a href="#prime"><code>prime</code></a>,
                          <a href="#nextPrime"><code>nextPrime</code></a>.
    """
    def _witness(a,n):
        """ witness for n being a product (see. Cormen p. 840)
            Local helper function
        ----de----
            Zeuge dafür, dass n zusammengesetzt ist (vgl. Cormen S. 840)
            Lokale Hilfsfunktion
        """
        b = NumeralSystem.binary(n-1)
        d = 1
        for i in b:
            x = d
            d = (d*d) % n
            if d == 1 and x != 1 and x != n-1:
                return True
            if i == 1:
                d = (d*a) % n
        return d != 1

    if n < 4:
        return n > 1
    for i in fromTo(1, s):
        a = 1 + rand(n-1)
        if _witness(a,n):
            return False
    return True

#............................................................
# nextPrime

def nextPrime(n):
    """ <b>nextPrime(n)</b><br/>
        <i>next prime larger than or equal to <var>n</var></i><br/>
        <b>See also</b> <a href="#isPrime"><code>isPrime</code></a>,
                        <a href="#prime"><code>prime</code></a>.
    ----de----
        <b>nextPrime(n)</b><br/>
        <i>nächste Primzahl ab (einschließlich) <var>n</var></i><br/>
        <b>Siehe auch</b> <a href="#isPrime"><code>isPrime</code></a>,
                          <a href="#prime"><code>prime</code></a>.
    """
    if n < 3:
        return 2
    if n % 2 == 0:
        n += 1
    while not isPrime(n):
        n += 2
    return n

#............................................................

# pre-calculated primes:
#        prime(0), prime(1000), prime(2000), ..., prime(99000),
_P1000 = [     0,    7919,   17389,   27449,   37813,   48611,   59359,   70657,   81799,   93179,
          104729,  116447,  128189,  139901,  151703,  163841,  176081,  187963,  200183,  212369,
          224737,  237203,  249439,  262139,  274529,  287117,  300023,  312583,  324949,  337541,
          350377,  363269,  376127,  389171,  401987,  414977,  427991,  440723,  453889,  467213,
          479909,  493127,  506131,  519227,  532333,  545747,  559081,  572311,  585493,  598687,
          611953,  625187,  638977,  652429,  665659,  679277,  692543,  706019,  719639,  732923,
          746773,  760267,  773603,  787207,  800573,  814279,  827719,  841459,  855359,  868771,
          882377,  896009,  909683,  923591,  937379,  951161,  965113,  978947,  993107, 1006721,
         1020379, 1034221, 1048129, 1062511, 1076143, 1090373, 1103923, 1117579, 1131617, 1145689,
         1159523, 1173301, 1187003, 1200949, 1215133, 1229269, 1243709, 1257517, 1271293, 1285517]

def prime(n):
    """ <b>prime(n)</b><br/>
        <i><var>n</var>-th prime number</i><br/>
        <b>See also</b> <a href="#isPrime"><code>isPrime</code></a>,
                        <a href="#nextPrime"><code>nextPrime</code></a>.
    ----de----
        <b>prime(n)</b><br/>
        <i><var>n</var>-te Primzahl</i><br/>
        <b>Siehe auch</b> <a href="#isPrime"><code>isPrime</code></a>,
                          <a href="#nextPrime"><code>nextPrime</code></a>.
    """
    if n < 100000:
        t, e = divmod(n, 1000)
        p = _P1000[t]
        for i in range(e):
            p = nextPrime(p+1)
        return p
    else:
        raise _MathGUIdeError("Argument is to large (must be &lt; 100000)!")

#............................................................

def factor(n):
    """ <b>factor(n)</b><br/>
        <i>Prime factorization</i><br/>
        Returns a list of pairs [factor, multiplicity]<br/>
        <b>Example</b>:<br/>
        <code>factor(24)</code><br/>
        returns <code>[[2,3],[3,1]]</code>.<br/>
        <b>See also</b> <a href="#factors"><code>factors</code></a>,
                        <a href="#allFactors"><code>allFactors</code></a>.
    ----de----
        <b>factor(n)</b><br/>
        <i>Primfaktorenzerlegung</i><br/>
        Rückgabewert: Liste mit Paaren [Primfaktor, Exponent]<br/>
        <b>Beispiel</b>:<br/>
        <code>factor(24)</code><br/>
        gibt <code>[[2,3],[3,1]]</code> zurück.<br/>
        <b>Siehe auch</b> <a href="#factors"><code>factors</code></a>,
                          <a href="#allFactors"><code>allFactors</code></a>.
    """
    p = 2
    i = 0
    r = []
    while p*p <= n:
        while n % p == 0:
            i += 1
            n //= p
        if i > 0:
            r.append([p,i])
        p = nextPrime(p+1)
        i = 0
    if n > 1:
        r.append([n,1])
    return r

#............................................................

def factors(n):
    """ <b>factors(n)</b><br/>
        <i>List of prime factors of <var>n</var> (without multiplicity)</i><br/>
        <b>Example</b>:<br/>
        <code>factors(24)</code>
        returns <code>[2, 3]</code>.<br/>
        <b>See also</b> <a href="#factor"><code>factor</code></a>,
                        <a href="#allFactors"><code>allFactors</code></a>.
    ----de----
        <b>factors(n)</b><br/>
        <i>Liste der Primfaktoren von <var>n</var> (jeder nur einmal)</i><br/>
        <b>Beispiel</b>:<br/>
        <code>factors(24)</code>
        gibt <code>[2, 3]</code> zurück.<br/>
        <b>Siehe auch</b> <a href="#factor"><code>factor</code></a>,
                            <a href="#allFactors"><code>allFactors</code></a>.
    """
    return [f[0] for f in factor(n)]

#............................................................

def allFactors(n):
    """ <b>allFactors(n)</b><br/>
        <i>List of all prime factors of <var>n</var></i><br/>
        <b>Example</b>: <code>allFactors(24)</code>
        returns <code>[2, 2, 2, 3]</code>.<br/>
        <b>See also</b> <a href="#factor"><code>factor</code></a>,
                        <a href="#factors"><code>factors</code></a>.
    ----de----
        <b>allFactors(n)</b><br/>
        <i>Liste aller Primfaktoren von <var>n</var></i><br/>
        <b>Beispiel</b>: <code>allFactors(24)</code>
        gibt <code>[2, 2, 2, 3]</code> zurück.<br/>
        <b>Siehe auch</b> <a href="#factor"><code>factor</code></a>,
                          <a href="#factors"><code>factors</code></a>.
    """
    p = 2
    r = []
    while p*p <= n:
        while n % p == 0:
            r.append(p)
            n //= p
        p = nextPrime(p+1)
    if n > 1:
        r.append(n)
    return r

#............................................................

def divisors(n):
    """ <b>divisors(n)</b><br/>
        <i>List of all divisors of <var>n</var></i>
    ----de----
        <b>divisors(n)</b><br/>
        <i>Liste der Teiler von <var>n</var></i>
    """
    # TODO: more efficient implementation!
    d = []
    for i in fromTo(1, n//2):
        if n % i == 0:
            d.append(i)
    d.append(n)
    return d


#===============================================================
#  Rational numbers and continued fractions
#  Rationale Zahlen, Kettenbrüche
#---------------------------------------------------------------

class Rational: # Rational numbers # Rationale Zahlen
    """ Class <code>Rational</code>: Rational numbers<br/>
        <b>Rational(p, q)</b> Example: <code>Rational(3,17)</code><br/>
        <b>Rational(n)</b> Example: <code>Rational(3)</code><br/>
        <b>Rational(s)</b> Example: <code>Rational("3/17")</code><br/>
        For <code>Rational</code> objects the arithmetical operators
        +, -, *, / are defined.
    ----de----
        Klasse <code>Rational</code>: Rationale Zahl<br/>
        <b>Rational(p, q)</b> Beispiel: <code>Rational(3,17)</code><br/>
        <b>Rational(n)</b> Beispiel: <code>Rational(3)</code><br/>
        <b>Rational(s)</b> Beispiel: <code>Rational("3/17")</code><br/>
        <code>Rational</code>-Objekte können mit den
        arithmetischen Operatoren +, -, *, / verknüpft werden.
    """
    def operators(self):
        """ <b>Rational.operators()</b><br/>
            <i>For documentation only</i><br/>
            The following operators are defined in the class <b>Rational</b>:<br/>
            <table>
             <tr><th>Op.</th><th>Function</th><th>Examples</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraction</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Multiplication</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>/</th><td>Division</td><td><code>a / b; a /= b</code></td></tr>
             <tr><th>-</th><td>Unary minus</td><td><code>-a</code></td></tr>
             <tr><th>&lt; > &lt;= >=</th><td>Comparision operators</td><td></td></tr>
            </table>
            <font color="#000080" size="-2">This method is for documentation only. It has no effect.</font>
        ----de----
            <b>Rational.operators()</b><br/>
            <i>Nur zur Dokumentation</i><br/>
            Die Klasse <b>Rational</b> erlaubt folgende Operatoren:<br/>
            <table>
             <tr><th>Op.</th><th>Funktion</th><th>Beispiele</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraktion</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Multiplikation</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>/</th><td>Division</td><td><code>a / b; a /= b</code></td></tr>
             <tr><th>-</th><td>unäres Minus</td><td><code>-a</code></td></tr>
             <tr><th>&lt; > &lt;= >=</th><td>Vergleichsoperatoren</td><td></td></tr>
            </table>
            <font color="#000080" size="-2">Diese Methode dient nur zur Dokumentation. Sie hat keine Wirkung.</font>
        """
        pass

    def __init__(self, p, q=1):
        """ Constructor
        ----de----
            Konstruktor
        """
        if type(p) == float:
            r = ContFrac(p,12).toRational()
            self.p, self.q = r.p, r.q
            return
        if type(p) == Rational:
            self.p, self.q = p.p, p.q
            return
        if type(p) == str:   #  e.g. "3/17"
            l = p.split("/")
            p = int(l[0])
            if len(l) > 1:
                q = int(l[1])
        if q < 0:
            p,q = -p, -q  # denominator always positive
        d = gcd(abs(p), q)
        if d > 1:         # reduce fraction if possible
            p //= d
            q //= d
        self.p = p
        self.q = q

    def copy(self):
        """ <b>r.copy()</b><br/>
            <i>independent copy ("clone") of r</i>
        ----de----
            <b>r.copy()</b><br/>
            <i>unabhängige Kopie von r</i>
        """
        return Rational(self.p, self.q)

    def __abs__(self):
        """ absolute value
        ----de----
            Absolutbetrag
        """
        return Rational(abs(self.p), self.q)

    def abs(self):
        """ <b>r.abs()</b><br/>
            <i>absolute value</i><br/>
            Also as global function: <b>abs(r)</b><br/>
        ----de----
            <b>r.abs()</b><br/>
            <i>Absolutbetrag</i><br/>
            Auch globale Funktion: <b>abs(r)</b><br/>
        """
        return Rational(abs(self.p), self.q)

    def __float__(self):
        """ Conversion to floating point number
        ----de----
            Umwandlung in Gleitkommazahl
        """
        return float(self.p) / float(self.q)

    def toFloat(self):
        """ <b>r.toFloat()</b><br/>
            <i>Conversion to floating point number</i><br/>
            Also as global function: <b>float(r)</b><br/>
        ----de----
            <b>r.toFloat()</b><br/>
            <i>Konvertierung in Gleitkommazahl</i><br/>
            Auch globale Funktion: <b>float(r)</b><br/>
        """
        return float(self.p) / float(self.q)

    def __int__(self):
        """ Round to the nearest integer
        ----de----
            Auf-/Abrundung zur nächsten Ganzzahl
        """
        assert self.q > 0
        return (self.p + self.q/2) // self.q

    def isInteger(self):
        return self.q == 1

    def toInt(self):
        """ <b>r.toInt()</b><br/>
            <i>Round to the nearest integer</i><br/>
            Also as global function: <b>int(r)</b><br/>
        ----de----
            <b>r.toInt()</b><br/>
            <i>Rundung zur nächsten ganzen Zahl</i><br/>
            Auch globale Funktion: <b>int(r)</b><br/>
        """

    def __long__(self):
        """ Round to the nearest integer
        ----de----
            Auf-/Abrundung zur nächsten Ganzzahl
        """
        assert self.q > 0
        return (self.p + self.q/2) // self.q

    def _unify(self, other):
        return (self, Rational(other))

    def __repr__(self):
        """ Object representation as string
        """
        if self.q == 1:
            return str(self.p)
        else:
            return "{0}/{1}".format(self.p, self.q)

    def toStr(self):
        """ <b>r.toStr()</b><br/>
            <i>Representation as string</i><br/>
            Also as global function: <b>str(r)</b><br/>
        ----de----
            <b>r.toStr()</b><br/>
            <i>Darstellung als String</i><br/>
            Auch globale Funktion: <b>str(r)</b><br/>
        """

    def _cmp(a, b):
        if type(b) == float:
            return _cmp(float(a), b)
        if not isinstance(b, Rational):
            a,b = a._unify(b)
        return _cmp(a.p*b.q, b.p*a.q)

    def __lt__(a,b): return a._cmp(b) < 0
    def __le__(a,b): return a._cmp(b) <= 0
    def __eq__(a,b): return a._cmp(b) == 0
    def __ge__(a,b): return a._cmp(b) >= 0
    def __gt__(a,b): return a._cmp(b) > 0
    def __ne__(a,b): return a._cmp(b) != 0

    def __neg__(self):
        """ Unary operator -
            -a --> a.__neg__()
        """
        return Rational(-self.p, self.q)

    def __add__(a, b):
        """ <b>a.__add__(b)</b><br/>
            <i>Addition</i><br/>
            Also operator notation: <b>a + b</b><br/>
            Instead of <code>a = a + b</code> you can write <code>a += b</code>.
        """
        if type(b) == float:
            return float(a) + b
        if not isinstance(b, Rational):
            a, b = a._unify(b)
        return Rational(a.p*b.q + b.p*a.q, a.q*b.q)

    def __radd__(a, b):
        """ Addition, see __add__
        """
        return a + b

    def __sub__(a, b):
        """ Subtraction:
            a - b  --> a.__sub__(b),  if a is of type Rational
                       b.__rsub__(a), otherwise
        """
        if type(b) == float:
            return float(a) - b
        if not isinstance(b, Rational):
            a,b = a._unify(b)
        return Rational(a.p*b.q - b.p*a.q, a.q*b.q)

    def __rsub__(b, a):
        """ Subtraction, see __sub__
        """
        if type(a) == float:
            return a - float(b)
        if not isinstance(a, Rational):
            b,a = b._unify(a)
        return a - b

    def __mul__(a, b):
        """ Multiplication:
            a * b  --> a.__mul__(b),  if a is of type Rational
                       b.__rmul__(a), otherwise
        """
        if type(b) == float:
            return float(a) * b
        if type(b) in (Matrix, Poly):
            return b * a
        if not isinstance(b, Rational):
            a,b = a._unify(b)
        return Rational(a.p*b.p, a.q*b.q)

    def __rmul__(b, a):
        """ Multiplication, see __mul__
        """
        return b * a

    def __div__(a, b):
        """ exact Division
            a / b  --> a.__div__(b),  if a is of type Rational
                       b.__rdiv__(a), otherwise
        """
        if type(b) == float:
            return float(a) / b
        if not isinstance(b, Rational):
            a,b = a._unify(b)
        return Rational(a.p*b.q, a.q*b.p)

    def __rdiv__(b, a):
        """ exact Division, see __div__
        """
        if type(a) == float:
            return a / float(b)
        if not isinstance(a, Rational):
            b,a = b._unify(a)
        return Rational(a.p*b.q, a.q*b.p)

    def __truediv__(a, b):   return a.__div__(b)
    def __rtruediv__(a, b):  return a.__rdiv__(b)
    def __floordiv__(a, b):  return a.__div__(b)
    def __rfloordiv__(a, b): return a.__rdiv__(b)

def toNumber(s):
    """ <b>toNumber(s)</b><br/>
        <i>converts the string s into a rational or floating point number</i><br/>
        <b>Examples</b>:<br/>
        <code>toNumber("3.1")</code><br/>
        <code>toNumber("7/3")</code>
        ----de----
        <b>toNumber(s)</b><br/>
        <i>verwandelt den String s in eine rationale oder Gleitkommazahl</i><br/>
        <b>Beispiele</b>:<br/>
        <code>toNumber("3.1")</code><br/>
        <code>toNumber("7/3")</code>
    """
    if "." in s or "e" in s or "E" in s:
        return float(s)
    else:
        return Rational(s)

#---------------------------------------------------------------

class ContFrac: # Continued fractions # Kettenbrüche
    """ <b>ContFrac(number, steps=5)</b><br/>
        <i>Continued fraction from <b>number</b> with maximal length <b>steps</b></i><br/>
        <b>Example</b>:<br/>
        <code>ContFrac(pi, 3)</code> returns
        <code>ContFrac(3, 7, 15)</code>.<br/>
        This represent the number 3 + 1/(7 + 1/15).
        ----de----
        <b>ContFrac(number, steps=5)</b><br/>
        <i>Kettenbruch aus <b>number</b> mit Maximallänge <b>steps</b></i><br/>
        <b>Beispiel</b>:<br/>
        <code>ContFrac(pi, 3)</code> ergibt
        <code>ContFrac(3, 7, 15)</code>.<br/>
        Das steht für die Zahl 3 + 1/(7 + 1/15).
    """
    def __init__(self, number, steps=5):
        """ Constructor
        ----de----
            Konstruktor
        """
        self._l = []
        for i in range(steps):
            i = floor(number)
            self._l.append(i)
            if abs(number-i) < 0.00000001:
                break
            number = 1 / (number-i)

    def __repr__(self):
        """ Object representation as string
        """
        return "ContFrac({0})".format(join(self._l, ","))

    def toRational(self, n=0):
        """ <b>toRational(n=0)</b><br/>
            <i>Rational number defined by the first n list elements</i><br/>
            Converts the continued fraction int a rational number.<br/>
            <b>Example</b>:<br/>
            <code>ContFrac(pi).toRational(2)</code> returns 22/7.
        ----de----
            <b>toRational(n=0)</b><br/>
            <i>Rationale Zahl aus den ersten n Gliedern</i><br/>
            Wandelt den Kettenbruch in eine rationale Zahl um.<br/>
            <b>Beispiel</b>:<br/>
            <code>ContFrac(pi).toRational(2)</code> ergibt 22/7.
        """
        def contFracListToRat(cf):
            if len(cf) == 0: return Rational(0)
            if len(cf) == 1: return Rational(cf[0])
            return cf[0] + Rational(1) / contFracListToRat(cf[1:])

        if n == 0 or n > len(self._l):
            n = len(self._l)
        return contFracListToRat(self._l[0:n])

#===============================================================
#  Lineare Algebra
#  Klassen Vector und Matrix
#---------------------------------------------------------------

class Vector (list): # Vectors # Vektoren
    """ Class <code>Vector</code> (with arithmetic operators)<br/>
        <b>Vector(v)</b> (v Liste or Tuple)<br/>
        The Indices of the elements of Vector are counted from 0 (as in Python lists etc.).
        <b>Example:</b><br/>
        <code>Vector([1,2,3])</code>
        ----de----
        Klasse <code>Vector</code> (mit arithmetischen Operatoren)<br/>
        <b>Vector(v)</b> (v Liste oder Tupel)<br/>
        Die Indizes der Elemente von Vector werden (wie in Python-Listen etc.) ab 0 gezählt.
        <b>Beispiel:</b><br/>
        <code>Vector([1,2,3])</code>
    """
    #............................................................
    _staticMethods = ("fromFunction", "fromString")

    @staticmethod
    def fromFunction(n, fn, offset=0):
        """ <b>Vector.fromFunction(n, fn, offset=0)</b><br/>
            <i>Vector, the elements of which are to be calculated using the function fn</i><br/>
            Returns: <code>Vector([fn(i+offset) for i in range(n)])</code><br/>
            <b>Example:</b><br/>
            <code>Vector.fromFunction(5, sqrt, 1)</code> returns Vector([1.0, 1.414, 1.732, 2.0, 2.236])
        ----de----
            <b>Vector.fromFunction(n, fn, offset=0)</b><br/>
            <i>Vector, dessen Elemente mit der Funktion fn berechnet werden</i><br/>
            Rückgabewert: <code>Vector([fn(i+offset) for i in range(n)])</code><br/>
            <b>Beispiel:</b><br/>
            <code>Vector.fromFunction(5, sqrt, 1)</code> ergibt Vector([1.0, 1.414, 1.732, 2.0, 2.236])
        """
        return Vector([fn(i+offset) for i in range(n)])

    @staticmethod
    def fromString(s):
        """ <b>Vector.fromString(s)</b><br/>
            <i>Vector from String</i><br/>
            Elements (may also bbe Rationals) must be given as comma separated string.<br/>
            <b>Example:</b><br/>
            <code>Vector.fromString("1, 2/3, 2")</code>
        ----de----
            <b>Vector.fromString(s)</b><br/>
            <i>Vector aus String</i><br/>
            Elemente (auch rationale Zahlen) müssen im String durch Kommas getrennt werden.<br/>
            <b>Beispiel:</b><br/>
            <code>Vector.fromString("1, 2/3, 2")</code>
        """
        return Vector([toNumber(x) for x in s.split(",")])
    #............................................................

    def operators(self):
        """ <b>Vector.operators()</b><br/>
            <i>For documentation only</i><br/>
            The following operators are defined in the class <b>Vector</b>:<br/>
            <table>
             <tr><th>Op.</th><th>Function</th><th>Examples</th></tr>
             <tr><th>+</th><td>elementwise Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>elementwise Subtraction</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>if both operators are Vectors: inner product,
               otherwise: scalar multiplication (elementwise)</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>-</th><td>Unary minus</td><td><code>-a</code></td></tr>
             <tr><th>[ ]</th><td>Index operator</td><td><code>A[i]</code> (Element)</td></tr>
            </table>
            <font color="#000080" size="-2">This method is for documentation only. It has no effect.</font>
        ----de----
            <b>Vector.operators()</b><br/>
            <i>Nur zur Dokumentation</i><br/>
            Die Klasse <b>Vector</b> erlaubt folgende Operatoren:<br/>
            <table>
             <tr><th>Op.</th><th>Funktion</th><th>Beispiele</th></tr>
             <tr><th>+</th><td>Elementweise Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Elementweise Subtraktion</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>wenn beide Operanden Vektoren sind: Skalarprodukt,
                sonst Skalarmultiplikation (elementweise)</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>-</th><td>unäres Minus</td><td><code>-a</code></td></tr>
             <tr><th>[ ]</th><td>Indexoperator</td><td><code>A[i]</code> (Element)</td></tr>
            </table>
            <font color="#000080" size="-2">Diese Methode dient nur zur Dokumentation. Sie hat keine Wirkung.</font>
        """
        pass

    def __init__(v, n):
        """ Constructor
        ----de----
            Konstruktor
        """
        if isinstance(n, list):
            list.__init__(v, n)
        elif isinstance(n, tuple):
            list.__init__(v, list(n))

    def __repr__(A):
        """ Object representation as string
        """
        return "Vector({0})".format(list.__repr__(A))

    def __invert__(v):
        """ Operator ~ (transposition)
        """
        return v.transp()

    def transp(v):
        """ <b>v.transp()</b><br/>
            <i>berechnet die transponierte Matrix (Spaltenvektor)</i><br/>
            kürzer: ~v
        ----de----
            <b>v.transp()</b><br/>
            <i>calculates the transposed Matrix (column vektor)</i><br/>
            shorter: ~v
        """
        return Matrix([[a] for a in v])

    def join(x, y):
        """ <b>v.join(w)</b><br/>
            <i>Concatenation with Vector w</i>
        ----de----
            <b>v.join(w)</b><br/>
            <i>Verkettung mit dem Vektor w</i>
        """
        return Vector(list(x)+list(y))

    def __neg__(v):
        """ unary oparator - (negative Vektor)
        """
        return Vector([-x for x in v])

    def __add__(x, y):
        """ operators + and += (Elementwise addition)
        """
        #   a + b  --> a.__add__ (b), if a is of type Vector
        #              b.__radd__(a), otherwise
        n = len(x)
        assert n == len(y)
        return Vector([x[i]+y[i] for i in range(n)])

    def __sub__(x, y):
        """ operators - and -= (elementwise subtraction)
        """
        return x + (-y)

    def __mul__(x, y):
        """ Operators * and *=
            Multiplikation
            if both operands are vektors sind: inner product,
            otherwise scalar multiplication (elementwise)
        """
        if isinstance(y, Vector): # Skalarprodukt
            n = len(x)
            assert n == len(y)
            return sum([x[i]*y[i] for i in range(n)])
        else:
            return Vector([y*x[i] for i in range(len(x))])

    def __rmul__(x, c):
        return x * c

    def __imul__(x, c):
        for i in range(len(x)):
            x[i] *= c
        return x

    def norm(x):
        """ <b>v.norm()</b><br/>
            <i>Norm des Vektors v</i>
        ----de----
            <b>v.norm()</b><br/>
            <i>Norm of the Vector v</i>
        """
        return math.sqrt(x*x)



class Matrix (Vector): # Matrices # Matrizen
    """ class <code>Matrix</code><br/>
        <i>see also: Menu: Insert -- Matrix</i><br/>
        <b>Matrix(v)</b>: v list of lists with uniform length.<br/>
        <b>Example</b>:<br/>
        <code>Matrix([[11,12,13],[21,22,23]])</code><br/>
        The class <code>Matrix</code> ist derived from <code>Vector</code>:<br/>
        A <code>Matrix</code> object ist a <code>Vector</code>
        of <code>Vector</code> objects with uniform length.<br/>
        The elements can be addressed using double indices (e.g.: <code>A[i,k]</code>).
        Indices are counted from 0 (as in Python lists etc.).<br/>
        see also. class methods with <code>Matrix.</code>
        ----de----
        Klasse <code>Matrix</code><br/>
        <i>vgl. Menü: Einfügen -- Matrix</i><br/>
        <b>Matrix(v)</b>: v Liste von gleichlangen Listen,<br/>
        <b>Beispiel</b>:<br/>
        <code>Matrix([[11,12,13],[21,22,23]])</code><br/>
        Die Klasse <code>Matrix</code> ist abgeleitet von <code>Vector</code>:<br/>
        Ein <code>Matrix</code>-Objekt ist ein <code>Vector</code>
        von gleichlangen <code>Vector</code>-Objekten.<br/>
        Die Elemente können mit Doppelindizes (z.B.: <code>A[i,k]</code>) angesprochen werden.
        Indizes werden (wie in Python-Listen etc.) ab 0 gezählt.<br/>
        vgl. Klassenmethoden mit <code>Matrix.</code>
    """
    #............................................................
    _staticMethods = ("null", "identity", "fromFunction", "fromString", "random")

    @staticmethod
    def null(m, n=0):
        """ <b>Matrix.null(m, n=0)</b><br/>
            <i>m*n-Null matrix</i><br/>
            Square Matrix, if n is omitted.<br/>
        ----de----
            <b>Matrix.null(m, n=0)</b><br/>
            <i>m*n-Nullmatrix</i><br/>
            Quadratische Matrix, falls n weggelassen wird.<br/>
        """
        if n == 0:
            n = m
        return Matrix([ [0 for k in range(n)]
                        for i in range(m)])

    @staticmethod
    def identity(n):
        """ <b>Matrix.identity(n)</b><br/>
            <i>n*n Identity Matrix</i><br/>
        ----de----
            <b>Matrix.identity(n)</b><br/>
            <i>n*n-Einheitsmatrix</i><br/>
        """
        return Matrix([ [Rational(int(i==k)) for k in range(n)]
                        for i in range(n)])

    @staticmethod
    def fromFunction(m, n, fn, offset=0):
        """ <b>Matrix.fromFunction(m,n, fn, offset=0)</b><br/>
            <i><code>m*n</code>-Matrix defined by function <code>fn</code></i><br/>
            <code>fn</code> must be a binary function.<br/>
            The Matrix element <code>A[i,k]</code> is defined as <code>fn(i+offset,k+offset)</code>.<br/>
            <b>Example</b>:<br/>
            <code>Matrix.fromFunction(2,4, pow, 1)</code><br/>
            returns the Matrix<pre>
     / 1 1 1  1 \
     |          |
     \ 2 4 8 16 /</pre>
        ----de----
            <b>Matrix.fromFunction(m,n, fn, offset=0)</b><br/>
            <i>Durch Funktion <code>fn</code> berechnete <code>m*n</code>-Matrix</i><br/>
            <code>fn</code> muss eine zweistellige Funktion sein.<br/>
            Das allgemeine Element <code>A[i,k]</code> wird mit <code>fn(i+offset,k+offset)</code> berechnet.<br/>
            <b>Beispiel</b>:<br/>
            <code>Matrix.fromFunction(2,4, pow, 1)</code><br/>
            liefert die Matrix<pre>
     / 1 1 1  1 \
     |          |
     \ 2 4 8 16 /</pre>
        """
        return Matrix([ [fn(i+offset,k+offset) for k in range(n)]
                        for i in range(m)])

    @staticmethod
    def fromString(s):
        """ <b>Matrix.fromString(s)</b><br/>
            <i>Matrix given by a string</i><br/>
            The string s must contains the elementa row by row.<br/>
            The rows are divided by semicolon, the elemente within a row by comma.<br/>
            Elemente may also be rational (notated with slash).<br/>
            <b>Example</b>:<br/>
            <code>Matrix.fromString("1, 2, 3.14; 4/5, 5, 6")</code>
            returns the matrix<pre>
     / 1    2  3.14 \
     |              |
     \ 4/5  5   6   /</pre>
        ----de----
            <b>Matrix.fromString(s)</b><br/>
            <i>Matrix aus String</i><br/>
            Im String s werden die Elemente der Matrix zeilenweise angegeben.<br/>
            Die Zeilen werden durch Semikolon getrennt, die Elemente innerhalb der Zeilen durch Komma.<br/>
            Elemente können auch rational sein (mit Schrägstrich).<br/>
            <b>Beispiel</b>:<br/>
            <code>Matrix.fromString("1, 2, 3.14; 4/5, 5, 6")</code>
            liefert die Matrix<pre>
     / 1    2  3.14 \
     |              |
     \ 4/5  5   6   /</pre>
        """
        return Matrix([ [toNumber(a) for a in row.split(",")]
                        for row in s.split(";")])

    @staticmethod
    def random(m,n=0, r=100):
        """ <b>Matrix.random(m,n=0, r=100)</b><br/>
            <i>m*n matrix with random elements in 0..r-1</i>
        ----de----
            <b>Matrix.random(m,n=0, r=100)</b><br/>
            <i>m*n-Matrix mit Zufallswerten 0..r-1</i>
        """
        if n == 0:
            n = m
        return Matrix([ [Rational(rand(r)) for k in range(n)]
                        for i in range(m)])
    #............................................................

    def operators(self):
        """ <b>Matrix.operators()</b><br/>
            <i>For documentation only</i><br/>
            The following operators are defined in the class <b>Matrix</b>:<br/>
            <table>
             <tr><th>Op.</th><th>Function</th><th width="30%">Examples</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraction</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Matrizenmultiplikation if both operands are Matrices, otherwise: Scalar multiplication</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>-</th><td>Unary minus</td><td><code>-a</code></td></tr>
             <tr><th>~</th><td>Transposed Matrix</td><td><code>~A</code></td></tr>
             <tr><th>|</th><td>Concatenation</td><td><code>A | B; A |= B</code></td></tr>
             <tr><th>[ ]</th><td>Index operator</td><td><code>A[i,k]</code><br/><code>A[i]</code> (Row Vector)</td></tr>
            </table>
            <font color="#000080" size="-2">This method is for documentation only. It has no effect.</font>
        ----de----
            <b>Matrix.operators()</b><br/>
            <i>Nur zur Dokumentation</i><br/>
            Die Klasse <b>Matrix</b> erlaubt folgende Operatoren:<br/>
            <table>
             <tr><th>Op.</th><th>Funktion</th><th width="30%">Beispiele</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>A + B; A += B</code></td></tr>
             <tr><th>-</th><td>Subtraktion</td><td><code>A - B; A -= B</code></td></tr>
             <tr><th>*</th><td>Matrizenmultiplikation (wenn beide Operanden Matrizen sind) bzw. Skalarmultiplikation (sonst)</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>-</th><td>unäres Minus</td><td><code>-A</code></td></tr>
             <tr><th>~</th><td>Transponierte Matrix</td><td><code>~A</code></td></tr>
             <tr><th>|</th><td>Verkettung</td><td><code>A | B; A |= B</code></td></tr>
             <tr><th>[ ]</th><td>Indexoperator</td><td><code>A[i,k]</code><br/><code>A[i]</code> (Zeilenvektor)</td></tr>
            </table>
            <font color="#000080" size="-2">Diese Methode dient nur zur Dokumentation. Sie hat keine Wirkung.</font>
        """
        pass

    def __init__(A, v):
        """ Constructor
        ----de----
            Konstruktor
        """
        Vector.__init__(A, [Vector(v[i]) for i in range(len(v))])
        A._makeRational()

    def _makeRational(A):
        for i in A.rowRange():
            for k in A.colRange():
                if isInteger(A[i,k]):
                    A[i,k] = Rational(A[i,k])

    def float(A):
        """ <b>A.float()</b><br/>
            <i>returns the Matrix A with all elements converted to <code>float</code></i>
        ----de----
            <b>A.float()</b><br/>
            <i>gibt die Matrix A zurück, in der alle Elemente nach <code>float</code> konvertiert sind</i>
        """
        return Matrix([ [float(A[i,k]) for k in A.colRange()]
                        for i in A.rowRange()])

    def copy(A):
        """ <b>A.copy()</b><br/>
            <i>independent copy of the matrix ("clone") A</i>
        ----de----
            <b>A.copy()</b><br/>
            <i>unabhängige Kopie der Matrix A</i>
        """
        return Matrix([ [copy(A[i,k]) for k in A.colRange()]
                        for i in A.rowRange()])

    def isSquare(A):
        """ <b>A.isSquare()</b><br/>
            <i>Truth value (A is square matrix)</i>
        ----de----
            <b>A.isSquare()</b><br/>
            <i>Wahrheitswert (A quadratisch)</i>
        """
        return A.height() == A.width()

    def str(A, i,k):
        """ <b>A.str(i,k)</b><br/>
            <i>string representation of the element [i,k]</i>
        ----de----
            <b>A.str(i,k)</b><br/>
            <i>String-Darstellung des Elements [i,k]</i>
        """
        if type(A[i,k]) == float:
            return "%g" % A[i,k]
        else:
            return str(A[i,k])

    def _pp(A, name=""):
        n = A.height()
        s = """{{html}}<table><tr valign="middle">
                 <td width="30"><font color="#3060C0">{0}</font></td>
                 <td><table cellspacing="0" cellpadding="-1">""".format(name)
        for i in range(n):
            left, right = "21", "22"
            if n > 1:
                if   i == 0:   left, right = "11", "12"
                elif i == n-1: left, right = "31", "32"
            else:
                left, right = "01", "02"
            s += """<tr><td align="center" valign="middle"><img src=":/img/matrix/{0}.png"/></td>""".format(left)
            spc = {False: "&nbsp;", True: ""}
            for k in range(A.width()):
                s += """<td align="center" valign="middle"><font color="#3060C0">{1}{0}{2}</font></td>""".format(A[i,k], spc[k==0], spc[k==A.width()-1])
            s += """<td align="center" valign="middle"><img src=":/img/matrix/{0}.png"/></td></tr>""".format(right)
        s += """</table></td></table>{/html}"""
        return s

    def __repr__(A):
        """ Object representation as string
        """
        return A._pp()

    def __getitem__(A, i):
        """ Definition of the Index operator []
            Counts from 0
            A[i]   --> i-th row vector
            A[i,k] --> element in i-th row, k-th column
        """
        if isinstance(i, tuple):
            return A[i[0]][i[1]]
        else:
            return Vector.__getitem__(A, i)

    def __setitem__(A, i, x):
        """ Definition of the Index operator [] for assignments
            Counts from 0
            A[i]   --> i-th row vector
            A[i,k] --> element in i-th row, k-th column
        """
        if isinstance(i, tuple):
            A[i[0]][i[1]] = x
        else:
            Vector.__setitem__(A, i, x)

    def height(A):
        """ <b>A.height()</b><br/>
            <i>Number of rows of the Matrix A</i>
        ----de----
            <b>A.height()</b><br/>
            <i>Anzahl der Zeilen der Matrix A</i>
        """
        return len(A)

    def width(A):
        """ <b>A.height()</b><br/>
            <i>Number of columns of the Matrix A</i>
        ----de----
            <b>A.width()</b><br/>
            <i>Anzahl der Spalten der Matrix A</i>
        """
        return len(A[0])

    def rowRange(A):
        """ <b>A.rowRange()</b><br/>
            <i>List of all row indices</i>
        ----de----
            <b>A.rowRange()</b><br/>
            <i>Liste aller Zeilenindizes</i>
        """
        return range(A.height())

    def colRange(A):
        """ <b>A.rowRange()</b><br/>
            <i>List of all column indices</i>
        ----de----
            <b>A.rowRange()</b><br/>
            <i>Liste aller Spaltenindizes</i>
        """
        return range(A.width())

    def __neg__(A):
        """ unary minus operator
        """
        return Matrix([ [-A[i,k] for k in A.colRange()]
                        for i in A.rowRange()])

    def joinRight(A, B):
        """ <b>A.joinRight(B)</b><br/>
            <i>Horizontal concatenation of A with B</i><br/>
            <b>Requires</b>: Both Matrices must have the same row count.
        ----de----
            <b>A.concat(B)</b><br/>
            <i>Horizontale Verkettung von A mit B</i><br/>
            <b>Voraussetzung</b>: Die beiden Matrizen müssen gleich viele Zeilen haben.
        """
        assert A.height() == B.height()
        return Matrix([ list(A[i])+list(B[i]) for i in A.rowRange()])

    def joinBottom(A, B):
        """ <b>A.joinBottom(B)</b><br/>
            <i>Vertical concatenation of A with B</i><br/>
            <b>Requires</b>: Both Matrices must have the same column count.
        ----de----
            <b>A.concat(B)</b><br/>
            <i>Vertikale Verkettung von A mit B</i><br/>
            <b>Voraussetzung</b>: Die beiden Matrizen müssen gleich viele Spalten haben.
        """
        assert A.width() == B.width()
        def el(i,k):
            if i < A.height(): return A[i,k]
            else:              return B[i-A.height(), k]
        return Matrix.fromFunction(A.height()+B.height(), A.width(), el)

    def __or__(A, B):
        """ <b>A.__or__(B)</b><br/>
            <i>Horizontal concatenation with B</i><br/>
            A | B is the same as A.joinRight(B)<br/>
            Instead of A = A | B you may write A |= B.
            <b>Requires</b>: Both matrices must have the same row count.
        """
        assert A.height() == B.height()
        return Matrix([ list(A[i])+list(B[i]) for i in A.rowRange()])

    def __add__(A, B):
        """ <b>A.__add__(B)</b><br/>
            <i>Matrix addition operator</i><br/>
            Instead of A = A + B you may write A += B.
        """
        #   a + b  --> a.__add__ (b), wenn a vom Typ Matrix ist
        #              b.__radd__(a), sonst
        assert A.height() == B.height() and A.width() == B.width()
        return Matrix([ [A[i,k]+B[i,k] for k in A.colRange()]
                        for i in A.rowRange()
                      ])

    def __sub__(A, B):
        """ <b>A.__sub__(B)</b><br/>
            <i>Matrix subtraction operator</i><br/>
            Instead of A = A - B you may write A -= B.
        """
        return A + (-B)

    def __mul__(A, B):
        """ <b>A.__sub__(B)</b><br/>
            <i>Scalar or matrix multiplication</i><br/>
            Instead of A = A * B you may write A *= B.
        """
        # If both operands are matrices, matrix multiplication is applied,
        # otherwise scalar multiplikation
        if isinstance(B, Matrix): # matrix multiplication
            assert A.width() == B.height()
            return Matrix(
                [ [ sum([A[i,j] * B[j,k] for j in A.colRange()])
                    for k in B.colRange()]
                  for i in A.rowRange()])
        else: # scalar multiplikation
            return Matrix([ [B*A[i,k] for k in A.colRange()]
                            for i in A.rowRange()])

    def __rmul__(A, B):
        assert not isinstance(B, Matrix)
        return A * B

    def transp(A):
        """ <b>A.transp()</b><br/>
            <i>returns the transposed Matrix</i><br/>
            short form: ~A
        ----de----
            <b>A.transp()</b><br/>
            <i>berechnet die Transponierte der Matrix A</i><br/>
            kürzer: ~A
        """
        return Matrix([ [A[i,k] for i in A.rowRange()]
                        for k in A.colRange()])

    def __invert__(A):
        """ Operator ~ (transposition)
        """
        return A.transp()

    def submatrix(A, i, k, m, n):
        """ <b>A.submatrix(i, k, m, n)</b><br/>
            <i>m*n Submatrix (rows  i..i+m-1, columns k..k+n-1)</i><br/>
        ----de----
            <b>A.submatrix(i, k, m, n)</b><br/>
            <i>m*n-Untermatrix (Zeilen i..i+m-1, Spalten k..k+n-1)</i><br/>
        """
        assert i+m-1 < A.height() and k+n-1 < A.width()
        return Matrix([ [A[i1,k1] for k1 in range(k,k+n)]
                        for i1 in range(i,i+m)])

    def complement(A, i, k):
        """ <b>A.complement(i, k)</b><br/>
            <i>Matrix without row i and column k</i><br/>
            (Algebraic complement)
        ----de----
            <b>A.complement(i, k)</b><br/>
            <i>Matrix ohne die Zeile i und die Spalte k</i><br/>
            (Algebraisches Komplement)
        """
        return Matrix([ [A[i1,k1] for k1 in A.colRange() if k1 != k]
                        for i1 in A.rowRange() if i1 != i])

    def minor(A, i, k):
        """ <b>A.minor(i, k)</b><br/>
            <i>Minor for row i, column k</i><br/>
            (Determinant of the algebraic complement)
        ----de----
            <b>A.minor(i, k)</b><br/>
            <i>Minor zur Zeile i und der Spalte k</i><br/>
            (Determinante des Algebraischen Komplements)
        """
        return A.complement(i,k).det()

    def cofactor(A, i, k):
        """ <b>A.cofactor(i, k)</b><br/>
            <i>Cofactor for row i, columns k</i><br/>
            (signed minor)
        ----de----
            <b>A.cofactor(i, k)</b><br/>
            <i>Kofaktor zur Zeile i und der Spalte k</i><br/>
            (Minor mit Vorzeichen)
        """
        return (-1)**(i+k) * A.minor(i,k)

    def adjoint(A):
        """ <b>A.adjoint()</b><br/>
            <i>Adjoint matrix</i><br/>
        ----de----
            <b>A.adjoint()</b><br/>
            <i>Adjungierte Matrix</i><br/>
        """
        assert A.isSquare()
        return Matrix([ [A.cofactor(k,i) for k in A.rowRange()]
                        for i in A.colRange()])
    def det(A):
        """ <b>A.det()</b><br/>
            <i>Determinant of the matrix A</i><br/>
        ----de----
            <b>A.det()</b><br/>
            <i>Determinante der Matrix A</i><br/>
        """
        if not A.isSquare():
            raise _MathGUIdeError("A is not a square matrix")
        n = A.height()
        if n == 1:
            return A[0,0]
        else:
            return sum([A[0,k] * A.cofactor(0,k)
                        for k in A.colRange()])

    def _gaussElim0(A, jordan=True):
        """ unsichere Vorstufe zu gaussElim
        """
        m, n = A.height(), A.width()
        k = 0
        for i0 in A.rowRange():
            k = i0
            # oberste Zeile für führende Eins passend durchmultiplizieren
            A[i0] *= 1/A[i0,k]

            # Passende Vielfache zu anderen Zeilen addieren, so dass
            # unterhalb der führenden Eins Nullen entstehen
            for i in range(i0+1, m):
                A[i] -= A[i0] * A[i,k]
            if jordan:
                # Gauß-Jordan: dto. auch oberhalb der führenden Eins
                for i in range(i0):
                    A[i] -= A[i0] * A[i,k]

    def gaussElim(self, jordan=True):
        """ <b>A.gaussElim(jordan=True)</b><br/>
            <i>Row echelon form of the matrix A</i><br/>
            If <var>jordan</var> is <code>True</code>, the reduced row echelon form
            is returned (Gaussian elimination), otherwise the normal row echelon form.
        ----de----
            <b>A.gaussElim(jordan=True)</b><br/>
            <i>Zeilenstufenform der Matrix A.</i><br/>
            Wenn <var>jordan</var> <code>True</code> ist, wird die reduzierte Zeilenstufenform
            (Gauß-Jordan-Elimination) zurückgegeben, sonst die einfache Zeilenstufenform (Gauß-Elimination).
        """
        A = self.copy()
        m, n = A.height(), A.width()
        k = -1
        for i0 in A.rowRange():
            # Bestimme die am weitesten links stehende Spalte k, die
            # (ab Zeile i0) von Null verschiedene Elemente enthält:
            k += 1
            aMax, iMax = 0, i0
            while aMax == 0 and k < n:
                for i in range(i0,m):
                    if abs(A[i,k]) > aMax:
                        aMax, iMax = abs(A[i,k]), i
                if aMax == 0:
                    k += 1
            if k < n:
                # Oberste Zeile mit der vertauschen, die das (dem Betrag nach)
                # größte Element in Spalte i0 enthält
                A[iMax], A[i0] = A[i0], A[iMax]

                # oberste Zeile für führende Eins passend durchmultiplizieren
                A[i0] *= 1/A[i0,k]

                # Passende Vielfache zu anderen Zeilen addieren, so dass
                # unterhalb der führenden Eins Nullen entstehen
                for i in range(i0+1, m):
                    A[i] -= A[i0] * A[i,k]
                if jordan:
                    # Gauß-Jordan: dto. auch oberhalb der führenden Eins
                    for i in range(i0):
                        A[i] -= A[i0] * A[i,k]
        return A

    def rank(A):
        """ <b>A.rank()</b><br/>
            <i>Rank of the matrix A</i><br/>
        ----de----
            <b>A.rank()</b><br/>
            <i>Rang der Matrix A</i><br/>
        """
        B = A.gaussElim()
        r = 0
        while r < B.height() and B[r].norm() != 0:
            r += 1
        return r

    def inverse(A):
        """ <b>A.inverse()</b><br/>
            <i>Inverse og the matrix A</i><br/>
        ----de----
            <b>A.inverse()</b><br/>
            <i>Inverse Matrix A</i><br/>
        """
        if not A.isSquare():
            raise _MathGUIdeError("A is not a square matrix")
        n = A.height()
        I = Matrix.identity(n)
        B = A.joinRight(I).gaussElim()
        if B.submatrix(0,0,n,n) != I:
            raise _MathGUIdeError("Matrix is not invertible")
        return B.submatrix(0,n,n,n)

    def gramSchmidt(self, normalize=False):
        """ <b>A.gramSchmidt(normalize=False)</b><br/>
            <i>Orthogonalizing using the Gram–Schmidt process</i><br/>
            <code>A</code> must be the matrix of the (linearly independent) vectors
            to be orthogonalized (given as rows).<br/>
            If <var>normalize</var> is True, the vectors are normalized.<br/>
            return value: Matrix consisting of the orthogonalen vectors.<br/>
            <b>Example</b>:<br/><pre>
     A = Matrix([[1,1],[2,0]])
     A.gramSchmidt()</pre>
            returns the matrix<pre>
     / 1   1 \
     |       |
     \ 1  -1 /</pre>
        ----de----
            <b>A.gramSchmidt(normalize=False)</b><br/>
            <i>Gram-Schmidt'sches Orthogonalisierungsverfahren.</i><br/>
            <code>A</code> muss Matrix aus den (linear unabhängigen)
            zu orthogonalisierenden Vektoren (als Zeilen) sein.<br/>
            Wenn <var>normalize</var> True ist, werden die Vektoren normalisiert.<br/>
            Rückgabewert: Matrix mit den orthogonalen Vektoren.<br/>
            <b>Beispiel</b><pre>
     A = Matrix([[1,1],[2,0]])
     A.gramSchmidt()</pre>
            liefert die Matrix<pre>
     / 1   1 \
     |       |
     \ 1  -1 /</pre>
        """
        A = self.copy()
        n = A.height()
        for i in range(1,n):
            for k in range(i):
                A[i] -= A[k] * ((A[i]*A[k])/(A[k]*A[k]))
        if normalize:
            for v in A:
                v *= 1/v.norm()
        return A

    def _luRecursive(A):
        """ Vorstufe: funktioniert nicht für jede reguläre Matrix!
        """
        assert A.isSquare()
        n = A.height()
        if n == 1: # Rekursionsabbruch: 1·1-Matrix
            return Matrix.identity(1), A.copy()

        #     /a11 :  w  \   / 1      :  0 \   /a11:     w       \
        #     |..........|   |.............|   |................ |
        # A = |    :     | = |  1     :    | * |   :      1      |
        #     | ~v :  A1 |   | --- ~v :  I |   | 0 : A1- --- ~v*w|
        #     \    :     /   \ a11    :    /   \   :     a11     /
        v = A.submatrix(1,0, n-1,1)
        w = A.submatrix(0,1, 1,n-1)
        A1 = A.submatrix(1,1, n-1,n-1)
        B = A1 - (1/A[0,0]) * v * w
        L1, U1 = B._luRecursive() # Rekursiver Aufruf für (n-1)·(n-1)-Matrix
        L_top = Matrix.identity(1).joinRight(Matrix.null(1,n-1))
        L_btm = ((1/A[0,0]) * v).joinRight(L1)
        L = L_top.joinBottom(L_btm)
        U_top = A.submatrix(0,0, 1,1).joinRight(w)
        U_btm = Matrix.null(n-1,1).joinRight(U1)
        U = U_top.joinBottom(U_btm)
        return L,U

    def _luIter(self):
        A = self.copy()
        assert A.isSquare()
        n = A.height()
        for k in range(n-1):
            assert A[k,k] != 0 # sonst scheitert der Algorithmus!
            for i in range(k+1,n):
                A[i,k] /= A[k,k]
                for j in range(k+1,n):
                    A[i,j] -= A[i,k] * A[k,j]
        # A in linke und rechte Dreiecksmatrix zerlegen
        L = Matrix.identity(n)
        U = Matrix.identity(n)
        for i in range(n):
            for k in range(n):
                if k < i:  L[i,k] = A[i,k]
                else:      U[i,k] = A[i,k]
        return L, U

    def lup(self):
        """ <b>A.lup()</b><br/>
            <i>LUP decomposition of the matrix</i><br/>
            Returns a triplet (L, U, P) with P*A = L*R, where:<br/>
            P is a Permutation matrix,<br/>
            L is a Lower triangular matrix where all diagonal elements are one.<br/>
            R is an upper triangular matrix.<br/>
            <bRequires</b>: Matrix ist invertible.
        ----de----
        <b>.lup()</b><br/>
            <i>LRP-Zerlegung (LUP decomposition) der Matrix</i><br/>
            Rückgabewert: Tripel (L, U, P) mit P*A = L*R, wobei:<br/>
            P Permutationsmatrix,<br/>
            L linke (lower) Dreiecksmatrix mit Einsen auf Diagonale,<br/>
            U rechte (upper) Dreiecksmatrix ist.<br/>
            <b>Voraussetzung</b>: Matrix ist invertierbar.
        """
        A = self.copy()
        assert A.isSquare()
        n = A.height()
        P = Matrix.identity(n)
        for k in range(n-1):
            # Suche das dem Betrag nach größte Element
            # der k-ten Spalte ab der k-ten Zeile und seinen Zeilenindex
            absMax, kMax = max([(abs(A[i,k]), -i) for i in range(k,n)])
            kMax = -kMax # -i im Paar --> bei mehreren gleichen erstes
            assert absMax > 0

            # Vertausche die Zeile des größten Elements
            # mit der k-ten Zeile (Permutation und Matrix)
            P[k], P[kMax] = P[kMax], P[k]
            A[k], A[kMax] = A[kMax], A[k]

            for i in range(k+1,n):
                A[i,k] /= A[k,k]
                for j in range(k+1,n):
                    A[i,j] -= A[i,k] * A[k,j]

        # A in linke und rechte Dreiecksmatrix zerlegen
        L = Matrix.identity(n)
        U = Matrix.identity(n)
        for i in range(n):
            for k in range(n):
                if k < i:  L[i,k] = A[i,k]
                else:      U[i,k] = A[i,k]
        return L, U, P

    def solve(A, b):
        """ <b>A.solve(b)</b><br/>
            <i>Solution of the linear equation system Ax = b</i><br/>
            b may be Vektor or column Matrix.
        ----de----
            <b>A.solve(b)</b><br/>
            <i>Lösung x des linearen Gleichungssystems Ax = b</i><br/>
            b kann Vektor oder Spaltenmatrix sein.
        """
        if not isinstance(b, Matrix):
            assert isinstance(b, Vector)
            b = ~b
        return A.inverse() * b

    def leastSquares(A, b):
        """ <b>A.leastSquares(b)</b><br/>
            <i>Solution of the normal system</i><br/>
            Returns: <code>(~A*A).solve(~A*b)</code>
        ----de----
            <b>A.leastSquares(b)</b><br/>
            <i>Lösung der Normalgleichung</i><br/>
            Rückgabewert: <code>(~A*A).solve(~A*b)</code>
        """
        return (~A*A).solve(~A*b)


def fit(data, values):
    """ <b>fit(data, values)</b><br/>
        <i>Interpolation using the Gaussian method of least squares</i><br/>
        <code>fit(data, values)</code>
        calculates (using the method of least squares)
        a linear combination of the functions mapping x
        to the terms in <code>values</code>.<br/>
        (corresponds to the Mathematica function <code>Fit</code>).<br/>
        <b>Example</b>:<pre>
 fit([(-1,2), (1,1), (2,1), (3,0), (5,3)],
     ["1", "x", "x**2"])</pre>
        calculates <code>[c1, c2, c3]</code> such that the function<br/>
        <code>f(x) = c1*1  + c2*x + c3 * x**2</code><br/>
        approximates the conditions f(-1)=2, f(1)=1, f(2)=1, f(3)=0, f(5)=3 .<br/>
        Result: "(6/5 * 1) + (-53/70 * x) + (3/14 * x**2)"
        ----de----
        <b>fit(data, values)</b><br/>
        <i>Interpolation mit der Gaußschen Methode der kleinsten Quadrate</i><br/>
        <code>fit(data, values)</code>
        berechnet nach der Methode der kleinsten Quadrate
        eine Linearkombination der Funktionen, die x auf die
        Terme in <code>values</code> abbilden.<br/>
        (entspricht der Mathematica-Funktion <code>Fit</code>).<br/>
        <b>Beispiel</b>:<pre>
 fit([(-1,2), (1,1), (2,1), (3,0), (5,3)],
     ["1", "x", "x**2"])</pre>
        berechnet <code>[c1, c2, c3]</code> so dass die Funktion<br/>
        <code>f(x) = c1*1  + c2*x + c3 * x**2</code><br/>
        die Bedingungen f(-1)=2, f(1)=1, f(2)=1, f(3)=0, f(5)=3 approximiert.<br/>
        Ergebnis: "(6/5 * 1) + (-53/70 * x) + (3/14 * x**2)"
    """
                              # from the example:
    m = len(data)             # m = 5
    n = len(values)           # n = 3
    functions = [eval("lambda x:{}".format(val)) for val in values]
    A = Matrix.fromFunction(m,n, lambda i,k: functions[k](data[i][0]))
    b = ~Vector.fromFunction(m, lambda i: data[i][1])
                              #         / 1  -1   1 \         /  2  \
                              #        |  1   1   1  |        |  1  |
                              # A =    |  1   2   4  |    b = |  1  |
                              #        |  1   3   9  |        |  0  |
                              #         \ 1   5  25 /         \  3  /
    y = A.leastSquares(b)     # Solution of the normal system (column matrix):
    s = ""                    # y = [[6/5], [-53/70], [3/14]]
    for i in y.rowRange():    # Create linear combination:
        s += "({0} * {1})".format(str(y[i,0]), values[i])
        if i < y.height()-1:
            s += " + "
    return s                  # "(6/5 * 1) + (-53/70 * x) + (3/14 * x**2)"

#---------------------------------------------------------------
#  RSA-Chiffrierung

class Rsa: # RSA Encryption # RSA-Verschlüsselung
    """ Class <code>Rsa</code>: RSA Encryption<br/>
        This is a static class (only static methods available).
    ----de----
        Klasse <code>Rsa</code>: RSA-Verschlüsselung<br/>
        Statische Klasse: nur Klassenmethoden.
    """

    _staticMethods = ("createKeys", "encrypt")

    @staticmethod
    def createKeys(bits=768):
        """ <b>createKeys(bits=768)</b><br/>
            <i>returns an RSA key pair</i><br/>
            The pair is returned as tuple <code>((e,n),(d,n))</code>.<br/>
            <b>Example</b>:<br/>
            <code>public, private = Rsa.createKeys(24)<br/>
                  m = 12345 # message<br/>
                  c = Rsa.encrypt(m, public) # encrypted message<br/>
                  assert Rsa.encrypt(c, private) == m</code><br/>
            <b>See also</b> <a href="#Rsa.encrypt"><code>Rsa.encrypt</code></a>.
        ----de----
            <b>createKeys(bits=768)</b><br/>
            <i>liefert ein RSA-Schlüsselpaar</i><br/>
            Das Paar wird als Tupel <code>((e,n),(d,n))</code> zurückgegeben.<br/>
            <b>Beispiel</b>:<br/>
            <code>public, private = Rsa.createKeys(24)<br/>
                  m = 12345 # Nachricht<br/>
                  c = Rsa.encrypt(m, public) # verschlüsselte Nachricht<br/>
                  assert Rsa.encrypt(c, private) == m</code><br/>
            <b>Siehe auch</b> <a href="#Rsa.encrypt"><code>Rsa.encrypt</code></a>.
        """
        m = 2 ** (bits//2 - 1)
        # wähle zwei Primzahlen p, q
        p = nextPrime(m + rand(m))
        q = nextPrime(m + rand(m))
        n = p * q
        phi = (p-1)*(q-1)       # phi(n) (Eulersche Phi-Funktion)
        d = phi                 # wähle d aus 1..phi(n)
        while gcd(d,phi) != 1:  # mit d, phi(n) teilerfremd
            d = rand(phi)
        t,x,y = gcdExt(d,phi)
        e = x % phi
        publicKey = (e,n)
        privateKey = (d,n)
        return (publicKey, privateKey)

    @staticmethod
    def encrypt(m, key):
        """ <b>encrypt(m, key)</b><br/>
            <i>RSA encryption</i><br/>
            encrypts m using the mit dem key <code>key = (e,n)</code><br/>
            <b>See also</b> <a href="#Rsa.createKeys"><code>Rsa.createKeys</code></a>.
        ----de----
            <b>encrypt(m, key)</b><br/>
            <i>RSA-Verschlüsselung</i><br/>
            Verschlüsselt m mit dem Schlüssel key = (e,n)<br/>
            <b>Siehe auch</b> <a href="#Rsa.createKeys"><code>Rsa.createKeys</code></a>.
        """
        return pow(m, key[0], key[1])


#===============================================================
#  Permutationen, Permutationsgruppen
#---------------------------------------------------------------

def permute(v, fn, prefix=[]):
    """ <b>permute(v, fn)</b><br/>
        <i>applies procedure <code>fn</code> to all permutations of liste v.</i><br/>
        If v is sorted, the permutations are also generated in sorted order.
        ----de----
        <b>permute(v, fn)</b><br/>
        <i>wendet Prozedur <code>fn</code> auf alle Permutationen von Liste v an.</i><br/>
        Wenn v sortiert ist, werden auch die Permutationen in
        sortierter Reihenfolge erzeugt.
    """
    if len(v) == 0: # nichts zu permutieren!
        fn(prefix)
    else:
        for i in v:
            p = prefix + [i]  # i an Praefix anhängen
            w = list(v)       # Duplikat von w, ...
            w.remove(i)       # ... ohne i
            permute(w, fn, p)

def permutations(v, prefix=[]):
    """ <b>permutations(v)</b><br/>
        <i>List of all permutations of v</i><br/>
        If v is sorted, the permutations are also generated in sorted order.
        ----de----
        <b>permutations(v)</b><br/>
        <i>Liste aller Permutationen von v</i><br/>
        Wenn v sortiert ist, werden auch die Permutationen in
        sortierter Reihenfolge erzeugt.
    """
    if len(v) == 0: # nichts zu permutieren!
        yield prefix
    else:
        for i in v:
            p = prefix + [i]  # i an Praefix anhängen
            w = list(v)       # Duplikat von w, ...
            w.remove(i)       # ... ohne i
            for i in permutations(w, p):
                yield i


class Perm: # Permutations (from dictionary or cycle representation) # Permutationen (aus Dictionary oder Zyklendarstellung)
    """ Class <code>Perm</code><br/>
        <i>Permutations (from dictionary or cycle representation)</i>
        <b>Examples</b> (three times the same permutation:<br/>
        <code>Perm({1:4, 2:3, 3:2, 4:1})</code><br/>
        <code>Perm("(1,4)(2,3)")</code><br/>
        <code>Perm([[1,4],[2,3]])</code>
        ----de----
        Klasse <code>Perm</code><br/>
        <i>Permutationen (aus Dictionary oder Zyklendarstellung)</i>
        <b>Beispiele</b> (dreimal die gleiche Permutation:<br/>
        <code>Perm({1:4, 2:3, 3:2, 4:1})</code><br/>
        <code>Perm("(1,4)(2,3)")</code><br/>
        <code>Perm([[1,4],[2,3]])</code>
    """
    _staticMethods = []

    def __init__(self, p):
        """ Constructor
        ----de----
            Konstruktor
        """
        if type(p) == dict:
            # Zuordnungstafel
            # Beispiel: Perm({1:4, 2:3, 3:2, 4:1})
            self._d = p
        elif type(p) == str:
            # Zyklendarstellung als String
            # Der String darf nur Zahlen enthalten
            # Beispiel: Perm("(1,4)(2,3)")
            v = []
            p = p.replace(" ", "") # Leerzeichen entfernen
            assert p[0] == "(" and p[-1] == ")"
            p = p[1:-1] # Klammern vorn und hinten entfernen
            for cycle in p.split(")("):
                v.append([eval(x) for x in cycle.split(",")])
            self.fromVector(v)
        else:
            # Zyklendarstellung
            # Beispiel: Perm([[1,4],[2,3]])
            self.fromVector(p)
        self.reduce()

    def fromVector(self, v):
        """ <b>p.fromVector(v)</b><br/>
            <i>Generation from vector v with argument/value pairs.</i><br/>
            Example: <code>p.fromVector([[1,4],[2,3]])</code>
        ----de----
            <b>p.fromVector(v)</b><br/>
            <i>Erzeugung aus Vektor v mit Argument-Wert-Paaren.</i><br/>
            Beispiel: <code>p.fromVector([[1,4],[2,3]])</code>
        """
        assert type(v) == list
        self._d = {}
        for cycle in v:
            assert type(cycle) == list
            for i in range(len(cycle)):
                if i < len(cycle) - 1:
                    self._d[cycle[i]] = cycle[i+1]
                else:
                    self._d[cycle[i]] = cycle[0]

    def __repr__(self):
        """ Object representation as string (as cycles)
        """
        if len(self._d) == 0:
            return "identity"
        else:
            return self.cycles()

    def __getitem__(self, i):
        return self._d.get(i, i)

    def __mul__(p1, p2):
        """ product of two Permutations (left first, then right!)
        """
        p = {}
        m = p1.members()
        for i in m:
            p[i] = p2[p1[i]]
        for i in p2.members():
            if i not in m:
                p[i] = p2[i]
        return Perm(p)

    def __eq__(p1,p2): return p1.cycles() == p2.cycles()
    def __ne__(p1,p2): return p1.cycles() != p2.cycles()

    def members(self):
        """ <b>p.members()</b><br/>
            <i>List of the permuted elements</i>
        ----de----
            <b>p.members()</b><br/>
            <i>Liste der permutierten Elemente</i>
        """
        m = list(self._d.keys())
        m.sort()
        return m

    def inverse(self):
        """ <b>p.inverse()</b><br/>
            <i>inverse permutation</i>
        ----de----
            <b>p.inverse()</b><br/>
            <i>Umkehrpermutation</i>
        """
        d = {}
        for i in self._d:
            d[self._d[i]] = i
        return Perm(d)

    def cycles(self):
        """ <b>p.cycles()</b><br/>
            <i>Cycle representation</i>
        ----de----
            <b>p.cycles()</b><br/>
            <i>Zyklendarstellung</i>
        """
        s = ""
        m = self.members()
        while len(m) > 0:
            first = m[0]
            m.remove(first)
            i = first
            s += "({0}".format(str(first))
            while self._d[i] != first:
                i = self._d[i]
                m.remove(i)
                s += ",{0}".format(str(i))
            s += ")"
        return s

    def check(self):
        """ <b>p.check()</b><br/>
            <i>check for consistency</i>
        ----de----
            <b>p.check()</b><br/>
            <i>Konsistenzprüfung</i>
        """
        m = self.members()
        v = self._d.values()
        v.sort()
        assert(k == v)

    def reduce(self):
        """ <b>p.reduce()</b><br/>
            <i>removes fixed points</i>
        ----de----
            <b>p.reduce()</b><br/>
            <i>entfernt Fixpunkte</i>
        """
        id = []
        for i in self._d:
            if self._d[i] == i:
                id.append(i)
        for i in id:
            del self._d[i]

    def inversions(self):
        """ <b>p.inversions()</b><br/>
            <i>Inversion number (count of "swapped pairs")</i>
        ----de----
            <b>p.inversions()</b><br/>
            <i>Inversionszahl (Anzahl der "vertauschten Paare")</i>
        """
        m = self.members()
        s = 0
        n = len(m)
        for i in range(n):
            for j in range(i+1, n):
                if self[m[i]] > self[m[j]]:
                    s += 1
        return s

    def sign(self):
        """ <b>p.sign()</b><br/>
            <i>signum</i><br/>
            1 for even, -1 for odd permutations
        ----de----
            <b>p.sign()</b><br/>
            <i>Signum</i><br/>
            1 bzw. -1 für gerade bzw. ungerade Permutation
        """
        return 1 - 2*(self.inversions() % 2)

    def odd(self):
        """ <b>p.odd()</b><br/>
            <i>odd permutation?</i> (bool)
        ----de----
            <b>p.odd()</b><br/>
            <i>ungerade Permutation?</i> (bool)
        """
        return self.inversions() % 2 == 1


#===============================================================
# Modulo-Arithmetik und Rechnen in Polynomringen
#---------------------------------------------------------------

class Mod: # Elements of the residue class rings Z[m] # Elemente der Restklassenringe Z[m]
    """ Class <code>Mod</code><br/>
        <i>Elements of the residue class rings Z[m]</i><br/>
        <code>Mod(n, m)</code> returns element n des of the residue class ring Z[m]<br/>
        Arithmetic operators may be applied to the objects.<br/>
        Some operations are defined only,
        if Z[m] is a field (&lt;==> m is prime number)<br/>
        <b>See also</b> <a href="mathguide.html#mod"><code>Mod</code></a>
        ----de----
        Klasse <code>Mod</code><br/>
        <i>Elemente der Restklassenringe Z[m]</i><br/>
        <code>Mod(n, m)</code> liefert Element n des Restklassenrings Z[m]<br/>
        Die Objekte können mit arithmetischen Operatoren verknüpft werden.<br/>
        Einige Operationen sind nur definiert,
        wenn Z[m] ein Körper ist (&lt;==> m ist Primzahl)<br/>
        <b>Siehe auch</b> <a href="mathguide.html#mod"><code>Mod</code></a>
    """
    _staticMethods = []

    def operators(self):
        """ <b>Mod.operators()</b><br/>
            <i>For documentation only</i><br/>
            The following operators are defined in the class <b>Mod</b>:<br/>
            <table>
             <tr><th>Op.</th><th>Function</th><th>Examples</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraction</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Multiplication</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>/</th><td>Division</td><td><code>a / b; a /= b</code></td></tr>
             <tr><th>%</th><td>Modulo (in fields only , i.e. modulus is prime)</td><td><code>a &amp; b; a &amp;= b</code></td></tr>
             <tr><th>-</th><td>Unary minus</td><td><code>-a</code></td></tr>
            </table>
            <font color="#000080" size="-2">This method is for documentation only. It has no effect.</font>
        ----de----
            <b>Mod.operators()</b><br/>
            <i>Nur zur Dokumentation</i><br/>
            Die Klasse <b>Mod</b> erlaubt folgende Operatoren:<br/>
            <table>
             <tr><th>Op.</th><th>Funktion</th><th>Beispiele</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraktion</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Multiplikation</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>/</th><td>Division (nur in Körpern, d.h. Primzahlmodul)</td><td><code>a / b; a /= b</code></td></tr>
             <tr><th>%</th><td>Modulo (nur in Körpern, d.h. Primzahlmodul)</td><td><code>a &amp; b; a &amp;= b</code></td></tr>
             <tr><th>-</th><td>unäres Minus</td><td>-a</td></tr>
            </table>
            <font color="#000080" size="-2">Diese Methode dient nur zur Dokumentation. Sie hat keine Wirkung.</font>
        """
        pass

    def __init__(self, n, m):
        """ Constructor
        ----de----
            Konstruktor
        """
        #assert isInteger(n) and isNatural(m)
        self.n = n % m
        self.m = m

    def copy(self):
        """ <b>A.copy()</b><br/>
            <i>independent copy ("clone") of A</i>
        ----de----
            <b>A.copy()</b><br/>
            <i>unabhängige Kopie von A</i>
        """
        return Mod(self.n, self.m)

    def __int__(self):
        return self.n

    def __repr__(self):
        """ Object representation as string
        """
        return "Mod({0}, {1})".format(str(self.n), str(self.m))

    def __cmp__(A, B):
        if not isinstance(B, Mod):
            B = Mod(B, A.m)
        return _cmp(A.n, B.n)

    def inverse(self):
        """ <b>p.inverse()</b><br/>
            <i>Multiplicative inverse</i><br/>
            defined in fields only. Therefore modulus must be prime number!
        ----de----
            <b>p.inverse()</b><br/>
            <i>Multiplikatives Inverses</i><br/>
            Nur in Körpern definiert. Deshalb muss der Modul Primzahl sein!
        """
        def gcdExt(a,b):
            if b == 0:
                return a, 1, 0
            else:
                q, r = divmod(a, b)
                d, z, x = gcdExt(b, r)
                return d, x, z - x * q

        d, inv, y = gcdExt(self.n, self.m)
        assert d == 1
        # inv * n + y * m == 1 ==> (inv * n) % m == 1
        assert (inv * self.n) % self.m == 1
        return Mod(inv, self.m)

    def __eq__(a,b):
        """ Test for equality (operator ==)
            with other Mod object or 0
        """
        if type(b) == int and b == 0:
            return a.n == 0
        return (a.n == b.n and a.m  == b.m)

    def __abs__(self):
        """ Absolute value
        """
        return self

    def __ne__(a,b):
        """ Test for unequality (Operator !=)
            with other Mod object or 0
        """
        return not a.__eq__(b)

    def __neg__(self):
        """ Unary Operator -
            -a --> a.__neg__()
        """
        return Mod((self.m - self.n) % self.m, self.m)

    def __add__(a, b):
        """ <b>a.__add__(b)</b><br/>
            <i>Addition</i><br/>
            Also operator notation: <b>a + b</b><br/>
            Instead of <code>a = a + b</code> you can write <code>a += b</code>.
        """
        #   a + b  --> a.__add__(b), wenn a vom Typ Mod ist
        #              b.__radd__(a), sonst
        if isinstance(b, Mod):
            d = gcd(a.m, b.m)
            return Mod(a.n + b.n, d)
        else:
            return Mod(a.n + b, a.m)

    def __radd__(b, a):
        """ Addition, see __add__ """
        return b.__add__(a)

    def __sub__(a, b):
        """ Subtraction:
            a - b  --> a.__sub__(b), if a is of type Mod
                       b.__rsub__(a), otherwise
        """
        if isinstance(b, Mod):
            d = gcd(a.m, b.m)
            return Mod(a.n - b.n, d)
        else:
            return Mod(a.n - b, a.m)

    def __rsub__(b, a):
        """ Subtraction, see __sub__
        """
        return Mod(a - b.m, b.m)

    def __mul__(a, b):
        """ Multiplication:
            a * b  --> a.__mul__(b), if a is of type Mod
                       b.__rmul__(a), otherwise
        """
        if isinstance(b, Mod):
            d = gcd(a.m, b.m)
            return Mod(a.n * b.n, d)
        else:
            return Mod(a.n * b, a.m)

    def __rmul__(b, a):
        """ Multiplikation, see __mul__
        """
        return b.__mul__(a)

    def __truediv__(a, b):
        """ exact division:
            a / b  --> a.__div__(b), if a is of type Mod
                       b.__rdiv__(a), otherwise
        """
        if not isinstance(b, Mod):
            b = Mod(b, a.m)
        assert a.m == b.m
        return a * b.inverse()

    def __rtruediv__(a, b):
        """ exact division, see __div__
        """
        assert not isinstance(b, Mod)
        b = Mod(b, a.m)
        return b.__div__(a)

    def __div__(a, b): return a.__truediv__(b)
    def __rdiv__(a, b): return a.__rtruediv__(b)

#===============================================================
# Poly
#---------------------------------------------------------------

class Poly: # Polynomials over arbitrary rings # Polynome über beliebigen Ringen
    """ Class <code>Poly</code><br/>
        <i>Polynomials</i><br/>
        <code>Poly(list)</code> returns a polynomial with the coefficients from <code>list</code> (ascending).<br/>
        The coefficients must comply with the ring axioms,<br/>
        for division and modulo operator (//, %, divmod) also the field axioms,<br/>
        and implement the equality with 0.<br/>
        Short form for polynomial with Mod objektes:<br/>
        <b>Example</b><br/>
        <code>Poly([Mod(1,2),Mod(0,2),Mod(1,2)])</code><br/>
        Short form:<br/>
        <code>Poly([1,0,1,1],2)</code>
        ----de----
        Klasse <code>Poly</code><br/>
        <i>Polynome</i><br/>
        <code>Poly(list)</code> liefert Polynom mit den Koeffizienten aus <code>list</code> (aufsteigend).<br/>
        Die Koeffizienten müssen die Ringaxiome erfüllen,<br/>
        für Division und Modulo-Operator (//, %, divmod) auch Körperaxiome,<br/>
        und die Abfrage auf Gleichheit mit 0 implementieren.<br/>
        Kurzform für Polynom mit Mod-Objekten:<br/>
        <b>Beispiel</b><br/>
        <code>Poly([Mod(1,2),Mod(0,2),Mod(1,2)])</code><br/>
        Dafür auch Kurzschreibweise:<br/>
        <code>Poly([1,0,1,1],2)</code>
    """
    #............................................................
    _staticMethods = ("gcd", "gcdExt", "irreducibles")

    @staticmethod
    def gcd(a,b):
        """ <b>gcd(a,b)</b><br/>
            <i>greatest common divisor of a and b</i><br/>
            Calculated using the Euclidean algorithm
        ----de----
            <b>gcd(a,b)</b><br/>
            <i>Größter gemeinsamer Teiler von a und b</i><br/>
            Berechnung mit dem Euklidischen Algorithmus
        """
        p = a.copy()
        q = b.copy()
        while not q.isZero():
            p, q = q, p % q
        return p.normalized()

    @staticmethod
    def gcdExt(a,b):
        """ <b>gcdExt(a,b)</b><br/>
            <i>Extended Euclidean algorithm.</i><br/>
            Returns (d, x, y) with d == gcd(a,b) and x*a + y*b == d
        ----de----
            <b>gcdExt(a,b)</b><br/>
            <i>Erweiterter Euklidischer Algorithmus.</i><br/>
            Gibt (d, x, y) zurück mit d == gcd(a,b) und  x*a + y*b == d
        """
        assert type(a[0]) == Mod
        m = a[0].m
        if b.isZero():
            return a.copy(), Poly([1],m), Poly([0],m)
        else:
            q, r = divmod(a, b)
            d, z, x = Poly.gcdExt(b, r)
            y = z - x * q
            return d, x, y

    @staticmethod
    def irreducibles(maxDegree, mod=2):
        """ <b>Poly.irreducibles(maxDegree, mod=2)</b><br/>
            <i>Irreducible polynomials</i><br/>
            Returns: List of irreducible polynomials
            over Z[mod] up to maxDegree
        ----de----
            <b>Poly.irreducibles(maxDegree, mod=2)</b><br/>
            <i>Irreduzible Polynome</i><br/>
            Rückgabewert: Liste der irreduziblen Polynome
            über Z[mod] bis zum Grad maxDegree
        """
        primeList = [Poly([0,1],mod)]
        for i in fromTo(mod+1, mod**(maxDegree+1)-1):
            f = Poly(NumeralSystem.toBase(i, mod, True), mod)
            divisible = False
            for p in primeList:
                if f % p == 0:
                    divisible = True
                    break
            if not divisible:
                primeList.append(f)
        return primeList
    #............................................................

    def operators(self):
        """ <b>Poly.operators()</b><br/>
            <i>For documentation only</i><br/>
            The following operators are defined in the class <b>Poly</b>:<br/>
            <table>
             <tr><th>Op.</th><th>Function</th><th>Examples</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraction</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Multiplication</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>/</th><td>Polynomial division (with remainder)</td><td><code>a / b; a /= b</code></td></tr>
             <tr><th>divmod</th><td>Division and remainder simultaneously</td><td><code>d,m = divmod(a,b)</code></td></tr>
             <tr><th>-</th><td>Unary minus</td><td><code>-a</code></td></tr>
            </table>
            <font color="#000080" size="-2">This method is for documentation only. It has no effect.</font>
        ----de----
            <b>Poly.operators()</b><br/>
            <i>Nur zur Dokumentation</i><br/>
            Die Klasse <b>Poly</b> erlaubt folgende Operatoren:<br/>
            <table>
             <tr><th>Op.</th><th>Funktion</th><th>Beispiele</th></tr>
             <tr><th>+</th><td>Addition</td><td><code>a + b; a += b</code></td></tr>
             <tr><th>-</th><td>Subtraktion</td><td><code>a - b; a -= b</code></td></tr>
             <tr><th>*</th><td>Multiplikation</td><td><code>a * b; a *= b</code></td></tr>
             <tr><th>//</th><td>Polynomdivision (mit Rest)</td><td><code>a // b; a //= b</code></td></tr>
             <tr><th>divmod</th><td>Division und Modulo simultan</td><td><code>d,m = divmod(a,b)</code></td></tr>
             <tr><th>-</th><td>unäres Minus</td><td>-a</td></tr>
            </table>
            <font color="#000080" size="-2">Diese Methode dient nur zur Dokumentation. Sie hat keine Wirkung.</font>
        """
        pass
    def __init__(self, coeff=[], mod=0):
        """ Constructor
        ----de----
            Konstruktor
        """
        if mod != 0:
            self.__p = [Mod(i,mod) for i in coeff]
            if len(coeff) == 0: # zero polynomial
                self.__p = [Mod(0, mod)]
        else:
            # make all coefficients rational numbers
            self.__p = [Rational(c) for c in coeff]
            if len(coeff) == 0: # zero polynomial
                self.__p = [Rational(0)]

        # drop leading zero coefficients
        n = len(self.__p)
        while n > 0 and self[n-1] == 0:
            n -= 1
        self.__p = self.__p[0:n]

    def copy(self):
        """ <b>p.copy()</b><br/>
            <i>independent copy ("clone") of p</i>
        ----de----
            <b>p.copy()</b><br/>
            <i>unabhängige Kopie von p</i>
        """
        c = Poly()
        c.__p = [copy(x) for x in self.__p]
        return c

    def isZero(self):
        return (len(self.__p) == 0
            or len(self.__p) == 1 and self.__p[0] == 0)

    def isOne(self):
        if len(self.__p) != 1:
            return False
        if type(self.__p[0]) == Mod:
            return self.__p[0].n == 1
        return self.__p[0] == 1

    def degree(self):
        """ <b>p.degree()</b><br/>
            <i>Grad des Polynoms</i>
        ----de----
            <b>p.degree()</b><br/>
            <i>degree of the polynomial</i>
        """
        if self.isZero():
            return -1
        else:
            return len(self.__p) - 1

    def value(self, x):
        """ <b>p.value(x)</b><br/>
            <i>Value of the polynomial for argument x</i>
        ----de----
            <b>p.value(x)</b><br/>
            <i>Wert des Polynoms für Argument x</i>
        """
        val = 0
        n = self.degree()
        for i in fromTo(0, self.degree()):
            val = (x * val) + self.__p[n-i]
        return val

    def __normalize(self):
        """ reduce to true degree
        """
        n = len(self.__p)
        while n > 0 and self[n-1] == 0:
            n -= 1
        self.__p = self.__p[0:n]

    def toDigits(f):
        """ <b>p.toDigits()</b><br/>
            <i>String of coefficients</i><br/>
            Short form, <b>nur für singled digit</b> Mod coefficients:<br/>
            Concatenation of the coefficient digits (without mod).<br/>
            Highest coeffizient left!<br/>
            <b>Example</b><br/>
            <code>p=Poly([1,0,1,1],2)</code><br/>
            <code>p.toDigits()</code><br/>
            Result: <code>"1101"</code>
        ----de----
            <b>p.toDigits()</b><br/>
            <i>String aus Koeffizienten</i><br/>
            Kurzform, <b>nur für einstellige</b> Mod-Koeffizienten:<br/>
            Verkettung der Koeffizienten-Ziffern (ohne mod-Angabe).<br/>
            Höchster Koeffizient links!<br/>
            <b>Beispiel</b><br/>
            <code>p=Poly([1,0,1,1],2)</code><br/>
            <code>p.toDigits()</code><br/>
            Ergebnis: <code>"1101"</code>
        """
        assert isinstance(f[0], Mod) and f[0].n < 10
        l = [str(x.n) for x in f.__p]
        l.reverse()
        return join(l)

    def __repr__(f):
        """ Object representation as string
        """
        # prüfen, ob mind. ein Koeffizien vom Typ Mod ist:
        m = 0
        for x in f.__p:
            if isinstance(x, Mod):
                m = x.m
                break
        if m > 0:
            # Voraussetzung: Mod-Objekte kompatibel!
            return "Poly({0}, {1})".format(str([int(x) for x in f.__p]), m)
        else:
            return "Poly([{0}])".format(join(f.__p, ", "))

    def expand(f, x="x"):
        """ <b>p.expand(x="x")</b><br/>
            <i>Polynomial written for the variable x</i><br/>
        ----de----
            <b>p.expand(x="x")</b><br/>
            <i>ausgeschriebenes Polynom für die Variable x</i><br/>
        """
        def pot(i):
            if i == 0:
                return 1
            elif i == 1:
                return x
            else:
                return "{0}^{1}".format(x, i)
        l = ["({0} * {1})".format(str(f[i]), pot(i)) for i in fromTo(0, f.degree())]
        return join(l, " + ")

    def pretty(f, width=0):
        """ <b>p.pretty(width=0)</b><br/>
            <i>Human readable written representation</i><br/>
        ----de----
            <b>p.pretty(width=0)</b><br/>
            <i>Lesbare Darstellung des Polynoms</i><br/>
        """
        if f.isZero():
            pp = "0"
        else:
            x = "x"
            def pot(i):
                if   i == 0:  return ""
                elif i == 1:  return x
                elif i == 2:  return "{0}²".format(x)
                elif i == 3:  return "{0}³".format(x)
                else:         return "{0}^{1}".format(x, i)
            def sign(i):
                if f[i] < 0: return "- "
                elif i > 0: return "+ "
                else: return ""
            pp = ""
            for i in fromTo(f.degree(), 0, -1):
                # sign
                sign = ""
                if type(f[i]) != Mod and f[i] < 0: sign = " - "
                elif i < f.degree(): sign = " + "

                # abs value
                a = str(abs(f[i]))
                if type(f[i]) == Mod:
                    a = f[i].n
                if i > 0 and type(f[i]) == Mod and f[i].m == 2:
                    a = ""

                if f[i] != 0 and pot != "":
                    pp += "{0}{1}{2}".format(sign, a, pot(i))
        if len(pp) < width:
            width -= len(pp)
            pp = "{0}{1}{2}".format((width//2)*" ", pp, (width-width//2)*" ")
        return pp

    def __getitem__(f, i):
        """ Definition of the index operator []
            f[i] returns the i-th coefficient of the polynomial.
        """
        if i <= f.degree():
            return f.__p[i]
        else:
            return 0 * f.__p[i]

    #--- unäre Operatoren: ---

    def __neg__(f):
        """ unärer Operator -
        """
        r = Poly()
        r.__p = [-copy(x) for x in f.coeff]
        return r

    #--- binäre Operatoren: ---

    def __eq__(f,g):
        """ Test for equality (operator ==)
            with other polynomial or ring element
        """
        if not isinstance(g, Poly):
            g = Poly([g])
        return f.__p == g.__p

    def __ne__(f,g):
        """ Test for unequality (operator !=)
            with other polynomial or ring element
        """
        return not f.__eq__(g)

    def __add__(f, g):
        """ <b>a.__add__(b)</b><br/>
            <i>Addition</i><br/>
            Also operator notation: <b>a + b</b><br/>
            Instead of <code>a = a + b</code> you can write <code>a += b</code>.
        """
        #   a + b  --> a.__add__ (b), wenn a vom Typ Poly ist
        #              b.__radd__(a), sonst
        if not isinstance(g, Poly):
            g = Poly([g])
        fDeg, gDeg = f.degree(), g.degree()
        minDeg = min(fDeg, gDeg)
        maxDeg = max(fDeg, gDeg)
        s = Poly()
        s.__p = (maxDeg + 1) * [0] # list with (maxDeg + 1) zeroes
        for i in fromTo(0, maxDeg):
            if i > fDeg:
                s.__p[i] = copy(g.__p[i])
            elif i > gDeg:
                s.__p[i] = copy(f.__p[i])
            else:
                s.__p[i] = f.__p[i] + g.__p[i]
        s.__normalize()
        return s

    def __radd__(f, g):
        """ Addition, see __add__
        """
        return f + g

    def __sub__(f, g):
        """ Subtraction (operators - and -=):
            a - b  --> a.__sub__(b), if a is of type Poly
                       b.__rsub__(a), otherwise
        """
        if not isinstance(g, Poly):
            g = Poly([g])
        fDeg, gDeg = f.degree(), g.degree()
        minDeg = min(fDeg, gDeg)
        maxDeg = max(fDeg, gDeg)
        s = Poly()
        s.__p = (maxDeg + 1) * [0] # list with (maxDeg + 1) zeroes
        for i in fromTo(0, maxDeg):
            if i > fDeg:
                s.__p[i] = -g.__p[i]
            elif i > gDeg:
                s.__p[i] = copy(f.__p[i])
            else:
                s.__p[i] = f.__p[i] - g.__p[i]
        s.__normalize()
        return s

    def __rsub__(f, g):
        """ Subtraktion, see __sub__
        """
        return Poly([g]) - f

    def __mul__(f, g):
        """ Multiplication (operators * and *=):
            a * b  --> a.__mul__(b), if a is of type Poly
                       b.__rmul__(a), otherwise
        """
        if not isinstance(g, Poly):
            g = Poly([g])
        if f.isZero():
            return f.copy()
        if g.isZero():
            return g.copy()
        n, m = f.degree(), g.degree()

        p = Poly()
        p.__p = (n + m + 1) * [0] # list with (n + m + 1) zeroes
        for i in fromTo(0, n):
            for j in fromTo(0, m):
                p.__p[i+j] += f.__p[i] * g.__p[j]
        p.__normalize()
        return p

    def __rmul__(f, g):
        """ Multiplikation, see __mul__
        """
        return f * g

    def __rshift__(f, n):
        """ f >> n -- Multiplication of Polynomial by x**n
        """
        r = Poly()
        r.__p = n * [0] + [copy(x) for x in f.__p]
        return r

    def __divmod__(f, g):
        """ Polynomial division with remainder. (global function divmod)
            divmod(f, g) --> f.__divmod__(g), if f is of type Poly
                             g.__rdivmod__(f), otherwise

            Returns the pair (q,r)
            with:  f == g * q + r
            and:   r.degree() &lt; g.degree() or r == 0
        """
        if not isinstance(g, Poly):
            g = Poly([g])
        r = f.copy()   # Kopie: Zählerpolynom wird zu Restpolynom
        q = Poly()     # Divisionsergebnis: beginnen mit Nullpolynom
        while r.degree() >= g.degree() and not r.isZero():
            assert f == g * q + r
            qCoeff = r[r.degree()] / g[g.degree()]  # Quotient der hoechsten Koeff.
            qPoly = Poly()
            qPoly.__p = [qCoeff]
            qPoly >>= (r.degree() - g.degree())
            q += qPoly
            assert f == g * (q-qPoly) + r
            deg0 = r.degree()
            r -= qPoly * g # grad(r) muss jetzt kleiner werden
            assert r.degree() < deg0
            assert f == g * (q-qPoly) + (r + qPoly*g)
            assert f == g * q + r
        return q, r

    def __rdivmod__(f, g):
        """ Polynomial division with remainder. See __divmod__
        """
        assert not isinstance(g, Poly)
        g = Poly([g])
        return g.__divmod__(f)

    def __div__(f, g):
        """ Polynomial division with remainder (operators // und //=):
            f // g  --> f.__floordiv__ (g), if f is of type Poly
                        g.__rfloordiv__(f), otherwise
        """
        return divmod(f, g)[0]

    def __rdiv__(f, g):
        """ Polynomial division with remainder. See __floordiv__
        """
        assert not isinstance(g, Poly)
        g = Poly([g])
        return divmod(g, f)[0]

    def __truediv__(a, b):   return a.__div__(b)
    def __rtruediv__(a, b):  return a.__rdiv__(b)
    def __floordiv__(a, b):  return a.__div__(b)
    def __rfloordiv__(a, b): return a.__rdiv__(b)

    #-- Modulo-Operator ---
    # a & b  --> __mod__ (a, b), wenn a ein Poly-Objekt ist
    #            __rmod__(b, a), sonst

    def __mod__(f, d):
        """ Modulo operator for polynomials (operators % und %=):
            a % b  --> a.__mod__ (b), if f is of type Poly
                       b.__rmod__(a), otherwise
        """
        return divmod(f, d)[1]

    def __rmod__(f, g):
        """ Modulo operator for polynomials. See __mod__
        """
        assert not isinstance(g, Poly)
        g = Poly([g])
        return divmod(g, f)[1]

    def __pow__(a, x):
        assert isNatural(x)
        if x == 1:
            return a
        elif x % 2 == 1:
            return a * a**(x-1)
        else:
            t = a ** (x//2)
            return t * t

    def normalized(self):
        """ Divides the polynomial by the highest coefficient
        ----de----
            Dividiert das Polynom durch den höchsten Koeffizienten
        """
        if self.degree() >= 0:
            r = self.copy()
            r.__p = [c/r.__p[-1] for c in r.__p]
            return r
        else:
            return self.copy()

    def inverse(self, mod):
        """ <b>p.inverse(m)</b><br/>
            <i>Multiplicative inverse modulo polynomial m</i><br/>
            The polynomial mod must be relative prime to p!
        ----de----
            <b>p.inverse(m)</b><br/>
            <i>Multiplikatives Inverses modulo Polynom m</i><br/>
            Das Polynom mod muss teilerfremd zu p sein!
        """
        assert type(self.__p[0]) == Mod
        m = self.__p[0].m
        d, inv, y = Poly.gcdExt(self, mod)
        if not d.isOne():
            raise _MathGUIdeError("Polynomial is not invertible")
        # inv * self + y * mod == d == 1
        # ==> (inv * self) % mod == 1
        return inv % mod


#===============================================================
# Set
#---------------------------------------------------------------

class Set (list): # Polymorphic sets # Polymorphe Mengen
    """ Class <code>Set</code><br/>
        <i>Polymorphic sets</i><br/>
        <b>Example</b>: <code>A = Set([1,2,3])</code><br/>
        The class Set allows the following operations:<br/>
        <ul>
          <li>Intersection (operator &amp;)</li>
          <li>Union (operator +)</li>
          <li>Set difference (operator -)</li>
          <li>Symmetric difference (operator ^)</li>
          <li>Power set (method powerset)</li>
          <li>Cartesian product (Operator *)</li>
          <li>Element test (Operator <code>in</code>)</li>
          <li>Cardinality (Python function <code>len</code>)</li>
        </ul>
    ----de----
        Klasse <code>Set</code><br/>
        <i>Polymorphe Mengen</i><br/>
        <b>Beispiel</b>: <code>A = Set([1,2,3])</code><br/>
        Die Klasse Set erlaubt folgende Operationen:<br/>
        <ul>
          <li>Durchschnitt (Operator &amp;)</li>
          <li>Vereinigung (Operator +)</li>
          <li>Mengendifferenz (Operator -)</li>
          <li>Symmetrische Differenz (Operator ^)</li>
          <li>Potenzmenge (powerset)</li>
          <li>Kartesisches Produkt (Operator *)</li>
          <li>Elementtest (Operator <code>in</code>)</li>
          <li>Kardinalzahl (Python-Funktion <code>len</code>)</li>
        </ul>
    """
    _staticMethods = []

    def operators(self):
        """ <b>Set.operators()</b><br/>
            <i>For documentation only</i><br/>
            The following operators are defined in the class <b>Set</b>:<br/>
            <table>
             <tr><th>Op.</th><th>Function</th><th>Examples</th></tr>
             <tr><th>&amp;</th><td>Intersection</td><td><code>A &amp; B; A &amp;= B</code></td></tr>
             <tr><th>+</th><td>Union</td><td><code>A + B; A += B</code></td></tr>
             <tr><th>-</th><td>Set difference</td><td><code>A - B; A -= B</code></td></tr>
             <tr><th>^</th><td>Symmetric difference</td><td><code>A ^ B; A ^= B</code></td></tr>
             <tr><th>*</th><td>Cartesian product</td><td><code>A * B; A *= B</code></td></tr>
             <tr><th>in</th><td>is element of</td><td><code>x in A</code></td></tr>
            </table>
            <font color="#000080" size="-2">This method is for documentation only. It has no effect.</font>
        ----de----
            <b>Set.operators()</b><br/>
            <i>Nur zur Dokumentation</i><br/>
            Die Klasse <b>Set</b> erlaubt folgende Operatoren:<br/>
            <table>
             <tr><th>Op.</th><th>Funktion</th><th>Beispiele</th></tr>
             <tr><th>&amp;</th><td>Durchschnitt</td><td><code>A &amp; B; A &amp;= B</code></td></tr>
             <tr><th>+</th><td>Vereinigung</td><td><code>A + B; A += B</code></td></tr>
             <tr><th>-</th><td>Mengendifferenz</td><td><code>A - B; A -= B</code></td></tr>
             <tr><th>^</th><td>Symmetrische Differenz</td><td><code>A ^ B; A ^= B</code></td></tr>
             <tr><th>*</th><td>Kartesisches Produkt</td><td><code>A * B; A *= B</code></td></tr>
             <tr><th>in</th><td>ist Element von</td><td><code>x in A</code></td></tr>
            </table>
            <font color="#000080" size="-2">Diese Methode dient nur zur Dokumentation. Sie hat keine Wirkung.</font>
        """
        pass

    def __init__(self, el):
        """ Constructor
        ----de----
            Konstruktor
        """
        list.__init__(self, list(el))
        self.__normalize()

    def __normalize(self):
        self.sort()
        for i in range(len(self)-1, 0, -1):
            if self[i] == self[i-1]:
                del self[i]

    def __repr__(self):
        """ Object representation as string (in curly braces)
        """
        r = "{"
        for e in self:
            r += str(e) + ","
        return (r+"}").replace(",}","}")

    # In der Basisklasse list sind die "rich comparison"
    # Methoden definiert und haben Vorrang vor __cmp___
    # Deshalb werden sie hier wieder auf __cmp__ geleitet:
    def __lt__(A, B):
        return A.__cmp__(B) < 0

    def __le__(A, B):
        return A.__cmp__(B) <= 0

    def __eq__(A, B):
        return A.__cmp__(B) == 0

    def __ne__(A, B):
        return A.__cmp__(B) != 0

    def __gt__(A, B):
        return A.__cmp__(B) > 0

    def __ge__(A, B):
        return A.__cmp__(B) >= 0

    def __cmp__(A, B):
        """ For ordering sets
            Elements, wihch are sets themself come first and are sorted by length (cardinality)

            simultaneous overloading of  &lt; &lt;=, >, >=, == and !=

            If A is a Set and b not, A.__cmp__(b)  is called for comparing A with b (or vice versa)!

        """
        if not isinstance(B, Set):
            return -1           # erst Mengen, dann Nichtmengen
        elif len(A) != len(B):  # zwei Mengen: nach Größe ordnen
            return _cmp(len(A), len(B))
        elif len(A) == 0:
            return 0            # zwei leere Mengen
        else:                   # zwei gleich große nichtleere Mengen
            if A[0] != B[0]:
                return _cmp(A[0], B[0])
            else:
                return _cmp(Set(A[1:]), Set(B[1:]))

    def insert(self, e):
        """ <b>A.insert(e)</b><br/>
            <i>Inserts an additional element ti the set</i><br/>
            A.insert(e) is equivalent to A += Set(e)
        ----de----
            <b>A.insert(e)</b><br/>
            <i>Fügt ein zusätzliches Element in die Menge ein</i><br/>
            A.insert(e) ist äquivalent zu A += Set(e)
        """
        self.append(e)
        self.__normalize()

    def issubset(A, B):
        """ <b>A.issubset(B)</b><br/>
            <i>Is A subset of (oder equal to) B?</i><br/>
            Return value: bool
        ----de----
            <b>A.issubset(B)</b><br/>
            <i>Ist A Teilmenge (oder gleich) B?</i><br/>
            Rückgabewert: bool
        """
        for e in A:
            if e not in B:
                return False
        return True

    def issuperset(A, B):
        """ <b>A.issuperset(B)</b><br/>
            <i>Is A superset of (oder equal to) B?</i><br/>
            Return value: bool
        ----de----
            <b>A.issuperset(B)</b><br/>
            <i>Ist A Obermenge (oder gleich) B</i><br/>
            Rückgabewert: bool
        """
        return B.issubset(A)

    def __add__(A, B):
        """ Union (operators + and +=):
            a + b  --> __add__ (a, b), if a is of type Set
                       __radd__(b, a), otherwise
        """
        return Set(list(A) + list(B))

    def union(A,B):
        """ <b>A.union(B)</b><br/>
            <i>Union set</i><br/>
            Also operator notation: <b>A + B</b><br/>
            Instead of <code>A = A + B</code> you can write <code>A += B</code>.
        ----de----
            <b>A.union(B)</b><br/>
            <i>Vereinigungsmenge</i><br/>
            Auch Operatorschreibweise: <b>A + B</b><br/>
            Statt A = A + B kann auch A += B geschrieben werden.
        """
        return A + B

    def __sub__(A, B):
        """ Set difference (operators - and -=)
        """
        result = list(A)
        for e in B:
            if e in result:
                result.remove(e)
        return Set(result)

    def difference(A,B):
        """ <b>A.difference(B)</b><br/>
            <i>Set difference</i><br/>
            Also operator notation: <b>A - B</b><br/>
            Instead of <code>A = A - B</code> you can write <code>A -= B</code>.
        ----de----
            <b>A.difference(B)</b><br/>
            <i>Mengendifferenz</i><br/>
            Auch Operatorschreibweise: <b>A - B</b><br/>
            Statt A = A - B kann auch A -= B geschrieben werden.
        """
        return A - B

    def __and__(A, B):
        """ Intersection (operators &amp; and &amp;=)
        """
        result = list(A)
        for e in A:
            if e not in B:
                result.remove(e)
        return Set(result)

    def intersection(A,B):
        """ <b>A.intersection(B)</b><br/>
            <i>Intersection</i><br/>
            Also operator notation: <b>A - B</b><br/>
            Instead of <code>A = A &amp; B</code> you can write <code>A &amp;= B</code>.
        ----de----
            <b>A.intersection(B)</b><br/>
            <i>Schnittmenge</i><br/>
            Auch Operatorschreibweise: <b>A &amp; B</b><br/>
            Statt A = A &amp; B kann auch A &amp;= B geschrieben werden.
        """
        return A & B

    def __xor__(A, B):
        """ Symmetric difference (operators ^ and ^=)
        """
        return (A + B) - (A & B)

    def __pow__(A, B):
        """ Symmetric difference (operators ^ and ^=)
        """
        return (A + B) - (A & B)

    def symmDiff(A,B):
        """ <b>A.symmDiff(B)</b><br/>
            <i>Symmetric difference</i><br/>
            Also operator notation: <b>A ^ B</b><br/>
            Instead of <code>A = A ^ B</code> you can write <code>A ^= B</code>.
        ----de----
            <b>A.symmDiff(B)</b><br/>
            <i>Symmetrische Differenz</i><br/>
            Auch Operatorschreibweise: <b>A ^ B</b><br/>
            Statt A = A ^ B kann auch A ^= B geschrieben werden.
        """
        return A^B

    def __mul__(A, B):
        """ Cartesian product (operator *)
        """
        return Set([(x,y) for x in A for y in B])

    def product(A,B):
        """ <b>A.product(B)</b><br/>
            <i>Cartesian product</i><br/>
            Also operator notation: <b>A * B</b><br/>
            Instead of <code>A = A * B</code> you can write <code>A *= B</code>.
        ----de----
            <b>A.product(B)</b><br/>
            <i>Kartesisches Produkt</i><br/>
            Auch Operatorschreibweise: <b>A * B</b><br/>
            Statt A = A * B kann auch A *= B geschrieben werden.
        """
        return A*B

    def powerset(self):
        """ <b>A.powerset()</b><br/>
            <i>Powerset (set of alle subsets)</i><br/>
        ----de----
            <b>A.powerset()</b><br/>
            <i>Potenzmenge (Menge aller Teilmengen)</i><br/>
        """
        if len(self) == 0:
            return Set([Set([])])
        else:
            first = self[0]
            rest = self[1:]
            powRest = Set(list(rest)).powerset()
            pl = [Set([first])+x for x in powRest]
            return powRest + Set(pl)

    def card(self):
        """ <b>A.card()</b><br/>
            <i>Cardinality</i><br/>
            Same as len(A)
        ----de----
            <b>A.card()</b><br/>
            <i>Kardinalzahl</i><br/>
            Synonym zu len(A)
        """
        return len(self)

###"Repräsentative Mengen"
##
##               # 1 2 12
##A1 = Set(1,12) # + - +
##A2 = Set(2,12) # - + +
##
##                      # 1 2 3 12 13 23 123
##B1 = Set(1,12,13,123) # + - - +  +  -   +
##B2 = Set(2,12,23,123) # - + - +  -  +   +
##B3 = Set(3,13,23,123) # - - + -  +  +   +
##
##C1 = Set(1,12,13,14, 123, 124, 134, 1234)
##C2 = Set(2,12,23,24, 123, 124, 234, 1234)
##C3 = Set(3,13,23,34, 123, 134, 234, 1234)
##C4 = Set(4,14,24,34, 124, 134, 234, 1234)
##
##D1 = Set(1, 12,13,14,15, 123,124,125,134,135,145, 1234,1235,1245,1345, 12345)
##D2 = Set(2, 12,23,24,25, 123,124,125,234,235,245, 1234,1235,1245,2345, 12345)
##D3 = Set(3, 13,23,34,35, 123,134,135,234,234,345, 1234,1235,1345,2345, 12345)
##D4 = Set(4, 14,24,34,45, 124,134,145,234,245,345, 1234,1245,1345,2345, 12345)
##D5 = Set(5, 15,25,35,45, 125,135,145,235,245,345, 1235,1245,1345,2345, 12345)


#===============================================================
#  Propositional Logic
#---------------------------------------------------------------

class Logic: # propositional logic # Aussagenlogik
    """ Class <code>Logic</code>: Propositional Logic<br/>
        This is a static class (only static methods available)<br/>
        Formulae of propositional logic, passed to the static methods,
        must be composed as follows:<br/>
        <b>Variables</b> must be single letters.<br/>
        <b>Junctors</b>: "<code>not</code>", "<code>and</code>", "<code>or</code>", "<code>xor</code>", "<code>-></code>", "<code>&lt;-></code>".
    ----de----
        Klasse <code>Logic</code>: Aussagenlogik<br/>
        Statische Klasse: nur Klassenmethoden<br/>
        Aussagenlogische Formeln, die den Klassenmethoden übergeben werden,
        müssen wie folgt aufgebaut sein:<br/>
        <b>Variablen</b> müssen einzelne Buchstaben sein.<br/>
        <b>Junktoren</b>: "<code>not</code>", "<code>and</code>", "<code>or</code>", "<code>xor</code>", "<code>-></code>", "<code>&lt;-></code>".
    """
    _staticMethods = ("truthTables", "satisfies", "valid", "satisfiable", "implies", "equivalent", "printTruthTable")

    @staticmethod
    def truthTables(formula):
        """ <b>truthTables(formula)</b><br/>
            <i>All possible truth tables for variables in formula</i><br/>
            The result is returned as a generator object (for a single iteration).<br/>
            <b>Example</b><br/>
            <code>for t in Logic.truthTables("A and B"): print(t)</code><br/>
            returns:<br/>
            <code>{'A': False, 'B': False}<br/>
            {'A': False, 'B': True}<br/>
            {'A': True, 'B': False}<br/>
            {'A': True, 'B': True}</code>
        ----de----
            <b>truthTables(formula)</b><br/>
            <i>Alle möglichen Wahrheitswerttabellen für im Ausdruck
            <b>formula</b> vorkommenden Variablen</i><br/>
            Das Ergebnis wird als Generatorobjekt (zur einmaligen Iteration) ausgegeben.<br/>
            <b>Beispiel</b><br/>
            <code>for t in Logic.truthTables("A and B"): print(t)</code><br/>
            gibt folgendes aus:<br/>
            <code>{'A': False, 'B': False}<br/>
            {'A': False, 'B': True}<br/>
            {'A': True, 'B': False}<br/>
            {'A': True, 'B': True}</code>
        """
        table = {}
        for op in ("not", "and", "or", "xor"):
            formula = formula.replace(op, " ")
        for c in formula:
            if c.isalpha(): # Variable!
                table[c] = False
        n = len(table)
        vars = sorted(list(table.keys()))
        while True:
            yield table
            k = n-1
            table[vars[k]] = not table[vars[k]]
            while not table[vars[k]]: # "Übertrag"
                k -= 1
                if k < 0:
                    return
                table[vars[k]] = not table[vars[k]]

    @staticmethod
    def satisfies(f, truthTable):
        """ <b>satisfies(f, truthTable)</b><br/>
            <i>Is the formula <b>f</b> true under the <b>truthTable</b>?</i><br/>
            <b>Example</b><br/>
            <code>Logic.satisfies("A or B", {"A":True, "B":False})</code>
            returns <code>True<code>.
        ----de----
            <b>satisfies(f, truthTable)</b><br/>
            <i>Wird die Formel <b>f</b> unter der Belegung <b>truthTable</b> wahr?</i><br/>
            <b>Beispiel</b><br/>
            <code>Logic.satisfies("A or B", {"A":True, "B":False})</code>
            gibt <code>True</code> zurück.
        """
        f = f.replace("-->", "<=").replace("->", "<=").replace("<->", "==").replace("xor", "!=").replace("<<=", "==").replace("||", "or").replace("&&", "and").replace("-", "not ")
        f = f.replace("not", "@0").replace("and", "@1").replace("or", "@2")
        for var in truthTable:
            if truthTable[var]:
                f = f.replace(var, "@3")
            else:
                f = f.replace(var, "@4")
        f = f.replace("@0", "not").replace("@1", "and").replace("@2", "or").replace("@3", "True").replace("@4", "False")
        return eval(f)

    @staticmethod
    def valid(f):
        """ <b>satisfies(f, truthTable)</b><br/>
            <i>Is the formula <b>f</b> valid</b>?</i><br/>
            <b>Example</b><br/>
            <code>Logic.valid("A or not A")</code>
            returns <code>True<code>.
        ----de----
            <b>satisfies(f, truthTable)</b><br/>
            <i>Ist die Formel <b>f</b> allgemeingültig?</i><br/>
            <b>Beispiel</b><br/>
            <code>Logic.valid("A or not A")</code>
            gibt <code>True</code> zurück.
        """
        for t in Logic.truthTables(f):
            if not Logic.satisfies(f, t):
                return False
        return True

    @staticmethod
    def satisfiable(f):
        """ <b>satisfiable(f)</b><br/>
            <i>Is the formula <b>f</b> satisfiable</b>?</i><br/>
            <b>Example</b><br/>
            <code>Logic.satisfiable("A and not A")</code>
            returns <code>False<code>.
        ----de----
            <b>satisfiable(f)</b><br/>
            <i>Ist die Formel <b>f</b> erfüllbar?</i><br/>
            <b>Beispiel</b><br/>
            <code>Logic.satisfiable("A and not A")</code>
            gibt <code>False</code> zurück.
        """
        return not Logic.valid("not({0})".format(f))

    @staticmethod
    def implies(f1, f2):
        """ <b>implies(f1, f2)</b><br/>
            <i>implies the formula <b>f1</b> the formula <b>f2</b>?</b>?</i><br/>
            <b>Example</b><br/>
            <code>Logic.implies("A and B", "A or B")</code>
            returns <code>True<code>.
        ----de----
            <b>implies(f1, f2)</b><br/>
            <i>Folgt aus der Formel <b>f1</b> die Formel <b>f2</b>?</i><br/>
            <b>Beispiel</b><br/>
            <code>Logic.implies("A and B", "A or B")</code>
            gibt <code>True</code> zurück.
        """
        return Logic.valid("not({0}) or ({1})".format(f1, f2))

    @staticmethod
    def equivalent(f1, f2):
        """ <b>equivalent(f1, f2)</b><br/>
            <i>Are the formulae <b>f1</b> and <b>f2</b> equivalent?</b>?</i><br/>
            <b>Example</b><br/>
            <code>Logic.equivalent("not (A and B)", "not A or not B"))</code>
            returns <code>True<code>.
        ----de----
            <b>equivalent(f1, f2)</b><br/>
            <i>Sind die Formeln <b>f1</b> und <b>f2</b> äquivalent?</i><br/>
            <b>Beispiel</b><br/>
            <code>Logic.equivalent("not (A and B)", "not A or not B"))</code>
            gibt <code>True</code> zurück.
        """
        return Logic.valid("({0}) == ({1})".format(f1, f2))
    @staticmethod
    def printTruthTable(formulae):
        """ <b>printTruthTable(formulae)</b><br/>
            <i>Print a truth table with the formulae equivalent?</i><br/>
            <b>Example</b><br/>
            <code>Logic.printTruthTable(["A","B","not (A and B)", "not A or not B"])</code>
        ----de----
            <b>printTruthTable(formulae)</b><br/>
            <i>Erzeuge eine Wahrheitstabelle mit den gegebenen Formeln</i><br/>
            <b>Beispiel</b><br/>
            <code>Logic.printTruthTable(["A","B","not (A and B)", "not A or not B"])</code>
        """
        table = [];
        for t in Logic.truthTables(" and ".join(formulae)):
            table.append(copy(t))
        printTable(pow(2,len(table[0])),len(formulae),
            lambda i,k: Logic.satisfies(formulae[k], table[i]),
            colHeadFn = lambda i: formulae[i].replace("<", "&lt;").replace(">", "&gt;")
)

#===============================================================
#  Graphs and Trees
#---------------------------------------------------------------

class Edge: # Edges of a Graph # Kanten eines Graphen
    """ Class <code>Edge</code>: Edge of a <a href="Graph.html">Graph</a><br/>
    ----de----
        Klasse <code>Edge</code>: Kante eines Graphen (Klasse <a href="Graph.html">Graph</a>)<br/>
    """
    _staticMethods = []

    def __init__(self, source, target, weight=0, highlight=False):
        """ Constructor
        ----de----
            Konstruktor
        """
        self.source = source
        self.target = target
        self.weight = weight
        self.highlight = highlight

    def __repr__(self):
        """ Object representation as string
        """
        if self.weight == 0:
            return "{{{0},{1}}}".format(self.source, self.target)
        else:
            return "({{{0},{1}}}, w={2})".format(self.source, self.target, self.weight)

    @staticmethod
    def _fromShortRepr(s):
        p = s.split(",")
        n0, n1 = p[0:2]
        w = 0
        m = len(p) >= 4
        if len(p) >= 3:
            w = p[2]
        return Edge(int(n0), int(n1), int(w), m)

    def isIncidentNode(self, node):
        return node in (self.source, self.target)

    def adjacentNode(self, node):
        if node == self.source:
            return self.target
        elif node == self.target:
            return self.source
        else:
            return None

    def _shortRepr(self):
        """ short representation as string
        """
        if self.highlight:
            return "{0},{1},{2},1".format(self.source, self.target, self.weight)
        elif self.weight != 0:
            return "{0},{1},{2}".format(self.source, self.target, self.weight)
        else:
            return "{0},{1}".format(self.source, self.target)


class Graph: # Graphs # Graphen
    """ Class <code>Graph</code>: Graph (in the sense of graph theory)<br/>
        A Graph consists of nodes and edges.
        An edge (class <a href="Edge.html">Edge</a>) connects two nodes.<br/>
        Edges optionally may have a weight.<br/>
        <p class="remark">Although a pure Graph does not carry geometrical data, mathGUIde allows to attach
        positions to nodes corresponding to a Graph representation in the Graph window.</p>
    ----de----
        Klasse <code>Graph</code>: Graph (im Sinne der Graphentheorie)<br/>
        Ein Graph besteht aus Knoten und Kanten.
        Eine Kante (Klasse <a href="Edge.html">Edge</a>) verbindet zwei Knoten.<br/>
        Kanten können auch mit einem Gewicht versehen werden.<br/>
        <p class="remark">Obwohl zur Graphendefinition keine geometrischen Informationen gehören,
        kann mathGUIde die Positionen von im Graphenfenster angeordneten Knoten speichern.</p>
    """
    #............................................................
    _staticMethods = ("fromShortRepr", "example1", "complete", "completeBipartite", "petersen")

    @staticmethod
    def fromShortRepr(s):
        """ <b>Graph.fromShortRepr(s)</b><br/>
            <i>Makes the Graph from the short representation s</i>
        ----de----
            <b>Graph.fromShortRepr(s)</b><br/>
            <i>Erzeugt den Graphen aus der Kurzdarstellung s</i>
        """
        g = Graph()
        parts = s.split("|")
        if len(parts[0]) > 0:
            # nodes
            for n in parts[0].split(";"):
                p = n.split(",")
                g.nodes.append(int(p[0]))
                if len(p) >= 3:
                    g.locations[int(p[0])] = float(p[1]), float(p[2])
        if len(parts[1]) > 0:
            # edges
            for e in parts[1].split(";"):
                g.edges.append(Edge._fromShortRepr(e))
        return g

    @staticmethod
    def example1():
        """ <b>Graph.example1()</b>
        ----de----
            <b>Graph.example1()</b>
        """
        return Graph.fromShortRepr("1,80,20;2,240,20;3,0,100;4,160,100;5,320,100;6,0,180;7,160,180;8,320,180;9,80,260;10,240,260|1,2,36;1,4,15;2,4,69;2,5,1;4,5,53;3,4,92;1,3,99;3,6,37;6,7,21;7,8,88;4,6,27;4,7,32;5,7,10;5,8,66;6,9,3;7,9,18;7,10,49;8,10,43;9,10,74")

    @staticmethod
    def complete(n, r=130, margin=20):
        """ <b>Graph.complete(n, r=130, margin=20)</b><br/>
            <i>Complete Graph of order n, displayed on a circle with radius r.</i>
        ----de----
            <b>Graph.complete(n, r=130, margin=20)</b><br/>
            <i>Vollständiger Graph der Ordnung n angeordnet auf einem Kreis mit Radius r.</i>
        """
        c = margin + r # center x and y
        nodes = ";".join(["{0},{1},{2}".format(i+1, c+r*sin(i*2*pi/n), c-r*cos(i*2*pi/n)) for i in range(n)])
        edges = ";".join(["{0},{1},{2}".format(i+1,j+1,0) for i in range(n) for j in range(n) if i < j])
        return Graph.fromShortRepr("{0}|{1}".format(nodes, edges))

    @staticmethod
    def completeBipartite(n1, n2):
        """ <b>Graph.completeBipartite(n1,n2)</b><br/>
            <i>Complete bipartite Graph with partition sizes n1 and n2.</i>
        ----de----
            <b>Graph.completeBipartite(n1,n2)</b><br/>
            <i>Vollständiger bipartiter Graph mit den Partitionsgrößen n1 und n2.</i>
        """
        dx, dy, margin = 60, 160, 20
        margin1 = margin2 = margin
        if n1 < n2:
            margin1 += (n2-n1) * dx / 2
        elif n2 < n1:
            margin2 += (n1-n2) * dx / 2
        nodes1 = ";".join(["{0},{1},{2}".format(i+1, margin1+i*dx, margin) for i in range(n1)])
        nodes2 = ";".join(["{0},{1},{2}".format(n1+i+1, margin2+i*dx, margin+dy) for i in range(n2)])
        edges = ";".join(["{0},{1},{2}".format(i+1,n1+j+1,0) for i in range(n1) for j in range(n2)])
        return Graph.fromShortRepr("{0};{1}|{2}".format(nodes1, nodes2, edges))

    @staticmethod
    def petersen(r=150, margin=20):
        """ <b>Graph.petersen()</b><br/>
            <i>The Peterson Graph</i><br/>
            It serves as an example and counterexample for many problems in graph theory.
        ----de----
            <b>Graph.completeBipartite()</b><br/>
            <i>Der Peterson-Graph</i><br/>
            Er dient als Beispiel oder Gegenbeispiel für viele Probleme der Graphentheorie.
        """
        c = margin + r # center x and y
        r0 = r/2
        nodes1 = ";".join(["{0},{1},{2}".format(i+1, c+r*sin(i*2*pi/5),  c-r*cos(i*2*pi/5))  for i in range(5)])
        nodes2 = ";".join(["{0},{1},{2}".format(i+6, c+r0*sin(i*2*pi/5), c-r0*cos(i*2*pi/5)) for i in range(5)])
        edges = "1,2;1,5;1,6;2,3;2,7;3,4;3,8;4,5;4,9;5,10;6,8;6,9;7,9;7,10;8,10"
        return Graph.fromShortRepr("{0};{1}|{2}".format(nodes1, nodes2, edges))

    #............................................................

    def __init__(self):
        """ Constructor
        ----de----
            Konstruktor
        """
        self.nodes = []
        self.edges = []
        self.locations = {}

    def __repr__(self):
        """ Object representation as string
        """
        return "Graph(Nodes = {{{0}}}, Edges = {{{1}}})".format(
                join([str(n) for n in self.nodes], ","),
                join([str(e) for e in self.edges], ","))

    def shortRepr(self):
        """ <b>G.shortRepr()</b><br/>
            <i>short representation of Graph G as string</i><br/>
        ----de----
            <b>.shortRepr()</b><br/>
            <i>Kurzdarstellung des Graphen G (als String)</i>
        """
        # example: "1;2;3;4|1,2,36;1,3,99;1,4,15;2,4,69"
        return "{0}|{1}".format(join([self._shortNodeRepr(n) for n in self.nodes], ";"),
                                join([e._shortRepr() for e in self.edges], ";"))

    def _shortNodeRepr(self, node):
        """ short representation of a Node as string
        """
        if node in self.locations:
            return "{0},{1},{2}".format(node, self.locations[node][0], self.locations[node][1])
        else:
            return "{0}".format(node)

    def order(self):
        """ <b>G.order()</b><br/>
            <i>Order (number of vertices) of the Graph G.</i>
        ----de----
            <b>G.order()</b><br/>
            <i>Ordnung (Anzahl der Knoten) des Graphen G.</i>
        """
        return len(self.nodes)

    def complement(self):
        """ <b>G.complement()</b><br/>
            <i>Returns the graph with the same vertices as G, such that two vertices are adjacent if and only if they are not adjacent in G.</i>
        ----de----
            <b>G.complement()</b><br/>
            <i>Gibt den Graphen mit den gleichen Knoten wie G zurück, in dem sich zwischen zwei Knoten genau dann eine Kante befindet, wenn G dort keine Kante hat.</i>
        """
        g = Graph()
        g.nodes = list(self.nodes) # independent copy
        g.edges = []
        g.locations = dict(self.locations)
        for n1 in g.nodes:
            for n2 in g.nodes:
                if n1 < n2:
                    if not self.adjacent(n1, n2):
                        g.addEdges(Edge(n1, n2))
        return g

    def connectedNodes(self, node):
        """ <b>G.componentsCount()</b><br/>
            <i>Returns a list of all nodes of G which are connected with node (including node).</i>
        ----de----
            <b>G.componentsCount()</b><br/>
            <i>Gibt eine Liste aller mit node durch einen Kantenzug verbundenen Knoten in G zurück (einschließlich node).</i>
        """
        checked = 0
        nodes = [node]
        while checked < len(nodes):
            n = nodes[checked]
            for m in self.adjacentNodes(n):
                if (m not in nodes):
                    nodes.append(m)
            checked += 1
        nodes.sort()
        return nodes

    def componentsCount(self):
        """ <b>G.componentsCount()</b><br/>
            <i>The number connectivity components of the Graph G</i>
        ----de----
            <b>G.componentsCount()</b><br/>
            <i>Anzahl der Zusammenhangskomponenten des Graphen G</i>
        """
        count = 0
        uncheckedNodes = list(self.nodes) # independent copy
        while len(uncheckedNodes) > 0:
            count += 1
            n = uncheckedNodes[0]
            for m in self.connectedNodes(n):
                uncheckedNodes.remove(m)
        return count

    def isConnected(self):
        """ <b>G.isConnected()</b><br/>
            <i>Returns a truth value indicating wether the Graph G is connected</i>
        ----de----
            <b>G.isConnected()</b><br/>
            <i>Gibt als Wahrheitswert zurück, ob der Graph G zusammenhängend ist</i>
        """
        if len(self.nodes) == 0: # an empty graph is connected!
            return True
        else:
            return (self.componentsCount() == 1)

    def isTree(self):
        """ <b>G.isTree()</b><br/>
            <i>Returns a truth value indicating wether the Graph G is a tree</i>
        ----de----
            <b>G.isTree()</b><br/>
            <i>Gibt als Wahrheitswert zurück, ob der Graph G ein Baum ist</i>
        """
        return self.isConnected() and len(self.edges) == len(self.nodes) - 1

    def adjacentNodes(self, node):
        """ <b>G.adjacentNodes(node)</b><br/>
            <i>Returns a sorted list of all nodes of G adjacent to <b>node</b>.</i>
        ----de----
            <b>G.adjacentNodes(node)</b><br/>
            <i>Gibt eine sortierte Liste aller zu <b>node</b> adjazenten Knoten von G zurück.</i>
        """
        adjNodes = []
        for e in self.edges:
            if node == e.source:
                adjNodes.append(e.target)
            elif node == e.target:
                adjNodes.append(e.source)
        adjNodes.sort()
        return adjNodes

    def adjacent(self, node1, node2):
        """ <b>G.adjacentNodes(node1, node2)</b><br/>
            <i>Determines wether node1 and node2 are adjacent (bool return value) in G.</i>
        ----de----
            <b>G.adjacentNodes(node1, node2)</b><br/>
            <i>Gibt an, ob node1 und node2 in G adjazent sind (bool).</i>
        """
        return node2 in self.adjacentNodes(node1)

    def incidentEdges(self, node):
        incEdges = []
        for e in self.edges:
            if e.isIncidentNode(node):
                incEdges.append(e)
        return incEdges

    def degree(self, node):
        """ <b>G.degree(node)</b><br/>
            <i>Returns the degree of node (count of adjacent nodes)</i>
        ----de----
            <b>G.degree(node)</b><br/>
            <i>Gibt den Grad des Knotens node (Anzahl der adjazenten Knoten) in G zurück</i>
        """
        return len(self.incidentEdges(node))

    def edge(self, node1, node2):
        """ <b>G.edge(node1, node2)</b><br/>
            <i>The connecting edge between the two nodes</i><br/>
            If node1 and node2 are adjacent, the connecting edge is returned, otherwise <code>None</code>.
        ----de----
            <b>G.edge(node1, node2)</b><br/>
            <i>Die Verbindungskante zwischen den zwei Knoten</i><br/>
            Wenn node1 und node2 adjazent sind, wird die the Verbindungskante zurückgegeben, sonst <code>None</code>.
        """
        for e in self.edges:
            if (e.source == node1 and e.target == node2) or (e.source == node2 and e.target == node1):
                return e
        return None

    def display(self, n=0):
        """ <b>G.display(n=0)</b><br/>
            <i>Displays the Graph G in the n-th Graph tab.</i><br/>
            if n == 0, the Graph is displayed in the active Graph window.
        ----de----
            <b>G.display(n=0)</b><br/>
            <i>Zeigt den Graphen G im n-ten Graphen-Fenster an.</i>
            Ist n = 0, wird der Graph im aktiven Graphen-Fenster angezeigt.
        """
        return "@G{0}:{1}".format(n, self.shortRepr())

    def addNodes(self, *nodes):
        self.nodes += nodes

    def addEdges(self, *edges):
        for e in edges:
            if e.source not in self.nodes:
                self.nodes.append(e.source)
            if e.target not in self.nodes:
                self.nodes.append(e.target)
        self.edges += edges

    def copy(self):
        """ <b>G.copy()</b><br/>
            <i>independent copy ("clone") of G</i>
        ----de----
            <b>G.copy()</b><br/>
            <i>unabhängige Kopie von G</i>
        """
        return Graph.fromShortRepr(self.shortRepr())

    def reduceToHighlightedEdges(self):
        """ <b>G.reduceToHighlightedEdges()</b><br/>
            <i>Removes all not highlighted edges from the Graph G.</i><br/>
            The highlighting of the remaining edges is turned off.
        ----de----
            <b>G.reduceToHighlightedEdges()</b><br/>
            <i>Entfernt alle nicht hervorgehobenen Kanten aus dem Graphen G.</i><br/>
            Die Hervorhebung der verbleibenden Kanten wird zurückgenommen.
        """
        highlightedEdges = [e for e in self.edges if e.highlight]
        otherEdges = [e for e in self.edges if not e.highlight]
        for e in highlightedEdges:
            e.highlight = False
        for e in otherEdges:
            self.edges.remove(e)

    def cancelHighlight(self):
        """ <b>G.cancelHighlight()</b><br/>
            <i>Resets all highlighted edges to normal state.</i>
        ----de----
            <b>G.cancelHighlight()</b><br/>
            <i>Setzt alle hervorgehobenen Kanten in den Normalzustand zurück.</i>
        """
        for e in self.edges:
            e.highlight = False

    def highlightShortestPath(self, startNode, goalNode):
        """ <b>G.highlightShortestPath(startNode, goalNode)</b><br/>
            <i>Highlightes a shortest path (if existent) from startNode to goalNode using the Dijkstra algorithm.</i><br/>
            Returns the length of the path (or inf if there is no connection).<br/>
            Use the method <a href="Graph.display"><code>display</code></a> in order
            to make the highlighting visible.
        ----de----
            <b>G.highlightShortestPath(startNode, goalNode)</b><br/>
            <i>Hebt einen kürzesten Pfad von startNode nach goalNode hervor (mit dem Dijkstra-Algorithmus).</i><br/>
            Die Länge des Pfades wird zurückgegeben (oder inf, falls keine Verbindung vorhanden).<br/>
            Mit der Methode <a href="Graph.display"><code>display</code></a> können Sie
            die Hervorhebung sichtbar machen.
        """
        self.cancelHighlight()
        predecessorNode = {}
        unfinishedNodes = list(self.nodes) # independent copy

        upperBound = {} # dictionary with upper bounds for path length from startNode to node
        infinite = float("inf")
        for n in self.nodes:
            upperBound[n] = infinite
        upperBound[startNode] = 0

        activeNode = startNode
        while True:
            unfinishedNodes.remove(activeNode)
            fail = True
            for e in self.incidentEdges(activeNode):
                n = e.adjacentNode(activeNode)
                if n in unfinishedNodes:
                    fail = False
                    # add 1 to weights in order to count path lengths
                    # for unweighted Graphs (default weight = 0):
                    dist = upperBound[activeNode] + e.weight + 1
                    if dist < upperBound[n]: # a shorter path to n discovered
                        upperBound[n] = dist
                        predecessorNode[n] = activeNode
            if fail:
                break
            best, newNode = min([(upperBound[n], n) for n in unfinishedNodes])
            activeNode = newNode

        if goalNode in unfinishedNodes:
            return infinite
        # go back from goalNode to startNode:
        n = goalNode
        length = 0
        while n != startNode:
            p = predecessorNode[n]
            e = self.edge(p, n)
            e.highlight = True
            length += e.weight
            n = p
        return length

    def highlightMinimalSpanTree(g, trace=False):
        """ <b>G.minimalSpanTree(trace=False)</b><br/>
            <i>Highlightes a minimal spanning tree using the Kruskal algorithm.</i><br/>
            If <code>trace</code> is <code>True</code>, all steps are printed.
        ----de----
            <b>G.minimalSpanTree(trace=False)</b><br/>
            <i>Hebt einen minimalen aufspannenden Baum hervor (mit dem Kruskal-Algorithmus).</i><br/>
            Wenn <code>trace</code> den Wert <code>True</code> hat, werden alle Schritte angezeigt.
        """

        nodeColor = {}
        marked = {}
        for n in g.nodes:
            nodeColor[n] = 0
        def weight(edge):
            return edge.weight
        g.edges.sort(key=weight)

        curColor = 1
        spanTreeSize = 0

        for edge in g.edges:
            marked[edge] = False

        for edge in g.edges:
            choose = True
            n0, n1 = edge.source, edge.target
            if nodeColor[n0] == nodeColor[n1]:
            # both nodes have tha same color
                if nodeColor[n0] == 0:
                    nodeColor[n0] = curColor   # both nodes uncolored (no edge):
                    nodeColor[n1] = curColor   # use current color,
                    curColor += 1              # new acurrent color
                else:
                    choose = False  # coonecting would result in a circle ==> skip edge
            else:
            # both nodes have different colors
                if min(nodeColor[n0], nodeColor[n1]) == 0:
                # one node uncolored: use color of other node
                    if nodeColor[n0] == 0:
                        nodeColor[n0] = nodeColor[n1]
                    else:
                        nodeColor[n1] = nodeColor[n0]
                else:
                # both nodes colored: join two components (change all nodes with lower color to higher color)
                    lowColor  = min(nodeColor[n0], nodeColor[n1])
                    highColor = max(nodeColor[n0], nodeColor[n1])
                    for n in nodeColor:
                        if nodeColor[n] == lowColor:
                            nodeColor[n] = highColor
            marked[edge] = choose
            if trace:
                if choose:
                    spanTreeSize += 1
                    print("{0} Choose".format(spanTreeSize), end=" ")
                else:
                    print("Skip", end=" ")
                print("Edge {0}-{1} (Weight {2})".format(edge.source, edge.target, edge.weight))
            if spanTreeSize == len(nodeColor) - 1:
                break
        for edge in marked:
            edge.highlight = marked[edge]

    def isomorphic(G,H):
        """ <b>G.isomorphic(H)</b><br/>
            <i>Determines wether G and H are isomorphic Graphs.</i><br/>
        ----de----
            <b>G.isomorphic(H)</b><br/>
            <i>Entscheidet, ob G und H isomorphe Graphen sind.</i><br/>
        """
        # Very inefficient implementation!
        # As a first optimization we could permute only nodes of equal degree
        g, h = G.nodes, H.nodes
        n = len(g)
        if n != len(h):
            return False
        if n > 8:
            raise _MathGUIdeError("max. 8 nodes!")
        for p in permutations(g):
            match = True
            for i in fromTo(0, n-2):
                for k in fromTo(i+1, n-1):
                    if G.adjacent(p[i], p[k]) != H.adjacent(h[i], h[k]):
                        match = False
                        break
                if not match:
                    break
            if match:
                return True
        return False

#===============================================================


def printTable(height, width, cellFunction, **options):
    """ <b>printTable(height, width, cellFunction, rowHeadFn=None, colHeadFn=None, corner="")</b><br/>
        <i>displays a table of <var>height</var> rows and <var>width</var> columns, whose cells are calculated by <var>cellFunction</var>.</i><br/>
        <var>cellFunction</var> must be a function with two parameters (row, column).<br/>
        The following optional named parameters are supported:<br/>
        <var>rowHeadFn</var>: if given, row header cells are added and calculated using this function<br/>
        <var>colHeadFn</var>: if given, column header cells are added and calculated using this function<br/>
        <var>headAlign</var>: "left" (default), "right" or "center"<br/>
        <var>cellAlign</var>: "left", "right" (default) or "center"<br/>
        <var>title</var>: if given, a title line is added, using this text<br/>
        <var>corner</var>: if given (and rowHeadFn and colHeadFn are given), this text is displayed in the top left table corner.<br/>
        This function is used by the menu command  Insert -- Table.<br/>
        <b>Example</b>:<pre>
 printTable(7, 7, lambda i,k: i*k % 7,
    rowHeadFn = lambda i: i,
    colHeadFn = lambda k: k,
    title = "Multiplication mod 7",
    corner = "*")</pre>
        prints a multiplication table modulo 7.
    ----de----
        <b>printTable(height, width, cellFunction, rowHeadFn=None, colHeadFn=None, corner="")</b><br/>
        <i>gibt eine Tabelle mit <var>height</var> Zeilen und <var>width</var> Spalten aus, deren Zellen mit <var>cellFunction</var> berechnet werden.</i><br/>
        <var>cellFunction</var> muss eine Funktion mit zwei Parametern (Zeile, Spalte) sein.<br/>
        Folgende optionalen benannten Parameter werden unterstützt:<br/>
        <var>rowHeadFn</var>: Falls vorhanden, werden Zeilenköpfe angezeigt und mit dieser Funktion berechnet.<br/>
        <var>colHeadFn</var>: Falls vorhanden, werden Spaltenköpfe angezeigt und mit dieser Funktion berechnet.<br/>
        <var>headAlign</var>: "left" (Standard), "right" oder "center"<br/>
        <var>cellAlign</var>: "left", "right" (Standard) oder "center"<br/>
        <var>title</var>: Falls vorhanden, wird eine Titelzeile mit diesem Text über der Tabelle angezeigt<br/>
        <var>corner</var>: Falls vorhanden (und auch rowHeadFn und colHeadFn), wird dieser Text
        in der oberen linken Tabellenecke angezeigt.<br/>
        Diese Funktion wird vom Menübefehl Einfügen -- Tabelle verwendet.<br/>
        <b>Beispiel</b>:<pre>
 printTable(7, 7, lambda i,k: i*k % 7,
    rowHeadFn = lambda i: i,
    colHeadFn = lambda k: k,
    title = "Multiplikation mod 7",
    corner = "*")</pre>
        Gibt eine Multiplikationstabelle modulo 7 aus.
    """
    rowHeadFn = options.get("rowHeadFn", None)
    colHeadFn = options.get("colHeadFn", None)
    headAlign = options.get("headAlign", "left")
    cellAlign = options.get("cellAlign", "right")
    title = options.get("title", "")
    titleAlign = options.get("titleAlign", "center")
    corner = options.get("corner", "")

    tdHead  = '<td bgcolor="#C6D9F1" align="{0}">'.format(headAlign)
    tdValue = '<td bgcolor="#F2F7FC" align="{0}">'.format(cellAlign)
    print('{html}<table cellspacing="6">') # don't convert to HTML again !
    if len(title) > 0:
        colspan = width
        if rowHeadFn:
            colspan = width + 1
        print('<tr><td colspan="{0}" align="{1}" bgcolor="#376092"><font color="white">{2}</font></td></tr>'.format(colspan, titleAlign, title))
    if colHeadFn:
        print("<tr>", end="") # heading
        if rowHeadFn:
            print("<td>{0}</td>".format(corner), end="") # top left corner
        for k in range(width):
            print(tdHead, colHeadFn(k), "</td>", end="")
        print("</tr>")
    for i in range(height):
        print("<tr>", end="") # heading
        if rowHeadFn:
            print(tdHead, rowHeadFn(i), "</td>", end="")
        for k in range(width):
            print(tdValue, cellFunction(i,k), "</td>", end="")
        print("</tr>")
    print("</table>{/html}")


def printValueTable(values, variable="n", start=0, end=10, increment=1, vertical=True):
    """ <b>printValueTable(values, variable="n", start=0, end=10, increment=1, vertical=True)</b><br/>
        <i>Displays a value table.</i><br/>
        This function is used by the menu command  Insert -- Value Table.<br/>
        <b>Example</b>:<pre>
printValueTable([(lambda n:n^2, "n²"), (lambda n:2^n, "2<sup>n</sup>")],
                 "n", 0, 10, True)</pre>
        displays the values of n² and 2<sup>n</sup> for n from 0 to 10.
    ----de----
        <b>printValueTable(values, variable="n", start=0, end=10, increment=1, vertical=True)</b><br/>
        <i>Gibt eine Wertetafel aus.</i><br/>
        Diese Funktion wird vom Menübefehl Einfügen -- Wertetafel verwendet.<br/>
        <b>Beispiel</b>:<pre>
printValueTable([(lambda n:n^2, "n²"), (lambda n:2^n, "2<sup>n</sup>")],
                 "n", 0, 10, True)</pre>
        Listet die Werte n² und 2<sup>n</sup> für n von 0 bis 10 auf.
    """

    titles = [variable] + [v[1] for v in values]
    functions = [lambda n:n] + [v[0] for v in values]

    tdHead  = '<td bgcolor="#C6D9F1" align="left">'
    tdValue = '<td bgcolor="#F2F7FC" align="right">'

    def args():
        n = start
        if increment > 0:
            while n <= end:
                yield n
                n += increment
        else:
            while n >= end:
                yield n
                n += increment

    print('{html}<table cellspacing="6">') # don't convert to HTML again !
    if vertical:
        print("<tr>", end="") # heading
        for t in titles:
            print(tdHead, t, "</td>", end="")
        print("</tr>")

        for n in args():
            print("<tr>", end="") # heading
            for f in functions:
                print(tdValue, f(n), "</td>", end="")
            print("</tr>")
    else:
        for i in range(len(functions)):
            print("<tr>", end="")
            print(tdHead, titles[i], "</td>", end="")
            for n in args():
                print(tdValue, functions[i](n), "</td>", end="")
    print("</table>{/html}")

def plot(terms, x0, x1, **options):
    termList = [terms]
    if type(terms) in (tuple, list):
        termList = terms
    nSteps = options.get("nSteps", 0)
    dots = options.get("dots", False)
    if nSteps == 0 and isInteger(x0) and isInteger(x1) and x0 < x1:
        bars = options.get("bars", True)
        lines = options.get("lines", False)
        nSteps = x1 - x0
    else:
        nSteps = options.get("nSteps", 200)
        bars = options.get("bars", False)
        lines = options.get("lines", True)
    optStr = ""
    if dots:  optStr += "d"
    if bars:  optStr += "b"
    if not lines: optStr += "n"
    if nSteps == x1 - x0:
        d = 1
    else:
        d = (x1-x0)/float(nSteps)
    val = []
    for i in range(nSteps+1):
        x = x0 + i*d
        try:
            s = str(float(eval(termList[0])))
        except:
            s = "0"
        for j in range(1, len(termList)):
            try:
                s += "&" + str(float(eval(termList[j])))
            except:
                s += "&0"
        val.append(s)
    mark = ["{0},{1}".format(p[0], p[1]) for p in options.get("mark", [])]
    return "@P{0}|{1}|{2}|{3}|{4}".format(optStr, str(x0), str(x1), ";".join(val), ";".join(mark))

def plotFunctions(functions, x0, x1, **options):
    if type(functions) not in (tuple, list):
        functions = [functions]
    nSteps = options.get("nSteps", 0)
    dots = options.get("dots", False)
    if nSteps == 0 and isInteger(x0) and isInteger(x1) and x0 < x1:
        bars = options.get("bars", True)
        lines = options.get("lines", False)
        nSteps = x1 - x0
    else:
        nSteps = options.get("nSteps", 200)
        bars = options.get("bars", False)
        lines = options.get("lines", True)
    optStr = ""
    if dots:  optStr += "d"
    if bars:  optStr += "b"
    if not lines: optStr += "n"
    if nSteps == x1 - x0:
        d = 1
    else:
        d = (x1-x0)/float(nSteps)
    val = []
    for i in range(nSteps+1):
        x = x0 + i*d
        try:
            s = str(float(functions[0](x)))
        except:
            s = "0"
        for j in range(1, len(functions)):
            try:
                s += "&" + str(float(functions[j](x)))
            except:
                s += "&0"
        val.append(s)
    mark = ["{0},{1}".format(p[0], p[1]) for p in options.get("mark", [])]
    return "@P{0}|{1}|{2}|{3}|{4}".format(optStr, str(x0), str(x1), ";".join(val), ";".join(mark))


#===============================================================
#  Introspection
#  (internal functions for the mathGUIde GUI)
#---------------------------------------------------------------

def _x_getFunc(glob, loc):
    all = [x.strip() for x in glob.keys() if not "_" in x]
    classes = [x+"C" for x in all if str(glob[x]).startswith("<class") ]
    functions = [x+"F" for x in all if str(glob[x]).startswith("<function") ]
    all = classes + functions
    all.sort(key=str.lower)
    return "L" + ",".join(all)

def _x_getMeth(c, glob, loc):
    s_static = [x.strip() for x in dir(eval(c, glob, loc)) if x[0] == "_"]
    all = [x.strip() for x in dir(eval(c, glob, loc)) if x[0] != "_"]
    typeDescr = str(type(eval(c, glob, loc)))
    elements = []
    if typeDescr == "<class 'type'>":
        # class: display static methods
        try:
            elements = [m+"F" for m in eval(c+"._staticMethods")]
        except:
            pass
    elif typeDescr[:6] == "<class":
        # object: display members
        for x in all:
            typeStr = str(type(eval(c+"."+x, glob, loc)))
            if "function" not in typeStr:
                if "'method'" in typeStr:
                    elements.append(x+"F")
                else:
                    elements.append(x+"E")
    else:
        # global functions
        elements = [x+"()" for x in all if "function" in str(type(eval(c+"."+x, glob, loc)))]
    elements.sort(key=str.lower)
    return "L" + ",".join(elements)

def _x_getFnPar(fn, glob, loc):
    fn = eval(fn, glob, loc)
    descr = str(fn)
    s = inspect.getdoc(fn)
    return "S" + s

def _x_getMethPar(obj, fn, glob, loc):
    fn = eval(obj + "." + fn, glob, loc)
    s = inspect.getdoc(fn)
    return "S" + s

def _x_parse(s):
    op = s[-1]
    s = s[:-1]
    if op == "(":                               # \w --> word characters [a-zA-Z0-9_]
        m = re.search(r"\w+\s*\.\s*\w+\s*$", s) # \s --> whitespace
        if m:
            return [x.strip() for x in s[m.start():].split(".")] + ["("]
    m = re.search(r"\w+\s*$", s)
    if m:
        return ["", s[m.start():].strip(), op]

def _contextInfo(s, glob, loc):
    """ Erwartet den Inhalt der Cursorzeile links vom Cursor.
        Rückgabewert:
          - erstes Zeichen "S": danach Info-String für Statuszeile
          - erstes Zeichen "L": danach durch Komma getrennte Methoden
    """
    if s == "@@@": # globale Klassen und Funktionen:
        try:
            r = _x_getFunc(glob, loc)
        except:
            r = "***"
        return r

    p = _x_parse(s)
    try:
        if p[-1] == ".":
            r = _x_getMeth(p[-2], glob, loc)
        elif p[-1] == "(":
            if p[-3] != "":
                r = _x_getMethPar(p[-3], p[-2], glob, loc)
            else:
                r = _x_getFnPar(p[-2], glob, loc)
    except:
        r = "***"
    return r

def _x_classMethInfo(s, glob, loc):
    c, m, p = _x_parse(s)
    assert p == "("
    if c == "": # global
        return "global.{0}".format(m)
    else:
        t = str(type(eval(c, glob, loc)))
        if not t == "<class 'type'>":
            c = t[18:-2] # e.g. "<class 'mathguide.Matrix'>" --> "Matrix"
        return "{0}.{1}".format(c, m)

#end
