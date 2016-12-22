clear
clc

X = [24 20 3 24;
    16 2 1 6;
    23 7 4 5;
    10 20 5 22];

a = 1 / 2;
b = sqrt(1 / 2) * cos(pi / 8);
c = sqrt(1 / 2) * cos(3 * pi / 8);
A = [a a a a;
    b c -c b;
    a -a -a a;
    c -b b -c];
A * A'; % != E
A * inv(A); % = E
Y = A * X * A';

%==========================================

d = c / b;
C = [1 1 1 1;
    1 d -d 1;
    1 -1 -1 1;
    d -1 1 -d];
E = [a*a a*b a*a a*b;
    a*b b*b a*b b*b;
    a*a a*b a*a a*b;
    a*b b*b a*b b*b;];
C * C'; % = diagonal matrix
C * inv(C); % = E
(2 * C) * inv(2 * C); % = E
Y = C * X * C' .* E; % = A * X * A'

%==========================================

a = 1 / 2;
b = sqrt(2 / 5);
c = sqrt(1 / 2) * cos(3 * pi / 8);
A = [a a a a;
    b c -c b;
    a -a -a a;
    c -b b -c];
A * inv(A)
d = c / b
Cf = [1 1 1 1;
    2 1 -1 2;
    1 -1 -1 1;
    1 -2 2 -1];

